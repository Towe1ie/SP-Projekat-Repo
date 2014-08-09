#include "Utility.h"

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