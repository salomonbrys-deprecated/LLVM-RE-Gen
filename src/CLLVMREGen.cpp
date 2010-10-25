//============================================================================
// Name        : LLVM-RE-Gen.cpp
// Author      : Salomon BRYS
// Copyright   : Salomon BRYS, Apache Lisence
// Description : Compile a regexp in LLVM
//============================================================================

#include "INode.h"

#include "CLLVMREGen.h"

#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/PassManager.h>
#include <llvm/Support/StandardPasses.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Target/TargetSelect.h>

#include <iostream>
#include <iomanip>

llvm::Function * CompileRE(llvm::Module * M, DFSM * dfsm, const std::string & fName);

CLLVMRE::~CLLVMRE()
{
	for (FuncMap::iterator it = funcMap.begin(); it != funcMap.end(); ++it)
		delete it->second;
	funcMap.clear();

	if (CLLVMREFunc::E)
	{
		delete CLLVMREFunc::E;
		CLLVMREFunc::E = 0;
		//llvm::llvm_shutdown();
	}
	delete C;
	instance = 0;
}

LLVMREFunc & CLLVMRE::createRE(const std::string regexp, int optimizationLevel /*= 0*/)
{
	if (funcMap.find(regexp) != funcMap.end())
		return *funcMap.find(regexp)->second;
		
	// Creating the AST
	INode * n = parseRegExp(regexp.begin(), regexp.end());

	// Transforming AST into non determinist finite state machine
	StateHelper helper;
	new State(helper);
	n->stateify(helper.map[0], 0, true, helper);

	// Deleting the AST
	delete n;

	// Determining the finite state machine
	DFSM dfsm;
	determine(helper.map, dfsm);

	// Deleting the non determinist finite state machine
	helper.clear();

	// Reducing the determinist finite state machine
	reduce(dfsm);

	// Compiling the state machine into LLVM
	llvm::Function * func = CompileRE(M, &dfsm, "re1");
	
	// Deleting the determinist finite state machine
	dfsm.clearStates();

	// Optimising the LLVM Code
	if (optimizationLevel)
	{
		llvm::FunctionPassManager fpm(M);
		llvm::createStandardFunctionPasses(&fpm, optimizationLevel);
		fpm.doInitialization();
		fpm.run(*func);
	}

	queue.push(func);

	CLLVMREFunc * reFunc = new CLLVMREFunc(func);
	funcMap[regexp] = reFunc;
	return *reFunc;
}

CLLVMRE & CLLVMRE::Instance()
{
	if (!instance)
		instance = new CLLVMRE();
	return *instance;
}

CLLVMRE::CLLVMRE()
{
	C = new llvm::LLVMContext;
	M = new llvm::Module("LLVMRegExp", *C);
}

int CLLVMREFunc::operator () (const char *str)
{
	if (!E)
		initializeJIT();
	JITFunc();
	return jit(str);
}

void CLLVMREFunc::JITFunc()
{
	if (!jit)
		jit = (REFunc)E->getPointerToFunction(func);
}

llvm::Function * CLLVMREFunc::getLLVMFunction()
{
	return func;
}

CLLVMREFunc::REFunc CLLVMREFunc::getREFunc()
{
	JITFunc();
	return jit;
}


void CLLVMREFunc::initializeJIT()
{
	llvm::InitializeNativeTarget();
	E = llvm::EngineBuilder(CLLVMRE::Instance().M).create();
}

CLLVMREFunc::CLLVMREFunc(llvm::Function * func) : func(func), jit(0)
{
}

CLLVMREFunc::~CLLVMREFunc()
{
	if (E)
		E->freeMachineCodeForFunction(func);
}

CLLVMRE * CLLVMRE::instance = 0;
llvm::ExecutionEngine * CLLVMREFunc::E = 0;

