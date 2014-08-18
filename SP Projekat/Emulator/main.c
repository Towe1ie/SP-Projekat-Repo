#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Core.h"

#include "ElfParser.h"
#include "ElfCreator.h"

Elf32_Ehdr* hdr;
FILE *file;

int main(int argc, char *argv[])
{
	if (argv[1] == NULL)
		return 1;

	if (strcmp(argv[1], "Emulate") == 0)
		Emulate();
	else if (strcmp(argv[1], "CreateElf") == 0)
		Create();
	else
		return 2;

	//file = fopen("ElfCreator/out.o", "rb");

	//elf_Load(file);

	//Create();

	//system("pause");
	
	return 0;
}