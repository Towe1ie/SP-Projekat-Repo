#include "ElfParser.h"
#include <stdlib.h>
#include <string.h>

void elf_Load(FILE *file)
{
	Elf32_Ehdr *elf_hdr = elf_ReadHeader(file);

	switch(elf_hdr->e_type)
	{
	case ET_REL:
		elf_LoadRelFile(file, elf_hdr);
		break;
	case ET_EXEC:
		elf_LoadExeFile(file, elf_hdr);
		break;
	}
}

Elf32_Ehdr* elf_ReadHeader(FILE *file)
{
	Elf32_Ehdr *hdr = malloc(sizeof(Elf32_Ehdr));

	fseek(file, 0, SEEK_SET);
	fread(hdr, sizeof(Elf32_Ehdr), 1, file);

	return hdr;
}

void elf_LoadExeFile(FILE *file, Elf32_Ehdr* hdr)
{
	Elf32_Phdr *phdr = (Elf32_Phdr*)malloc(hdr->e_phentsize * hdr->e_phnum);
	fseek(file, hdr->e_phoff, SEEK_SET);
	fread(phdr, sizeof(Elf32_Phdr), hdr->e_phnum, file);

	printf("JO\n");
}

void elf_LoadRelFile(FILE *file, Elf32_Ehdr* ehdr)
{
	Elf32_Shdr *shdr = (Elf32_Shdr*)malloc(ehdr->e_shentsize * ehdr->e_shnum);
	fseek(file, ehdr->e_shoff, SEEK_SET);
	fread(shdr, sizeof(Elf32_Shdr), ehdr->e_shnum, file);

	char *shstrtab = elf_GetShstrtab(ehdr, shdr, file);

	char *name = shstrtab + shdr[1].sh_name;

	void *textContent = elf_GetSectionContent(name, shstrtab, ehdr, shdr, file);

	printf("JO\n");
}

char* elf_GetShstrtab(Elf32_Ehdr *ehdr, Elf32_Shdr* shdr, FILE* file)
{
	unsigned int strindex = ehdr->e_shstrndx;
	unsigned int offset = shdr[strindex].sh_offset;
	unsigned int size = shdr[strindex].sh_size;
	char* shstrtab = (char*)malloc(sizeof(char)*size);

	fseek(file, offset, SEEK_SET);
	fread(shstrtab, sizeof(char), size, file);

	return shstrtab;
}

void* elf_GetSectionContent(char* sectionName, char* shstrtab, Elf32_Ehdr *ehdr, Elf32_Shdr *shdr, FILE* file)
{
	unsigned int i = 0;
	for (i = 0; i < ehdr->e_shnum; i++)
		if (strcmp(sectionName, shstrtab + shdr[i].sh_name) == 0)
			break;

	if (i == ehdr->e_shnum)
		return NULL;

	void *p = malloc(1 * shdr[i].sh_size);
	fseek(file, shdr[i].sh_offset, SEEK_SET);
	fread(p, 1, shdr[i].sh_size, file);

	return p;
}