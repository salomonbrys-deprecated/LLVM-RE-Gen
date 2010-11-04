
#include "LLVMREGen.h"
#include "LLVMREGen-c-llvm.h"
#include "CLLVMREGen.h"

extern "C"
{
	LLVMRE & LLVMRE_Instance()
	{
		return CLLVMRE::Instance();
	}

	void LLVMRE_initializeJITEngine(int optimizationLevel)
	{
		CLLVMRE::Instance().initializeJITEngine(optimizationLevel);
	}

	void	LLVMRE_Explicit_Initialization()
	{
		CLLVMRE::Instance();
	}

	void LLVMRE_Destruction()
	{
		delete &CLLVMRE::Instance();
	}

	void * LLVMRE_createRE(const char * regexp)
	{
		return (void*)CLLVMRE::Instance().createRE(regexp);
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

	void LLVMREFunc_Destruct(LLVMREFunc re)
	{
		delete (CLLVMRE::CFunc*)re;
	}

	int LLVMREFunc_Execute(LLVMREFunc re, const char * str)
	{
		return ((CLLVMRE::CFunc*)re)->execute(str);
	}

	void LLVMREFunc_JITFunc(LLVMREFunc re, int optimizationLevel /*=0*/)
	{
		((CLLVMRE::CFunc*)re)->JITFunc(optimizationLevel);
	}

	void LLVMREFunc_compileInLLVM(LLVMREFunc re, int optimizationLevel /*=0*/)
	{
		((CLLVMRE::CFunc*)re)->compileInLLVM(optimizationLevel);
	}

	LLVMRECFuncPtr LLVMREFunc_getCFunc(LLVMREFunc re)
	{
		return ((CLLVMRE::CFunc*)re)->getCFunc();
	}

	int LLVMREFunc_isJIT(LLVMREFunc re)
	{
		return ((CLLVMRE::CFunc*)re)->isJIT();
	}

	const char * LLVMREFunc_getFuncName(LLVMREFunc re)
	{
		return ((CLLVMRE::CFunc*)re)->getFuncName().c_str();
	}

	const char * LLVMREFunc_getRegexp(LLVMREFunc re)
	{
		return ((CLLVMRE::CFunc*)re)->getRegexp().c_str();
	}

	enum LLVMRE_Policy	LLVMREFunc_getPolicy(LLVMREFunc re)
	{
		return (LLVMRE_Policy)((CLLVMRE::CFunc*)re)->getPolicy();
	}

	void LLVMREFunc_setPolicy(LLVMREFunc re, enum LLVMRE_Policy p)
	{
		((CLLVMRE::CFunc*)re)->setPolicy((LLVMRE::Func::Policy)p);
	}





	LLVMValueRef LLVMREFunc_getLLVMFuncValue(void * re)
	{
		return wrap(((CLLVMRE::CFunc*)re)->getLLVMFunction());
	}
}
