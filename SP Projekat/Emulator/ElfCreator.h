#ifndef _ELFCREATOR_H_
#define _ELFCREATOR_H_

typedef enum {TEXT = 1, INTR0, INTR1, DATA = 18, BSS, RODATA, SHSTRTAB, SYMTAB} Section;

void Create();

#endif