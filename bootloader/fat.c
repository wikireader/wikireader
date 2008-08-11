#include "spi.h"
#include "sdcard.h"
#include "misc.h"

#define FAT_ADDR_RESERVED_SECTORS	0x0e
#define FAT_ADDR_NUM_FATS		0x10

struct partition_record {
	unsigned char status;
	unsigned int  chs_first:24;
	unsigned char type;
	unsigned int  chs_last:24;
	unsigned int  first_sector;
	unsigned int  n_sectors;
} __attribute__((packed));

struct boot_sector {
	unsigned char  jump_instr[3];
	unsigned char  oem_name[8];
	unsigned short bytes_per_sector;
	unsigned char  sectors_per_cluster;
	unsigned short reserved_sectors;
	unsigned char  num_fats;
	unsigned short max_root_entries;
	unsigned short  total_sectors;
	unsigned char  media_descriptor;
	unsigned short sectors_per_fat;
	/* not interested in the rest */
	unsigned char  unused[512 - 26];
	/* except for the signature */
	unsigned short signature;
} __attribute__((packed));

struct dir_entry {
	unsigned char  name[8];
	unsigned char  ext[3];
	unsigned char  attrs;
	unsigned char  reserved;
	unsigned char  ignored[13];
	unsigned short first_cluster;
	unsigned int   file_size;
}__attribute__((packed));

static struct boot_sector boot;
static unsigned int fat_start;
static unsigned int first_cluster_sector;
static unsigned int root_entry;
static unsigned char buf[BYTES_PER_SECTOR];

#define CLUSTER_TO_SECTOR(c)	\
	(first_cluster_sector + (c - 2) * boot.sectors_per_cluster)

/* returns the first sector of a given partition number,
 * read from the partition table */
static int read_mbr(int partition_num)
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

static int read_bootsector(int sector)
{
	sdcard_read_sector(sector, (unsigned char *) &boot);

	if (boot.bytes_per_sector != BYTES_PER_SECTOR) {
		print("invalid number of bytes per sector, bummer.\n");
		return -1;
	}

	if (boot.signature != 0xaa55) {
		print("invalid boot sector signature\n");
		return -1;
	}

	hex_dump(&boot, 512);
	
	fat_start = sector + boot.reserved_sectors;
	root_entry = fat_start + (boot.num_fats * boot.sectors_per_fat);
	first_cluster_sector = root_entry
		+ 32; //boot.max_root_entries / (BYTES_PER_SECTOR / sizeof(struct dir_entry));

	return 0;
}

int fat_init(int partition_num)
{
	int first_sector;

	root_entry = -1;
	first_sector = read_mbr(partition_num);

	if (first_sector < 0)
		/* if this failed, we maybe have a FAT FS at the first sector
		 * and no partiton table at all. Let's try that ... */
		first_sector = 0;

	return read_bootsector(first_sector);
}

#define ENTRIES_PER_FAT_SECTOR (256)

static int get_fat_entry(unsigned int cluster)
{
	unsigned short *fat = (unsigned short *) buf;
	unsigned int sector = fat_start;

	sector += cluster >> 8;
	sdcard_read_sector(sector, buf);
	return fat[cluster & 0xff];
}

static int load_file(struct dir_entry *e, char *dest, int maxsize)
{
	unsigned int cluster = e->first_cluster;

	if (maxsize > e->file_size)
		maxsize = e->file_size;

	/* alignment */
	maxsize &= ~(BYTES_PER_SECTOR - 1);
	maxsize +=   BYTES_PER_SECTOR;

	while (maxsize) {
		int i, sector = CLUSTER_TO_SECTOR(cluster);
		
		for (i = 0; i < boot.sectors_per_cluster; i++) {
			sdcard_read_sector(sector++, dest);
			dest += BYTES_PER_SECTOR;
			maxsize -= BYTES_PER_SECTOR;

			if (!maxsize)
				break;
		}

		/* we need to read more bytes from another cluster. */
		cluster = get_fat_entry(cluster);
	}

	return 0;
}

static int str_equal(const char *s1, const char *s2)
{
	while (*s1 && *s2)
		if (*s1++ != *s2++)
			return 0;

	return 1;
}

/* currently, this function only supports files located at the root directory */
int fat_read_file(const char *filename, char *dest, int maxsize)
{
	unsigned int entry_cnt = 0;
	unsigned int sector = root_entry;
	struct dir_entry *e;

	if (root_entry < 0)
		return -1;

	while (entry_cnt < boot.max_root_entries) {
		sdcard_read_sector(sector++, buf);

		for (e = (struct dir_entry *) buf;
		     (unsigned char *) e < (buf + sizeof(buf)); 
		     e++, entry_cnt++) {
			if (e->attrs & 0xde || e->name[0] & 0x80)
				continue;

			//print("entry:\n");
			//hex_dump(e, sizeof(*e));

			if (str_equal(e->name, filename))
				return load_file(e, dest, maxsize);
		}
	}

	print("file not found\n");
	return -1;
}

