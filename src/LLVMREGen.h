
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

		virtual void JITFunc() = 0;

		virtual const llvm::Function * getLLVMFunction() const = 0;

		typedef int (*CFuncPtr)(const char *);

		virtual CFuncPtr getCFunc() = 0;
		virtual CFuncPtr getCFunc() const = 0;

		virtual bool isJIT() const = 0;

		virtual std::string getFuncName() const = 0;
		virtual const std::string & getRegexp() const = 0;

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

	virtual Func * createRE(const std::string & regexp, int optimizationLevel = 0) = 0;

	virtual void WriteBitcodeToFile(llvm::raw_ostream * os) const = 0;

	virtual Func::Policy getDefaultPolicy() const = 0;
	virtual void setDefaultPolicy(Func::Policy) = 0;
};

extern "C"
{
	__LLVMRE_Dll LLVMRE & LLVMRE_Instance();
	__LLVMRE_Dll void LLVMREFunc_initializeJITEngine();
	__LLVMRE_Dll void LLVMREFunc_initializeInterpEngine();
}

#endif /* LLVM_RE_GEN_H_ */
