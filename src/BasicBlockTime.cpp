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
    struct BBTime:public BasicBlockPass{
        static char ID;
        BBTime():BasicBlockPass(ID) {}
        bool runOnBasicBlock(BasicBlock &bb) override
        {
            //unsigned opcode, pipeline = 0;
            BasicBlock::iterator start,end;
            Module *M = bb.getParent()->getParent();
            LLVMContext& Context = bb.getContext();
            Type* int64ty = Type::getInt64Ty(Context);
            Constant* FuncEntry1 = M->getOrInsertFunction("getBBTime", Type::getVoidTy(Context),int64ty,NULL);
            //Constant* FuncEntry2 = M->getOrInsertFunction("getBBTime2", Type::getVoidTy(Context),int64ty,NULL);
            Value *args[1] = {ConstantInt::get(int64ty,bbid)};
            bbid++;
            start = bb.getFirstInsertionPt();
            end = bb.end();
            end--;
            Instruction *first = &*start;
            Instruction *last = &*end;
            if(first!=last)
            {
                CallInst::Create(FuncEntry1,args,"",first);
                CallInst::Create(FuncEntry1,args,"",last);
            }
            /*for(start=bb.begin(),end=bb.end();start!=end;start++)
            {
                Instruction *inst = &*start;
                opcode = inst->getOpcode();
                
            }*/
            return true;
        }
    };
}

char BBTime::ID = 0;
static RegisterPass<BBTime> X("BBTime","get the execute time of each basic block",false,false);
