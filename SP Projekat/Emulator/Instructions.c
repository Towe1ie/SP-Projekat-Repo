#include "Instructions.h"
#include "Utility.h"
#include "Core.h"

#include <stdio.h>

#define UPDATE_ALL_FLAGS \
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

#define UPDATE_ZN_FLAGS	\
if (z)\
	SetFlag(Z);\
else\
	ResetFlag(Z);\
if (n)\
	SetFlag(N);\
else\
	ResetFlag(N);

unsigned char disassembly = 1;
WORD dst, src, imm, v1, v2; // ovo sam imao u skoro svakoj instrukciji, pa da ne bih stalno alocirao prostor na steku
unsigned char c, o, n, z;

int p = 0;

void _add()
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
		"jno l1add\n"
		"mov %2, 1\n"
	"l1add : jnc l2add\n"
		"mov %1, 1\n"
	"l2add:\n"
		:"=m"(v2), "=m"(c), "=m"(o)
		: "m"(v1), "m"(imm), "m"(v2)
		: "ax"
		);

	// sabiranje destinacije i prethodnog zbira i provera za oveflow i carry
	__asm__
		(
		"mov ax, %5\n"
		"add ax, %6\n"
		"mov %0, ax\n"
		"jno l11add\n"
		"mov %2, 1\n"
	"l11add: jnc l22add\n"
		"mov %1, 1\n"
	"l22add: jns l33add\n"
		"mov %4, 1\n"
	"l33add: jnz l44add\n"
		"mov %3, 1\n"
	"l44add:\n"
		:"=m"(v1), "=m"(c), "=m"(o), "=m"(z), "=m"(n)
		: "m"(v1), "m"(v2)
		: "ax"
		);

#endif
#endif

	cpu.r[dst] = v1;

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("ADD R[%d], R[%d], %d\n", dst, src, imm);


}

void _sub()
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
		"jno l1sub\n"
		"mov %2, 1\n"
	"l1sub : jnc l2sub\n"
		"mov %1, 1\n"
	"l2sub:\n"
		:"=m"(v2), "=m"(c), "=m"(o)
		: "m"(v1), "m"(imm), "m"(v2)
		: "ax"
		);

	// oduzimanje destinacije i prethodnog zbira i provera za oveflow i carry

	__asm__
		(
		"mov ax, %5\n"
		"sub ax, %6\n"
		"mov %0, ax\n"
		"jno l11sub\n"
		"mov %2, 1\n"
	"l11sub: jnc l22sub\n"
		"mov %1, 1\n"
	"l22sub: jns l33sub\n"
		"mov %4, 1\n"
	"l33sub: jnz l44sub\n"
		"mov %3, 1\n"
	"l44sub:\n"
		:"=m"(v1), "=m"(c), "=m"(o), "=m"(z), "=m"(n)
		: "m"(v1), "m"(v2)
		: "ax"
		);

#endif
#endif

	cpu.r[dst] = v1;

	// **** Setovanje flegova ****

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("SUB R[%d], R[%d], %d\n", dst, src, imm);
}

void _mul()
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

void _div()
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

void _cmp()
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
#else // _WIN32
#ifdef __gnu_linux__
	__asm__
	(
		"mov ax, %4\n"
		"add ax, %5\n"
		"cmp %6, ax\n"
		"jno l1cmp\n"
		"mov %1, 1\n"
	"l1cmp: jnc l2cmp\n"
		"mov %0, 1\n"
	"l2cmp: jns l3cmp\n"
		"mov %3, 1\n"
	"l3cmp: jnz l4cmp\n"
		"mov %2, 1\n"
	"l4cmp:\n"
	:"=m"(c), "=m"(o), "=m"(z), "=m"(n)
	:"m"(imm), "m"(v2), "m"(v1)
	:"ax"
	);
#endif // __gnu_linux__
#endif

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("CMP R[%d], R[%d], %d\n", dst, src, imm);
}

void _and()
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
		jns l1;
		mov n, 1;
	l1: jnz l2;
		mov z, 1;
	l2:;
	}
