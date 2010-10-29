//============================================================================
// Name        : Compile.cpp
// Author      : Salomon BRYS
// Copyright   : Salomon BRYS, Apache Lisence
//============================================================================

#include "State.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>

#include <vector>
#include <string>

using namespace llvm;

Function * CompileRE(Module * M, DFSM * dfsm, const std::string & fName)
{
	LLVMContext & C = M->getContext();

//	Function * fPutchar = cast<Function>(M->getOrInsertFunction("putchar", Type::getInt32Ty(C), Type::getInt32Ty(C), (Type *)0));
	
	Function * func = cast<Function>(M->getOrInsertFunction(fName, Type::getInt32Ty(C), Type::getInt8PtrTy(C), (Type *)0));
	Argument * str = func->arg_begin();
	str->setName("str");

	BasicBlock * entryBB = BasicBlock::Create(C, "entry", func);
	Value * posPtr = new AllocaInst(Type::getInt32Ty(C), "posPtr", entryBB);
	new StoreInst(ConstantInt::get(Type::getInt32Ty(C), -1), posPtr, entryBB);
	Value * retPtr = new AllocaInst(Type::getInt32Ty(C), "retPtr", entryBB);
	new StoreInst(ConstantInt::get(Type::getInt32Ty(C), 0), retPtr, entryBB);

//	typedef std::map<int, BasicBlock *> BBMap;
//	BBMap bbmap;
	std::vector<BasicBlock *> bbmap(dfsm->size() + 1);

	int stateIndex = 0;
	for (DFSM::const_iterator state = dfsm->begin(); state != dfsm->end(); ++state, ++stateIndex)
		if (*state)
		{
//			bbmap.insert(BBMap::value_type(state->first, BasicBlock::Create(C, "State", func)));
			bbmap[stateIndex] = BasicBlock::Create(C, "State", func);
		}
	BranchInst::Create(bbmap[0], entryBB);

	BasicBlock * endBB = BasicBlock::Create(C, "End", func);
	Value * retVal = new LoadInst(retPtr, "ret", endBB);
	ReturnInst::Create(C, retVal, endBB);

	ConstantInt * cst8_0 = ConstantInt::get(Type::getInt8Ty(C), 0);
	ConstantInt * cst32_1 = ConstantInt::get(Type::getInt32Ty(C), 1);

	stateIndex = 0;
	for (DFSM::const_iterator state = dfsm->begin(); state != dfsm->end(); ++state, ++stateIndex)
		if (*state)
		{
			BasicBlock * bb = bbmap[stateIndex];

//			{
//				std::vector<Value*> arg;
//				arg.push_back(ConstantInt::get(Type::getInt32Ty(C), state->first + '0'));
//				CallInst::Create(fPutchar, arg.begin(), arg.end(), "", bbmap[state->first]);
//			}

			Value * pos = new LoadInst(posPtr, "pos", bb);
			pos = BinaryOperator::Create(Instruction::Add, pos, cst32_1, "pos", bb);

			new StoreInst(pos, posPtr, bb);
			if ((*state)->final)
				new StoreInst(pos, retPtr, bb);

			Value * cPtr = GetElementPtrInst::Create(str, pos, "charPtr", bb);
			Value * cVal = new LoadInst(cPtr, "char", bb);

			BasicBlock * defBB = endBB;
			DStateTransitions::const_iterator anyIt = (*state)->transitions.find(-1);
			if (anyIt != (*state)->transitions.end())
				defBB = bbmap[anyIt->second];

			int nbSw = (*state)->transitions.size();
			SwitchInst * sw = SwitchInst::Create(cVal, defBB, nbSw, bb);
			for (DStateTransitions::const_iterator tr = (*state)->transitions.begin(); tr != (*state)->transitions.end(); ++tr)
				if (tr->first != -1)
					sw->addCase(ConstantInt::get(Type::getInt8Ty(C), tr->first), bbmap[tr->second]);
			if (anyIt != (*state)->transitions.end())
				sw->addCase(cst8_0, endBB);
		}

	return func;
}
