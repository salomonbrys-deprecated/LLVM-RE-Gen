
#include <iostream>
#include <sstream>
#include <string>

#include <cstdlib>

#include "INode.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/PassManager.h>
#include <llvm/Support/StandardPasses.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include "CLLVMREGen.h"

//#define TEST_DISPLAY_INTERMEDIATE

llvm::Function * CompileRE(llvm::Module * M, DFSM * dfsm, const std::string & fName);

#include "LLVMREGen.h"

int	main()
{
//	CLLVMRE::CFunc * func = (CLLVMRE::CFunc*)CLLVMRE::Instance().createRE("a*ab?c?.(bb)?.b(cc)?.?");
//	std::cout << func->execute("acbbdef");

	std::string regexp("a*a");
	llvm::LLVMContext  nC;// = new llvm::LLVMContext;
	llvm::Module  nM/* = new llvm::Module*/("LLVMRegExp", nC);

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
	sstr << "llvmre_" << 1;
	llvm::Function * func = CompileRE(&nM, &dfsm, sstr.str());

	llvm::outs() << nM;

	// Deleting the determinist finite state machine
	dfsm.clearStates();

	// Optimising the LLVM Code
	if (0)
	{
		llvm::FunctionPassManager fpm(&nM);
		llvm::createStandardFunctionPasses(&fpm, 2);
		fpm.doInitialization();
		fpm.run(*func);
	}

//	CFunc * reFunc = new CFunc(func, regexp, defaultPolicy);
//	std::cout << reFunc->execute("acbbdef");

	llvm::InitializeNativeTarget();
	llvm::ExecutionEngine * E = llvm::ExecutionEngine::create(&nM);
	std::vector<llvm::GenericValue> args(1);
	llvm::GenericValue a((void*)"a");
	args[0] = a;
	llvm::GenericValue retgv = E->runFunction(func, args);
	llvm::outs() << retgv.IntVal;

}
