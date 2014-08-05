#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "Core.h"

#define BIT15 0x8000

UBYTE GetOPCode(UBYTE byte);
UBYTE GetInfoFromByte(unsigned int hightBit, unsigned int lowBit, UBYTE byte);

WORD ExtSgn(UBYTE byte, int numBits);

void SetFlag(FLAG flag);
void ResetFlag(FLAG flag);
BIT GetFlag(FLAG flag);

#endif