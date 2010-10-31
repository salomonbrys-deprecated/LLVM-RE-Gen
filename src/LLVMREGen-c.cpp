
#include "LLVMREGen.h"
#include "LLVMREGen-c-llvm.h"
#include "CLLVMREGen.h"

extern "C"
{
	LLVMRE & LLVMRE_Instance()
	{
		return CLLVMRE::Instance();
	}

	void LLVMREFunc_initializeJITEngine()
	{
		CLLVMRE::CFunc::initializeJITEngine();
	}

	void LLVMREFunc_initializeInterpEngine()
	{
		CLLVMRE::CFunc::initializeInterpEngine();
	}





	void LLVMRE_Destruction()
	{
		delete &CLLVMRE::Instance();
	}

	void * LLVMRE_createRE(const char * regexp, int optimizationLevel)
	{
		return (void*)CLLVMRE::Instance().createRE(regexp, optimizationLevel);
	}

	void LLVMRE_WriteBitcodeToFile(int fd)
	{
		llvm::raw_fd_ostream rawfd(fd, false);
		CLLVMRE::Instance().WriteBitcodeToFile(&rawfd);
	}

	enum LLVMRE_Policy LLVMRE_getDefaultPolicy()
	{
		return (LLVMRE_Policy)CLLVMRE::Instance().getDefaultPolicy();
	}

	void LLVMRE_setDefaultPolicy(enum LLVMRE_Policy p)
	{
		CLLVMRE::Instance().setDefaultPolicy((LLVMRE::Func::Policy)p);
	}

	void LLVMREFunc_Destruct(void * re)
	{
		delete (CLLVMRE::CFunc*)re;
	}

	int LLVMREFunc_Execute(void * re, const char * str)
	{
		return ((CLLVMRE::CFunc*)re)->execute(str);
	}

	LLVMRECFuncPtr LLVMREFunc_getCFunc(void * re)
	{
		return ((CLLVMRE::CFunc*)re)->getCFunc();
	}

	LLVMValueRef LLVMREFunc_getLLVMFuncValue(void * re)
	{
		return wrap(((CLLVMRE::CFunc*)re)->getLLVMFunction());
	}
}
