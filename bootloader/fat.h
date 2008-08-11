#ifndef FAT_H
#define FAT_H

int fat_init(int partition);
int fat_read_file(const char *filename, char *dest, int maxsize);

#endif /* FAT_H */

