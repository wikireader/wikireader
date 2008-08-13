#ifndef FAT_H
#define FAT_H

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
	unsigned short total_sectors;
	unsigned char  media_descriptor;
	unsigned short sectors_per_fat16;
	unsigned short sectors_per_track;
	unsigned short num_heads;
	unsigned long  hidden_sectors;
	unsigned long  total_sectors2;
	/* fat32 specific extensions */
	unsigned long  sectors_per_fat32;
	unsigned short fat_flags;
	unsigned short version;
	unsigned long  root_cluster;
	unsigned short fs_info_sector;
	unsigned short bootsector_copy;
	unsigned char  reserved[12];
	unsigned char  pyhsical_driver_num;
	unsigned char  reserved2;
	unsigned char  extended_boot_signature;
	unsigned long  id;
	unsigned char  volume_label[11];
	unsigned char  fat_type[8];
	unsigned char  boot_code[420];
	/* ... */
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

int fat_init(int partition);
int fat_read_file(const char *filename, char *dest, int maxsize);

#endif /* FAT_H */

