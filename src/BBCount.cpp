#include <llvm/Pass.h>
#include <ValueProfiling.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/ValueSymbolTable.h>

using namespace llvm;

/*enum {LShr,Sub,Or,Shl,Xor,Add,AShr,And,PtrToInt,BitCast,GetElementPtr,ICmp,UIToFP,Select,Mul,FAdd,IntToPtr,ZExt,SExt,Trunc,
      FSub,FCmp,SIToFP,FPToUI,FPToSI,FMul,Alloca,FPExt,FPTrunc,UDiv,SDiv,URem,}*/
namespace{
    struct BBCount:public ModulePass{
        static char ID;
        enum inst_type {reg_inst, incall_inst, mpicall_inst, outcall_inst};
        BBCount():ModulePass(ID) {}
        static void IncrementBlockCounters(llvm::Value* Inc, unsigned Index, 
                             GlobalVariable* Counters, IRBuilder<>& Builder)
        {
            LLVMContext &Context = Inc->getContext();
            std::vector<Constant*> Indices(2);
            Indices[0] = Constant::getNullValue(Type::getInt32Ty(Context));
            Indices[1] = ConstantInt::get(Type::getInt32Ty(Context), Index);
            Constant *ElementPtr = ConstantExpr::getGetElementPtr(Counters, Indices);

            Value* OldVal = Builder.CreateLoad(ElementPtr, "OldBlockCounter");
            Value* NewVal = Builder.CreateAdd(
                OldVal,Builder.CreateZExtOrBitCast(Inc,Type::getInt64Ty(Context)),
                "NewBlockCounter");
            Builder.CreateStore(NewVal, ElementPtr);
        }

        bool runOnModule(Module &M) override
        {
            uint64_t bbid = -1;
            unsigned NumBlocks = 2000;
            LLVMContext &Context = M.getContext();
            IRBuilder<> Builder(Context);
            
            Type* ATy = ArrayType::get(Type::getInt64Ty(M.getContext()),NumBlocks);
            
            //this will not insert a definition in the file
            //GlobalVariable* Counters = new GlobalVariable(ATy, false, GlobalVariable::InternalLinkage,
            //        Constant::getNullValue(ATy),"BlockPredCounters");
            GlobalVariable* Counters = new GlobalVariable(M, ATy, false,
                    GlobalVariable::ExternalLinkage, Constant::getNullValue(ATy),
                    "BlockPredCount");
            Constant * Inc = ConstantInt::get(Type::getInt32Ty(Context),1);
            int phiinstcount = 0, continue_inst = 0;             
            for(Module::iterator itefunc=M.begin(),endfunc=M.end();itefunc!=endfunc;++itefunc)
            {
                Function &f = *itefunc;
                if(f.getName()=="main") continue;
                if(f.isDeclaration()) continue;
                for(Function::iterator itebb=f.begin(),endbb=f.end();itebb!=endbb;++itebb)
                {
                    BasicBlock &bb = *itebb;
                    phiinstcount = 0;
                    for(BasicBlock::iterator tbegin=bb.begin();;++tbegin)
                    {
                        Instruction &tinst = *tbegin;
                        if(std::string(tinst.getOpcodeName())=="phi") 
                            ++phiinstcount;
                        else break;
                    }
                   
                    BasicBlock::iterator itet = bb.getFirstInsertionPt();
                    Instruction *first = &*itet;
                    Instruction *bbstart = first;
                    Instruction *last = &*(--(bb.end()));
                    bool insert = false;                    
                    if(std::string(f.getName())=="MAIN__" && 
                       std::string(last->getOpcodeName())=="ret")
                    {
                        Constant *FuncEntry = M.getOrInsertFunction("outinfo_bbcount",Type::getVoidTy(Context),NULL,NULL);
                        CallInst::Create(FuncEntry,"",last);
                    }
                    if(first==last) continue;
                    if(bb.size()-phiinstcount <= 2)
                    {
                        if(std::string(first->getOpcodeName())=="call" && my_inst_type(first,M)!=incall_inst)
                        {
                            Builder.SetInsertPoint(first);
                            IncrementBlockCounters(Inc, ++bbid, Counters, Builder);
                        }
                        continue;
                    }
                    while(my_inst_type((Instruction*)itet,M)==incall_inst) { ++itet; }
                    while(((Instruction*)itet)!=last)
                    {
                        inst_type temp_inst_type = my_inst_type((Instruction*)itet,M);
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
                            if(continue_inst>=3)
                            {
                                ++bbid;
                                if(!insert)
                                {
                                    Builder.SetInsertPoint(bbstart);
                                    IncrementBlockCounters(Inc, bbid, Counters, Builder);
                                    insert = true;
                                }
                            }
                            ++itet;
                            while(my_inst_type((Instruction*)itet,M)==incall_inst) { ++itet; }
                            if(((Instruction*)itet)==last) { continue; }
                            continue_inst = 0;
                        }
                        ++itet;
                        if(((Instruction*)itet)==last)
                        {
                            if(continue_inst>=3)
                            {
                                ++bbid;
                                if(!insert)
                                {
                                    Builder.SetInsertPoint(bbstart);
                                    IncrementBlockCounters(Inc,bbid,Counters,Builder);
                                    insert = true;
                                }
                            }
                        }
                    }
                }
            }
            return true;
        }
        
        /*
         * if the instruction is not a call, then return reg_inst;
         * if the instruction is a call inst, but i cannot get the callee(see llvm document for getCalledFunction),
         * return outcall_inst;
         * if the instruction is a call inst, and the callee is defined in the module.return incall_inst;
         */
        inst_type  my_inst_type(Instruction *inst,Module &M)
        {
            CallInst *callfunc = (CallInst *)inst;
            if(std::string(inst->getOpcodeName())!="call") return reg_inst;
            if(callfunc->getCalledFunction()==nullptr) 
            {
                std::string inststr;
                raw_string_ostream inststream(inststr); 
                callfunc->getCalledValue()->print(inststream);
                std::string temp = inststream.str();
                std::size_t pos = temp.find("@"), length=0;
                if(pos==std::string::npos) errs() << "wrong activity\n";
                ++pos;
                while(temp[pos]!=' ')
                {
                    ++length;
                    ++pos;
                }
                Function *func = M.getFunction(temp.substr(pos-length,length));
                if(func==nullptr) errs() << "that is a wrong function name: " << temp.substr(pos-length,length) << "\n";
                if(func->isDeclaration()) return outcall_inst;
                else return incall_inst;
            }
            if(callfunc->getCalledFunction()->isDeclaration()) return outcall_inst;
            
            return incall_inst;
        }
    };
}

char BBCount::ID = 0;
static RegisterPass<BBCount> X("BBCount","get the execution count of basic block",false,false);
