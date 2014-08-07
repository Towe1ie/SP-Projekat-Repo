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

#ifdef __gnu_linux__
	// sabiranje druga dva sabirka i provera za overflow i carry
	__asm__
	(
		"mov ax, %5\n"
		"add ax, %4\n"
		"mov %0, ax\n"
		"jno l1\n"
		"pushf\n"
		"mov al, %2\n"
		"inc al\n"
		"mov %2, al\n"
		"popf\n"
	"l1 : jnc l2\n"
		 "pushf\n"
		 "mov al, %1\n"
		 "inc al\n"
		 "mov %1, al\n"
		 "popf\n"
	 "l2:\n"
	 :"=m"(v2), "=m"(carry), "=m"(overflow)
	 :"m"(v1), "m"(imm), "m"(v2)
	 : "ax", "al"
	);

	// sabiranje destinacije i prethodnog zbira i provera za oveflow i carry
	__asm__
	(
		"mov ax, %3\n"
		"add ax, %4\n"
		"mov %0, ax\n"
		"jno l11\n"
		"pushf\n"
		"mov al, %2\n"
		"inc al\n"
		"mov %2, al\n"
		"popf\n"
	"l11: jnc l22\n"
		"pushf\n"
		 "mov al, %1\n"
		 "inc al\n"
		 "mov %1, al\n"
		"popf\n"
	"l22:\n"
	:"=m"(v1), "=m"(carry), "=m"(overflow)
	:"m"(v1), "m"(v2)
	:"ax", "al"
	);

#endif

	cpu.r[dst] = v1;

	// **** Setovanje flegova ****

	if (cpu.r[dst] == 0)
		SetFlag(Z);
	else
		ResetFlag(Z);

	if (cpu.r[dst] < 0)
		SetFlag(N);
	else
		ResetFlag(N);

	if (carry)
		SetFlag(C);
	else
		ResetFlag(C);

	if (overflow)
		SetFlag(O);
	else
		ResetFlag(O);


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

#ifdef __gnu_linux__

	__asm__
	(
		"mov ax, %5\n"
		"add ax, %4\n"
		"mov %0, ax\n"
		"jno l1s\n"
		"pushf\n"
		"mov al, %2\n"
		"inc al\n"
		"mov %2, al\n"
		"popf\n"
	"l1s : jnc l2s\n"
		 "pushf\n"
		 "mov al, %1\n"
		 "inc al\n"
		 "mov %1, al\n"
		 "popf\n"
	 "l2s:\n"
	 :"=m"(v2), "=m"(carry), "=m"(overflow)
	 :"m"(v1), "m"(imm), "m"(v2)
	 :"ax", "al"
	);

	// oduzimanje destinacije i prethodnog zbira i provera za oveflow i carry

	__asm__
	(
		"mov ax, %3\n"
		"sub ax, %4\n"
		"mov %0, ax\n"
		"jno l11s\n"
		"pushf\n"
		"mov al, %2\n"
		"inc al\n"
		"mov %2, al\n"
		"popf\n"
	"l11s: jnc l22s\n"
		"pushf\n"
		 "mov al, %1\n"
		 "inc al\n"
		 "mov %1, al\n"
		"popf\n"
	"l22s:\n"
	:"=m"(v1), "=m"(carry), "=m"(overflow)
	:"m"(v1), "m"(v2)
	:"ax", "al"
	);

#endif

	cpu.r[dst] = v1;

	// **** Setovanje flegova ****

	if (cpu.r[dst] == 0)
		SetFlag(Z);
	else
		ResetFlag(Z);

	if (cpu.r[dst] < 0)
		SetFlag(N);
	else
		ResetFlag(N);

	if (carry)
		SetFlag(C);
	else
		ResetFlag(C);

	if (overflow)
		SetFlag(O);
	else
		ResetFlag(O);
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
	else
		ResetFlag(Z);

	if (cpu.r[dst] < 0)
		SetFlag(N);
	else
		ResetFlag(N);

	if (disassembly)
		printf("DIV R[%d], R[%d], %d\n", dst, src, imm);
}