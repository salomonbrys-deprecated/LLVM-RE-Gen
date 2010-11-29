
#include "LLVMREGen.h"

#include <iostream>

#if defined(_WIN)
  #include <windows.h>
#endif

int main()
{
	LLVMRE_Instance(); // Explicit initialization

	//LLVMRE::Func * func = LLVMRE_Instance().createRE("a*ab?c?.(bb)?.b(cc)?.?");
	LLVMRE::Func * func = LLVMRE_Instance().createRE("[[:alpha:]_][[:alnum:]_]*");

	for (unsigned int i = 0; i < 50; ++i)
	{
		std::string test = func->getRandomTryString();
		std::cout << test << " = " << func->execute(test.c_str()) << std::endl;
	}

	delete func;
	delete &LLVMRE_Instance();

	#if defined(_WIN)
		system("pause");
	#endif

	return 0;
}
