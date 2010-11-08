
#ifndef LLVM_RE_GEN_C_H_
#define LLVM_RE_GEN_C_H_

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

#if defined(_CXX)
	#define __LLVMRE_EXTERN_C_BEGIN extern "C" {
	#define __LLVMRE_EXTERN_C_END }
#else
	#define __LLVMRE_EXTERN_C_BEGIN
	#define __LLVMRE_EXTERN_C_END
#endif

__LLVMRE_EXTERN_C_BEGIN
	enum LLVMRE_Policy
	{
		JIT_ALWAYS,
		JIT_AUTO,
		JIT_NEVER
	};

	typedef int (*LLVMRECFuncPtr)(const char *);
	typedef void * LLVMREFunc;

	/* Explicit Initializations */
	__LLVMRE_Dll void	LLVMRE_Explicit_Initialization();
	__LLVMRE_Dll void	LLVMRE_initializeJITEngine(int optimizationLevel /*=0*/);

	/* LLVMRE Bindings */
	__LLVMRE_Dll void				LLVMRE_Destruction();
	__LLVMRE_Dll LLVMREFunc			LLVMRE_createRE(const char * regexp);
	__LLVMRE_Dll void				LLVMRE_WriteBitcodeToFile(int fd);
	__LLVMRE_Dll enum LLVMRE_Policy	LLVMRE_getDefaultPolicy();
	__LLVMRE_Dll void				LLVMRE_setDefaultPolicy(enum LLVMRE_Policy p);

	/* LLVMRE::Func Bindings */
	__LLVMRE_Dll void				LLVMREFunc_Destruct(LLVMREFunc re);
	__LLVMRE_Dll int				LLVMREFunc_Execute(LLVMREFunc re, const char * str);
	__LLVMRE_Dll void				LLVMREFunc_JITFunc(LLVMREFunc re, int optimizationLevel /*=0*/);
	__LLVMRE_Dll void				LLVMREFunc_compileInLLVM(LLVMREFunc re, int optimizationLevel /*=0*/);
	__LLVMRE_Dll LLVMRECFuncPtr		LLVMREFunc_getCFunc(LLVMREFunc re);
	__LLVMRE_Dll int				LLVMREFunc_isJIT(LLVMREFunc re);
	__LLVMRE_Dll const char *		LLVMREFunc_getFuncName(LLVMREFunc re);
	__LLVMRE_Dll const char *		LLVMREFunc_getRegexp(LLVMREFunc re);
	__LLVMRE_Dll enum LLVMRE_Policy	LLVMREFunc_getPolicy(LLVMREFunc re);
	__LLVMRE_Dll void				LLVMREFunc_setPolicy(LLVMREFunc re, enum LLVMRE_Policy p);

__LLVMRE_EXTERN_C_END

#endif /* LLVM_RE_GEN_C_H_ */
