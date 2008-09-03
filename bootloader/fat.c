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

/*
 * Support code for FAT16 and FAT32 file systems.
 *
 * Restrictions:
 * 	* supports read only, no writes
 * 	* no convenient high-level functions
 * 	* supports one FS only
 */

#include "types.h"
#include "spi.h"
#include "sdcard.h"
#include "misc.h"
#include "fat.h"

#define FAT_ADDR_RESERVED_SECTORS	0x0e
#define FAT_ADDR_NUM_FATS		0x10

static struct boot_sector boot;
static u32 fat_start;
static u32 first_cluster_sector;
static u32 root_entry;
static u8 buf[BYTES_PER_SECTOR];
static u8 is_fat32;
static struct dir_entry current_file;

extern void memcpy(void *dst, void *src, u32 len);

#define CLUSTER_TO_SECTOR(c)	\
	(first_cluster_sector + (c - 2) * boot.sectors_per_cluster)

/* returns the first sector of a given partition number,
 * read from the partition table */
static int read_mbr (u32 partition_num)
{
	struct partition_record *part;

	if (partition_num >= 4)
		return -1;

	sdcard_read_sector(0, buf);

	if (buf[0x1fe] != 0x55 || buf[0x1ff] != 0xaa) {
		print("invalid MBR signature\n");
		return -1;
	}

	/* partition table starts @offset 0x1be, 16 bytes per partition record */
	part = (struct partition_record *) (buf + 0x1be + (16 * partition_num));

	if (part->status != 0x00 && part->status != 0x80) {
		print("invalid partition status\n");
		return -1;
	}

	return part->first_sector;
}

static int read_bootsector (u32 sector)
{
	sdcard_read_sector(sector, (u8 *) &boot);

	if (boot.bytes_per_sector != BYTES_PER_SECTOR) {
		print("invalid number of bytes per sector, bummer.\n");
		return -1;
	}

	if (boot.signature != 0xaa55) {
		print("invalid boot sector signature\n");
		return -1;
	}

	//print("BOOT SECTOR:\n");
	//hex_dump((u8 *) &boot, 512);

	fat_start = sector + boot.reserved_sectors;

	if (boot.fat_type[0] == 'F' &&
	    boot.fat_type[1] == 'A' &&
	    boot.fat_type[2] == 'T' &&
	    boot.fat_type[3] == '3' &&
	    boot.fat_type[4] == '2') {
		first_cluster_sector = fat_start + (boot.num_fats * boot.sectors_per_fat32);
		root_entry = CLUSTER_TO_SECTOR(boot.root_cluster);
		boot.max_root_entries = 512; /* ??? */
		is_fat32 = 1;
		print("FAT32 fs detected.\n");
	} else {
		root_entry = fat_start + (boot.num_fats * boot.sectors_per_fat16);
		first_cluster_sector = root_entry
			+ 32; //boot.max_root_entries / (BYTES_PER_SECTOR / sizeof(struct dir_entry));
		print("FAT16 fs detected.\n");
	}

	return 0;
}

int fat_init(u32 partition_num)
{
	int first_sector;

	is_fat32 = 0;
	root_entry = -1;
	first_sector = read_mbr(partition_num);
	current_file.first_cluster = 0;

	if (first_sector < 0)
		/* if this failed, we maybe have a FAT FS at the first sector
		 * and no partiton table at all. Let's try that ... */
		first_sector = 0;

	return read_bootsector(first_sector);
}

#define ENTRIES_PER_FAT_SECTOR (256)

static int get_fat_entry(u32 cluster)
{
	u32 sector = fat_start;
	
	if (is_fat32) {
		u32 *fat = (u32 *) buf;
		sector += cluster >> 7;
		sdcard_read_sector(sector, buf);
		return fat[cluster & 0x7f];
	} else {
		u16 *fat = (u16 *) buf;
		sector += cluster >> 8;
		sdcard_read_sector(sector, buf);
		return fat[cluster & 0xff];
	}
}

int fat_read_file(u32 offset, u8 *dest, u32 len)
{
	u32 cluster = current_file.first_cluster;
	u8 buf[512];

	if (!cluster)
		return -1;

	if (len > current_file.file_size - offset)
		len = current_file.file_size - offset;

	while (len) {
		u32 i, b, sector = CLUSTER_TO_SECTOR(cluster);

		/* read one cluster, which is spread over multiple sectors */
		for (i = 0; i < boot.sectors_per_cluster; i++) {
			/* we loop here until the offset shrunk to something
			 * we can reach in one block access */
			if (offset >= BYTES_PER_SECTOR) {
				offset -= BYTES_PER_SECTOR;
				continue;
			}

			/* Hence, once we reached here, offset is always
			 * < BYTES_PER_OFFSET. We need that information only for
			 * the first block access, all other cycles will use
			 * an offset of 0. */

			sdcard_read_sector(sector + i, buf);

			for (b = offset; b < sizeof(buf); b++) {
				*dest = buf[b];
				dest++;
				if (--len == 0)
					break;
			}
			
			offset = 0;

			if (len == 0)
				break;
		}

		/* we need to read more bytes from another cluster. */
		cluster = get_fat_entry(cluster);
	}

	return 0;
}

static int str_equal(const u8 *s1, const u8 *s2)
{
	while (*s1 && *s2)
		if (*s1++ != *s2++)
			return 0;

	return 1;
}

/* currently, this function only supports files located at the root directory */
int fat_open_file(const u8 *filename)
{
	u32 entry_cnt = 0;
	u32 sector = root_entry;
	struct dir_entry *e;

	if (root_entry < 0)
		return -1;

	while (entry_cnt < boot.max_root_entries) {
		sdcard_read_sector(sector++, buf);

		for (e = (struct dir_entry *) buf;
		     (u8 *) e < (buf + sizeof(buf));
		     e++, entry_cnt++) {
			if (e->attrs & 0xde || e->name[0] & 0x80)
				continue;

			//print("entry:\n");
			//hex_dump(e, sizeof(*e));

			if (str_equal(e->name, filename)) {
				memcpy(&current_file, e, sizeof(*e));
				return 0;
			}
		}
	}

	print("file not found\n");
	return -1;
}

