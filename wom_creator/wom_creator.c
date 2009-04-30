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
#include "msg.h"
#include "wom_file_format.h"

#define DBG_WOM_CREATOR 1

#define STR_BUF_LEN	4096 // arbitrary length, used for parsing .work and .blib files, paths, etc.
#define NUM_BITMAP_SLOTS	2048 // enough for the ca. 1600 characters we use in 4 Liberation fonts

typedef struct font_bitmaps
{
	char font_size[64];
	int32_t font_code;
	int32_t advance_x, advance_y, bitmap_left_bearing, bitmap_top_bearing, glyph_height, glyph_width;
	int pbm_width, pbm_height;
	uint8_t bits[64];
} font_bitmaps_t;
font_bitmaps_t font_bitmaps[NUM_BITMAP_SLOTS];
int32_t num_bitmaps_avail = NUM_BITMAP_SLOTS;
int32_t num_bitmaps_used = 0;

static int load_bitmap(font_bitmaps_t* font_bitmaps, int num_bitmaps_avail, int* num_bitmaps_used, const char* work_path_base, const char* font_and_size, int font_code);
static int ftell_size(const char * file, size_t * size);
static int fscan_num(const char* path, int32_t* number);

int main(int argc, char** argv)
{
	FILE* dest_f = 0;
	FILE* work_f = 0;
	FILE* blib_f = 0;
	uint8_t wom_page_buf[WOM_PAGE_SIZE];
	char line_buf[STR_BUF_LEN], hash[STR_BUF_LEN], article_title[STR_BUF_LEN], work_path_base[STR_BUF_LEN], font_and_size[STR_BUF_LEN];
	int cur_work_arg, i, x_coord, y_coord, font_code;

	if (argc < 3) {
		printf("\nUsage: wom_creator OUTPUT WORK...\n\n");
		return 1;
	}
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
		while (fgets(line_buf, sizeof(line_buf), work_f)) {
			// Note that multiple article titles can point to the same blib file (same article content).
			// For example in 002808.work, there are 1000 titles but only 962 unique article contents.
			// for example 'Zeus_Alexikakos' and 'Zeus_Alexicacus' point to the same article content.

			if (sscanf(line_buf, "%s http://127.0.0.1/mediawiki/index.php/%s", hash, article_title) != 2) {
				fprintf(stderr, "X Error parsing .work line '%s'.\n", line_buf);
				continue;
			}
			sprintf(line_buf, "%sarticles/%c/%.2s/%s.blib", work_path_base, hash[0], &hash[1], hash);
			printf("O Processing article '%s' in '%s'\n", article_title, line_buf);
			blib_f = fopen(line_buf, "r");
			if (!blib_f) {
				fprintf(stderr, "X Error opening blib file '%s' (article '%s').\n", line_buf, article_title);
				continue;
			}
			while (fgets(line_buf, sizeof(line_buf), blib_f)) {
				if (!strcmp(line_buf, "0,0,0,0\n")) continue;
				if (sscanf(line_buf, "%i,%i,%[^,],%i", &x_coord, &y_coord, font_and_size, &font_code) != 4) {
					fprintf(stderr, "X Error parsing .blib line '%s'.\n", line_buf);
					continue;
				}
				if (strncmp(font_and_size, "Liberation_Sans", 15 /* strlen */)) {
					printf("X Skipping non-'Liberation Sans' character ('%s').\n", font_and_size);
					continue;
				}

				if (!load_bitmap(font_bitmaps, num_bitmaps_avail, &num_bitmaps_used, work_path_base, font_and_size, font_code)) {
					printf("X Error loading font bitmap.\n");
					continue;
				}
// 1/fonts/Liberation_Sans_9/214/
// advance_c, advance_y, bitmap_left_bearing, bitmap_top_bearing, glyph_height, glyph_width, bitmap.pbm
			}
			fclose(blib_f);
break; // temporarily only one article
		}
		fclose(work_f);
		work_f = 0;
	}
	//
	// 2. Write index (ca. 1% of total output size)
	//
	// 3. Write bitmaps (1-5% of total output size)
	//
	// 4. Write articles (>90% of total output size)
	//
	fclose(dest_f);
	return 0;

xout:
	if (work_f) fclose(work_f);
	if (dest_f) fclose(dest_f);
	printf("X wom_creator failed.\n");
	return 1;
}

// work/zecke-1/1/02808.work
//                articles/0/00/hash.blib
//                fonts/Liberation_Sans_9

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
	return 1;
xout_pbmf:
	fclose(pbm_f);
xout:
	fprintf(stderr, "X load_bitmap() failed.\n");
	return 0;
}

static int ftell_size(const char * file, size_t * size)
{
	FILE * f = fopen(file, "rb");
	if (!f) {
		fprintf(stderr, "X Error opening %s.\n", file);
		return -1;
	}
	if (fseek(f, 0, SEEK_END)) {
		fclose(f);
		fprintf(stderr, "X Error seeking to end of %s.\n", file);
		return -1;
	}
	*size = ftell(f);
	fclose(f);
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
