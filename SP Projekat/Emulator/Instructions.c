#include "Instructions.h"
#include "Utility.h"
#include "Core.h"

#include <stdio.h>

unsigned char disassembly = 1;

void add()
{
	unsigned char carry = 0, overflow = 0;
	UBYTE dst = GetInfoFromByte(2, 0, ir1);
	UBYTE src = GetInfoFromByte(7, 5, ir0);
	WORD imm = ExtSgn(ir0, 5);

	WORD v1 = cpu.r[dst];
	WORD v2 = cpu.r[src];
	WORD res = 0;

	// sabiranje druga dva sabirka i provera za overflow i carry
	__asm
	{
		push ax
		mov ax, v2
		add ax, imm
		mov v2, ax
		pop ax
		jno l1
		pushf
		inc overflow
		popf
	l1 : jnc l2
		 pushf
		 inc carry
		 popf
	 l2 :
	}

	// sabiranje destinacije i prethodnog zbira i provera za oveflow i carry
	__asm
	{
		push ax
		mov ax, v1
		add ax, v2
		mov v1, ax
		pop ax
		jno l11
		pushf
		inc overflow
		popf
		
	l11: jnc l22
		pushf
		inc carry
		popf
	l22:
	}

	cpu.r[dst] = v1;

	// **** Setovanje flegova ****

	if (cpu.r[dst] == 0)
		SetFlag(Z);
	if (cpu.r[dst] < 0)
		SetFlag(N);
	if (carry)
		SetFlag(C);
	if (overflow)
		SetFlag(O);


	if (disassembly)
		printf("ADD R[%d], R[%d], %d\n", dst, src, imm);
}

void sub()
{
	unsigned char carry = 0, overflow = 0;
	UBYTE dst = GetInfoFromByte(2, 0, ir1);
	UBYTE src = GetInfoFromByte(7, 5, ir0);
	WORD imm = ExtSgn(ir0, 5);

	WORD v1 = cpu.r[dst];
	WORD v2 = cpu.r[src];
	WORD res = 0;

	
	// oduzimanje druga dva sabirka i provera za overflow i carry
	__asm
	{
		push ax
			mov ax, v2
			add ax, imm
			mov v2, ax
			pop ax
			jno l1
			pushf
			inc overflow
			popf

		l1 : jnc l2
			 pushf
			 inc carry
			 popf
		 l2 :
	}

	// oduzimanje destinacije i prethodnog zbira i provera za oveflow i carry
	__asm
	{
		push ax
		mov ax, v1 
		sub ax, v2
		mov v1, ax
		pop ax
		jno l11
		pushf
		inc overflow
		popf
		
	l11 : jnc l22 // provera za carry
		 pushf
		 inc carry
		 popf
	l22 :
	}

	cpu.r[dst] = v1;

	// **** Setovanje flegova ****

	if (cpu.r[dst] == 0)
		SetFlag(Z);
	if (cpu.r[dst] < 0)
		SetFlag(N);
	if (carry)
		SetFlag(C);
	if (overflow)
		SetFlag(O);

	if (disassembly)
		printf("SUB R[%d], R[%d], %d\n", dst, src, imm);
}

void mul()
{
	UBYTE dst = GetInfoFromByte(2, 0, ir1);
	UBYTE src = GetInfoFromByte(7, 5, ir0);
	WORD imm = ExtSgn(ir0, 5);

	cpu.r[dst] *= cpu.r[src] * imm;

	if (cpu.r[dst] == 0)
		SetFlag(Z);
	if (cpu.r[dst] < 0)
		SetFlag(N);

	if (disassembly)
		printf("MUL R[%d], R[%d], %d\n", dst, src, imm);
}

void div()
{
	UBYTE dst = GetInfoFromByte(2, 0, ir1);
	UBYTE src = GetInfoFromByte(7, 5, ir0);
	WORD imm = ExtSgn(ir0, 5);

	cpu.r[dst] /= cpu.r[src] / imm;

	if (cpu.r[dst] == 0)
		SetFlag(Z);
	if (cpu.r[dst] < 0)
		SetFlag(N);

	if (disassembly)
		printf("DIV R[%d], R[%d], %d\n", dst, src, imm);
}