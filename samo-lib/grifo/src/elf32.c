/*
 * Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>
 * Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "standard.h"

#include <string.h>

#include <tff.h>
#include <regs.h>
#include <samo.h>

#include "elf32.h"
#include "serial.h"

// 0 = no-debugging
// 1 = serious errors
// 2 = section data
// 3 = messages for things that should not fail

#define DEBUG_LEVEL 3

#define DEBUG_ELF(level, format...)             \
	do {                                    \
		if (level <= DEBUG_LEVEL) {     \
			Serial_printf(format);  \
		}                               \
	} while (0)

#define ET_EXEC		2
#define EM_C33		0x6b

#define ELFMAG0		0x7f
#define ELFMAG1		0x45
#define ELFMAG2		0x4c
#define ELFMAG3		0x46

typedef struct {
	uint8_t  e_ident[16];	        // ELF "magic number"
	uint16_t e_type;		// Identifies object file type
	uint16_t e_machine;		// Specifies required architecture
	uint32_t e_version;		// Identifies object file version
	uint32_t e_entry;		// Entry point virtual address
	uint32_t e_phoff;		// Program header table file offset
	uint32_t e_shoff;		// Section header table file offset
	uint32_t e_flags;		// Processor-specific flags
	uint16_t e_ehsize;		// ELF header size in bytes
	uint16_t e_phentsize;	        // Program header table entry size
	uint16_t e_phnum;		// Program header table entry count
	uint16_t e_shentsize;           // Section header table entry size
	uint16_t e_shnum;		// Section header table entry count
	uint16_t e_shstrndx;		// Section header string table index
} __attribute__((packed)) elf32_hdr;

typedef struct {
	uint32_t sh_name;		// Section name, index in string tbl
	uint32_t sh_type;		// Type of section
	uint32_t sh_flags;		// Miscellaneous section attributes
	uint32_t sh_addr;		// Section virtual addr at execution
	uint32_t sh_offset;		// Section file offset
	uint32_t sh_size;		// Size of section in bytes
	uint32_t sh_link;		// Index of another section
	uint32_t sh_info;		// Additional section information
	uint32_t sh_addralign;	        // Section alignment
	uint32_t sh_entsize;		// Entry size if section holds table
} __attribute__((packed)) elf32_sec;

// sh_type

#define SHT_NULL	0		// Section header table entry unused
#define SHT_PROGBITS	1		// Program specific (private) data
#define SHT_SYMTAB	2		// Link editing symbol table
#define SHT_STRTAB	3		// A string table
#define SHT_RELA	4		// Relocation entries with addends
#define SHT_HASH	5		// A symbol hash table
#define SHT_DYNAMIC	6		// Information for dynamic linking
#define SHT_NOTE	7		// Information that marks file
#define SHT_NOBITS	8		// Section occupies no space in file
#define SHT_REL		9		// Relocation entries, no addends
#define SHT_SHLIB	10		// Reserved, unspecified semantics
#define SHT_DYNSYM	11		// Dynamic linking symbol table

// sh_flags

#define SHF_WRITE            (1 << 0)   // Writable
#define SHF_ALLOC            (1 << 1)   // Occupies memory during execution
#define SHF_EXECINSTR        (1 << 2)   // Executable
#define SHF_MERGE            (1 << 4)   // Might be merged
#define SHF_STRINGS          (1 << 5)   // Contains nul-terminated strings
#define SHF_INFO_LINK        (1 << 6)   // `sh_info' contains SHT index
#define SHF_LINK_ORDER       (1 << 7)   // Preserve order after combining
#define SHF_OS_NONCONFORMING (1 << 8)   // Non-standard OS specific handling required
#define SHF_GROUP            (1 << 9)   // Section is member of a group
#define SHF_TLS              (1 << 10)  // Section hold thread-local data
#define SHF_MASKOS           0x0ff00000 // OS-specific.
#define SHF_MASKPROC         0xf0000000 // Processor-specific
#define SHF_ORDERED          (1 << 30)  // Special ordering requirement (Solaris).
#define SHF_EXCLUDE          (1 << 31)  // Section is excluded unless
					// referenced or allocated (Solaris)

int ELF32_load(uint32_t *execution_address, const char *filename)
{
	FRESULT r = 0;
	FATFS fatfs;
	FIL file;
	int rc = ELF32_OK;

	if (NULL == execution_address) {
		DEBUG_ELF(3, "ELF: NULL execution_address pointer\n");
		rc = ELF32_NULL_ADDRESS;
		goto abort_umount;
	}

	DEBUG_ELF(2, "ELF: load file = %s\n", filename);

	r = f_mount(0, &fatfs);
	if (r != FR_OK) {
		DEBUG_ELF(3, "ELF: mount error = %u\n", r);
		rc = ELF32_MOUNT_FAIL;
		goto abort_umount;
	}

	r = f_open(&file, filename, FA_READ);
	if (r != FR_OK) {
		DEBUG_ELF(1, "ELF: open file '%s' error = %u\n", filename, r);
		rc = ELF32_OPEN_FAIL;
		goto abort_umount;
	}

	elf32_hdr hdr;
	UINT n = 0;
	r = f_read(&file, &hdr, sizeof(hdr), &n);
	if (r != FR_OK) {
		DEBUG_ELF(3,"ELF: read header error = %u\n", r);
		rc = ELF32_INVALID_HEADER;
		goto abort_close;
	}
	if (n != sizeof(hdr)) {
		DEBUG_ELF(3, "ELF: read header size mismatch: %u != %u\n", n, sizeof(hdr));
		rc = ELF32_INVALID_HEADER;
		goto abort_close;
	}

	if (hdr.e_ident[0] != ELFMAG0 ||
	    hdr.e_ident[1] != ELFMAG1 ||
	    hdr.e_ident[2] != ELFMAG2 ||
	    hdr.e_ident[3] != ELFMAG3) {
		DEBUG_ELF(3, "ELF: invalid magic\n");
		rc = ELF32_INVALID_MAGIC_NUMBER;
		goto abort_close;
	}

	if (hdr.e_type != ET_EXEC) {
		DEBUG_ELF(3, "ELF: invalid file type: %u != ET_EXEC (%u)\n", hdr.e_type, ET_EXEC);
		rc = ELF32_FILE_TYPE;
		goto abort_close;
	}

	if (hdr.e_machine != EM_C33) {
		DEBUG_ELF(3, "ELF: invalid machine: %u != EM_C33 (%u)\n", hdr.e_machine, EM_C33);
		rc = ELF32_MACHINE_TYPE;
		goto abort_close;
	}

	int i;
	for (i = 0; i < hdr.e_shnum; i++) {
		elf32_sec sec;
		f_lseek(&file, hdr.e_shoff + sizeof(sec) * i);
		if ((r = f_read(&file, (uint8_t *) &sec, sizeof(sec), &n)) || n != sizeof(sec)) {
			DEBUG_ELF(3, "ELF: section read: error=%u read=%u expected=%u\n",
				  r, n, sizeof(sec));
			continue;
		}

		switch (sec.sh_type) {

		case SHT_PROGBITS:
			if (0 != (SHF_ALLOC & sec.sh_flags)) {
				f_lseek(&file, sec.sh_offset);
				if (f_read(&file, (uint8_t *) sec.sh_addr, sec.sh_size, &n) || n != sec.sh_size) {
					DEBUG_ELF(1, "ELF: PROGBITS read: error=%u read=%u expected=%lu\n",
						  r, n, sec.sh_size);
					rc = ELF32_DATA_READ_FAIL;
					goto abort_close;
				}
				DEBUG_ELF(2, "LOAD: 0x%08lx %08lx %08lx\n", sec.sh_addr, sec.sh_size, sec.sh_flags);
			} else {
				DEBUG_ELF(2, "SKIP: 0x%08lx %08lx %08lx\n", sec.sh_addr, sec.sh_size, sec.sh_flags);
			}
			break;

		case SHT_NOBITS:
			DEBUG_ELF(2, "ZERO: 0x%08lx %08lx\n", sec.sh_addr, sec.sh_size);

			memset((uint8_t *) sec.sh_addr, 0, sec.sh_size);
			break;

		default:
			DEBUG_ELF(3, "????: type = %lu\n", sec.sh_type);
			break;
		}
	}

	f_close(&file);

	SDCARD_CS_HI();
	disable_card_power();

	DEBUG_ELF(2, "EXEC: 0x%08lx\n", hdr.e_entry);

	*execution_address = hdr.e_entry;
	goto abort_umount;

// make sure every thing is cleaned up if the load fails fail
abort_close:
	f_close(&file);
abort_umount:
	SDCARD_CS_HI();
	disable_card_power();
	return rc;
}

