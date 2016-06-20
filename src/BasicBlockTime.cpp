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
            Module::FunctionListType &funclist = M.getFunctionList();
            LLVMContext &Context = M.getContext();
            Type *int64ty = Type::getInt64Ty(Context);
            Constant *FuncEntry1 = M.getOrInsertFunction("getBBTime",Type::getVoidTy(Context),int64ty,NULL);
            Value *args[1] = {ConstantInt::get(int64ty,bbid)};
            
            for(Module::FunctionListType::iterator ite=funclist.begin(),end=funclist.end();ite!=end;++ite)
            {
                Function &f = *ite;
                if(M.getFunction(f.getName())!=nullptr) errs() << f.getName() << "\n";
            }           
            
            for(Module::iterator itefunc=M.begin(),endfunc=M.end();itefunc!=endfunc;++itefunc)
            {
                Function &f = *itefunc;
                
                if(f.getName()=="main") continue;

                for(Function::iterator itebb=f.begin(),endbb=f.end();itebb!=endbb;++itebb)
                {
                    BasicBlock &bb = *itebb;
                    errs() << bb.getName() << "\n";
                    Instruction *first = &*(bb.getFirstInsertionPt());
                    Instruction *last = &*(--(bb.end()));
                    errs() << "can u reach there\n";
                    if(first==last) continue;
                    while(isInsideCall(first,funclist)) { ++first; }
                    CallInst::Create(FuncEntry1,args,"",first);
                    errs() << "###1\n";
                    for(Instruction *iteinst=first;iteinst<=last;++iteinst)
                    {
                        errs() << "###2\n";
                        if(isInsideCall(iteinst,funclist))
                        {
                            errs() << "###3\n";
                             CallInst::Create(FuncEntry1,args,"",iteinst++);
                             ++bbid;    
                             args[0] = {ConstantInt::get(int64ty,bbid)};
                             while(isInsideCall(iteinst,funclist)) { ++iteinst; }
                             if(iteinst==last) continue;
                             CallInst::Create(FuncEntry1,args,"",iteinst);
                        }
                        else if(iteinst==last)
                        {
                            CallInst::Create(FuncEntry1,args,"",iteinst);
                        }
                    }
                }
            }
            /*errs() << f.getName() << "\n";
            if(f.getName()!="MAIN__")
            {
                Module *M = f.getParent();
                LLVMContext &Context = f.getContext();
                Type *int64ty = Type::getInt64Ty(Context);
                Constant *FuncEntry1 = M->getOrInsertFunction("getBBTime",Type::getVoidTy(Context),int64ty,NULL);
                for(Function::iterator ite=f.begin(), end=f.end();ite!=end;ite++)
                {
                    BasicBlock &bb = *ite;
                    Instruction *first = &*(bb.getFirstInsertionPt());
                    Instruction *last  = &*(--(bb.end()));
                    Value *args[1] = {ConstantInt::get(int64ty,bbid)};
                    bbid++;
                    if(first!=last)
                    {
                        CallInst::Create(FuncEntry1,args,"",first);
                        CallInst::Create(FuncEntry1,args,"",last);
                    }
                    if(std::string(last->getOpcodeName())=="ret")
                    {
                        Constant *FuncEntry=M->getOrInsertFunction("outinfo",Type::getVoidTy(Context),NULL,NULL);
                        CallInst::Create(FuncEntry,"",last); 
                    }
                }
            }*/
           
            return true;
        }
        bool  isInsideCall(Instruction *inst, Module::FunctionListType &funclist)
        {
            if(std::string(inst->getOpcodeName())!="call") return false;
            CallInst *callfunc = (CallInst *)inst;
            std::string str = callfunc->getCalledFunction()->getName();
            for(Module::FunctionListType::iterator ite=funclist.begin(),end=funclist.end();ite!=end;++ite)
            {
                Function &f = *ite;
                if(str==f.getName()) return true;
            }
            return false;
        }
    };
}

char BBTime::ID = 0;
static RegisterPass<BBTime> X("BBTime","get the execute time of main basic block",false,false);
