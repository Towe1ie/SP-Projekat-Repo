#include <stdio.h>
#include <stdlib.h>

#include "Core.h"

#include "ElfParser.h"

Elf32_Ehdr* hdr;
FILE *file;

int main()
{
	//Emulate();

	file = fopen("test.o", "rb");

	elf_Load(file);


	//system("pause");
	
	return 0;
}