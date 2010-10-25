
#include "LLVMREGen.h"

#include <iostream>

#if defined(_WIN)
  #include <windows.h>
#endif

int main()
{
	for (unsigned int i = 0; i < 15; ++i)
	{
		LLVMREFunc & func = LLVMRE_Instance().createRE("a*ab?c?.(bb)?.b(cc)?.?", 0);

		std::cout << func("acbbdef") << std::endl;

		delete &LLVMRE_Instance();
	}
	#if defined(_WIN)
		system("pause");
	#endif

	return 0;
}
