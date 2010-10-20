/*
 * Compile.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: salomon
 */

#include "State.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>

#include <vector>
#include <string>
#include <sstream>

using namespace llvm;

void CompileRE(Module * M, LLVMContext & C, DFSM * dfsm, const std::string & fName)
{
//	Function * fPuts = cast<Function>(M->getOrInsertFunction("putchar", Type::getInt32Ty(C), Type::getInt32Ty(C), (Type *)0));

	Function * func = cast<Function>(M->getOrInsertFunction(fName, Type::getInt1Ty(C), Type::getInt8PtrTy(C), (Type *)0));
	Argument * str = func->arg_begin();
	str->setName("str");

	BasicBlock * entryBB = BasicBlock::Create(C, "entry", func);
	Value * posPtr = new AllocaInst(Type::getInt32Ty(C), "posPtr", entryBB);
	new StoreInst(ConstantInt::get(Type::getInt32Ty(C), 0), posPtr, entryBB);

	std::map<int, BasicBlock*> bbmap;

	for (DFSM::const_iterator state = dfsm->begin(); state != dfsm->end(); ++state)
	{
		std::stringstream stateName;
		stateName << "State_" << state->first;
		bbmap[state->first] = BasicBlock::Create(C, stateName.str(), func);
	}
	BranchInst::Create(bbmap[0], entryBB);

	BasicBlock * successBB = BasicBlock::Create(C, "success", func);
	ReturnInst::Create(C, ConstantInt::get(Type::getInt1Ty(C), 1), successBB);

	BasicBlock * failureBB = BasicBlock::Create(C, "failure", func);
	ReturnInst::Create(C, ConstantInt::get(Type::getInt1Ty(C), 0), failureBB);

	for (DFSM::const_iterator state = dfsm->begin(); state != dfsm->end(); ++state)
	{
//		{
//			std::vector<Value*> args;
//			args.push_back(ConstantInt::get(Type::getInt32Ty(C), state->first / 100 % 10 + '0'));
//			CallInst::Create(fPuts, args.begin(), args.end(), "", bbmap[state->first]);
//		}
//		{
//			std::vector<Value*> args;
//			args.push_back(ConstantInt::get(Type::getInt32Ty(C), state->first / 10 % 10 + '0'));
//			CallInst::Create(fPuts, args.begin(), args.end(), "", bbmap[state->first]);
//		}
//		{
//			std::vector<Value*> args;
//			args.push_back(ConstantInt::get(Type::getInt32Ty(C), state->first % 10 + '0'));
//			CallInst::Create(fPuts, args.begin(), args.end(), "", bbmap[state->first]);
//		}
//		{
//			std::vector<Value*> args;
//			args.push_back(ConstantInt::get(Type::getInt32Ty(C), '\n'));
//			CallInst::Create(fPuts, args.begin(), args.end(), "", bbmap[state->first]);
//		}

		Value * pos = new LoadInst(posPtr, "pos", bbmap[state->first]);
		Value * cPtr = GetElementPtrInst::Create(str, pos, "valPtr", bbmap[state->first]);
		Value * cVal = new LoadInst(cPtr, "val", bbmap[state->first]);
		Value * nPos = BinaryOperator::Create(Instruction::Add, pos, ConstantInt::get(Type::getInt32Ty(C), 1), "nPos", bbmap[state->first]);
		new StoreInst(nPos, posPtr, bbmap[state->first]);

		BasicBlock * defBB = failureBB;
		BasicBlock * prevDefBB = 0;
		if (state->second->final)
			defBB = successBB;
		int nbSw = state->second->transitions.size();
		if (state->second->transitions.find(-1) != state->second->transitions.end())
		{
			prevDefBB = defBB;
			defBB = bbmap[state->second->transitions.find(-1)->second];
		}

		SwitchInst * sw = SwitchInst::Create(cVal, defBB, nbSw, bbmap[state->first]);
		for (DStateTransitions::const_iterator tr = state->second->transitions.begin(); tr != state->second->transitions.end(); ++tr)
			sw->addCase(ConstantInt::get(Type::getInt8Ty(C), tr->first), bbmap[tr->second]);
		if (state->second->transitions.find(-1) != state->second->transitions.end())
			sw->addCase(ConstantInt::get(Type::getInt8Ty(C), 0), prevDefBB);
	}
}
