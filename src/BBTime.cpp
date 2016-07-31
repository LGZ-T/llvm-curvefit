#include <llvm/Pass.h>
#include <ValueProfiling.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
using namespace llvm;

namespace{
    struct BBTime:public ModulePass{
        static char ID;
        enum inst_type {reg_inst, incall_inst, mpicall_inst, outcall_inst, notfound};            
        
        BBTime():ModulePass(ID) {}
        static void IncrementBlockCounters(llvm::Value* Inc, unsigned bbid,
                            GlobalVariable* Counters, IRBuilder<>& Builder)
        {
            LLVMContext &Context = Inc->getContext();
            std::vector<Constant*> Indices(2);
            Indices[0] = Constant::getNullValue(Type::getInt32Ty(Context));
            Indices[1] = ConstantInt::get(Type::getInt32Ty(Context),bbid);
            Constant *ElementPtr = ConstantExpr::getGetElementPtr(Counters,Indices);

            Value *OldVal = Builder.CreateLoad(ElementPtr, "OldBlockCounter");
            Value *NewVal = Builder.CreateAdd(OldVal,
                    Builder.CreateZExtOrBitCast(Inc,Type::getInt64Ty(Context)),
                    "NewBlockCounter");
            Builder.CreateStore(NewVal, ElementPtr);
            return;
        }

