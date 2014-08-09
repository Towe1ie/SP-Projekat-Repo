#include "Instructions.h"
#include "Utility.h"
#include "Core.h"

#include <stdio.h>

#define UPDATE_ALL_FLAGS 	\
if (z)\
	SetFlag(Z);\
else\
	ResetFlag(Z);\
if (n)\
	SetFlag(N);\
else\
	ResetFlag(N);\
if (c)\
	SetFlag(C);\
else\
	ResetFlag(C);\
if (o)\
	SetFlag(O);\
else\
	ResetFlag(O);\

unsigned char disassembly = 1;
WORD dst, src, imm, v1, v2; // ovo sam imao u skoro svakoj instrukciji, pa da ne bih stalno alocirao prostor na steku
unsigned char c, o, n, z;

int p = 0;

void add()
{
	c = 0, o = 0, n = 0, z = 0;
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);


	v1 = cpu.r[dst];
	v2 = cpu.r[src];

#ifdef _WIN32
	// sabiranje druga dva sabirka i provera za overflow i carry
	__asm
	{
		push ax;
		mov ax, v2;
		add ax, imm;
		mov v2, ax;
		pop ax;
		jno l1;
		mov o, 1;
	l1: jnc l2;
		mov c, 1
		l2:;
	}

	// sabiranje destinacije i prethodnog zbira i provera za oveflow i carry
	__asm
	{
		push ax;
		mov ax, v1;
		add ax, v2;
		mov v1, ax
		pop ax;
		jno l11;
		mov o, 1;
	l11: jnc l22;
		mov c, 1;
	l22: jns l33;
		mov n, 1;
	l33: jnz l44;
		mov z, 1;
	l44:;
	}

#else
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
		: "m"(v1), "m"(imm), "m"(v2)
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
		: "m"(v1), "m"(v2)
		: "ax", "al"
		);

#endif
#endif

	cpu.r[dst] = v1;

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("ADD R[%d], R[%d], %d\n", dst, src, imm);


}

void sub()
{
	c = 0, n = 0, z = 0, o = 0;
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);

	v1 = cpu.r[dst];
	v2 = cpu.r[src];

#ifdef _WIN32
	// oduzimanje druga dva sabirka i provera za overflow i carry

	__asm
	{
		push ax;
		mov ax, v2;
		add ax, imm;
		mov v2, ax;
		pop ax;
		jno l1;
		mov o, 1;
	l1: jnc l2;
		mov c, 1
		l2:;
	}

	// oduzimanje destinacije i prethodnog zbira i provera za oveflow i carry

	__asm
	{
		push ax;
		mov ax, v1;
		sub ax, v2;
		mov v1, ax;
		pop ax;
		jno l11;
		mov o, 1;
	l11: jnc l22;
		mov c, 1;
	l22: jns l33;
		mov n, 1;
	l33: jnz l44;
		mov z, 1;
	l44:;
	}

#else
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
		: "m"(v1), "m"(imm), "m"(v2)
		: "ax", "al"
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
		: "m"(v1), "m"(v2)
		: "ax", "al"
		);

#endif
#endif

	cpu.r[dst] = v1;

	// **** Setovanje flegova ****

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("SUB R[%d], R[%d], %d\n", dst, src, imm);
}

void mul()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);

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
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);

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

void cmp()
{
	c = 0, o = 0, n = 0, z = 0;
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);

	v1 = cpu.r[dst];
	v2 = cpu.r[src];

#ifdef _WIN32
	__asm
	{
		push ax;
		mov ax, imm;
		add ax, v2;
		cmp v1, ax;
		pop ax;
		jno l1;
		mov o, 1;
	l1: jnc l2;
		mov c, 1;
	l2: jns l3;
		mov n, 1;
	l3: jnz l4;
		mov z, 1;
	l4:;
	}
#endif // _WIN32

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("CMP R[%d], R[%d], %d\n", dst, src, imm);
}

void and()
{
	c = 0, n = 0, z = 0, o = 0;
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	v1 = cpu.r[dst];
	v2 = cpu.r[src];

#ifdef _WIN32
	__asm
	{
		push ax;
		mov ax, v1;
		and ax, v2;
		mov v1, ax;
		pop ax;
	jno l1;
		mov o, 1;
	l1: jnc l2;
		mov c, 1;
	l2: jns l3;
		mov n, 1;
	l3: jnz l4;
		mov z, 1;
	l4:;
	}
#endif

	cpu.r[dst] = v1;

	UPDATE_ALL_FLAGS;
	
	if (disassembly)
		printf("AND R[%d], R[%d]\n", dst, src);
}

void or()
{
	c = 0, n = 0, z = 0, o = 0;
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	v1 = cpu.r[dst];
	v2 = cpu.r[src];

#ifdef _WIN32
	__asm
	{
		push ax;
		mov ax, v1;
		or ax, v2;
		mov v1, ax;
		pop ax;
		jno l1;
		mov o, 1;
	l1: jnc l2;
		mov c, 1;
	l2: jns l3;
		mov n, 1;
	l3: jnz l4;
		mov z, 1;
	l4:;
	}
#endif

	cpu.r[dst] = v1;

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("OR R[%d], R[%d]\n", dst, src);
}

void not()
{
	c = 0, n = 0, z = 0, o = 0;
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	v1 = cpu.r[src];

#ifdef _WIN32
	__asm
	{
		push ax;
		mov ax, v1;
		not ax;
		mov v1, ax;
		pop ax;
		jno l1;
		mov o, 1;
	l1: jnc l2;
		mov c, 1;
	l2: jns l3;
		mov n, 1;
	l3: jnz l4;
		mov z, 1;
	l4:;
	}
#endif

	cpu.r[dst] = v1;

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("NOT R[%d], R[%d]\n", dst, src);
}

void test()
{
	c = 0, n = 0, z = 0, o = 0;
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	v1 = cpu.r[dst];
	v2 = cpu.r[src];

#ifdef _WIN32
	__asm
	{
		push ax;
		mov ax, v1;
		and ax, v2;
		pop ax;
		jno l1;
		mov o, 1;
	l1: jnc l2;
		mov c, 1;
	l2: jns l3;
		mov n, 1;
	l3: jnz l4;
		mov z, 1;
	l4:;
	}
#endif

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("TEST R[%d], R[%d]\n", dst, src);
}

char ldr()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);

	REG srcReg = (src == 7) ? cpu.pc : cpu.r[src];

	ADDR addr = GetPA(srcReg + imm);

	if (addr == -1)
		return 0;

	cpu.r[dst] = 0;
	cpu.r[dst] |= memory[addr];
	cpu.r[dst] |= memory[addr + 1] << sizeof(REG) * 4;

	if (disassembly)
		(src != 7) ? printf("LDR R[%d], R[%d], %d\n", dst, src, imm) : printf("LDR R[%d], PC, %d\n", dst, imm);

	return 1;
}

char str()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);

	REG srcReg = (src == 7) ? cpu.pc : cpu.r[src];

	ADDR addr = GetPA(cpu.r[dst] + imm);

	if (addr == -1)
		return 0;

	memory[addr] = 0;
	memory[addr + 1] = 0;
	memory[addr] = GetLowerByte(srcReg);
	memory[addr + 1] = GetHigherByte(srcReg);

	if (disassembly)
		(src != 7) ? printf("STR R[%d], R[%d], %d\n", dst, src, imm) : printf("STR R[%d], PC, %d\n", dst, imm);

	return 1;
}