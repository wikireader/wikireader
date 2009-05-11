//
// Authors:	Wolfgang Spraul <wolfgang@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <guilib.h>
#include <malloc-simple.h>
#include "file-io.h"
#include "wom_reader.h"
#include "tff.h"
#include "misc.h"

#define DBG_WOM_READER 0

struct wom_file
{
	// sort for better alignment
	uint8_t page_buffer[WOM_PAGE_SIZE];
	uint32_t cur_search_page, next_search_offset;
	wom_article_index_t article_idx;
	FIL fileh;
	wom_header_t hdr;
};

wom_file_t* wom_open(const char* filename)
{
	wom_file_t* womh;
	UINT num_read;
	FRESULT fr;

	womh = (wom_file_t*) malloc_simple(sizeof(*womh), MEM_TAG_WOMR_M1);
	if (!womh) goto xout;
	memset(womh, 0, sizeof(*womh));
	fr = f_open(&womh->fileh, filename, FA_READ);
	if (fr != FR_OK) {
		DP(1, ("X wom\tf_open() '%s' failed with code %d\n", filename, fr));
		goto xout_womh;
	}
	fr = f_read(&womh->fileh, &womh->hdr, sizeof(womh->hdr), &num_read);
	if (fr != FR_OK || num_read != sizeof(womh->hdr)) {
		DP(1, ("X wom\tf_read() %i bytes failed, code %d / num_read %i\n", sizeof(womh->hdr), fr, num_read));
		goto xout_fileh;
	}

	DP(DBG_WOM_READER, ("O wom\twom_open() '%s' succeeded.\n", filename));
	return womh;

xout_fileh:
	f_close(&womh->fileh);
xout_womh:
	free_simple(womh, MEM_TAG_WOMR_F1);
xout:
	DP(1, ("X wom\twom_open() '%s' failed.\n", filename));
	return 0;
}

void wom_close(wom_file_t* womh)
{
	if (!womh) return;
	f_close(&womh->fileh);
	free_simple(womh, MEM_TAG_WOMR_F2);
}

const wom_article_index_t* wom_find_article(wom_file_t* womh, const char* search_string, size_t search_str_len)
{
	UINT num_read;
	FRESULT fr;

	DP(DBG_WOM_READER, ("O wom_find_article('%.*s')\n", search_str_len, search_string));
	if (!womh) goto xout;
	for (womh->cur_search_page = womh->hdr.index_first_page;
		womh->cur_search_page < womh->hdr.index_first_page + womh->hdr.index_num_pages; womh->cur_search_page++) {
		if (f_lseek(&womh->fileh, womh->cur_search_page * WOM_PAGE_SIZE) != FR_OK) goto xout;
		fr = f_read(&womh->fileh, womh->page_buffer, WOM_PAGE_SIZE, &num_read);
		if (fr != FR_OK || num_read != WOM_PAGE_SIZE) goto xout;
		womh->next_search_offset = 0;
		while (womh->next_search_offset <= WOM_PAGE_SIZE-6) {
			womh->article_idx.offset_into_articles = __get_unaligned_4(
				&womh->page_buffer[womh->next_search_offset + WOM_INDEX_OFF_INTO_ARTICLE]);
			womh->article_idx.uri_len = __get_unaligned_2(
				&womh->page_buffer[womh->next_search_offset + WOM_INDEX_URI_LEN]);
			womh->article_idx.abbreviated_uri = &womh->page_buffer[womh->next_search_offset + WOM_INDEX_ABBREVIATED_URI];

			if (womh->article_idx.offset_into_articles == END_OF_INDEX_PAGE)
				break;
			womh->next_search_offset += 6 + womh->article_idx.uri_len;
			if (!strncasecmp(womh->article_idx.abbreviated_uri, search_string, min(womh->article_idx.uri_len, search_str_len))) {
				DP(DBG_WOM_READER, ("O Found '%.*s' (uri_len %u) at page %i off %i.\n",
					womh->article_idx.uri_len, womh->article_idx.abbreviated_uri, womh->article_idx.uri_len,
					womh->cur_search_page, womh->next_search_offset - 6 - womh->article_idx.uri_len));
				return &womh->article_idx;
			}
		}
	}
	DP(DBG_WOM_READER, ("O No match.\n"));
	return 0;
xout:
	DX();
	return 0;
}

