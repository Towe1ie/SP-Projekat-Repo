#include "Core.h"
#include <stdio.h>
#include "Utility.h"
#include "Instructions.h"

CPU cpu;
MEM memory;
UBYTE ir0, ir1;

char work = 1;

void Emulate()
{
	printf("Emulation stared!\n");

	cpu.pc = LoadProgram("program.bin");

	if (cpu.pc == -1)
		return;

	BYTE opCode;

	//cpu.r[0] = -32768;
	cpu.r[1] = -1;

	while (work)
	{
		ir1 = memory[cpu.pc];
		ir0 = memory[cpu.pc + 1];
		cpu.pc += 2;

		opCode = GetInfoFromByte(7, 3, ir1);

		switch (opCode)
		{
		case 0:
			add();
			break;
		case 1:
			sub();
			break;
		case 2:
			mul();
			break;
		case 3:
			div();
			break;
		}
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

	fread(memory + entryAddr, sizeof(BYTE), 12, file);

	return entryAddr;
}