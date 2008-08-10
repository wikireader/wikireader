#define BYTES_PER_SECTOR 512

int sdcard_init(void);
int sdcard_read_sector(unsigned int sector, unsigned char *buf);

