#include <llvm/Pass.h>
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
    struct BBTime:public FunctionPass{
        static char ID;
        BBTime():FunctionPass(ID) {}
        bool runOnFunction(Function &f) override
        {
            if(f.getName()=="MAIN__")
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
            }
           
            return true;
        }
    };
}

char BBTime::ID = 0;
static RegisterPass<BBTime> X("BBTime","get the execute time of main basic block",false,false);
