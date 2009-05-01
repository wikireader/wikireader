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
#include <stdio.h>
#include <time.h>
#include "msg.h"
#include "wom_file_format.h"

#define DBG_WOM_CREATOR 0

#define STR_BUF_LEN	4096 // arbitrary length, used for parsing .work and .blib files, paths, etc.
#define NUM_BITMAP_SLOTS	2048 // enough for the ca. 1600 characters we use in 4 Liberation fonts
#define NUM_TITLE_SLOTS		5000 // tbd: needs to become dynamic, support up to 800,000 or so

typedef struct font_bitmaps
{
	char font_size[64];
	int32_t font_code;
	int32_t advance_x, advance_y, bitmap_left_bearing, bitmap_top_bearing, glyph_height, glyph_width;
	int pbm_width, pbm_height;
	uint8_t bits[64];
	uint32_t bitmaps_file_off; // offset into the bitmaps in the output file, to where this bitmap is located (0 if bitmap skipped)
} font_bitmaps_t;
font_bitmaps_t font_bitmaps[NUM_BITMAP_SLOTS];
int32_t num_bitmaps_avail = NUM_BITMAP_SLOTS;
int32_t num_bitmaps_used = 0;

typedef struct article_titles
{
	char title[256]; // longest title in the 200810 dump is 214 characters
	char hash[64]; // hash is 40-bytes hex string
	char blib_path[256];
	uint32_t index_file_off; // offset into the index in the output file, to where the index entry for this article is located
} article_titles_t;
article_titles_t article_titles[NUM_TITLE_SLOTS];
int32_t num_titles_avail = NUM_TITLE_SLOTS;
int32_t num_titles_used = 0;

typedef struct stats
{
	time_t start_time, stop_time;
	long unsigned int num_work_files;
	long unsigned int max_chars_in_title;
	long unsigned int min_chars_in_title;
	long unsigned int num_characters;
	long unsigned int num_skipped_chars;
	long unsigned int max_characters_per_article;
	long unsigned int min_characters_per_article;
} stats_t;

static int load_bitmap(font_bitmaps_t* font_bitmaps, int num_bitmaps_avail, int* num_bitmaps_used, const char* work_path_base, const char* font_and_size, int font_code);
static int fscan_num(const char* path, int32_t* number);

