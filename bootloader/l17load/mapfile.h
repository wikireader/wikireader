#ifndef MAPFILE_H
#define MAPFILE_H

int mapfile_parse(const char *fname);
int mapfile_write_eeprom(int ttyfd);

extern int num_mapfile_entries;

#endif /* MAPFILE_H */
