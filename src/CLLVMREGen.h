
#ifndef C_LLVM_RE_GEN_H_
#define C_LLVM_RE_GEN_H_

#include "LLVMREGen.h"

#include <string>
#include <queue>
#include <map>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/raw_ostream.h>

class CLLVMRE : public LLVMRE
{
public:
	class CFunc : public Func
	{
	public:
		virtual ~CFunc();

		virtual int operator () (const char *str);
		virtual int execute(const char *str);
		virtual int operator () (const char *str) const;
		virtual int execute(const char *str) const;

		virtual void JITFunc();

		virtual const llvm::Function * getLLVMFunction() const;

		virtual CFuncPtr getCFunc();
		virtual CFuncPtr getCFunc() const;

		virtual bool isJIT() const;

		virtual std::string getFuncName() const;
		virtual const std::string & getRegexp() const;

		virtual Policy getPolicy() const;
		virtual void setPolicy(Func::Policy);

		static void initializeJITEngine();
		static void initializeInterpEngine();

	private:
		CFunc(llvm::Function * func, const std::string & regexp, Policy policy);
		int interpret(const char * str) const;

		llvm::Function * func;
		std::string regexp;

		Policy policy;
		mutable int count;
		CFuncPtr jit;

		static const int NB_EXEC_AUTO_JIT = 242;

		static llvm::ExecutionEngine * JITEngine;
		static llvm::ExecutionEngine * InterpEngine;

		friend class CLLVMRE;
	};


	virtual ~CLLVMRE();

	virtual Func * createRE(const std::string & regexp, int optimizationLevel = 0);

	virtual void WriteBitcodeToFile(llvm::raw_ostream * os) const;

	virtual Func::Policy getDefaultPolicy() const;
	virtual void setDefaultPolicy(Func::Policy);

	static CLLVMRE & Instance();

private:
	CLLVMRE();

	llvm::Module * M;
	llvm::LLVMContext *C;

	Func::Policy defaultPolicy;

	static CLLVMRE *instance;

	int nextFuncId;
};

#endif /* C_LLVM_RE_GEN_H_ */
