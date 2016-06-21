#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <ValueProfiling.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>

using namespace llvm;

uint64_t bbid = 0;
/*enum {LShr,Sub,Or,Shl,Xor,Add,AShr,And,PtrToInt,BitCast,GetElementPtr,ICmp,UIToFP,Select,Mul,FAdd,IntToPtr,ZExt,SExt,Trunc,
      FSub,FCmp,SIToFP,FPToUI,FPToSI,FMul,Alloca,FPExt,FPTrunc,UDiv,SDiv,URem,}*/
namespace{
    struct BBTime:public ModulePass{
        static char ID;
        BBTime():ModulePass(ID) {}
        bool runOnModule(Module &M) override
        {
            LLVMContext &Context = M.getContext();
            Type *int64ty = Type::getInt64Ty(Context);
            Constant *FuncEntry1 = M.getOrInsertFunction("getBBTime",Type::getVoidTy(Context),int64ty,NULL);
            Value *args[1] = {ConstantInt::get(int64ty,bbid)};
            
            for(Module::iterator itefunc=M.begin(),endfunc=M.end();itefunc!=endfunc;++itefunc)
            {
                Function &f = *itefunc;
                if(f.getName()=="main") continue;
                if(f.isDeclaration()) continue;
                for(Function::iterator itebb=f.begin(),endbb=f.end();itebb!=endbb;++itebb)
                {
                    BasicBlock &bb = *itebb;
                    BasicBlock::iterator itet = bb.getFirstInsertionPt();
                    Instruction *first = &*itet;
                    Instruction *last = &*(--(bb.end()));
                    if(first==last) continue;
                    while(isInsideCall((Instruction*)itet)) { ++itet; }
                    CallInst::Create(FuncEntry1,args,"",(Instruction*)itet);
                    while(((Instruction*)itet)<=last)
                    {
                        if(isInsideCall((Instruction*)itet))
                        {
                             CallInst::Create(FuncEntry1,args,"",(Instruction*)itet);
                             ++itet;
                             ++bbid;    
                             args[0] = {ConstantInt::get(int64ty,bbid)};
                             while(isInsideCall((Instruction*)itet)) { ++itet; }
                             if(((Instruction*)itet)==last) { ++itet; continue; }
                             CallInst::Create(FuncEntry1,args,"",(Instruction*)itet);
                        }
                        else if(((Instruction*)itet)==last)
                        {
                            errs() << "insert before last instruction\n";
                            CallInst::Create(FuncEntry1,args,"",(Instruction*)itet);
                        }
                        ++itet;
                    }
                }
            }
            return true;
        }

        bool  isInsideCall(Instruction *inst)
        {
            CallInst *callfunc = (CallInst *)inst;
            if(std::string(inst->getOpcodeName())!="call" || callfunc->getCalledFunction()==nullptr ) return false;
            return !(callfunc->getCalledFunction()->isDeclaration());
        }
    };
}

char BBTime::ID = 0;
static RegisterPass<BBTime> X("BBTime","get the execute time of main basic block",false,false);
