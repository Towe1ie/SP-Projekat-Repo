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
char status;

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

void _ldr()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);

	REG srcReg = (src == 7) ? cpu.pc : cpu.r[src];

	WORD w = ReadWord(srcReg + imm, &status);

	if (status != 1)
		return;

	cpu.r[dst] = w;

	if (disassembly)
		(src != 7) ? printf("LDR R[%d], R[%d], %d\n", dst, src, imm) : printf("LDR R[%d], PC, %d\n", dst, imm);
}

void _str()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = ExtSgn(ir0, 5);

	REG srcReg = (src == 7) ? cpu.pc : cpu.r[src];

	WriteWord(cpu.r[dst] + imm, srcReg, &status);

	if (status != 1)
		return;

	if (disassembly)
		(src != 7) ? printf("STR R[%d], R[%d], %d\n", dst, src, imm) : printf("STR R[%d], PC, %d\n", dst, imm);
}

void _je()
{
	JmpFunc("JE", disassembly, GetFlag(Z));
}

void _jne()
{
	JmpFunc("JNE", disassembly, !GetFlag(Z));
}

void _jge()
{
	JmpFunc("JGE", disassembly, !GetFlag(N) && GetFlag(Z));
}

void _jg()
{
	JmpFunc("JG", disassembly, !GetFlag(N));
}

void _jle()
{
	JmpFunc("JLE", disassembly, GetFlag(N) && GetFlag(Z));
}

void _jl()
{
	JmpFunc("JL", disassembly, GetFlag(N));
}

void _jp()
{
	JmpFunc("JP", disassembly, !GetFlag(N));
}

void _jn()
{
	JmpFunc("JN", disassembly, GetFlag(N));
}

void _jo()
{
	JmpFunc("JO", disassembly, GetFlag(O));
}

void _jno()
{
	JmpFunc("JNO", disassembly, !GetFlag(O));
}

void _call()
{
	WriteWord(cpu.sp, cpu.pc, &status);
	if (status != 1)
		return;

	cpu.sp += 2;

	JmpFunc("CALL", disassembly, ONE);
}

void _rij()
{
	UBYTE type = GetInfoFromByte(2, 1, ir1);
	WORD imm = ExtSgnW(MergeBytes(ir1, ir0), 8);
	WORD w;

	if (type == 1)
	{
		w = ReadWord(cpu.sp - 2, &status);
		if (status != 1)
			return;

		cpu.psw = w;
		cpu.sp -= 2;
	}

	if (type == 1 || type == 0)
	{
		w = ReadWord(cpu.sp - 2, &status);
		if (status != 1)
			return;

		cpu.pc = w;
		cpu.sp -= 2;
	}
	else if (type == 2)
		cpu.pc += imm;
	else
	{
		w = ReadWord(cpu.pc + imm, &status);
		if (status != 1)
			return;

		cpu.pc = w;
	}

	if (disassembly)
		printf("RET/IRET/JMP %d, %d\n", type, imm);
}

void _push()
{
	dst = GetInfoFromWord(10, 7, MergeBytes(ir1, ir0));
	WORD w;

	if (dst <= 7)
		w = cpu.r[dst];
	else if (dst == 8)
		w = cpu.psw;
	else if (dst = 9)
		w = cpu.pmtp;

	WriteWord(cpu.sp, w, &status);
	if (status != 1)
		return;

	cpu.sp += 2;

	if (disassembly)
		if (dst <= 7)
			printf("PUSH R[%d]\n", dst);
		else if (dst == 8)
			printf("PUSH PSW\n");
		else if (dst = 9)
			printf("PUSH PMTP\n");
}

void _pop()
{
	o = 0, z = 0, n = 0, c = 0;

	dst = GetInfoFromWord(10, 7, MergeBytes(ir1, ir0));
	WORD w = ReadWord(cpu.sp - 2, &status);

	if (status != 1)
		return;

	if (dst <= 7)
		cpu.r[dst] = w;
	else if (dst == 8)
		cpu.psw = w;
	else if (dst = 9)
		cpu.pmtp = w;

	cpu.sp -= 2;

	if (w < 0)
		n = 1;
	if (w == 0)
		z = 1;

	UPDATE_ALL_FLAGS;

	if (disassembly)
		if (dst <= 7)
			printf("POP R[%d]\n", dst);
		else if (dst == 8)
			printf("POP PSW\n");
		else if (dst = 9)
			printf("POP PMTP\n");
}

void _movtosfr()
{
	src = GetInfoFromByte(2, 0, ir1);
	dst = GetInfoFromByte(7, 6, ir0);
	char *reg = NULL;

	switch (dst)
	{
	case 0:
		cpu.pc = cpu.r[dst];
		reg = "PC";
		break;
	case 1:
		cpu.sp = cpu.r[dst];
		reg = "SP";
		break;
	case 2:
		cpu.psw = cpu.r[dst];
		reg = "PSW";
		break;
	case 3:
		reg = "PMTP";
		cpu.pmtp = cpu.r[dst];
		break;
	}

	if (disassembly)
		printf("MOVTOSFR R[%d], %s", src, reg);
}

void _movfromsfr()
{
	o = 0, z = 0, n = 0, c = 0;
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 6, ir0);
	char *reg = NULL;
	REG srcReg;

	switch (src)
	{
	case 0:
		srcReg = cpu.pc;
		reg = "PC";
		break;
	case 1:
		srcReg = cpu.sp;
		reg = "SP";
		break;
	case 2:
		srcReg = cpu.psw;
		reg = "PSW";
		break;
	case 3:
		srcReg = cpu.pmtp;
		reg = "PMTP";
		break;
	}

	cpu.r[dst] = srcReg;

	if (srcReg < 0)
		n = 1;
	if (srcReg == 0)
		z = 1;

	UPDATE_ALL_FLAGS;

	if (disassembly)
		printf("MOVFROMSFR R[%d], %s", dst, reg);
}

void _mov()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);

	cpu.r[dst] = cpu.r[src];

	if (disassembly)
		printf("MOV R[%d], R[%d]", dst, src);
}

void _in()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);

	cpu.r[dst] = ReadIO(io[cpu.r[src]]);

	if (disassembly)
		printf("IN R[%d], R[%d]", dst, src);
}

void _out()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);

	WriteIO(cpu.r[dst], cpu.r[src]);

	if (disassembly)
		printf("IN R[%d], R[%d]", dst, src);
}

void _shr()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = GetInfoFromByte(4, 1, ir0);

	cpu.r[dst] = (cpu.r[src] >> (UWORD)imm);
	
	if (disassembly)
		printf("SHR R[%d], R[%d], %d", dst, src, (UWORD)imm);
}

void _shl()
{
	dst = GetInfoFromByte(2, 0, ir1);
	src = GetInfoFromByte(7, 5, ir0);
	imm = GetInfoFromByte(4, 1, ir0);

	cpu.r[dst] = (cpu.r[src] << (UWORD)imm);

	if (disassembly)
		printf("SHL R[%d], R[%d], %d", dst, src, (UWORD)imm);
}