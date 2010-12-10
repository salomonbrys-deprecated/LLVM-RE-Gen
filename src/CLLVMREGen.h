#ifndef C_LLVM_RE_GEN_H_
#define C_LLVM_RE_GEN_H_

#if defined(WIN32)
#define NOMINMAX
#endif

#include "LLVMREGen.h"
#include "State.h"
#include "REParser.h"

#include <string>
#include <queue>
#include <map>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/raw_ostream.h>

class CLLVMRE: public LLVMRE
{
public:
	class CFunc: public Func
	{
	public:
		virtual ~CFunc();

		virtual int operator ()(const char *str);
		virtual int execute(const char *str);
		virtual int operator ()(const char *str) const;
		virtual int execute(const char *str) const;

		virtual void JITFunc(int optimizationLevel = 0, const std::string & name = "");
		virtual void compileInLLVM(int optimizationLevel = 0, const std::string & name = "");

		virtual const llvm::Function * getLLVMFunction();
		virtual const llvm::Function * getLLVMFunction() const;

		virtual CFuncPtr getCFunc();
		virtual CFuncPtr getCFunc() const;

		virtual bool isJIT() const;

		virtual std::string getFuncName() const;
		virtual const std::string & getRegexp() const;

		virtual std::string getRandomTryString() const;

		virtual Policy getPolicy() const;
		virtual void setPolicy(Func::Policy);

	private:
		CFunc(DFSM * dfsm, const std::string & regexp, Policy policy);
		int interpret(const char * str) const;

		std::string _getRandomTryStringFrom(int stateId) const;

		DFSM * dfsm;
		llvm::Function * func;
		std::string regexp;

		Policy policy;
		mutable int count;
		CFuncPtr jit;

		static const int NB_EXEC_AUTO_JIT = 242;

		static int nextFuncId;

		friend class CLLVMRE;
	};

	virtual ~CLLVMRE();

	virtual Func * createRE(const std::string & regexp, bool stopAtFirstMatch = false);

	virtual void setLLVMModule(llvm::Module * m);
	virtual const llvm::Module * getLLVMModule() const;

	virtual void WriteBitcodeToFile(llvm::raw_ostream * os) const;

	virtual Func::Policy getDefaultPolicy() const;
	virtual void setDefaultPolicy(Func::Policy);
	virtual const std::string & getDefaultPrefix();
	virtual void setDefaultPrefix(const std::string &);

	virtual void initilizeLLVM();
	virtual void initializeJITEngine(int optimizationLevel = 2);

	static CLLVMRE & Instance();

private:
	CLLVMRE();

	llvm::Module * M;
	llvm::LLVMContext *C;

	llvm::ExecutionEngine * JITEngine;

	Func::Policy defaultPolicy;
	std::string defaultPrefix;

	REParser parse;

	static CLLVMRE *instance;
};

#endif /* C_LLVM_RE_GEN_H_ */
