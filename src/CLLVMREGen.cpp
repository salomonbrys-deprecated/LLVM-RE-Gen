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

#include <stdlib.h>
#include <time.h>

#define LLVMREGEN_AUTO_JIT_COUNT 10000

llvm::Function * CompileRE(llvm::Module * M, DFSM * dfsm, const std::string & fName);

CLLVMRE::CFunc::~CFunc()
{
	if (jit && func)
		CLLVMRE::instance->JITEngine->freeMachineCodeForFunction(func);

	delete dfsm;
}

int CLLVMRE::CFunc::operator () (const char *str)
{
	return this->execute(str);
}

int CLLVMRE::CFunc::execute(const char *str)
{
	if (!jit)
	{
		if (policy == JIT_AUTO)
			++count;

		if (policy == JIT_ALWAYS || (policy == JIT_AUTO && count >= NB_EXEC_AUTO_JIT))
			JITFunc();
	}

	if (jit)
		return jit(str);

	return interpret(str);
}

int CLLVMRE::CFunc::operator () (const char *str) const
{
	return this->execute(str);
}

int CLLVMRE::CFunc::execute(const char *str) const
{
	if (jit)
		return jit(str);

	if (policy == JIT_AUTO && count < LLVMREGEN_AUTO_JIT_COUNT)
		++count;

	return interpret(str);
}

void CLLVMRE::CFunc::JITFunc(int optimizationLevel /*= 0*/, const std::string & name /* = "" */)
{
	if (jit)
		return ;

	if (!CLLVMRE::instance->JITEngine)
		CLLVMRE::instance->initializeJITEngine();

	if (!func)
		compileInLLVM(optimizationLevel, name);

	union { void * obj; CFuncPtr func; } u;
	u.obj = CLLVMRE::instance->JITEngine->getPointerToFunction(func);
	jit = u.func;
}

void CLLVMRE::CFunc::compileInLLVM(int optimizationLevel /*= 0*/, const std::string & name /* = "" */)
{
	if (!CLLVMRE::instance->M)
		CLLVMRE::instance->initilizeLLVM();

	std::string fName = name;
	if (fName.empty())
	{
		std::stringstream sstr;
		sstr << CLLVMRE::instance->defaultPrefix << '_' << ++nextFuncId;
		fName = sstr.str();
	}
	func = CompileRE(CLLVMRE::instance->M, dfsm, fName);

	// Optimising the LLVM Code
	if (optimizationLevel)
	{
		llvm::FunctionPassManager fpm(CLLVMRE::instance->M);
		llvm::createStandardFunctionPasses(&fpm, optimizationLevel);
		fpm.doInitialization();
		fpm.run(*func);
	}
}

