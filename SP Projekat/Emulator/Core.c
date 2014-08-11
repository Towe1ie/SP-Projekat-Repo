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

BYTE* mem = memory;

ADDR GetPA(VADDR vaddr)
{
	if (!GetFlag(VM))
		return vaddr;
	else
	{
		prekid = 1;
		brPrekid = 1; //page fault
		return 0;
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

	cpu.sp = 0x100;

	//cpu.r[0] = -32768;
	//cpu.r[1] = -1;

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
			_add();
			break;
		case 1:
			_sub();
			break;
		case 2:
			_mul();
			break;
		case 3:
			_div();
			break;
		case 4:
			_cmp();
			break;
		case 5:
			_and();
			break;
		case 6:
			_or();
			break;
		case 7:
			_not();
			break;
		case 8:
			_test();
			break;
		case 9:
			_ldr();
			break;
		case 10:
			_str();
			break;
		case 11:
			_je();
			break;
		case 12:
			_jne();
			break;
		case 13:
			_jge();
			break;
		case 14:
			_jg();
			break;
		case 15:
			_jle();
			break;
		case 16:
			_jl();
			break;
		case 17:
			_jp();
			break;
		case 18:
			_jn();
			break;
		case 19:
			_jo();
			break;
		case 20:
			_jno();
			break;
		case 21:
			_call();
			break;
		case 22:
			_rij();
			break;
		case 23:
			_push();
			break;
		case 24:
			_pop();
			break;
		}
		
		CHECK_PAGE_FAULT;

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

			prekid = 0;
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

	fread(memory + entryAddr, sizeof(BYTE), 104, file);

	return entryAddr;
}
