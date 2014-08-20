#include "ElfCreator.h"
#include "Core.h"
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 41
#define STR_ENT 12

unsigned int offset = 0;
unsigned int contentOffset = 0;

char header[] = "ELF\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0|";
char text[] = "...";
char intr1[] = "...";
char intr2[] = "...";
char data[] = {0x2, 3, 4, 5};
char rodata[] = {0x0};
unsigned int bss = 0;
char shstrtable[] = "\0\0\0\0\0\0\0\0\0\0\0\0\
.rel.text\0\0\0\
.rel.intr0\0\0\
.rel.intr1\0\0\
.rel.intr2\0\0\
.rel.intr3\0\0\
.rel.intr4\0\0\
.rel.intr5\0\0\
.rel.intr6\0\0\
.rel.intr7\0\0\
.rel.intr8\0\0\
.rel.intr9\0\0\
.rel.intr10\0\
.rel.intr11\0\
.rel.intr12\0\
.rel.intr13\0\
.rel.intr14\0\
.rel.intr15\0\
.rel.data\0\0\0\
.rel.rodata\0\
.rel.bss\0\0\0\0\
.rel.shstrt\0\
.rel.symtab\0\
main\0\0\0\0\0\0\0\0";

//simboli na 


WORD addresses[] = {0, 256, 100};
char* names[] = {"",
				".rel.text.txt", 
				".rel.intr0.txt",
				".rel.intr1.txt", 
				".rel.intr2.txt", 
				".rel.intr3.txt", 
				".rel.intr4.txt",
				".rel.intr5.txt",
				".rel.intr6.txt",
				".rel.intr7.txt",
				".rel.intr8.txt",
				".rel.intr9.txt",
				".rel.intr10.txt",
				".rel.intr11.txt",
				".rel.intr12.txt",
				".rel.intr13.txt",
				".rel.intr14.txt",
				".rel.intr15.txt",
				".rel.data.txt",
				".rel.rodata.txt",
				".rel.symtab.txt",
				};

void *elf_ReadSectionFromFile(char *fname, unsigned int *size)
{
	FILE *file = fopen(fname, "rb");
	if (file == NULL)
		return NULL;

	fseek(file, 0, SEEK_END);
	*size = ftell(file);
	fseek(file, 0, SEEK_SET);

	void *p = malloc(*size);
	fread(p, 1, *size, file);
	fclose(file);

	return p;
}

void Create()
{
	unsigned int size;
	unsigned int i = 0;
	unsigned int shnum = 1;
	void *content;
	contentOffset = sizeof(Elf32_Ehdr) + N* sizeof(Elf32_Shdr);
	Elf32_Ehdr ehdr;
	offset += sizeof(ehdr);
	memcpy((void*)(&ehdr), (void*)header, (size_t)0x20);

	ehdr.e_shoff = offset;

	FILE *file = NULL;

	file = fopen("ElfCreator/out.o", "wb");


	Elf32_Shdr shdr;
	memset(&shdr, 0, sizeof(Elf32_Shdr));
	fseek(file, offset, SEEK_SET);
	fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

	offset += sizeof(Elf32_Shdr);

	// standard sections
	for (i = 1; i < 20; ++i)
	{
		char fname[100] = "ElfCreator/";
		strcat(fname, names[i] + 5);
		content = elf_ReadSectionFromFile(fname, &size);
		if (content == NULL)
			continue;

		++shnum;

		shdr.sh_name = i * STR_ENT + 4;
		shdr.sh_flags = (i == SHSTRTAB) ? 0 : SHF_ALLOC;
		if (i == TEXT || i >= INTR0 && i < DATA)
			shdr.sh_flags |= SHF_EXECINSTR;
		else if (i >= DATA && i < SHSTRTAB)
			shdr.sh_flags |= SHF_WRITE;

		shdr.sh_offset = contentOffset;
		shdr.sh_size = size;
		shdr.sh_addr = addresses[i];
		shdr.sh_type = SHT_PROGBITS;

		fseek(file, offset, SEEK_SET);
		fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

		fseek(file, contentOffset, SEEK_SET);
		fwrite(content, 1, size, file);

		offset += sizeof(Elf32_Shdr);
		contentOffset += size;
	}

	// bss section
	printf("\nbss size = ");
	scanf("%d", &bss);
	if (bss > 0)
	{
		shdr.sh_name = shdr.sh_name = 20* STR_ENT + 4;
		shdr.sh_size = bss;
		shdr.sh_flags = 0;
		shdr.sh_flags = SHF_ALLOC | SHF_WRITE;
		shdr.sh_type = SHT_NOBITS;

		fseek(file, offset, SEEK_SET);
		fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

		offset += sizeof(Elf32_Shdr);

		++shnum;
	}

	// symbol talbe

	content = elf_ReadSectionFromFile("ElfCreator/symtab.txt", &size);
	if (content != NULL)
	{
		shdr.sh_name = shdr.sh_name = 22 * STR_ENT + 4;
		shdr.sh_offset = contentOffset;
		shdr.sh_size = size;
		shdr.sh_type = SHT_SYMTAB;
		shdr.sh_flags = 0;

		fseek(file, offset, SEEK_SET);
		fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

		fseek(file, contentOffset, SEEK_SET);
		fwrite(content, 1, size, file);

		++shnum;

		offset += sizeof(Elf32_Shdr);
		contentOffset += size;
	}

	// rel. sections

	for (i = 1; i < 20; ++i)
	{
		char fname[100] = "ElfCreator/";
		strcat(fname, names[i] + 1);
		content = elf_ReadSectionFromFile(fname, &size);
		if (content == NULL)
			continue;

		++shnum;

		shdr.sh_name = i * STR_ENT;
		shdr.sh_offset = contentOffset;
		shdr.sh_size = size;
		shdr.sh_addr = 0;
		shdr.sh_type = SHT_RELA;
		shdr.sh_flags = 0;

		fseek(file, offset, SEEK_SET);
		fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

		fseek(file, contentOffset, SEEK_SET);
		fwrite(content, 1, size, file);

		offset += sizeof(Elf32_Shdr);
		contentOffset += size;
	}

	// string table
	shdr.sh_name = shdr.sh_name = 21 * STR_ENT + 4;
	shdr.sh_offset = contentOffset;
	shdr.sh_size = sizeof(shstrtable);
	shdr.sh_flags = 0;
	shdr.sh_type = SHT_STRTAB;

	++shnum;

	fseek(file, offset, SEEK_SET);
	fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

	fseek(file, contentOffset, SEEK_SET);
	fwrite(shstrtable, 1, sizeof(shstrtable), file);

	offset += sizeof(Elf32_Shdr);
	contentOffset += sizeof(shstrtable);

	// elf header
	ehdr.e_phnum = 0;
	ehdr.e_phoff = 0;
	ehdr.e_ehsize = sizeof(Elf32_Ehdr);
	ehdr.e_shnum = shnum;
	ehdr.e_shstrndx = shnum - 1;
	ehdr.e_shentsize = sizeof(Elf32_Shdr);
	fseek(file, 0, SEEK_SET);
	fwrite(&ehdr, 1, sizeof(Elf32_Ehdr), file);


	fclose(file);
}
