#include "ElfCreator.h"
#include "Core.h"
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
.rel.shstrt\0";

WORD addresses[] = {0, 256, 100};
char* names[] = {"",
				"text.txt", 
				"intr0.txt",
				"intr1.txt", 
				"intr2.txt", 
				"intr3.txt", 
				"intr4.txt",
				"intr5.txt",
				"intr6.txt",
				"intr7.txt",
				"intr8.txt",
				"intr9.txt",
				"intr10.txt",
				"intr11.txt",
				"intr12.txt",
				"intr13.txt",
				"intr14.txt",
				"intr15.txt",
				"data.txt",
				"rodata.txt",
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
	unsigned int i = 0;
	unsigned int shnum = 1;
	contentOffset = sizeof(Elf32_Ehdr) + 20 * sizeof(Elf32_Shdr);
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

	for (i = 1; i < 10; ++i)
	{
		unsigned int size;
		char fname[100] = "ElfCreator/";
		strcat(fname, names[i]);
		void *content = elf_ReadSectionFromFile(fname, &size);
		if (content == NULL)
			continue;

		++shnum;

		shdr.sh_name = i*12 + 4;
		shdr.sh_flags = (i == SHSTRTAB) ? 0 : SHF_ALLOC;
		if (i == TEXT || i >= INTR0 && i < DATA)
			shdr.sh_flags |= SHF_EXECINSTR;
		else if (i >= DATA && i < SHSTRTAB)
			shdr.sh_flags |= SHF_WRITE;

		shdr.sh_offset = contentOffset;
		shdr.sh_size = size;
		shdr.sh_addr = addresses[i];

		fseek(file, offset, SEEK_SET);
		fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

		fseek(file, contentOffset, SEEK_SET);
		fwrite(content, 1, size, file);

		offset += sizeof(Elf32_Shdr);
		contentOffset += size;
	}

	if (bss > 0)
	{
		shdr.sh_name = shdr.sh_name = 20*12 + 4;
		shdr.sh_size = bss;
		shdr.sh_flags = 0;
		shdr.sh_flags = SHF_ALLOC | SHF_WRITE;

		fseek(file, offset, SEEK_SET);
		fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

		offset += sizeof(Elf32_Shdr);

		++shnum;
	}

	shdr.sh_name = shdr.sh_name = 21*12 + 4;
	shdr.sh_offset = contentOffset;
	shdr.sh_size = sizeof(shstrtable);
	shdr.sh_flags = 0;

	++shnum;

	fseek(file, offset, SEEK_SET);
	fwrite(&shdr, 1, sizeof(Elf32_Shdr), file);

	fseek(file, contentOffset, SEEK_SET);
	fwrite(shstrtable, 1, sizeof(shstrtable), file);

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
