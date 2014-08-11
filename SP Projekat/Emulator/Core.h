#ifndef _CORE_H_
#define _CORE_H_

#define MEMCAP 0x8000

#define CHECK_INTERRUPTS \
if (prekid)\
	goto PREKID;\

typedef short WORD;
typedef unsigned short UWORD;
typedef char BYTE;
typedef unsigned char UBYTE;

typedef unsigned short ADDR;
typedef unsigned short VADDR;

typedef short REG;

typedef BYTE MEM[MEMCAP];

typedef enum {ZERO = 0, ONE} BIT;
typedef enum {Z = 0, O, C, N, VM = 15} FLAG;
typedef enum {EXE = 3, WRITE, READ} MEM_OP;

typedef struct
{
	REG r[8];
	REG pc, sp, psw, pmtp;
} CPU;

typedef struct
{
	BYTE flags;
	BYTE block;
} Descriptor;


extern CPU cpu;
extern MEM memory;
extern MEM io;
extern UBYTE ir0, ir1;

extern char work;
extern char prekid;
extern unsigned char brPrekid;

void Emulate();
ADDR GetPA(VADDR vaddr, char *status, MEM_OP memOp); // status : 0 = page fault, 1 = uspesno, 2 = nedozvoljeno pravo
ADDR LoadProgram(char *fileName);

#endif