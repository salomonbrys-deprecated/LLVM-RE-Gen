
#ifndef C_LLVM_RE_GEN_H_
#define C_LLVM_RE_GEN_H_

#include "LLVMREGen.h"

#include <string>
#include <queue>
#include <map>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

typedef std::queue<llvm::Function*> FunctionQueue;

class CLLVMREFunc;

class CLLVMRE : public LLVMRE
{
public:
	virtual ~CLLVMRE();
	virtual LLVMREFunc & createRE(const std::string regexp, int optimizationLevel = 0);

	static CLLVMRE & Instance();

private:
	CLLVMRE();

	FunctionQueue queue;

	llvm::Module * M;
	llvm::LLVMContext *C;

	typedef std::map<std::string, CLLVMREFunc*> FuncMap;
	FuncMap funcMap;
	static CLLVMRE *instance;

	friend class CLLVMREFunc;
};

class CLLVMREFunc : public LLVMREFunc
{
public:
	virtual ~CLLVMREFunc();
	virtual int operator () (const char *str);
	virtual void JITFunc();

	virtual llvm::Function * getLLVMFunction();
	virtual REFunc getREFunc();

	static void initializeJIT();

private:
	CLLVMREFunc(llvm::Function * func);

	llvm::Function * func;

	REFunc jit;

	static llvm::ExecutionEngine * E;

	friend class CLLVMRE;
};

#endif /* C_LLVM_RE_GEN_H_ */
