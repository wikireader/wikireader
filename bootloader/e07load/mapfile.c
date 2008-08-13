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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "eeprom.h"
#include "mapfile.h"
#include "misc.h"

int num_mapfile_entries = 0;

static int check_overlaps(void)
{
	int i, endptr = 0;

	for (i = 0; i < num_mapfile_entries; i++) {
		if (endptr > mapfile_entry[i].addr) {
			error("region #%d (%s) is overlapped by region #%d (%s, %lu bytes)\n",
				i, mapfile_entry[i].fname, 
				i-1, mapfile_entry[i-1].fname,
				(unsigned long) mapfile_entry[i-1].size);
			return -1;
		}

		endptr = mapfile_entry[i].addr + mapfile_entry[i].size;
	}

	return 0;
}

static unsigned char *read_file(const char *fname, ssize_t *len)
{
	unsigned char *buf;
	int file_fd, ret;
	struct stat statbuf;

	if (stat(fname, &statbuf) < 0) {
		error("unable to stat file %s: %s\n", fname, strerror(errno));
		return NULL;
	}

	*len = statbuf.st_size;
	buf = (unsigned char *) malloc(*len);
	
	file_fd = open(fname, O_RDONLY);
	if (file_fd < 0) {
		error("unable to open file %s: %s\n", fname, strerror(errno));
		return NULL;
	}

	ret = read(file_fd, buf, *len);
	close(file_fd);

	return buf;
}

static void dump_map(void)
{
	int i;

	msg("dumping EEPROM map, %d entries:\n\n", num_mapfile_entries);
	msg("\tstart address\tdata size\tsource file name\n");
	msg("\t-------------\t---------\t----------------\n");

	for (i = 0; i < num_mapfile_entries; i++)
		msg("\t0x%08x\t%lu\t\t%s\n",
			(unsigned int) mapfile_entry[i].addr,
			(unsigned long) mapfile_entry[i].size,
			mapfile_entry[i].fname);

	msg("\n");
}

int mapfile_write_eeprom(int ttyfd)
{
	int i;

	for (i = 0; i < num_mapfile_entries; i++) {
		struct mapfile_entry *e = mapfile_entry + i;

		debug("%s(): area #%d\n", __func__, i);

		if (write_eeprom(ttyfd, e->data, e->size, e->addr) < 0)
			return -1;

		if (verify_eeprom(ttyfd, e->data, e->size, e->addr) < 0)
			return -1;
	}

	return 0;
}

int mapfile_parse(const char *filename)
{
	char buf[512];
	FILE *f = fopen(filename, "r");
	
	if (!f) {
		error("unable to open file %s: %s\n", filename, strerror(errno));
		return -1;
	}

	debug("parsing mapfile %s\n", filename);
	bzero(mapfile_entry, sizeof(mapfile_entry));

	while (fgets(buf, sizeof(buf), f)) {
		char *addr, *fname;

		if (strlen(buf) == 0 || buf[0] == '#')
			continue;

		addr = strtok(buf, " \t");
		if (!addr)
			continue;

		fname = strtok(NULL, " \t");
		if (!fname)
			continue;

		strchomp(fname);

		mapfile_entry[num_mapfile_entries].addr = strtol(addr, NULL, 0);
		mapfile_entry[num_mapfile_entries].data = read_file(fname, &mapfile_entry[num_mapfile_entries].size);

		if (!mapfile_entry[num_mapfile_entries].data)
			return -1;
		
		mapfile_entry[num_mapfile_entries].fname = strdup(fname);
		num_mapfile_entries++;
	}

	if (verbose_flag)
		dump_map();

	if (num_mapfile_entries == 0) {
		error("no map entries found.\n");
		return -1;
	}

	return check_overlaps();
}

