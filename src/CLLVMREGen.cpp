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
#include <iostream>
#include <iomanip>

llvm::Function * CompileRE(llvm::Module * M, DFSM * dfsm, const std::string & fName);

CLLVMRE::CFunc::~CFunc()
{
	if (jit)
		JITEngine->freeMachineCodeForFunction(func);
	delete func;
}

int CLLVMRE::CFunc::operator () (const char *str)
{
	return this->execute(str);
}

int CLLVMRE::CFunc::execute(const char *str)
{
	++count;

	if (!jit && (policy == JIT_ALWAYS || (policy == JIT_AUTO && count >= NB_EXEC_AUTO_JIT)))
		JITFunc();

	if (jit)
		return jit(str);

	if (!InterpEngine)
		initializeInterpEngine();

	return interpret(str);
}

int CLLVMRE::CFunc::operator () (const char *str) const
{
	return this->execute(str);
}

int CLLVMRE::CFunc::execute(const char *str) const
{
	++count;

	if (jit)
		return jit(str);
	else if (!InterpEngine)
		return -1;

	return interpret(str);
}

void CLLVMRE::CFunc::JITFunc()
{
	if (!JITEngine)
		initializeJITEngine();

		union { void * obj; CFuncPtr func; } u;
		u.obj = JITEngine->getPointerToFunction(func);
		jit = u.func;
}

const llvm::Function * CLLVMRE::CFunc::getLLVMFunction() const
{
	return func;
}

LLVMRE::Func::CFuncPtr CLLVMRE::CFunc::getCFunc()
{
	if (!jit)
		JITFunc();
	return jit;
}

LLVMRE::Func::CFuncPtr CLLVMRE::CFunc::getCFunc() const
{
	return jit;
}

bool CLLVMRE::CFunc::isJIT() const
{
	return jit != 0;
}

std::string CLLVMRE::CFunc::getFuncName() const
{
	return func->getNameStr();
}

const std::string & CLLVMRE::CFunc::getRegexp() const
{
	return regexp;
}

LLVMRE::Func::Policy CLLVMRE::CFunc::getPolicy() const
{
	return policy;
}

void CLLVMRE::CFunc::setPolicy(LLVMRE::Func::Policy p)
{
	policy = p;
}

void CLLVMRE::CFunc::initializeJITEngine()
{
	llvm::InitializeNativeTarget();
	JITEngine = llvm::ExecutionEngine::create(CLLVMRE::instance->M);
}

void CLLVMRE::CFunc::initializeInterpEngine()
{
	InterpEngine = llvm::ExecutionEngine::create(CLLVMRE::instance->M, true);
}

CLLVMRE::CFunc::CFunc(llvm::Function * func, const std::string & regexp, Policy policy)
	: func(func), regexp(regexp), policy(policy), count(0), jit(0)
{
}

int CLLVMRE::CFunc::interpret(const char * str) const
{
	std::vector<llvm::GenericValue> args(1);
	llvm::GenericValue a;
	a.PointerVal = (void*)str;
	args[0] = a;
	llvm::GenericValue retgv = InterpEngine->runFunction(func, args);
	return *retgv.IntVal.getRawData();
}





CLLVMRE::~CLLVMRE()
{
	if (CFunc::JITEngine)
	{
		delete CFunc::JITEngine;
		CFunc::JITEngine = 0;
	}
	if (CFunc::InterpEngine)
	{
		delete CFunc::InterpEngine;
		CFunc::InterpEngine = 0;
	}
	delete C;
	instance = 0;
}

LLVMRE::Func * CLLVMRE::createRE(const std::string & regexp, int optimizationLevel /*= 0*/)
{
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

	CFunc * reFunc = new CFunc(func, regexp, defaultPolicy);

	return reFunc;
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

LLVMRE::Func::Policy CLLVMRE::getDefaultPolicy() const
{
	return defaultPolicy;
}

void CLLVMRE::setDefaultPolicy(LLVMRE::Func::Policy p)
{
	defaultPolicy = p;
}

CLLVMRE::CLLVMRE() : defaultPolicy(LLVMRE::Func::JIT_AUTO), nextFuncId(0)
{
	C = new llvm::LLVMContext;
	M = new llvm::Module("LLVMRegExp", *C);
}





CLLVMRE * CLLVMRE::instance = 0;
llvm::ExecutionEngine * CLLVMRE::CFunc::JITEngine = 0;
llvm::ExecutionEngine * CLLVMRE::CFunc::InterpEngine = 0;