#else
#ifdef __gnu_linux__
		__asm__
		(
		"mov ax, %3\n"
		"mov bx, %4\n"
		"and ax, bx\n"
		"mov %0, ax\n"
		"jns l1annd\n"
		"mov %2, 1\n"
	"l1annd: jnz l2annd\n"
		"mov %1, 1\n"
	"l2annd:\n"
		:"=m"(v1), "=m"(z), "=m"(n)
		: "m"(v1), "m"(v2)
		: "ax", "bx"
		);
#endif
#endif

	cpu.r[dst] = v1;

	UPDATE_ZN_FLAGS;
	
	if (disassembly)
		printf("AND R[%d], R[%d]\n", dst, src);
}

void _or()
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
		jns l1;
		mov n, 1;
	l1: jnz l2;
		mov z, 1;
	l2:;
	}
#else
#ifdef __gnu_linux__
		__asm__
		(
		"mov ax, %3\n"
		"or ax, %4\n"
		"mov %0, ax\n"
		"jns l1or\n"
		"mov %2, 1\n"
	"l1or: jnz l2or\n"
		"mov %1, 1\n"
	"l2or:\n"
		:"=m"(v1), "=m"(z), "=m"(n)
		: "m"(v1), "m"(v2)
		: "ax"
		);
#endif
#endif

	cpu.r[dst] = v1;

	UPDATE_ZN_FLAGS;

	if (disassembly)
		printf("OR R[%d], R[%d]\n", dst, src);
}

void _not()
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
		jns l1;
		mov n, 1;
	l1: jnz l2;
		mov z, 1;
	l2:;
	}
#else
#ifdef __gnu_linux__
		__asm__
		(
		"mov ax, %3\n"
		"not ax\n"
		"mov %0, ax\n"
		"jns l1not\n"
		"mov %2, 1\n"
	"l1not: jnz l2not\n"
		"mov %1, 1\n"
	"l2not:\n"
		:"=m"(v1), "=m"(z), "=m"(n)
		: "m"(v1)
		: "ax"
		);
#endif
#endif

	cpu.r[dst] = v1;

	UPDATE_ZN_FLAGS;

	if (disassembly)
		printf("NOT R[%d], R[%d]\n", dst, src);
}

void _test()
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
		jns l1;
		mov n, 1;
	l1: jnz l2;
		mov z, 1;
	l2:;
	}
#else
#ifdef __gnu_linux__
		__asm__
		(
		"mov ax, %2\n"
		"and ax, %3\n"
		"jns l1test\n"
		"mov %1, 1\n"
	"l1test: jnz l2test\n"
		"mov %0, 1\n"
	"l2test:\n"
		:"=m"(z), "=m"(n)
		: "m"(v1), "m"(v2)
		: "ax"
		);
#endif
#endif

	UPDATE_ZN_FLAGS;

	if (disassembly)
		printf("TEST R[%d], R[%d]\n", dst, src);
}

char _ldr()
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

char _str()
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

char _je()
{
	return JmpFunc("JE", disassembly, GetFlag(Z));
}

char _jne()
{
	return JmpFunc("JE", disassembly, !GetFlag(Z));
}

char _jge()
{
	return JmpFunc("JE", disassembly, !GetFlag(N) && GetFlag(Z));
}

char _jg()
{
	return JmpFunc("JE", disassembly, !GetFlag(N));
}

char _jle()
{
		return JmpFunc("JE", disassembly, GetFlag(N) && GetFlag(Z));
}

char _jl()
{
	return JmpFunc("JE", disassembly, GetFlag(N));
}

char _jp()
{
	return JmpFunc("JE", disassembly, !GetFlag(N));
}

char _jn()
{
	return JmpFunc("JE", disassembly, GetFlag(N));
}

char _jo()
{
	return JmpFunc("JE", disassembly, GetFlag(O));
}

char _jno()
{
	return JmpFunc("JE", disassembly, !GetFlag(O));
}

char _call()
{
	ADDR addr = GetPA(cpu.sp++);
	if (addr == 0)
		return 0;

	memory[addr] = GetLowerByte(cpu.pc);
	addr = GetPA(cpu.sp++);
	if (addr == 0)
	{
		cpu.sp -= 2;
		return 0;
	}

	memory[addr] = GetHigherByte(cpu.pc);

	return JmpFunc("JE", disassembly, ONE);
}