const llvm::Function * CLLVMRE::CFunc::getLLVMFunction()
{
	if (!func)
		compileInLLVM();

	return func;
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

std::string CLLVMRE::CFunc::getRandomTryString() const
{
	static bool init = false;

	if (!init)
	{
		srand(time(NULL));
		init = true;
	}
	return _getRandomTryStringFrom(0);
}

LLVMRE::Func::Policy CLLVMRE::CFunc::getPolicy() const
{
	return policy;
}

void CLLVMRE::CFunc::setPolicy(LLVMRE::Func::Policy p)
{
	policy = p;
}

CLLVMRE::CFunc::CFunc(DFSM * dfsm, const std::string & regexp, Policy policy)
	: dfsm(dfsm), func(0), regexp(regexp), policy(policy), count(0), jit(0)
{
}

int CLLVMRE::CFunc::interpret(const char * str) const
{
	int pos = -1;
	int ret = 0;

	DState * state = (*dfsm)[0];

	for (;;)
	{
		++pos;

		if (state->final)
			ret = pos;

		if (str[pos] == '\0')
			break ;

		DStateTransitions::const_iterator tr = state->transitions.find(str[pos]);
		if (tr != state->transitions.end())
			state = (*dfsm)[tr->second];
		else
		{
			DStateTransitions::const_iterator any = state->transitions.find(-1);
			if (any != state->transitions.end())
				state = (*dfsm)[any->second];
			else
				break ;
		}
	}

	return ret;
}

std::string CLLVMRE::CFunc::_getRandomTryStringFrom(int stateId) const
{
	const DState * state = (*dfsm)[stateId];

	if (state->final && (state->transitions.empty() || (rand() % 6) == 0))
		return "";

	DStateTransitions::const_iterator item;

	item = state->transitions.find(-1);
	if (item == state->transitions.end() || (rand() % 3) != 0)
	{
		item = state->transitions.begin();
		std::advance(item, rand() % state->transitions.size());
	}

	std::string ret;

	if (item->first != -1)
		ret += (char)item->first;

	return ret + _getRandomTryStringFrom(item->second);
}






CLLVMRE::~CLLVMRE()
{
	if (JITEngine)
	{
		delete JITEngine;
		JITEngine = 0;
	}

	if (C)
		delete C;

	instance = 0;
}

LLVMRE::Func * CLLVMRE::createRE(const std::string & regexp)
{
	// Creating the AST
	INode * n = parse(regexp.begin(), regexp.end());

	// Transforming AST into non determinist finite state machine
	StateHelper helper;
	new State(helper);
	n->stateify(helper.states[0], 0, true, helper);

	// Deleting the AST
	delete n;

	// Determining the finite state machine
	DFSM * dfsm = new DFSM;
	determine(helper.states, *dfsm);

	// Deleting the non determinist finite state machine
	helper.clear();

	// Reducing the determinist finite state machine
	reduce(*dfsm);

	return new CFunc(dfsm, regexp, defaultPolicy);
}

void CLLVMRE::setLLVMModule(llvm::Module * m)
{
	assert(!M && "Cannot set a module if a module has already been set");

	M = m;
}

const llvm::Module * CLLVMRE::getLLVMModule() const
{
	return M;
}

void CLLVMRE::WriteBitcodeToFile(llvm::raw_ostream * os) const
{
	llvm::WriteBitcodeToFile(M, *os);
}

LLVMRE::Func::Policy CLLVMRE::getDefaultPolicy() const
{
	return defaultPolicy;
}

void CLLVMRE::setDefaultPolicy(LLVMRE::Func::Policy p)
{
	defaultPolicy = p;
}

const std::string & CLLVMRE::getDefaultPrefix()
{
	return defaultPrefix;
}

void CLLVMRE::setDefaultPrefix(const std::string & prefix)
{
	defaultPrefix = prefix;
}

void CLLVMRE::initilizeLLVM()
{
	if (!M && !C)
		C = new llvm::LLVMContext;

	if (!M)
		M = new llvm::Module("LLVMRegExp", *C);
}

void CLLVMRE::initializeJITEngine(int optimizationLevel /*= 0*/)
{
	llvm::CodeGenOpt::Level opt = llvm::CodeGenOpt::None;
	switch (optimizationLevel)
	{
	case 1:
		opt = llvm::CodeGenOpt::Less;
		break ;
	case 2:
		opt = llvm::CodeGenOpt::Default;
		break ;
	case 3:
		opt = llvm::CodeGenOpt::Aggressive;
		break ;
	}

	llvm::InitializeNativeTarget();

	if (!M)
		initilizeLLVM();

	JITEngine = llvm::EngineBuilder(M).setEngineKind(llvm::EngineKind::JIT).setOptLevel(opt).create();
}

CLLVMRE & CLLVMRE::Instance()
{
	if (!instance)
		instance = new CLLVMRE();
	return *instance;
}

CLLVMRE::CLLVMRE() : M(0), C(0), JITEngine(0), defaultPolicy(LLVMRE::Func::JIT_AUTO), defaultPrefix("llvmre")
{
}


extern "C"
{
	LLVMRE & LLVMRE_Instance()
	{
		return CLLVMRE::Instance();
	}
}


CLLVMRE * CLLVMRE::instance = 0;
int CLLVMRE::CFunc::nextFuncId = 0;

