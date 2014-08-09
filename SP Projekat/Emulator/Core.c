#include "Core.h"
#include <stdio.h>
#include "Utility.h"
#include "Instructions.h"

CPU cpu;
MEM memory;
UBYTE ir0, ir1;

char work = 1;
char prekid = 0;
unsigned char brPrekid = -1;

ADDR GetPA(VADDR vaddr)
{
	if (!GetFlag(VM))
		return vaddr;
	else
	{
		prekid = 1;
		return -1;
	}
}

void Emulate()
{
	printf("Emulation stared!\n");

	cpu.pc = LoadProgram("program.bin");

	if (cpu.pc == -1)
		return;

	char o, c, n, z;
	BYTE opCode;

	//cpu.r[0] = -32768;
	cpu.r[1] = -1;

	while (work)
	{
		char noPageFault = 1;

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
		case 4:
			cmp();
			break;
		case 5:
			and();
			break;
		case 6:
			or();
			break;
		case 7:
			not();
			break;
		case 8:
			test();
			break;
		case 9:
			noPageFault = ldr();
			if (!noPageFault)
			{
				prekid = 1;
				brPrekid = 1;
				goto PREKID;
			}
			break;
		case 10:
			noPageFault = str();
			if (!noPageFault)
			{
				prekid = 1;
				brPrekid = 1;
				goto PREKID;
			}
			break;
		}

		z = GetFlag(Z);
		n = GetFlag(N);
		c = GetFlag(C);
		o = GetFlag(O);

		printf("Flags N Z O C\n      %d %d %d %d\n", n, z, o, c);

	PREKID:
		if (prekid)
		{
			if (brPrekid == 1)
				cpu.pc -= 2;

			// save context

			cpu.pc = brPrekid << 1;
		}
	}
}


ADDR LoadProgram(char *fileName)
{
	FILE *file = NULL;
	//int error = fopen_s(&file, fileName, "rb");
#ifdef __gnu_linux__
	file = fopen(fileName, "rb");
#else
#ifdef _WIN32
	fopen_s(&file, fileName, "rb");
#endif
#endif



	ADDR entryAddr = 0;

	if (file == NULL)
	{
		printf("Error in opening file! Error code:\n");
		return -1;
	}

	fread(memory + entryAddr, sizeof(BYTE), 18, file);

	return entryAddr;
}
