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

#include <sstream>
#include <iomanip>

llvm::Function * CompileRE(llvm::Module * M, DFSM * dfsm, const std::string & fName);

CLLVMRE::~CLLVMRE()
{
	for (FuncMap::iterator it = funcMap.begin(); it != funcMap.end(); ++it)
		delete it->second;

	if (CLLVMREFunc::E)
	{
		delete CLLVMREFunc::E;
		CLLVMREFunc::E = 0;
		//llvm::llvm_shutdown();
	}
	delete C;
	instance = 0;
}

LLVMREFunc & CLLVMRE::createRE(const std::string & regexp, int optimizationLevel /*= 0*/)
{
	if (funcMap.find(regexp) != funcMap.end())
		return *funcMap.find(regexp)->second;
		
	// Creating the AST
	INode * n = parseRegExp(regexp.begin(), regexp.end());

	// Transforming AST into non determinist finite state machine
	StateHelper helper;
	new State(helper);
	n->stateify(helper.states[0], 0, true, helper);

	// Deleting the AST
	delete n;

	// Determining the finite state machine
	DFSM dfsm;
	determine(helper.states, dfsm);

	// Deleting the non determinist finite state machine
	helper.clear();

	// Reducing the determinist finite state machine
	reduce(dfsm);

	// Compiling the state machine into LLVM
	std::stringstream sstr;
	sstr << "llvmre_" << ++nextFuncId;
	llvm::Function * func = CompileRE(M, &dfsm, sstr.str());
	
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

	CLLVMREFunc * reFunc = new CLLVMREFunc(func, regexp);
	funcMap[regexp] = reFunc;

	return *reFunc;
}

void CLLVMRE::WriteBitcodeToFile(llvm::raw_ostream * os) const
{
	llvm::WriteBitcodeToFile(M, *os);
}

CLLVMRE & CLLVMRE::Instance()
{
	if (!instance)
		instance = new CLLVMRE();
	return *instance;
}

CLLVMRE::CLLVMRE() : nextFuncId(0)
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
	{
		union { void * obj; REFunc func; } u;
		u.obj = E->getPointerToFunction(func);
		jit = u.func;
    }
}

llvm::Function * CLLVMREFunc::getLLVMFunction()
{
	return func;
}

CLLVMREFunc::REFunc CLLVMREFunc::getREFunc()
{
	if (!jit)
		JITFunc();
	return jit;
}

CLLVMREFunc::REFunc CLLVMREFunc::getREFunc() const
{
	if (!jit)
		return 0;
	return jit;
}

std::string CLLVMREFunc::getName() const
{
	return func->getNameStr();
}

const std::string & CLLVMREFunc::getRegexp() const
{
	return regexp;
}


void CLLVMREFunc::initializeJIT()
{
	llvm::InitializeNativeTarget();
	E = llvm::EngineBuilder(CLLVMRE::instance->M).create();
}

CLLVMREFunc::CLLVMREFunc(llvm::Function * func, const std::string & regexp) : func(func), regexp(regexp), jit(0)
{
}

CLLVMREFunc::~CLLVMREFunc()
{
	if (E)
		E->freeMachineCodeForFunction(func);
	CLLVMRE::FuncMap::iterator it = CLLVMRE::instance->funcMap.find(regexp);
	if (it != CLLVMRE::instance->funcMap.end())
		CLLVMRE::instance->funcMap.erase(it);
}

CLLVMRE * CLLVMRE::instance = 0;
llvm::ExecutionEngine * CLLVMREFunc::E = 0;

