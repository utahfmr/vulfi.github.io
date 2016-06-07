
#include "AddrSites.h"
#include "AddrInstrList.h"
#include <stack>

using namespace llvm;
using namespace std;

Instruction* getInstrByVal(Value* val, Function* fn){
  Instruction* targetInstr=NULL;
  for (inst_iterator I=inst_begin(fn),E=inst_end(fn);I!=E;++I){
    Instruction* instr = &*I;
    Value* instrVal = &*instr;
    if(instrVal==val){
      targetInstr=instr;
      break;
    }
  }
  return targetInstr;
}

set<Instruction*> getBkwdSlice(Instruction* instr){
  Function *fn=instr->getParent()->getParent();
  set<Instruction*> visited;
  set<Instruction*>::iterator si;
  stack<Instruction*> S;
  S.push(instr);
  while (!S.empty()){
    // Pop a vertex from stack and print it
    Instruction* ci = S.top();
    S.pop();

    for (Use &U : ci->operands()) {
      Value *v = U.get();
      Instruction *vi=getInstrByVal(v,fn);
      if(vi){
	if(visited.find(vi)==visited.end()){
	  visited.insert(vi);
	  S.push(vi);
	}
      }
    }
  } 
  return visited;
}

set<Value*> getArgList(Function *fn){
  set<Value*> arglist;
  for(Function::arg_iterator I=fn->arg_begin();I!=fn->arg_end();I++){
    Value *argVal=&*I;
    if(arglist.find(argVal)==arglist.end())
      arglist.insert(argVal);
  }  
  return arglist;
}

bool isBaseAdrInArgLst(GetElementPtrInst* gep){
  Function* parent=gep->getParent()->getParent();
  set<Value*> arglist=getArgList(parent);    
  Value* gepPtrOp = gep->getPointerOperand();
  if(arglist.find(gepPtrOp)!=arglist.end())
    return true;

  return false;
}

set<Instruction*> AddrSites::getfaultSites(Module *M, CLData *Cl, FunctionList *Fl){  
  // 1st Pass - Build the initial instruction list
  // Get list of target functions
  vector<Function*> fnList = Fl->getTargetFnList();
  enum Common::FSAlgo fsalg=Cl->getFSAlgo();
  
  // Iterative over instructions of the target functions
  for(unsigned i=0;i<fnList.size();i++){
    for(inst_iterator it=inst_begin(fnList[i]),itEnd=inst_end(fnList[i]);
	it!=itEnd;it++){
      Instruction* currentInstr = &*it;
      //Skip PHI Nodes
      if(isa<PHINode>(currentInstr)    ||
         isa<AllocaInst>(currentInstr) ||
	     isa<LandingPadInst>(currentInstr)){
    	  continue;
      }

	  Type* ty = currentInstr->getType();
	  Type::TypeID tyID = ty->getTypeID();
	  if(tyID == Type::VectorTyID){
		  Type* ety = currentInstr->getType()->getVectorElementType();
		  if(!ety->isPointerTy()){
			  continue;
		  }
	  } else if(!ty->isPointerTy()) {
		  continue;
	  }

      if((fsalg==Common::FS_ADDG || fsalg==Common::FS_ADDI) &&
         !isa<GetElementPtrInst>(currentInstr))
    	  continue;

      // Build initial list of fault sites
      if(instrList.find(currentInstr)==instrList.end())
    	  instrList.insert(currentInstr);

      // Update instruction to function hash table
      this->instrToFuncTable.insert(pair<Instruction*, Function*>(currentInstr,fnList[i]));

      #ifdef DEBUG
      // Print to screen initial target instructions
      errs() << "\nInstruction: "<< *currentInstr;
      errs() << "\nTypeID: "<< currentInstr->getType()->getTypeID();
      #endif
      
    }
  }
  
  // 2nd/Final Pass - Update the initial instruction list

  //write debug data if dbgfile name is provided
  string dbgfile = Cl->getDbgFile();
  set<Instruction*> bcksliceInstrList;
  if(!dbgfile.empty()){
    for(set<Instruction*>::iterator si = instrList.begin();si!=instrList.end();++si){
      Instruction* currentInstr = *si;
      set<Instruction*> bckslice=getBkwdSlice(currentInstr);
      for(set<Instruction*>::iterator si1 = bckslice.begin();si1!=bckslice.end();++si1){
      	Instruction* bckInstr= *si1;
      	if(isa<PHINode>(bckInstr) ||
      	   isa<AllocaInst>(bckInstr) ||
      	   isa<LandingPadInst>(bckInstr)){
      	  continue;
      	} else if(bcksliceInstrList.find(bckInstr)==bcksliceInstrList.end()){
      		bcksliceInstrList.insert(bckInstr);
      	}
      }
      
      // print info for each instruction
      if(fsalg==Common::FS_ADDI)
    	  this->writeDbgData(currentInstr,Cl,"addi");
      else if(fsalg==Common::FS_ADDG)
    	  this->writeDbgData(currentInstr,Cl,"addg");
      else if(fsalg==Common::FS_ADDR)
    	  this->writeDbgData(currentInstr,Cl,"addr");
    }
  }

  if(fsalg==Common::FS_ADDI) return bcksliceInstrList;
  else if(fsalg==Common::FS_ADDG) return instrList;
  else if(fsalg==Common::FS_ADDR){
	  for(set<Instruction*>::iterator si = bcksliceInstrList.begin();si!=bcksliceInstrList.end();++si){
		  Instruction* bcksliceInstr=*si;
		  if(instrList.find(bcksliceInstr)==instrList.end()){
			  instrList.insert(bcksliceInstr);
		  }
	  }
  }
  return instrList;
}
