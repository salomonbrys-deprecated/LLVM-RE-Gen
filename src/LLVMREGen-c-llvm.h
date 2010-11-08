
#ifndef LLVM_RE_GEN_C_LLVM_H_
#define LLVM_RE_GEN_C_LLVM_H_

#include "LLVMREGen-c.h"

#include <llvm-c/Core.h>

extern "C"
{
	__LLVMRE_Dll LLVMValueRef LLVMREFunc_getLLVMFuncValue(void * re);
}

#endif /* LLVM_RE_GEN_C_LLVM_H_ */
