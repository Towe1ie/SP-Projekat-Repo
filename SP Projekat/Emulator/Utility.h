#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "Core.h"

#define BIT15 0x8000
#define BIT2 0x4
#define BIT1 0x2

UBYTE GetOPCode(UBYTE byte);
UBYTE GetInfoFromByte(unsigned int hightBit, unsigned int lowBit, UBYTE byte);
UWORD GetInfoFromWord(unsigned int highBit, unsigned int lowBit, UWORD word);

UWORD MergeBytes(UBYTE high, UBYTE low);

WORD ExtSgn(UBYTE byte, int numBits);
WORD ExtSgnW(WORD word, int numBits);

void SetFlag(FLAG flag);
void ResetFlag(FLAG flag);
BIT GetFlag(FLAG flag);

UBYTE GetLowerByte(UWORD word);
UBYTE GetHigherByte(UWORD word);

WORD ReadWord(ADDR addr, MEM mem);
void WriteWord(ADDR addr, WORD word, MEM mem);

void JmpFunc(char* callingFuncName, char disasseble, BIT condition);

#endif