
#ifndef LLVM_RE_GEN_H_
#define LLVM_RE_GEN_H_

#include <string>

#if !defined(__LLVMRE_Dll)
	#if defined(_WIN) && (defined(_WINDLL) || defined(LLVMRE_USE_WINDLL))
		#if defined(_WINDLL)
			#define __LLVMRE_Dll __declspec( dllexport )
		#else
			#define __LLVMRE_Dll __declspec( dllimport )
		#endif
	#else
		#define __LLVMRE_Dll
	#endif
#endif

#if defined(_WIN)
#include <windows.h>
#define int32_t INT32
#else
#include <sys/types.h>
#endif

namespace llvm
{
	class Function;
	class Module;
	class raw_ostream;
}

class LLVMRE
{
public:

	class Func
	{
	public:
		virtual ~Func() {}

		virtual int operator () (const char *str) = 0;
		virtual int execute(const char *str) = 0;
		virtual int operator () (const char *str) const = 0;
		virtual int execute(const char *str) const = 0;

		virtual void JITFunc(int optimizationLevel = 0, const std::string & name = "") = 0;
		virtual void compileInLLVM(int optimizationLevel = 0, const std::string & name = "") = 0;

		virtual const llvm::Function * getLLVMFunction() = 0;
		virtual const llvm::Function * getLLVMFunction() const = 0;

		typedef int32_t (*CFuncPtr)(const char *);

		virtual CFuncPtr getCFunc() = 0;
		virtual CFuncPtr getCFunc() const = 0;

		virtual bool isJIT() const = 0;

		virtual std::string getFuncName() const = 0;
		virtual const std::string & getRegexp() const = 0;

		virtual std::string getRandomTryString() const = 0;

		enum Policy
		{
			JIT_ALWAYS,
			JIT_AUTO,
			JIT_NEVER
		};

		virtual Policy getPolicy() const = 0;
		virtual void setPolicy(Func::Policy) = 0;
	};

	virtual ~LLVMRE() {}

	virtual Func * createRE(const std::string & regexp, bool stopAtFirstMatch = false) = 0;

	virtual void initilizeLLVM() = 0;
	virtual void initializeJITEngine(int optimizationLevel = 0) = 0;

	virtual void setLLVMModule(llvm::Module * m) = 0;
	virtual const llvm::Module * getLLVMModule() const = 0;

	virtual void WriteBitcodeToFile(llvm::raw_ostream * os) const = 0;

	virtual Func::Policy getDefaultPolicy() const = 0;
	virtual void setDefaultPolicy(Func::Policy) = 0;
	virtual const std::string & getDefaultPrefix() = 0;
	virtual void setDefaultPrefix(const std::string &) = 0;
};

extern "C"
{
	__LLVMRE_Dll LLVMRE & LLVMRE_Instance();
}

#endif /* LLVM_RE_GEN_H_ */
