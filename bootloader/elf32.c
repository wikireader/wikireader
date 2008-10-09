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

#include "regs.h"
#include "types.h"
#include "wikireader.h"
#include "misc.h"
#include "fat.h"

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

#define SHT_NULL        0               /* Section header table entry unused */
#define SHT_PROGBITS    1               /* Program specific (private) data */
#define SHT_SYMTAB      2               /* Link editing symbol table */
#define SHT_STRTAB      3               /* A string table */
#define SHT_RELA        4               /* Relocation entries with addends */
#define SHT_HASH        5               /* A symbol hash table */
#define SHT_DYNAMIC     6               /* Information for dynamic linking */
#define SHT_NOTE        7               /* Information that marks file */
#define SHT_NOBITS      8               /* Section occupies no space in file */
#define SHT_REL         9               /* Relocation entries, no addends */
#define SHT_SHLIB       10              /* Reserved, unspecified semantics */
#define SHT_DYNSYM      11              /* Dynamic linking symbol table */

int elf_read(const u8 *filename)
{
	elf32_hdr hdr;
	elf32_sec sec;
	u32 i;
	void *exec;

	if (fat_open_file(filename) < 0)
		return -1;

	fat_read_file(0, (u8 *) &hdr, sizeof(hdr));
	
	if (hdr.e_ident[0] != ELFMAG0 ||
	    hdr.e_ident[1] != ELFMAG1 ||
	    hdr.e_ident[2] != ELFMAG2 ||
	    hdr.e_ident[3] != ELFMAG3) {
	    	print("invalid ELF magic\n");
		return -1;
	}

	if (hdr.e_type != ET_EXEC) {
		print("invalid ELF file type\n");
		return -1;
	}

	if (hdr.e_machine != EM_C33) {
		print("invalid ELF machine type\n");
		return -1;
	}

	print("\n");

	for (i = 0; i < hdr.e_shnum; i++) {
		fat_read_file(hdr.e_shoff + sizeof(sec) * i, (u8 *) &sec, sizeof(sec));

		switch (sec.sh_type) {
			case SHT_PROGBITS:
				fat_read_file(sec.sh_offset, (u8 *) sec.sh_addr, sec.sh_size);
				print("PROGBITS loaded.\n");
				break;
			case SHT_NOBITS:
				/* clean the .bss section */
				memset(sec.sh_addr, 0, sec.sh_size);
				break;
			default:
				break;
		}
	}

	print("all sections loaded, jumping to ");
	print_u32(hdr.e_entry);
	print(". Sayonara.\n\n");
	disable_card_power();

	exec = (void *) hdr.e_entry;
	hex_dump(exec, 256);
	((void (*) (void)) exec) ();

	/* never reached */
	return 0;
}

