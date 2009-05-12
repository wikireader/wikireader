/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define APPLICATION_TITLE "rs232 loader"

#include "application.h"
#include <string.h>

#define ET_EXEC		2
#define EM_C33		0x6b

#define ELFMAG0		0x7f
#define ELFMAG1		0x45
#define ELFMAG2		0x4c
#define ELFMAG3		0x46

typedef struct {
	u8  e_ident[16];	/* ELF "magic number" */
	u16 e_type;		/* Identifies object file type */
	u16 e_machine;		/* Specifies required architecture */
	u32 e_version;		/* Identifies object file version */
	u32 e_entry;		/* Entry point virtual address */
	u32 e_phoff;		/* Program header table file offset */
	u32 e_shoff;		/* Section header table file offset */
	u32 e_flags;		/* Processor-specific flags */
	u16 e_ehsize;		/* ELF header size in bytes */
	u16 e_phentsize;	/* Program header table entry size */
	u16 e_phnum;		/* Program header table entry count */
	u16 e_shentsize;	/* Section header table entry size */
	u16 e_shnum;		/* Section header table entry count */
	u16 e_shstrndx;		/* Section header string table index */
} __attribute__((packed)) elf32_hdr;

typedef struct {
	u32 sh_name;		/* Section name, index in string tbl */
	u32 sh_type;		/* Type of section */
	u32 sh_flags;		/* Miscellaneous section attributes */
	u32 sh_addr;		/* Section virtual addr at execution */
	u32 sh_offset;		/* Section file offset */
	u32 sh_size;		/* Size of section in bytes */
	u32 sh_link;		/* Index of another section */
	u32 sh_info;		/* Additional section information */
	u32 sh_addralign;	/* Section alignment */
	u32 sh_entsize;		/* Entry size if section holds table */
} __attribute__((packed)) elf32_sec;

#define SHT_NULL	0	/* Section header table entry unused */
#define SHT_PROGBITS	1	/* Program specific (private) data */
#define SHT_SYMTAB	2	/* Link editing symbol table */
#define SHT_STRTAB	3	/* A string table */
#define SHT_RELA	4	/* Relocation entries with addends */
#define SHT_HASH	5	/* A symbol hash table */
#define SHT_DYNAMIC	6	/* Information for dynamic linking */
#define SHT_NOTE	7	/* Information that marks file */
#define SHT_NOBITS	8	/* Section occupies no space in file */
#define SHT_REL		9	/* Relocation entries, no addends */
#define SHT_SHLIB	10	/* Reserved, unspecified semantics */
#define SHT_DYNSYM	11	/* Dynamic linking symbol table */

#define BUFFER (u8 *)(0x10000000 + 16 * 1024 * 1024)

void rs232_elf_load(void);

// this must be the first executable code as the loader executes from the first program address
ReturnType rs232_loader(int block, int status)
{
	APPLICATION_INITIALISE();
	rs232_elf_load();
	APPLICATION_FINALISE(0, 0);
}

void rs232_elf_load(void)
{
	u32 file_size;
	elf32_hdr *hdr;
	unsigned int i;
	void *exec;
	elf32_sec *sec;

	print("Waiting for file from the serial line ... \n");

	for (i = 0; i < sizeof(u32); i++) {
		((unsigned char *)&file_size)[i] = serial_input_char();
	}

	for (i = 0; i < file_size; i++) {
		(BUFFER)[i] = serial_input_char();
	}

	hdr = (elf32_hdr *)BUFFER;

	if (hdr->e_ident[0] != ELFMAG0 ||
	      hdr->e_ident[1] != ELFMAG1 ||
	      hdr->e_ident[2] != ELFMAG2 ||
	      hdr->e_ident[3] != ELFMAG3) {
		print("invalid ELF magic\n");
		return;
	}

	if (hdr->e_type != ET_EXEC) {
		print("invalid ELF file type\n");
		return;
	}

	if (hdr->e_machine != EM_C33) {
		print("FAIL: machine\n");
		return;
	}

	for (i = 0; i < hdr->e_shnum; i++) {
		sec = (elf32_sec *)(BUFFER + (hdr->e_shoff + (sizeof(elf32_sec) * i)));

		switch (sec->sh_type) {
		case SHT_PROGBITS:
			memcpy((u8 *)sec->sh_addr, BUFFER + sec->sh_offset, sec->sh_size);
			print("PROG: ");
			print_u32(sec->sh_addr);
			print("\n");
			break;
		case SHT_NOBITS:
			/* clean .bss sections */
			memset((u8 *)sec->sh_addr, 0, sec->sh_size);
			break;
		default:
			break;
		}
	}

	print("EXEC from serial: ");
	print_u32(hdr->e_entry);
	print("\n");

	exec = (void *)hdr->e_entry;
	((void (*) (void)) exec) ();
}

