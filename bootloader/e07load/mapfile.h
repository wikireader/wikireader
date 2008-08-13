#ifndef MAPFILE_H
#define MAPFILE_H

#define MAX_MAPFILE_ENTRIES 64

struct mapfile_entry
{
	unsigned long  addr;
	ssize_t        size;
	unsigned char *data;
	char          *fname;
} mapfile_entry[MAX_MAPFILE_ENTRIES];

int mapfile_parse(const char *fname);
int mapfile_write_eeprom(int ttyfd);

extern int num_mapfile_entries;

#endif /* MAPFILE_H */