int main(int argc, char** argv)
{
	FILE* dest_f = 0;
	FILE* work_f = 0;
	FILE* blib_f = 0;
	uint8_t wom_page_buf[WOM_PAGE_SIZE];
	char line_buf[STR_BUF_LEN], hash[STR_BUF_LEN], article_title[STR_BUF_LEN], work_path_base[STR_BUF_LEN], font_and_size[STR_BUF_LEN];
	uint32_t cur_page, index_first_page, bitmaps_first_page, articles_first_page, cur_dest_off, dest_file_len, last_index_off, last_bitmap_off;
	char blib_path[256];
	int cur_work_arg, i, j, x_coord, y_coord, font_code;
	stats_t stats = {0};

	if (argc < 3) {
		printf("\nUsage: wom_creator OUTPUT WORK...\n\n");
		return 1;
	}
	stats.start_time = time(0);
	dest_f = fopen(argv[1], "w+b");
	if (!dest_f) {
		fprintf(stderr, "Error opening output file '%s'\n", argv[1]);
		goto xout;
	}
	//
	// 1. Iterate over all .work, .blib and font files to determine index and bitmaps.
	//
	for (cur_work_arg = 2; cur_work_arg < argc; cur_work_arg++) {

		// tbd: POSIX dirname() is an alternative...
		strcpy(work_path_base, argv[cur_work_arg]);
		i = strlen(work_path_base);
		while (i && work_path_base[i] != '/') i--;
		work_path_base[i+1] = 0;

		work_f = fopen(argv[cur_work_arg], "r");
		if (!work_f) {
			fprintf(stderr, "X Error opening .work file '%s'\n", argv[cur_work_arg]);
			continue;
		}
		stats.num_work_files++;
		while (fgets(line_buf, sizeof(line_buf), work_f)) {
			// Note that multiple article titles can point to the same blib file (same article content).
			// For example in 002808.work, there are 1000 titles but only 962 unique article contents.
			// for example 'Zeus_Alexikakos' and 'Zeus_Alexicacus' point to the same article content.

			if (sscanf(line_buf, "%s http://127.0.0.1/mediawiki/index.php/%s", hash, article_title) != 2) {
				fprintf(stderr, "X Error parsing .work line '%s'.\n", line_buf);
				continue;
			}
			if (num_titles_used >= num_titles_avail) {
				fprintf(stderr, "X Ran out of title buffer.\n");
				break;
			}
			sprintf(blib_path, "%sarticles/%c/%.2s/%s.blib", work_path_base, hash[0], &hash[1], hash);

			strcpy(article_titles[num_titles_used].title, article_title);
			strcpy(article_titles[num_titles_used].hash, hash);
			strcpy(article_titles[num_titles_used].blib_path, blib_path);
			num_titles_used++;

			DP(DBG_WOM_CREATOR, (MSG_INFO, "O Processing article '%s' in '%s'\n", article_title, blib_path));
			stats.min_chars_in_title = stats.min_chars_in_title ? min(stats.min_chars_in_title, strlen(article_title)) : strlen(article_title);
			stats.max_chars_in_title = max(stats.max_chars_in_title, strlen(article_title));
			blib_f = fopen(blib_path, "r");
			if (!blib_f) {
				fprintf(stderr, "X Error opening blib file '%s' (article '%s').\n", blib_path, article_title);
				continue;
			}
			while (fgets(line_buf, sizeof(line_buf), blib_f)) {
				if (!strcmp(line_buf, "0,0,0,0\n")) continue;
				if (sscanf(line_buf, "%i,%i,%[^,],%i", &x_coord, &y_coord, font_and_size, &font_code) != 4) {
					fprintf(stderr, "X Error parsing .blib line '%s'.\n", line_buf);
					continue;
				}
				stats.num_characters++;
				if (strncmp(font_and_size, "Liberation_Sans", 15 /* strlen */)) {
					stats.num_skipped_chars++;
					DP(DBG_WOM_CREATOR, (MSG_INFO, "X Skipping non-'Liberation Sans' character ('%s').\n", font_and_size));
					continue;
				}

				if (!load_bitmap(font_bitmaps, num_bitmaps_avail, &num_bitmaps_used, work_path_base, font_and_size, font_code)) {
					printf("X Error loading font bitmap.\n");
					continue;
				}
			}
			fclose(blib_f);
//break; // temporarily only one article
		}
		fclose(work_f);
		work_f = 0;
	}

	//
	// 2. Write index (ca. 1% of total output size)
	//
	{
		int16_t uri_len;

		cur_page = 1; // page 0 is for header
		index_first_page = cur_page;
		if (fseek(dest_f, cur_page * WOM_PAGE_SIZE, SEEK_SET))
			fprintf(stderr, "X Error seeking in file.\n");
		for (i = 0; i < num_titles_used; i++) {
			// offset_into_articles unknown and can only be written after 2nd run over articles below
			article_titles[i].index_file_off = ftell(dest_f);
			if (fseek(dest_f, sizeof(uint32_t), SEEK_CUR))
				fprintf(stderr, "X Error seeking in file.\n");
			// write uri_len
			uri_len = strlen(article_titles[i].title);
			if (fwrite(&uri_len, 1 /* size */, sizeof(uri_len), dest_f) != sizeof(uri_len))
				fprintf(stderr, "X Error writing file.\n");
			// write abbreviated_uri[]
			if (fwrite(article_titles[i].title, 1 /* size */, uri_len, dest_f) != uri_len)
				fprintf(stderr, "X Error writing file.\n");
		}
		last_index_off = ftell(dest_f);
		cur_page = (last_index_off + WOM_PAGE_SIZE - 1) / WOM_PAGE_SIZE;
	}
	//
	// 3. Write bitmaps (1-5% of total output size)
	//
	{
		wom_bitmap_t* wom_bitmap = (wom_bitmap_t*) wom_page_buf;
		int bitmap_size;

		bitmaps_first_page = cur_page;
		if (fseek(dest_f, cur_page * WOM_PAGE_SIZE, SEEK_SET))
			fprintf(stderr, "X Error seeking in file.\n");
		for (i = 0; i < num_bitmaps_used; i++) {
			if (!font_bitmaps[i].pbm_width || !font_bitmaps[i].pbm_height
			    || font_bitmaps[i].pbm_width >= 256 || font_bitmaps[i].pbm_height >= 256) {
				fprintf(stderr, "X Bitmap size problem (width %d height %d).\n", font_bitmaps[i].pbm_width, font_bitmaps[i].pbm_height);
				continue;
			}
			font_bitmaps[i].bitmaps_file_off = ftell(dest_f);
			wom_bitmap->width = font_bitmaps[i].pbm_width;
			wom_bitmap->height = font_bitmaps[i].pbm_height;
			bitmap_size = (wom_bitmap->width + 7)/8 * wom_bitmap->height;
			memcpy(wom_bitmap->bits, font_bitmaps[i].bits, bitmap_size);
			if (fwrite(wom_bitmap, 1 /* size */, 2*sizeof(uint8_t) + bitmap_size, dest_f) != 2*sizeof(uint8_t) + bitmap_size)
				fprintf(stderr, "X Error writing file.\n");
		}
		last_bitmap_off = ftell(dest_f);
		cur_page = (last_bitmap_off + WOM_PAGE_SIZE - 1) / WOM_PAGE_SIZE;
	}
	//
	// 4. Write articles (>90% of total output size)
	//
	{
		uint32_t article_start_off;
		uint8_t cur_display_x;
		uint32_t cur_display_y;

		articles_first_page = cur_page;
		if (fseek(dest_f, cur_page * WOM_PAGE_SIZE, SEEK_SET))
			fprintf(stderr, "X Error seeking in file.\n");
		for (i = 0; i < num_titles_used; i++) {
			if (!article_titles[i].index_file_off) {
				fprintf(stderr, "X No index for this article?\n");
				continue;
			}
			article_start_off = ftell(dest_f);

			// Open .blib file and iterate over characters
			blib_f = fopen(article_titles[i].blib_path, "r");
			if (!blib_f) {
				fprintf(stderr, "X Error opening blib file '%s' (article '%s').\n",
					article_titles[i].blib_path, article_titles[i].title);
				continue;
			}
			cur_display_x = 0;
			cur_display_y = 0;
			while (fgets(line_buf, sizeof(line_buf), blib_f)) {
				if (!strcmp(line_buf, "0,0,0,0\n")) continue;
				if (sscanf(line_buf, "%i,%i,%[^,],%i", &x_coord, &y_coord, font_and_size, &font_code) != 4) {
					fprintf(stderr, "X Error parsing .blib line '%s'.\n", line_buf);
					continue;
				}
				if (x_coord < 0 || x_coord >= 240 || y_coord < 0) {
					fprintf(stderr, "X x/y out of bounds (%i/%i)\n", x_coord, y_coord);
					continue;
				}
				for (j = 0; j < num_bitmaps_used; j++) {
					if (!strcmp(font_bitmaps[j].font_size, font_and_size)
					    && font_bitmaps[j].font_code == font_code)
						break;
				}
				if (j >= num_bitmaps_used) {
					DP(DBG_WOM_CREATOR, (MSG_INFO, "X Skipping missing glyph.\n"));
					continue;
				}
				// tbd: we should make y ascending only, determine the baseline etc.
				if (y_coord + 128 < cur_display_y) {
					DP(1, (MSG_INFO, "X y_coord out of range (y_coord %i cur_display_y %i)\n", y_coord, cur_display_y));
					continue;
				}
				DP(y_coord >= cur_display_y + 128, (MSG_INFO, "X Long empty section (%i rows)\n", y_coord - cur_display_y));
// tbd: if y coordinates are ascending only, we may want to cut out large empty vertical spaces
				while (y_coord >= cur_display_y + 128) {
					wom_page_buf[0] = WOM_Y_ADVANCE_ONLY;
					if (fwrite(wom_page_buf, 1 /* size */, sizeof(uint8_t), dest_f) != sizeof(uint8_t)) {
						fprintf(stderr, "X Error writing to file.\n");
						continue;
					}
					cur_display_y += 127;
				}
				if (x_coord >= cur_display_x)
					wom_page_buf[0] = x_coord - cur_display_x;
				else
					wom_page_buf[0] = 256 - (cur_display_x - x_coord);
				cur_display_x = x_coord;
				wom_page_buf[1] = (int8_t) (y_coord - cur_display_y);
				cur_display_y = y_coord;
				// write uint32_t offset after x and y coordinates
				*(uint32_t*)&wom_page_buf[2] = font_bitmaps[j].bitmaps_file_off;
				if (fwrite(wom_page_buf, 1 /* size */, 6, dest_f) != 6) {
					fprintf(stderr, "X Error writing to file.\n");
					continue;
				}
			}
			// write end of article marker
			wom_page_buf[0] = WOM_END_OF_ARTICLE; // delta_x value to mark end of article
			if (fwrite(wom_page_buf, 1 /* size */, sizeof(uint8_t), dest_f) != sizeof(uint8_t)) {
				fprintf(stderr, "X Error writing to file.\n");
				continue;
			}
				
			// write offset of this article into index array earlier in the file
			cur_dest_off = ftell(dest_f);
			if (fseek(dest_f, article_titles[i].index_file_off, SEEK_SET)) {
				fprintf(stderr, "X Error seeking in file.\n");
				continue;
			}
			if (fwrite(&article_start_off, 1 /* size */, sizeof(article_start_off), dest_f) != sizeof(article_start_off)) {
				fseek(dest_f, cur_dest_off, SEEK_SET);
				fprintf(stderr, "X Error writing to file.\n");
				continue;
			}
			if (fseek(dest_f, cur_dest_off, SEEK_SET)) {
				fprintf(stderr, "X Error seeking in file.\n");
				continue;
			}
		}
	}
	//
	// 5. Write header
	//
	{
		wom_header_t* wom_hdr = (wom_header_t*) wom_page_buf;
		memset(wom_page_buf, 0, sizeof(wom_page_buf));
		strcpy(wom_hdr->mime_type, WOM_MIME_TYPE);
		wom_hdr->binary_magic = WOM_BINARY_MAGIC;
		wom_hdr->file_format_version = WOM_FORMAT_VERSION;
		wom_hdr->page_size = WOM_PAGE_SIZE;
		wom_hdr->index_first_page = index_first_page;
		wom_hdr->index_num_pages = bitmaps_first_page - index_first_page;
		wom_hdr->index_num_entries = num_titles_used;
		wom_hdr->bitmaps_first_page = bitmaps_first_page;
		wom_hdr->bitmaps_num_pages = articles_first_page - bitmaps_first_page;
		wom_hdr->bitmaps_num_entries = num_bitmaps_used;
		wom_hdr->articles_first_page = articles_first_page;
		wom_hdr->articles_num_pages = cur_page - articles_first_page;

		if (fseek(dest_f, 0 /* offset */, SEEK_SET))
			fprintf(stderr, "X Error seeking in file.\n");
		if (fwrite(wom_page_buf, 1 /* size */, sizeof(wom_page_buf), dest_f) != sizeof(wom_page_buf))
			fprintf(stderr, "X Error writing file.\n");
	}
		
	// determine file length
	fseek(dest_f, 0, SEEK_END);
	dest_file_len = ftell(dest_f);

	fclose(dest_f);
	stats.stop_time = time(0);

	//
	// 6. Printf stats.
	//
	printf("\nwom_creator stats for this run:\n\n");
	printf("  duration of run:                     %lu seconds\n", stats.stop_time - stats.start_time);
	printf("  num_work_files:                      %lu\n", stats.num_work_files);
	printf("  num_titles:                          %lu\n", num_titles_used);
	printf("  article title length (min/max):      %lu/%lu\n", stats.min_chars_in_title, stats.max_chars_in_title);
	printf("  num_characters:                      %lu (avg %lu/article)\n", stats.num_characters, stats.num_characters / num_titles_used);
	printf("  num skipped chars from rare fonts:   %lu\n", stats.num_skipped_chars);
	printf("  num unique glyphs:                   %lu\n", num_bitmaps_used);
	printf("  output file length:                  %lu\n", dest_file_len);
	printf("  index start/length:                  %lxh/%lu\n", index_first_page * WOM_PAGE_SIZE, last_index_off - index_first_page * WOM_PAGE_SIZE);
	printf("  bitmaps start/length:                %lxh/%lu\n", bitmaps_first_page * WOM_PAGE_SIZE, last_bitmap_off - bitmaps_first_page * WOM_PAGE_SIZE);
	printf("  articles start/length:               %lxh/%lu\n", articles_first_page * WOM_PAGE_SIZE, dest_file_len - articles_first_page * WOM_PAGE_SIZE);
	printf("\n");

	return 0;

xout:
	if (work_f) fclose(work_f);
	if (dest_f) fclose(dest_f);
	printf("X wom_creator failed.\n");
	return 1;
}

