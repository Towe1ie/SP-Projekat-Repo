#include "Core.h"
#include <stdio.h>
#include "Utility.h"
#include "Instructions.h"
#include "ElfParser.h"

CPU cpu;
MEM memory;
MEM io;
UBYTE ir0, ir1;

char work = 1;
char prekid = 0;
unsigned char brPrekid = -1;
VADDR entryPoint;

BYTE* mem = memory;

// **** Test ****
char useVM = 0;

void fillPmt()
{
	cpu.pmtp = 256;
	Descriptor *PMT = (Descriptor*)(memory + cpu.pmtp);

	Descriptor d; //za ovo ubaciti primer sa VM
	
	// code
	SetDescriptorFlag(E, &d, ONE);
	SetDescriptorFlag(V, &d, ONE);
	SetDescriptorFlag(R, &d, ONE);
	SetDescriptorFlag(W, &d, ZERO);
	SetDescriptorFlag(D, &d, ZERO);

	d.block = 5;
	PMT[1] = d;

	// 0th block
	SetDescriptorFlag(R, &d, ONE);
	d.block = 0;
	PMT[0] = d;

	// pg.2
	d.block = 89;
	SetDescriptorFlag(W, &d, ONE);
	SetDescriptorFlag(E, &d, ZERO);
	PMT[2] = d;

	// stack
	SetDescriptorFlag(E, &d, ZERO);
	SetDescriptorFlag(R, &d, ONE);
	SetDescriptorFlag(W, &d, ONE);
	d.block = 17;
	PMT[253] = d;

	d.block = 102;
	PMT[254] = d;

	d.block = 44;
	PMT[255] = d;

	SetFlag(VM);
}
// **************


ADDR GetPA(VADDR vaddr, char *status, MEM_OP memOp)
{
	if (!GetFlag(VM))
	{
		*status = 1;
		return vaddr;
	}
	else
	{
		Descriptor* pmt = (Descriptor*)(memory + cpu.pmtp);
		Descriptor descriptor = pmt[GetHigherByte(vaddr)];
		if (GetDescriptorFlag(V, descriptor) == ZERO)
		{
			*status = 0;
			prekid = 1;
			brPrekid = 1;

			return 0;
		}
		else if (memOp != LOAD && GetDescriptorFlag(memOp, descriptor) == ZERO)
		{
			*status = 2;
			prekid = 1;
			brPrekid = 2;

			return 0;
		}
		
		*status = 1;

		return (descriptor.block << 8 | GetLowerByte(vaddr));
	}
}



void init(char* fileName)
{
	if (useVM)
		fillPmt();

	printf("Emulation stared!\n");

	cpu.pc = LoadProgram(fileName);

	cpu.sp = MEMCAP - 256*3 + 1;


}

void Emulate(char* fileName)
{
	init(fileName);

	if (cpu.pc == -1)
		return;

	char o, c, n, z;
	BYTE opCode;

	while (work)
	{
		char status;


		ir1 = ReadByteInstr(cpu.pc, &status);
		CHECK_INTERRUPTS;
		ir0 = ReadByteInstr(cpu.pc + 1, &status);
		CHECK_INTERRUPTS;

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
		case 25:
			_movtosfr();
			break;
		case 26:
			_movfromsfr();
			break;
		case 27:
			_mov();
			break;
		case 28:
			_in();
			break;
		case 29:
			_out();
			break;
		case 30:
			_shr();
			break;
		case 31:
			_shl();
			break;
			
		}
		
		CHECK_INTERRUPTS;

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

			switch(brPrekid)
			{
			case 0:
				printf("Reset Interrupt\n");
				break;
			case 1:
				printf("PAGE FAULT!\n");
				break;
			case 2:
				printf("Premission denied!\n");
				break;
			}

			// save context
			WriteWord(cpu.sp, cpu.pc, &status);
			CHECK_INTERRUPTS;
			WriteWord(cpu.sp, cpu.psw, &status);
			CHECK_INTERRUPTS;

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

	elf_Load(file);
	char status;
	entryAddr = ReadWord(0, &status);

	//fread(memory + entryAddr, sizeof(BYTE), 32786, file);

	return entryAddr;
}
