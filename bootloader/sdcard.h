#ifndef SDCARD_H
#define SDCARD_H

#define BYTES_PER_SECTOR 512

int sdcard_init(void);
int sdcard_read_sector(u32 sector, u8 *buf);

#endif /* SDCARD_H */

