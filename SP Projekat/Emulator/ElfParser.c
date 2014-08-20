#include "ElfParser.h"
#include <stdlib.h>
#include <string.h>


#define CHAR_TO_INT(c) c - '0'

char elf_Load(FILE *file)
{
	Elf32_Ehdr *elf_hdr = elf_ReadHeader(file);
	VADDR textAddr;
	char status;

	switch(elf_hdr->e_type)
	{
	case ET_REL:
		status = elf_LoadRelFile(file, elf_hdr);
		if (status == 0)
			return 0;
		break;
	}

	return 1;
}

Elf32_Ehdr* elf_ReadHeader(FILE *file)
{
	Elf32_Ehdr *hdr = malloc(sizeof(Elf32_Ehdr));

	fseek(file, 0, SEEK_SET);
	fread(hdr, sizeof(Elf32_Ehdr), 1, file);

	return hdr;
}

Elf32_Shdr* elf_GetSectionHederTable(FILE *file, Elf32_Ehdr *ehdr)
{
	unsigned int i;
	Elf32_Shdr *shdr = (Elf32_Shdr*)malloc(ehdr->e_shnum * sizeof(Elf32_Shdr));

	fseek(file, ehdr->e_shoff, SEEK_SET);
	fread(shdr, sizeof(Elf32_Shdr), ehdr->e_shnum, file);

	return shdr;
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

Elf32_Shdr* elf_FindSectionHeader(Elf32_Ehdr *ehdr, Elf32_Shdr* shdr, char *name, char* shstrtab)
{
	unsigned int i = 0;
	Elf32_Shdr *p = NULL;

	for (i = 0; i < ehdr->e_shnum; ++i)
		if (strcmp(name, shstrtab + shdr[i].sh_name) == 0)
		{
			p = shdr + i;
		}

	return p;
}

void* elf_GetSectionContent_i(unsigned int i, Elf32_Shdr *shdr, FILE* file)
{
	void *p = malloc(1 * shdr[i].sh_size);
	fseek(file, shdr[i].sh_offset, SEEK_SET);
	fread(p, 1, shdr[i].sh_size, file);

	return p;
}

void* elf_GetSectionContent(char* sectionName, char* shstrtab, Elf32_Ehdr *ehdr, Elf32_Shdr *shdr, FILE* file)
{
	unsigned int i = 0;
	for (i = 0; i < ehdr->e_shnum; i++)
		if (strcmp(sectionName, shstrtab + shdr[i].sh_name) == 0)
			break;

	if (i == ehdr->e_shnum)
		return NULL;

	return elf_GetSectionContent_i(i, shdr, file);
}

char elf_LoadContent(FILE *file, Elf32_Shdr *shdr, unsigned int i, char* shstrtab)
{
	unsigned int j;
	char status;
	void *content = elf_GetSectionContent_i(i, shdr, file);
			
	for (j = 0; j < shdr[i].sh_size; ++j)
	{
		WriteByteLoader(shdr[i].sh_addr + j, ((BYTE*)content)[j], &status);
		if (status == 0)
			return 0;
	}

	if (strstr(shstrtab + shdr[i].sh_name, ".intr") != NULL)
	{
		int intrNum;
		sscanf(shstrtab + shdr[i].sh_name + 5, "%d", &intrNum);
		WriteWordLoader(intrNum << 1, shdr[i].sh_addr, &status);
		if (status == 0)
			return 0;
	}
	return 1;
}

char elf_LoadNoRels(FILE *file, Elf32_Ehdr* ehdr, Elf32_Shdr *shdr, char *shstrtab)
{
	unsigned int i;
	char status;

	for (i = 0; i < ehdr->e_shnum; ++i)
	{
		if (shdr[i].sh_flags & SHF_ALLOC)
		{
			status = elf_LoadContent(file, shdr, i, shstrtab);
			if (status == 0)
				return 0;
		}
	}
}

char elf_LoadWithRels(FILE *file, Elf32_Ehdr* ehdr, Elf32_Shdr *shdr, Elf32_Sym *symtab, char *shstrtab)
{
	Elf32_Word prems[3] = {SHF_EXECINSTR, SHF_WRITE, SHF_WRITE | SHF_EXECINSTR};
	VADDR currentAddr = entryPoint;
	unsigned int i, j;
	char status;

	for (i = 0; i < 3; ++i)
	{
		Elf32_Word currentPrem = prems[i];
		if (currentAddr % BLOCK_SIZE != 0)
			currentAddr += BLOCK_SIZE - (currentAddr % BLOCK_SIZE);

		for (j = 0; j < ehdr->e_shnum; ++j)
		{
			if (i < 2)
			{
				if ((shdr[j].sh_flags & SHF_ALLOC) && (shdr[j].sh_flags & currentPrem))
				{
					shdr[j].sh_addr = currentAddr;
					symtab[j].st_value = currentAddr;
					status = elf_LoadContent(file, shdr, j, shstrtab);
					if (status == 0)
						return 0;
					currentAddr += shdr[j].sh_size;
				}
			}
			else if ((shdr[j].sh_flags & SHF_ALLOC) && (~shdr[j].sh_flags & SHF_EXECINSTR) && (~shdr[j].sh_flags & SHF_WRITE))
			{
					shdr[j].sh_addr = currentAddr;
					status = elf_LoadContent(file, shdr, j, shstrtab);
					if (status == 0)
						return 0;
					currentAddr += shdr[j].sh_size;
			}
		}
	}

	for (i = 0; i < ehdr->e_shnum; ++i)
	{
		if (strstr(shstrtab + shdr[i].sh_name, ".rel") != NULL)
		{
			Elf32_Rela *rel = elf_GetSectionContent_i(i, shdr, file);
			Elf32_Shdr *sectionH = elf_FindSectionHeader(ehdr, shdr, shstrtab + shdr[i].sh_name + 4, shstrtab);

			for (j = 0; j < shdr[i].sh_size/sizeof(Elf32_Rela); ++j)
			{				
				elf_ResolveRelocation(rel + j, symtab, shdr, sectionH);
			}
		}
	}

	return 1;
}

char elf_LoadRelFile(FILE *file, Elf32_Ehdr* ehdr)
{
	unsigned int i = 0;
	char status;

	Elf32_Shdr *shdr = elf_GetSectionHederTable(file, ehdr);

	char *shstrtab = elf_GetShstrtab(ehdr, shdr, file);

	if (elf_RelocationExists(ehdr, shdr))
	{
		Elf32_Sym* symtab = (Elf32_Sym*)elf_GetSectionContent(".symtab", shstrtab, ehdr, shdr, file);
		status = elf_LoadWithRels(file, ehdr, shdr, symtab, shstrtab);
		if (status == 0)
			return 0;
	}
	else
	{
		status = elf_LoadNoRels(file, ehdr, shdr, shstrtab);
		if (status == 0)
			return 0;
	}

	return 1;
}

void elf_ResolveRelocation(Elf32_Rela *rela, Elf32_Sym* symtable, Elf32_Shdr *shdr, Elf32_Shdr* section)
{
	unsigned int s = ELF32_R_SYM(rela->r_info);
	Elf32_Sym *sym = symtable + ELF32_R_SYM(rela->r_info);
	WORD fill = sym->st_value;

	if (ELF32_ST_TYPE(sym->st_info) != STT_SECTION)
		fill += symtable[sym->st_shndx].st_value;

	fill -= rela->r_offset;
	fill -= section->sh_addr;
	fill += rela->r_addend;

	char status;
	VADDR vaddr = section->sh_addr + rela->r_offset;
	WORD w = ReadWordLoaderReverseOrder(vaddr, &status);

	WriteInfoIntoWord(fill, &w, 9);

	WriteWordLoaderReverseOrder(vaddr, w, &status); 
}

char elf_RelocationExists(Elf32_Ehdr *ehdr, Elf32_Shdr *shdr)
{
	unsigned int i;

	for (i = 0; i < ehdr->e_shnum; ++i)
		if (shdr[i].sh_type == SHT_RELA)
			return 1;

	return 0;
}