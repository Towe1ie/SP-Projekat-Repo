#include "Utility.h"

#include <stdio.h>

UBYTE GetOPCode(UBYTE byte)
{
	byte &= 0xF8; // 1111 1000
	byte >>= 3;

	return byte;
}

UBYTE GetInfoFromByte(unsigned int hightBit, unsigned int lowBit, UBYTE byte)
{
	unsigned char mask = 0xFE;
	mask <<= (hightBit - lowBit);
	mask = ~mask;
	mask <<= lowBit;

	byte &= mask;
	byte >>= lowBit;

	return byte;
}

UWORD GetInfoFromWord(unsigned int highBit, unsigned int lowBit, UWORD word)
{
	unsigned short mask = 0xFFFE;
	mask <<= (highBit - lowBit);
	mask = ~mask;
	mask <<= lowBit;

	word &= mask;
	word >>= lowBit;

	return word;
}

void WriteInfoIntoWord(WORD info, WORD *dst, unsigned int bits)
{
	UWORD mask = ~0;
	UWORD uinfo = info;
	UWORD udst = *dst;

	mask <<= bits;
	udst &= mask;
	mask = ~mask;
	uinfo &= mask;

	udst |= uinfo;

	*dst = udst;
}

UWORD MergeBytes(UBYTE high, UBYTE low)
{
	UWORD w = high;
	UWORD w1 = low;
	w <<= 8;
	w |= w1;

	return w;
}

WORD ExtSgn(UBYTE byte, int numBits)
{
	UBYTE b = GetInfoFromByte(numBits - 1, 0, byte);
	WORD w = b;
	unsigned int i = 0;

	if (w & (1 << (numBits - 1)))
		for (i = numBits; i < sizeof(WORD) * 8; ++i)
			w |= (1 << i);

	return w;
}

WORD ExtSgnW(WORD word, int numBits)
{
	UWORD w = GetInfoFromWord(numBits - 1, 0, word);
	WORD w1 = w;
	unsigned int i = 0;

	if (w1 & (1 << (numBits - 1)))
		for (i = numBits; i < sizeof(WORD) * 8; ++i)
			w1 |= (1 << i);

	return w1;
}

void SetFlag(FLAG flag)
{
	cpu.psw |= (1 << flag);
}

void ResetFlag(FLAG flag)
{
	cpu.psw &= ~(1 << flag);
}

BIT GetFlag(FLAG flag)
{
	if (cpu.psw & (1 << flag))
		return ONE;
	else
		return ZERO;
}

UBYTE GetLowerByte(UWORD word)
{
	unsigned char i = 0;
	UBYTE b = 0;
	UWORD mask = 1;

	for (i = 0; i < 8; i++)
		b += ((mask << i) & word); // nisam siguran kako radi |= za razlicite velicine (byte, word) pa sam stavio +=

	return b;
}

UBYTE GetHigherByte(UWORD word)
{
	return GetLowerByte(word >> 8);
}

BYTE ReadByteInstr(VADDR vaddr, char *status)
{
		ADDR pa = GetPA(vaddr, status, EXE);

	if (*status != 1)
		return 0;

	return memory[pa];
}

BYTE ReadByte(VADDR vaddr, char *status)
{
	ADDR pa = GetPA(vaddr, status, READ);

	if (*status != 1)
		return 0;

	return memory[pa];
}

void WriteByte(VADDR vaddr, BYTE byte, char *status)
{
	ADDR pa = GetPA(vaddr, status, WRITE);

	if (*status != 1)
		return;

	memory[pa] = byte;
}

void WriteByteLoader(VADDR vaddr, BYTE byte, char *status)
{
	ADDR pa = GetPA(vaddr, status, LOAD);

	if (*status != 1 && *status != 2)
		return;

	memory[pa] = byte;
}

