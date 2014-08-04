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

typedef enum {ZERO = 0, ONE} BIT;

typedef struct
{
	REG r[8];
	REG pc, sp, psw, pmt;
} CPU;


extern CPU cpu;
extern MEM memory;

extern char work;

void Emulate();
ADDR LoadProgram(char *fileName);

#endif