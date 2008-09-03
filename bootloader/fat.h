#ifndef FAT_H
#define FAT_H

struct partition_record {
	u8  status;
	u32 chs_first:24;
	u8  type;
	u32 chs_last:24;
	u32 first_sector;
	u32 n_sectors;
} __attribute__((packed));

struct boot_sector {
	u8  jump_instr[3];
	u8  oem_name[8];
	u16 bytes_per_sector;
	u8  sectors_per_cluster;
	u16 reserved_sectors;
	u8  num_fats;
	u16 max_root_entries;
	u16 total_sectors;
	u8  media_descriptor;
	u16 sectors_per_fat16;
	u16 sectors_per_track;
	u16 num_heads;
	u32 hidden_sectors;
	u32 total_sectors2;
	/* fat32 specific extensions from here */
	u32 sectors_per_fat32;
	u16 fat_flags;
	u16 version;
	u32 root_cluster;
	u16 fs_info_sector;
	u16 bootsector_copy;
	u8  reserved[12];
	u8  pyhsical_driver_num;
	u8  reserved2;
	u8  extended_boot_signature;
	u32 id;
	u8  volume_label[11];
	u8  fat_type[8];
	u8  boot_code[420];
	/* ... */
	u16 signature;
} __attribute__((packed));

struct dir_entry {
	u8  name[8];
	u8  ext[3];
	u8  attrs;
	u8  reserved;
	u8  ignored[13];
	u16 first_cluster;
	u32 file_size;
}__attribute__((packed));

int fat_init(u32 partition);
int fat_open_file(const u8 *filename);
int fat_read_file(u32 offset, u8 *dest, u32 maxsize);

#endif /* FAT_H */

