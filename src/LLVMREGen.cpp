
#include "LLVMREGen.h"
#include "LLVMREGen-c-llvm.h"
#include "CLLVMREGen.h"

extern "C"
{
	LLVMRE & LLVMRE_Instance()
	{
		return CLLVMRE::Instance();
	}

	void LLVMREFunc_initializeJIT()
	{
		CLLVMREFunc::initializeJIT();
	}



	void LLVMRE_Initialization()
	{
		CLLVMRE::Instance();
	}

	void LLVMRE_Destruct()
	{
		delete &CLLVMRE::Instance();
	}

	void * LLVMRE_createRE(const char * regexp, int optimizationLevel)
	{
		return (void*)&CLLVMRE::Instance().createRE(regexp, optimizationLevel);
	}

	void LLVMREFunc_Destruct(void * re)
	{
		delete (LLVMREFunc*)re;
	}

	int LLVMREFunc_Execute(void * re, const char * str)
	{
		return (*((LLVMREFunc*)re))(str);
	}

	LLVMRECFunc LLVMREFunc_getCFunc(void * re)
	{
		return ((LLVMREFunc*)re)->getREFunc();
	}

	LLVMValueRef LLVMREFunc_getLLVMFuncValue(void * re)
	{
		return wrap(((LLVMREFunc*)re)->getLLVMFunction());
	}
}
