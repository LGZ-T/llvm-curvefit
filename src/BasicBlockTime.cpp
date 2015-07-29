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

uint64_t funcid = 0;
/*enum {LShr,Sub,Or,Shl,Xor,Add,AShr,And,PtrToInt,BitCast,GetElementPtr,ICmp,UIToFP,Select,Mul,FAdd,IntToPtr,ZExt,SExt,Trunc,
      FSub,FCmp,SIToFP,FPToUI,FPToSI,FMul,Alloca,FPExt,FPTrunc,UDiv,SDiv,URem,}*/
namespace{
    struct funcTime:public FunctionPass{
        static char ID;
        funcTime():FunctionPass(ID) {}
        bool runOnFunction(Function &func) override
        {
            //unsigned opcode, pipeline = 0;
            if(func.getName().equals("main"))
            {
                Module *M = func.getParent();
                LLVMContext& context = func.getContext();
//                Type * int64ty = Type::getInt64Ty(context);
                Constant *FuncEntry1 = M->getOrInsertFunction("getfuncTime1",Type::getVoidTy(context),NULL);
                Constant *FuncEntry2 = M->getOrInsertFunction("getfuncTime2",Type::getVoidTy(context),NULL);
                Function::iterator ite;
                for(ite=func.begin();ite!=func.end();ite++)
                {
                    BasicBlock *bb = &*ite;
                    BasicBlock::iterator start, end;
                    start = bb->getFirstInsertionPt();
                    end = bb->end();
                    end--;
                    Instruction *first = &*start;
                    Instruction *last = &*end;
                    if(first!=last)//it is not empty
                    {
                        CallInst::Create(FuncEntry1,"",first);
                        CallInst::Create(FuncEntry2,"",last);
                    }
                }
                
            }
//            BasicBlock::iterator start,end;
//            Module *M = func.getParent()->getParent();
//            LLVMContext& Context = func.getContext();
//            Type* int64ty = Type::getInt64Ty(Context);
//            Constant* FuncEntry1 = M->getOrInsertFunction("getfuncTime", Type::getVoidTy(Context),int64ty,NULL);
//            //Constant* FuncEntry2 = M->getOrInsertFunction("getfuncTime2", Type::getVoidTy(Context),int64ty,NULL);
//            Value *args[1] = {ConstantInt::get(int64ty,funcid)};
//            funcid++;
//            start = func.getFirstInsertionPt();
//            end = func.end();
//            end--;
//            Instruction *first = &*start;
//            Instruction *last = &*end;
//            if(first!=last)
//            {
//                CallInst::Create(FuncEntry1,args,"",first);
//                CallInst::Create(FuncEntry1,args,"",last);
//            }
//            /*for(start=func.begin(),end=func.end();start!=end;start++)
//            {
//                Instruction *inst = &*start;
//                opcode = inst->getOpcode();
//                
//            }*/
            return true;
        }
    };
}

char funcTime::ID = 0;
static RegisterPass<funcTime> X("mfuncTime","get exec time of basic blocks which resides in the main func",false,false);
