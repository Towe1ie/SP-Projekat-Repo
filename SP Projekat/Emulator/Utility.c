#include "Utility.h"

BYTE GetOPCode(BYTE byte)
{
	byte &= 0xF8; // 1111 1000
	byte >>= 3;

	return byte;
}