        static void insertgetcyclecall(Function *getcycle, unsigned bbid, 
                        GlobalVariable *cyclearray, IRBuilder<>& Builder, bool isfirst)
        {
            static CallInst *cycle1, *cycle2;
            if(isfirst)
            {
                cycle1 = Builder.CreateCall(getcycle,"");
                return;
            }
            cycle2 = Builder.CreateCall(getcycle,"");
            Value *sub = Builder.CreateSub(cycle2,cycle1);
            std::vector<Constant*> Indices(2);
            LLVMContext &Context = sub->getContext();
            Indices[0] = Constant::getNullValue(Type::getInt32Ty(Context));
            Indices[1] = ConstantInt::get(Type::getInt32Ty(Context),bbid);
            Constant *ElementPtr = ConstantExpr::getGetElementPtr(cyclearray,Indices);
            LoadInst *loadinst = Builder.CreateLoad(ElementPtr,"");
            Value *add = Builder.CreateAdd(loadinst,sub);
            Builder.CreateStore(add,ElementPtr);
            return;
        }
        bool runOnModule(Module &M) override
        {
            /* this is for getting loop info
            for(Module::iterator itefunc=M.begin(),endfunc=M.end();itefunc!=endfunc;++itefunc)
            {
                Function &f = *itefunc;
                if(f.isDeclaration()) continue;
                errs() << "in func " << f.getName() << ":\n";
                LoopInfo &loopinfo = getAnalysis<LoopInfo>(f);
                for(auto iteloop=loopinfo.begin(),endloop=loopinfo.end();iteloop!=endloop;++iteloop)
                {
                    Loop *loop = *iteloop;
                    unsigned loopdepth = loop->getLoopDepth();
                    errs() << "loop depth: " << loopdepth << "##";
                    if(loop->getCanonicalInductionVariable()!=nullptr)
                        errs() << "ok##" << *(loop->getCanonicalInductionVariable());
                    errs() << "\n";
                }
            }*/
            uint64_t bbid = -1;
            unsigned NumBlocks = 2000;
            LLVMContext &Context = M.getContext();
            IRBuilder<> Builder(Context);

            Type* ATy = ArrayType::get(Type::getInt64Ty(Context),NumBlocks);

            //this will not insert a definition in the file
            GlobalVariable* CountArray = new GlobalVariable(M, ATy, false,
                    GlobalVariable::ExternalLinkage, Constant::getNullValue(ATy),
                    "BlockPredCount");
            GlobalVariable* CycleArray = new GlobalVariable(M, ATy, false,
                    GlobalVariable::ExternalLinkage, Constant::getNullValue(ATy),
                    "BlockPredCycle");

            Constant *Inc = ConstantInt::get(Type::getInt32Ty(Context),1);
            Type *int64ty = Type::getInt64Ty(Context);
            Constant *GC = M.getOrInsertFunction("llvm.readcyclecounter",int64ty,NULL);
            Function *GetCycle = cast<Function>(GC);

            int phiinstcount = 0, continue_inst = 0;
            std::string mainfunc="MAIN__", skifunc="main";
            if(M.getFunction("MAIN__")==nullptr)
            {
                mainfunc = "main";
                skifunc = "";
            }
            for(Module::iterator itefunc=M.begin(),endfunc=M.end();itefunc!=endfunc;++itefunc)
            {
                Function &f = *itefunc;
                if(f.getName()==skifunc) continue;
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
                    Instruction *last = &*(--(bb.end()));
                    bool hasinserted = false;
                    

                    if(std::string(f.getName())==mainfunc && 
                       std::string(last->getOpcodeName())=="ret")
                    {
                        Constant *FuncEntry = M.getOrInsertFunction("outinfo_bbtime",Type::getVoidTy(Context),NULL,NULL);
                        CallInst::Create(FuncEntry,"",last);
                    }

                    if(first==last) continue;
                    if(bb.size()-phiinstcount <= 2)
                    {
                        if(std::string(first->getOpcodeName())=="call" && my_inst_type(first,M)!=incall_inst)
                        {
                            Builder.SetInsertPoint(first);
                            IncrementBlockCounters(Inc,++bbid,CountArray,Builder);
                            insertgetcyclecall(GetCycle,bbid,CycleArray,Builder,true);
                            Builder.SetInsertPoint(last);
                            insertgetcyclecall(GetCycle,bbid,CycleArray,Builder,false);
                        }
                        continue;
                    }

                    while(my_inst_type((Instruction*)itet,M)==incall_inst) { ++itet; }
                    first = (Instruction*)itet;
                    Instruction *bbstart = first;
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
                                if(!hasinserted)
                                {
                                    Builder.SetInsertPoint(bbstart);
                                    IncrementBlockCounters(Inc,bbid,CountArray,Builder);
                                    hasinserted = true;
                                }
                                Builder.SetInsertPoint(first);
                                insertgetcyclecall(GetCycle,bbid,CycleArray,Builder,true);
                                Builder.SetInsertPoint(itet);
                                insertgetcyclecall(GetCycle,bbid,CycleArray,Builder,false);
                            }
                            ++itet;
                            while(my_inst_type((Instruction*)itet,M)==incall_inst) { ++itet; }
                            if(((Instruction*)itet)==last) { continue; }
                            first = (Instruction*)itet;
                            continue_inst = 0;
                        }
                        ++itet;
                        if(((Instruction*)itet)==last)
                        {
                            if(continue_inst>=3)
                            {
                                ++bbid;
                                if(!hasinserted)
                                {
                                    Builder.SetInsertPoint(bbstart);
                                    IncrementBlockCounters(Inc,bbid, CountArray, Builder);
                                    hasinserted = true;
                                }
                                Builder.SetInsertPoint(first);
                                insertgetcyclecall(GetCycle,bbid,CycleArray,Builder,true);
                                Builder.SetInsertPoint(last);
                                insertgetcyclecall(GetCycle,bbid,CycleArray,Builder,false);
                            }
                        }
                    }
                }
            }
            return true;
        }

        /*void getAnalysisUsage(AnalysisUsage &AU) const override
        {
            AU.addRequired<LoopInfo>();
            AU.setPreservesAll();
        }*/
        
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

                //this could happen when the callee is a func pointer
                //for now, we just consider it as a incall_inst
                if(pos==std::string::npos){ errs() << "callee is not found\n"; return incall_inst; }
                ++pos;
                while(temp[pos]!=' ')
                {
                    ++length;
                    ++pos;
                }
                Function *func = M.getFunction(temp.substr(pos-length,length));
                if(func==nullptr)
                {
                    errs() << "callee's name can not be found in the symbol table: "
                           << temp.substr(pos-length,length) << "\n";
                    return  notfound;
                }
                if(func->isDeclaration()) return outcall_inst;
                else return incall_inst;
            }
            
            if(callfunc->getCalledFunction()->isDeclaration()) return outcall_inst;
            return incall_inst;
        }
    };
}

char BBTime::ID = 0;
static RegisterPass<BBTime> X("BBTime","get the execute time of main basic block",false,false);
