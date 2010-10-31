
#include "LLVMREGen-c.h"

#include <stdio.h>

#if defined(_WIN)
  #include <windows.h>
#endif

int main()
{
	unsigned int i;
	for (i = 0; i < 15; ++i)
	{
		void * re = LLVMRE_createRE("a*ab?c?.(bb)?.b(cc)?.?", 0);

		printf("%i\n", LLVMREFunc_Execute(re, "acbbdef"));

		LLVMREFunc_Destruct(re);
	}

	LLVMRE_Destruction();

	#if defined(_WIN)
		system("pause");
	#endif

	return 0;
}
