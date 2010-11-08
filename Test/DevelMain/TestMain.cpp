#include <iostream>
#include <string>
#include "REParser.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/PassManager.h>
#include <llvm/Support/StandardPasses.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Target/TargetSelect.h>

#define TEST_DISPLAY_INTERMEDIATE

llvm::Function * CompileRE(llvm::Module * M, DFSM * dfsm, const std::string & fName);

int	main()
{
	std::string regexp("[[:alpha:][:digit:]_\\-\\.]+\\@[[:alpha:][:digit:]_\\-]+\\.[[:alpha:]]{2,5}");

	// Creating the AST
	REParser parse;
	INode * n = parse(regexp.begin(), regexp.end());

#ifdef TEST_DISPLAY_INTERMEDIATE
	std::cout << *n;
	std::cout << std::endl << "1-----------------------------------------------------------" << std::endl << std::endl;
#endif

	// Transforming AST into non determinist finite state machine
	StateHelper helper;
	new State(helper);
	n->stateify(helper.states[0], 0, true, helper);

#ifdef TEST_DISPLAY_INTERMEDIATE
	std::cout << helper;
	std::cout << std::endl << "2-----------------------------------------------------------" << std::endl << std::endl;
#endif

	// Deleting the AST
	delete n;

	// Determining the finite state machine
	DFSM * dfsm = new DFSM;
	determine(helper.states, *dfsm);

#ifdef TEST_DISPLAY_INTERMEDIATE
	std::cout << *dfsm;
	std::cout << std::endl << "3-----------------------------------------------------------" << std::endl << std::endl;
#endif

	// Deleting the non determinist finite state machine
	helper.clear();

	// Reducing the determinist finite state machine
	reduce(*dfsm);

#ifdef TEST_DISPLAY_INTERMEDIATE
	std::cout << *dfsm;
	std::cout << std::endl << "4-----------------------------------------------------------" << std::endl << std::endl;
#endif

	// Compiling the state machine into LLVM
	llvm::LLVMContext C;
	llvm::Module M("llvmre", C);
	llvm::Function * func = CompileRE(&M, dfsm, "llvmre_devel_test");

#ifdef TEST_DISPLAY_INTERMEDIATE
	llvm::outs() << M;
	std::cout << std::endl << "5-----------------------------------------------------------" << std::endl << std::endl;
#endif

	// Deleting the determinist finite state machine
	delete dfsm;

	// Optimising the LLVM Code
	llvm::FunctionPassManager fpm(&M);
	llvm::createStandardFunctionPasses(&fpm, 2);
	fpm.doInitialization();
	fpm.run(*func);

#ifdef TEST_DISPLAY_INTERMEDIATE
	llvm::outs() << M;
	std::cout << std::endl << "6-----------------------------------------------------------" << std::endl << std::endl;
#endif

	llvm::InitializeNativeTarget();
	llvm::ExecutionEngine * E  = llvm::EngineBuilder(&M).create();
	typedef int (*REFunc)(const char *, int *);
	union { void * obj; REFunc func; } u;
	u.obj = E->getPointerToFunction(func);
	REFunc jit = u.func;
	int ret = jit("salomon.brys@gmail.com", 0);
	std::cout << std::endl << ret << std::endl;
}
