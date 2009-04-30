//
// Authors:	Wolfgang Spraul <wolfgang@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//           

#include <stdlib.h>
#include <string.h>
#include "file-io.h"
#include "wom_reader.h"

#define DBG_WOM_READER 1

/*
int wl_open(const char *filename, int flags);
void wl_close(int fd);
int wl_read(int fd, void *buf, unsigned int count);
int wl_write(int fd, void *buf, unsigned int count);
int wl_seek(int fd, unsigned int pos);
int wl_fsize(int fd, unsigned int *size);
unsigned int wl_tell(int fd);
*/

struct wom_file
{
	int fileh;
	uint8_t page_buffer[WOM_PAGE_SIZE];
};

wom_file_t* wom_open(const char* filename)
{
	wom_file_t* womh;

	womh = (wom_file_t*) malloc(sizeof(*womh));
	if (!womh) goto xout;
	womh->fileh = wl_open(filename, WL_O_RDONLY);
	if (womh->fileh == -1) goto xout_womh;
	
	DP(DBG_WOM_READER, (MSG_INFO, "O wom\twom_open() '%s' succeeded.\n", filename));
	return womh;
xout_womh:
	free(womh);
xout:
	DX();
	return 0;
}

void wom_close(wom_file_t* womh)
{
	if (!womh) return;
	wl_close(womh->fileh);
	free(womh);
}

const wom_index_entry_t* wom_find_article(wom_file_t* womh, const char* search_string, size_t search_str_len)
{
	wom_index_entry_t* idx = (wom_index_entry_t*) womh->page_buffer;
	idx->offset_into_articles = 0;
	strcpy(idx->abbreviated_uri, "en/Experiment");
	idx->uri_len = strlen(idx->abbreviated_uri);
	return idx;
}

const wom_index_entry_t* wom_get_next_article(wom_file_t* womh)
{
	return 0;
}

void wom_draw(wom_file_t* womh, uint32_t offset_into_articles, uint8_t* frame_buffer_dest, int32_t y_start_in_article, int32_t lines_to_draw)
{
}
