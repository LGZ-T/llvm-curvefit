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

namespace{
    struct ReuseDis:public BasicBlockPass{
        static char ID;
        ReuseDis():BasicBlockPass(ID) {}
        bool runOnBasicBlock(BasicBlock &bb) override
        {
            BasicBlock::iterator start,end;
            Module *M = bb.getParent()->getParent();
            Function *f = bb.getParent();
            LLVMContext& Context = bb.getContext();
            Type* int32ty = Type::getInt32Ty(Context);
            //Type* CharPtr = Type::getInt8PtrTy(Content);
            Constant* FuncEntry = M->getOrInsertFunction("getReuseDistance", Type::getVoidTy(Context), int32ty,NULL);
            //Constant* Funcout = M->getOrInsertFunction("outinfo", Type::getVoidTy(Context), NULL);
            Value* args[1] = {0};
            Value *operand;
             
            for(start=bb.begin(),end=bb.end();start!=end;start++){
                Instruction *inst = &*start;
                unsigned opcode = inst->getOpcode();

                operand = NULL;
                if(opcode==Instruction::Load){
                    operand = cast<LoadInst>(inst)->getPointerOperand();
                }
                else if(opcode==Instruction::Store){
                    operand = cast<StoreInst>(inst)->getPointerOperand();
                    /*StoreInst *store = (StoreInst *)inst;
                    operand = store->getPointerOperand();*/
                }

                if(operand){
                    if(operand->getType()!=int32ty){
                        CastInst::CastOps opc = CastInst::getCastOpcode(operand, true, int32ty, true);
                        args[0] = CastInst::Create(opc,operand,int32ty,"reuse.cast",inst);
                    }else{
                        args[0] = operand;
                    }
                    CallInst::Create(FuncEntry, args,"",inst);
                }
            }
            return true;
        }
    };
}

char ReuseDis::ID = 0;
static RegisterPass<ReuseDis> X("Reuse","get the reuse distance of memory reference",false,false);
