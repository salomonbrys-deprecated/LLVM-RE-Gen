
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

class LLVMREFunc;

class LLVMRE
{
public:
	virtual ~LLVMRE() {};
	virtual LLVMREFunc & createRE(const std::string regexp, int optimizationLevel = 0) = 0;
	virtual void WriteBitcodeToFile(llvm::raw_ostream * os) const = 0;
};

class LLVMREFunc
{
public:
	virtual ~LLVMREFunc() {}
	virtual int operator () (const char *str) = 0;
	virtual void JITFunc() = 0;

	virtual llvm::Function * getLLVMFunction() = 0;
	typedef int (*REFunc)(const char *);
	virtual REFunc getREFunc() = 0;
	virtual std::string getName() const = 0;
};

extern "C"
{
	__LLVMRE_Dll LLVMRE & LLVMRE_Instance();
	__LLVMRE_Dll void LLVMREFunc_initializeJIT();
}

#endif /* LLVM_RE_GEN_H_ */
