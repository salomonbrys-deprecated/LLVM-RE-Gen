
#include "LLVMREGen.h"

#include <iostream>

#if defined(_WIN)
  #include <windows.h>
#endif

int main()
{
	LLVMRE_Instance(); // Explicit initialization

	LLVMRE::Func * func = LLVMRE_Instance().createRE("a*ab?c?.(bb)?.b(cc)?.?");

	for (unsigned int i = 0; i < 400; ++i)
		std::cout << func->execute("acbbdef") << ':' << (int)func->isJIT() << ' ';

	delete func;
	delete &LLVMRE_Instance();

	#if defined(_WIN)
		system("pause");
	#endif

	return 0;
}
