#include "ElfParser.h"
#include <stdlib.h>
#include <string.h>


#define CHAR_TO_INT(c) c - '0'

VADDR elf_Load(FILE *file)
{
	Elf32_Ehdr *elf_hdr = elf_ReadHeader(file);
	VADDR textAddr;

	switch(elf_hdr->e_type)
	{
	case ET_REL:
		textAddr = elf_LoadRelFile(file, elf_hdr);
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

void elf_LoadContent(FILE *file, Elf32_Shdr *shdr, unsigned int i, char* shstrtab)
{
				unsigned int j;
			char status;
			void *content = elf_GetSectionContent_i(i, shdr, file);
			//memcpy(memory + shdr[i].sh_addr, content, shdr[i].sh_size);
			for (j = 0; j < shdr[i].sh_size; ++j)
			{
				WriteByteLoader(shdr[i].sh_addr + j, ((BYTE*)content)[j], &status);
			}

			if (strstr(shstrtab + shdr[i].sh_name, ".intr") != NULL)
			{
				int intrNum;
				sscanf(shstrtab + shdr[i].sh_name + 5, "%d", &intrNum);
				WriteWordLoader(intrNum << 1, shdr[i].sh_addr, &status);
			}
}

void elf_LoadNoRels(FILE *file, Elf32_Ehdr* ehdr, Elf32_Shdr *shdr, char *shstrtab)
{
	unsigned int i;
	for (i = 0; i < ehdr->e_shnum; ++i)
	{
		if (shdr[i].sh_flags & SHF_ALLOC)
		{
			elf_LoadContent(file, shdr, i, shstrtab);
		}
	}
}

VADDR elf_LoadWithRels(FILE *file, Elf32_Ehdr* ehdr, Elf32_Shdr *shdr, Elf32_Sym *symtab, char *shstrtab)
{
	Elf32_Word prems[3] = {SHF_EXECINSTR, SHF_WRITE, SHF_WRITE | SHF_EXECINSTR};
	VADDR currentAddr = entryPoint, textAddr;
	unsigned int i, j, k;

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
					if (strcmp(".text", shstrtab + shdr[j].sh_name) == 0)
						textAddr = currentAddr;

					shdr[j].sh_addr = currentAddr;
					symtab[j].st_value = currentAddr;
					elf_LoadContent(file, shdr, j, shstrtab);
					currentAddr += shdr[j].sh_size;
				}
			}
			else if ((shdr[j].sh_flags & SHF_ALLOC) && (~shdr[j].sh_flags & SHF_EXECINSTR) && (~shdr[j].sh_flags & SHF_WRITE))
			{
					shdr[j].sh_addr = currentAddr;
					elf_LoadContent(file, shdr, j, shstrtab);
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
				elf_ResolveRelocation(rel + i, symtab, shdr, sectionH);
			}
		}
	}

	return textAddr;
}

VADDR elf_LoadRelFile(FILE *file, Elf32_Ehdr* ehdr)
{
	unsigned int i = 0;
	VADDR textAddr;

	Elf32_Shdr *shdr = elf_GetSectionHederTable(file, ehdr);

	char *shstrtab = elf_GetShstrtab(ehdr, shdr, file);

	Elf32_Sym* symtab = (Elf32_Sym*)elf_GetSectionContent(".symtab", shstrtab, ehdr, shdr, file);

	//elf_LoadNoRels(file, ehdr, shdr, shstrtab);

	textAddr = elf_LoadWithRels(file, ehdr, shdr, symtab, shstrtab);

	return textAddr;
}

void elf_ResolveRelocation(Elf32_Rela *rela, Elf32_Sym* symtable, Elf32_Shdr *shdr, Elf32_Shdr* section)
{
	Elf32_Sym *sym = symtable + ELF32_R_SYM(rela->r_info);
	WORD fill = sym->st_value;

	if (ELF32_ST_TYPE(sym->st_info) != STT_SECTION)
		fill += symtable[sym->st_shndx].st_value;

	fill -= rela->r_offset;
	fill += rela->r_addend;

	char status;
	VADDR vaddr = section->sh_addr + rela->r_offset;
	WORD w = ReadWordLoader(vaddr, &status);

	WriteInfoIntoWord(fill, &w, 9);

	WriteWordLoader(vaddr, w, &status); 
}