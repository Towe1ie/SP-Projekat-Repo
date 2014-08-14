#ifndef _ELFPARSER_H_
#define _ELFPARSER_H_

#include <elf.h>
#include <stdio.h>

void elf_Load(FILE *file);
void elf_LoadRelFile(FILE *file, Elf32_Ehdr* elf_header);
void elf_LoadExeFile(FILE *file, Elf32_Ehdr* elf_header);

void* elf_GetSectionContent(char* sectionName, char* shstrtab, Elf32_Ehdr *ehdr, Elf32_Shdr *shdr, FILE *file);

char* elf_GetShstrtab(Elf32_Ehdr *ehdr, Elf32_Shdr* shdr, FILE* file);

Elf32_Ehdr* elf_ReadHeader(FILE* file);

#endif