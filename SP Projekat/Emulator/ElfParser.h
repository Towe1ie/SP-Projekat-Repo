#ifndef _ELFPARSER_H_
#define _ELFPARSER_H_

#include <elf.h>
#include <stdio.h>
#include "Core.h"

char elf_Load(FILE *file);
char elf_LoadRelFile(FILE *file, Elf32_Ehdr* elf_header);

Elf32_Ehdr* elf_ReadHeader(FILE* file);
Elf32_Shdr* elf_GetSectionHederTable(FILE *file, Elf32_Ehdr *ehdr);

char* elf_GetShstrtab(Elf32_Ehdr *ehdr, Elf32_Shdr* shdr, FILE* file);

Elf32_Shdr* elf_FindSectionHeader(Elf32_Ehdr *ehdr, Elf32_Shdr* shdr, char *name, char* shstrtab);

void* elf_GetSectionContent(char* sectionName, char* shstrtab, Elf32_Ehdr *ehdr, Elf32_Shdr *shdr, FILE *file);

void elf_ResolveRelocation(Elf32_Rela *rela, Elf32_Sym* symtable, Elf32_Shdr *shdr, Elf32_Shdr* section);

char elf_RelocationExists(Elf32_Ehdr *ehdr, Elf32_Shdr *shdr);

#endif