#include <stdio.h>
#include <stdlib.h>

#include "Core.h"



int main()
{
	memory[0x7ed5] = -5;
	for (unsigned int i = 0; i < MEMCAP; i++)
		printf("0x%04X = %hd\n", i, memory[i]);

	Emulate();

	system("pause");
	


	return 0;
}