WORD ReadWordLoaderReverseOrder(VADDR vaddr, char *status)
{
	ADDR pal, pah; 
	pal = GetPA(vaddr + 1, status, LOAD);

	if (*status != 1)
		return 0;

	pah = GetPA(vaddr, status, LOAD);

	if (*status != 1 && *status != 2)
		return 0;

	return MergeBytes(memory[pah], memory[pal]);
}

void WriteWordLoaderReverseOrder(VADDR vaddr, WORD word, char *status)
{
	ADDR pal, pah;
	pal = GetPA(vaddr + 1, status, LOAD);

	if (*status != 1 && *status != 2)
		return;

	pah = GetPA(vaddr, status, LOAD);

	if (*status != 1 && *status != 2)
		return;

	memory[pal] = GetLowerByte(word);
	memory[pah] = GetHigherByte(word);
}

void WriteWordLoader(VADDR vaddr, WORD word, char *status)
{
	ADDR pal, pah;
	pal = GetPA(vaddr, status, LOAD);

	if (*status != 1 && *status != 2)
		return;

	pah = GetPA(vaddr + 1, status, LOAD);

	if (*status != 1 && *status != 2)
		return;

	memory[pal] = GetLowerByte(word);
	memory[pah] = GetHigherByte(word);
}

WORD ReadWord(VADDR vaddr, char *status)
{
	ADDR pal, pah; 
	pal = GetPA(vaddr, status, READ);

	if (*status != 1)
		return 0;

	pah = GetPA(vaddr + 1, status, READ);

	if (*status != 1)
		return 0;

	return MergeBytes(memory[pah], memory[pal]);
}

void WriteWord(ADDR vaddr, WORD word, char *status)
{
	ADDR pal, pah;
	pal = GetPA(vaddr, status, WRITE);

	if (*status != 1)
		return;

	pah = GetPA(vaddr + 1, status, WRITE);

	if (*status != 1)
		return;

	memory[pal] = GetLowerByte(word);
	memory[pah] = GetHigherByte(word);
}

WORD ReadIO(ADDR addr)
{
	BYTE bl, bh;
	int t;
	WORD w;

	if (addr == 0x2000)
	{
		scanf("%d", &t);
		w = t;
		WriteIO(0x2000, w);
	}

	bl = io[addr++];
	bh = io[addr];

	return MergeBytes(bh, bl);
}

void WriteIO(ADDR addr, WORD word)
{
	if (addr == 0x1000)
		printf("%d", word);

	io[addr++] = GetLowerByte(word);
	io[addr] = GetHigherByte(word);
}

void JmpFunc(char* callingFuncName, char disassemble, BIT condition)
{
	WORD dst = 0, imm;
	if (!(ir1 & BIT2))
	{
		dst = GetInfoFromWord(9, 7, MergeBytes(ir1, ir0));
		imm = ExtSgn(ir0, 7);

		if (disassemble)
			printf("%s R[%d], %d\n", callingFuncName, dst, imm);

		if (condition)
			cpu.pc = cpu.r[dst] + imm;
	}
	else
	{
		imm = ExtSgnW(MergeBytes(ir1, ir0), 9);

		if (!(ir1 & BIT1))
		{
			if (disassemble)
				printf("%s PC, %d\n", callingFuncName, imm);

			if (condition)
				cpu.pc += imm;
		}
		else
		{
			char status;
			WORD w = ReadWord(cpu.pc + imm, &status);

			if (status != 1)
				return;

			if (disassemble)
				printf("%s [PC, %d]\n", callingFuncName, imm);

			if (condition)
				cpu.pc = w;
		}
	}
}

BIT GetDescriptorFlag(DescriptorFlag flag, Descriptor descriptor)
{
	if (1 << flag & descriptor.flags)
		return ONE;
	else
		return ZERO;
}

void SetDescriptorFlag(DescriptorFlag flag, Descriptor* descriptor, BIT value)
{
	if (value == ONE)
		descriptor->flags |= (1 << flag);
	else
		descriptor->flags &= ~(1 << flag);
}