static int load_bitmap(font_bitmaps_t* font_bitmaps, int num_bitmaps_avail, int* num_bitmaps_used,
		const char* work_path_base, const char* font_and_size, int font_code)
{
	char line_buf[1024];
	uint32_t file_off, i, cur_x, cur_y, bytes_per_line;
	int32_t scanned_num;
	font_bitmaps_t* new_bitmap;
	FILE* pbm_f;

	if (*num_bitmaps_used >= num_bitmaps_avail)
		goto xout;
	for (i = 0; i < num_bitmaps_avail; i++) {
		if (!strcmp(font_bitmaps[i].font_size, font_and_size) && font_bitmaps[i].font_code == font_code) {
			// bitmap already loaded
			return 1;
		}
	}
	snprintf(line_buf, sizeof(line_buf), "%sfonts/%s/%d/", work_path_base, font_and_size, font_code);
	file_off = strlen(line_buf);

	new_bitmap = &font_bitmaps[*num_bitmaps_used];
	strcpy(new_bitmap->font_size, font_and_size);
	new_bitmap->font_code = font_code;

	strcpy(&line_buf[file_off], "advance_x");
	if (!fscan_num(line_buf, &scanned_num)) goto xout;
	new_bitmap->advance_x = scanned_num;

	strcpy(&line_buf[file_off], "advance_y");
	if (!fscan_num(line_buf, &scanned_num)) goto xout;
	new_bitmap->advance_y = scanned_num;

	strcpy(&line_buf[file_off], "bitmap_left_bearing");
	if (!fscan_num(line_buf, &scanned_num)) goto xout;
	new_bitmap->bitmap_left_bearing = scanned_num;

	strcpy(&line_buf[file_off], "bitmap_top_bearing");
	if (!fscan_num(line_buf, &scanned_num)) goto xout;
	new_bitmap->bitmap_top_bearing = scanned_num;

	strcpy(&line_buf[file_off], "glyph_height");
	if (!fscan_num(line_buf, &scanned_num)) goto xout;
	new_bitmap->glyph_height = scanned_num;

	strcpy(&line_buf[file_off], "glyph_width");
	if (!fscan_num(line_buf, &scanned_num)) goto xout;
	new_bitmap->glyph_width = scanned_num;

	// Read PBM file.

	strcpy(&line_buf[file_off], "bitmap.pbm");
	pbm_f = fopen(line_buf, "r");
	if (!pbm_f) goto xout;
	if (!fgets(line_buf, sizeof(line_buf), pbm_f)) goto xout_pbmf;
	if (strcmp(line_buf, "P1\n")) {
		fprintf(stderr, "X Unsupported PBM file, first line '%s'.\n", line_buf);
		goto xout_pbmf;
	}
	while (fgets(line_buf, sizeof(line_buf), pbm_f) && line_buf[0] == '#');
	if (sscanf(line_buf, "%i %i", &new_bitmap->pbm_width, &new_bitmap->pbm_height) != 2) goto xout_pbmf;
	cur_x = 0;
	cur_y = 0;
	bytes_per_line = (new_bitmap->pbm_width + 7) / 8;
	while (fgets(line_buf, sizeof(line_buf), pbm_f)) {
		for (i = 0; i < strlen(line_buf); i++) {
			if (line_buf[i] == '\n') break;
			if (line_buf[i] < '0' || line_buf[i] > '1') goto xout_pbmf;
			if (line_buf[i] == '1')
				new_bitmap->bits[cur_y * bytes_per_line + cur_x/8] |= 1<<(cur_x%8);
			cur_x++;
			if (cur_x % new_bitmap->pbm_width == 0) {
				cur_y++;
				cur_x = 0;
			}
		}
	}
	fclose(pbm_f);
	(*num_bitmaps_used)++;
	DP(DBG_WOM_CREATOR, (MSG_INFO, "O load_bitmap() '%s' code %d width %d height %d\n",
		new_bitmap->font_size, new_bitmap->font_code, new_bitmap->pbm_width, new_bitmap->pbm_height));
	return 1;
xout_pbmf:
	fclose(pbm_f);
xout:
	fprintf(stderr, "X load_bitmap() failed.\n");
	return 0;
}

static int fscan_num(const char* path, int32_t* number)
{
	FILE* f = fopen(path, "r");
	if (!f) goto xout;
	if (fscanf(f, "%li", (long int*) number) != 1) goto xout_f;
	fclose(f);
	return 1;
xout_f:
	fclose(f);
xout:
	fprintf(stderr, "X file_scan(%s) failed.\n", path);
	return 0;
}