const wom_article_index_t* wom_get_next_article(wom_file_t* womh)
{
	UINT num_read;
	FRESULT fr;

	DP(DBG_WOM_READER, ("O wom_get_next_article()\n"));
	if (!womh) goto xout;
	// Upon entry, womh->page_buffer is assumed to contain womh->cur_search_page, and
	// womh->next_search_offset points to where the next index should be.
	while (womh->cur_search_page < womh->hdr.index_first_page + womh->hdr.index_num_pages) {
		while (womh->next_search_offset <= WOM_PAGE_SIZE-6) {
			womh->article_idx.offset_into_articles = __get_unaligned_4(
				&womh->page_buffer[womh->next_search_offset + WOM_INDEX_OFF_INTO_ARTICLE]);
			womh->article_idx.uri_len = __get_unaligned_2(
				&womh->page_buffer[womh->next_search_offset + WOM_INDEX_URI_LEN]);
			womh->article_idx.abbreviated_uri = &womh->page_buffer[womh->next_search_offset + WOM_INDEX_ABBREVIATED_URI];

			if (womh->article_idx.offset_into_articles == END_OF_INDEX_PAGE)
				break;
			womh->next_search_offset += 6 + womh->article_idx.uri_len;
			DP(DBG_WOM_READER, ("O Found '%.*s' (uri_len %u) at page %i off %i.\n",
				womh->article_idx.uri_len, womh->article_idx.abbreviated_uri, womh->article_idx.uri_len,
				womh->cur_search_page, womh->next_search_offset - 6 - womh->article_idx.uri_len));
			return &womh->article_idx;
		}
		womh->cur_search_page++;
		if (f_lseek(&womh->fileh, womh->cur_search_page * WOM_PAGE_SIZE) != FR_OK) goto xout;
		fr = f_read(&womh->fileh, womh->page_buffer, WOM_PAGE_SIZE, &num_read);
		if (fr != FR_OK || num_read != WOM_PAGE_SIZE) goto xout;
		womh->next_search_offset = 0;
	}
	DP(DBG_WOM_READER, ("O No more match.\n"));
	return 0;
xout:
	DX();
	return 0;
}

void wom_draw(wom_file_t* womh, uint32_t offset_into_articles, uint8_t* frame_buffer_dest, int32_t y_start_in_article, int32_t lines_to_draw)
{
	UINT num_read;
	uint8_t cur_x;
	uint32_t cur_y, bitmap_off;
	int i, j;
	FRESULT fr;

	if (!womh) goto xout;
	DP(DBG_WOM_READER, ("O wom\twom_draw() offset_into_articles %u y_start %i lines_to_draw %i\n",
		offset_into_articles, y_start_in_article, lines_to_draw));
	if (f_lseek(&womh->fileh, offset_into_articles) != FR_OK) goto xout;

	cur_x = cur_y = 0;
	guilib_fb_lock();
	guilib_clear();
		
	for (i = 0;; i++) {
		if (i == 0 || i >= WOM_PAGE_SIZE) {
			i = 0;
			fr = f_read(&womh->fileh, womh->page_buffer, WOM_PAGE_SIZE, &num_read);
			if (fr != FR_OK || num_read != WOM_PAGE_SIZE) goto xout;
		}
		if (womh->page_buffer[i] == WOM_END_OF_ARTICLE)
			break;
		if (womh->page_buffer[i] == WOM_Y_ADVANCE_ONLY) {
			cur_y += (int8_t) womh->page_buffer[++i];
			continue;
		}
		if (womh->page_buffer[i] == WOM_PAGE_PADDING) {
			i = WOM_PAGE_SIZE;
			continue;
		}
		cur_x += womh->page_buffer[i];
//printf("1 cur_y %lu\n", cur_y);
		cur_y += (int8_t) womh->page_buffer[i+1];
//printf("2 cur_y %lu\n", cur_y);
		bitmap_off = __get_unaligned_4(&womh->page_buffer[i+2]);
		i+=5;

		if (cur_y >= y_start_in_article + lines_to_draw + 300 /* hack */) {
//printf("cur_y %lu y_start %i lines_to_draw %i\n", cur_y, y_start_in_article, lines_to_draw);
break;
}

		if (bitmap_off) {
			uint8_t bits[2+64];
			int bytes_per_line, bit_x, bit_y;

			if (f_lseek(&womh->fileh, bitmap_off) != FR_OK) continue;
			fr = f_read(&womh->fileh, bits, sizeof(bits), &num_read);
			if (fr != FR_OK || num_read != sizeof(bits)) continue;
			if (cur_y + bits[1] < y_start_in_article) continue;
			bytes_per_line = (bits[0]+7)/8;
			for (bit_y = 0; bit_y < bits[1]; bit_y++) {
				if (cur_y + bit_y >= y_start_in_article + lines_to_draw)
					break;
				for (bit_x = 0; bit_x < bytes_per_line; bit_x++) {
					uint8_t this_byte = bits[2+bit_y*bytes_per_line + bit_x];
					for (j = 0; j < 8; j++) {
						if (this_byte & 0x80>>j)
							guilib_set_pixel(cur_x + bit_x*8 + j, cur_y+bit_y-y_start_in_article, 1);
					}
				}
			}
		}
	}
	guilib_fb_unlock();
	return;
xout:
	DX();
}
