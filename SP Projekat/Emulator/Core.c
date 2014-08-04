#include "Core.h"
#include <stdio.h>
#include "Utility.h"

CPU cpu;
MEM memory;

char work = 1;

void Emulate()
{
	printf("Emulation stared!\n");

	cpu.pc = LoadProgram("program.bin");

	if (cpu.pc == -1)
		return;

	BYTE opCode;

	while (work)
	{
		 opCode = GetOPCode(memory[cpu.pc]);
		

		 cpu.pc += 2;
	}
}


ADDR LoadProgram(char *fileName)
{
	FILE *file = NULL;
	errno_t error = fopen_s(&file, fileName, "rb");

	ADDR entryAddr = 0;

	if (file == NULL)
	{
		printf("Error in opening file! Error code: %d\n", error);
		return -1;
	}

	fread(memory + entryAddr, sizeof(BYTE), 4, file);

	return entryAddr;
}