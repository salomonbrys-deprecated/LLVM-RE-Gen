
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
	__LLVMRE_Dll void LLVMRE_Initialization();
	__LLVMRE_Dll void LLVMRE_Destruct();
	__LLVMRE_Dll void * LLVMRE_createRE(const char * regexp, int optimizationLevel);

	__LLVMRE_Dll void LLVMREFunc_initializeJIT();
	__LLVMRE_Dll void LLVMREFunc_Destruct(void * re);
	__LLVMRE_Dll int LLVMREFunc_Execute(void * re, const char * str);
	typedef int (*LLVMRECFunc)(const char *);
	__LLVMRE_Dll LLVMRECFunc LLVMREFunc_getCFunc(void * re);
__LLVMRE_EXTERN_C_END

#endif /* LLVM_RE_GEN_C_H_ */
