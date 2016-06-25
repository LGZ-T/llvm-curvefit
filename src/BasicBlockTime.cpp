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

uint64_t bbid = -1;
/*enum {LShr,Sub,Or,Shl,Xor,Add,AShr,And,PtrToInt,BitCast,GetElementPtr,ICmp,UIToFP,Select,Mul,FAdd,IntToPtr,ZExt,SExt,Trunc,
      FSub,FCmp,SIToFP,FPToUI,FPToSI,FMul,Alloca,FPExt,FPTrunc,UDiv,SDiv,URem,}*/
namespace{
    struct BBTime:public ModulePass{
        static char ID;
        enum inst_type {reg_inst, incall_inst, outcall_inst};
        BBTime():ModulePass(ID) {}
        bool runOnModule(Module &M) override
        {
            LLVMContext &Context = M.getContext();
            Type *int64ty = Type::getInt64Ty(Context);
            Constant *FuncEntry1 = M.getOrInsertFunction("getBBTime1",Type::getVoidTy(Context),int64ty,NULL);
            Constant *FuncEntry2 = M.getOrInsertFunction("getBBTime2",Type::getVoidTy(Context),int64ty,NULL);
            M.getFunction("getBBTime1")->addFnAttr(Attribute::AlwaysInline);
            M.getFunction("getBBTime2")->addFnAttr(Attribute::AlwaysInline);
            Value *args[1];
            int phiinstcount = 0, continue_inst = 0;
            
            for(Module::iterator itefunc=M.begin(),endfunc=M.end();itefunc!=endfunc;++itefunc)
            {
                Function &f = *itefunc;
                if(f.getName()=="main") continue;
                if(f.isDeclaration()) continue;
                for(Function::iterator itebb=f.begin(),endbb=f.end();itebb!=endbb;++itebb)
                {
                    phiinstcount = 0;
                    continue_inst = 0;
                    BasicBlock &bb = *itebb;
                    for(BasicBlock::iterator tbegin=bb.begin();;++tbegin)
                    {
                        Instruction &tinst = *tbegin;
                        if(std::string(tinst.getOpcodeName())=="phi") 
                            ++phiinstcount;
                        else break;
                    }
                    BasicBlock::iterator itet = bb.getFirstInsertionPt();
                    Instruction *first = &*itet;
                    Instruction *last = &*(--(bb.end()));

                    if(std::string(f.getName())=="MAIN__" && 
                       std::string(last->getOpcodeName())=="ret")
                    {
                        Constant *FuncEntry = M.getOrInsertFunction("outinfo",Type::getVoidTy(Context),NULL,NULL);
                        CallInst::Create(FuncEntry,"",last);
                    }

                    if(first==last) continue;
                    if(bb.size()-phiinstcount == 2)
                    {
                        if(std::string(first->getOpcodeName())=="call" && my_inst_type(first)!=incall_inst)
                        {
                            args[0] = {ConstantInt::get(int64ty,++bbid)};
                            CallInst::Create(FuncEntry1,args,"",first);
                            CallInst::Create(FuncEntry2,args,"",last);
                        }
                        continue;
                    }

                    while(my_inst_type((Instruction*)itet)==incall_inst) { ++itet; }
                    first = (Instruction*)itet;
                    while(((Instruction*)itet)!=last)
                    {
                        inst_type temp_inst_type = my_inst_type((Instruction*)itet);
                        if(temp_inst_type==reg_inst)
                        {
                            ++continue_inst;
                        }
                        else if(temp_inst_type==outcall_inst)
                        {
                            continue_inst=5;
                        }
                        else
                        {
                            if(continue_inst>4)
                            {
                                 args[0] = {ConstantInt::get(int64ty,++bbid)};
                                 CallInst::Create(FuncEntry1,args,"",first);
                                 CallInst::Create(FuncEntry2,args,"",(Instruction*)itet);
                            }
                            ++itet;
                            while(my_inst_type((Instruction*)itet)==incall_inst) { ++itet; }
                            if(((Instruction*)itet)==last) { ++itet; continue; }
                            first = (Instruction*)itet;
                            continue_inst = 0;
                        }
                        ++itet;
                        if(((Instruction*)itet)==last)
                        {
                            if(continue_inst>4)
                            {
                                args[0] = {ConstantInt::get(int64ty,++bbid)};
                                CallInst::Create(FuncEntry1,args,"",first);
                                CallInst::Create(FuncEntry2,args,"",(Instruction*)itet);
                            }
                        }
                    }
                }
            }
            errs() << "bbid is: " << bbid << "\n";
            return true;
        }
        
        /*
         * if the instruction is not a call, then return reg_inst;
         * if the instruction is a call inst, but i cannot get the callee(see llvm document for getCalledFunction),
         * return outcall_inst;
         * if the instruction is a call inst, and the callee is defined in the module.return incall_inst;
         */
        inst_type  my_inst_type(Instruction *inst)
        {
            CallInst *callfunc = (CallInst *)inst;
            if(std::string(inst->getOpcodeName())!="call") return reg_inst;
            else if(callfunc->getCalledFunction()==nullptr) return outcall_inst;
            else if(callfunc->getCalledFunction()->isDeclaration()) return outcall_inst;
            else return incall_inst;
        }
    };
}

char BBTime::ID = 0;
static RegisterPass<BBTime> X("BBTime","get the execute time of main basic block",false,false);
