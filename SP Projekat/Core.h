#ifndef _CORE_H_
#define _CORE_H_

#define MEMCAP 0x8000

typedef short WORD;
typedef unsigned short UWORD;
typedef char BYTE;
typedef unsigned char UBYTE;

typedef unsigned short ADDR;

typedef short REG;

typedef BYTE MEM[MEMCAP];

typedef struct
{
	REG r[8];
	REG pc, sp, psw, pmt;
} CPU;


CPU cpu;
MEM memory;

void Emulate();

#endif