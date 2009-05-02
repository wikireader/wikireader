//
// File format for the Openmoko Wikipedia offline reader.
//
// Authors:	Wolfgang Spraul <wolfgang@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//           

#include <inttypes.h>

#define WOM_MIME_TYPE		"application/x-openmoko-wikipedia"	// RFC 2045 MIME type
#define WOM_BINARY_MAGIC	0x07B507D9				// 1973,2009
#define WOM_FORMAT_VERSION	1

// SD card block sizes for 8 GiB SDHC cards seem to always be 512.
// On our factory SD card, we set a FAT32 cluster size of 64KiB to format the 8GiB card with 1 partition.
// 64KiB is the maximum FAT32 cluster size, Win98 and higher should support it (needs some testing).

// tbd: this needs more work, for example right now an article name must fit into one page. If the page size would go to, for example to 512 bytes, we would need to support article names over multiple pages.
#define WOM_PAGE_SIZE		4096

// tbd: right now I'm assuming 1-byte alignment. pragma pack does not seem to be supported by our S1C33 GCC.

// We support files up to 4GiB right now, in practice we stay below 2GiB to avoid problems with some Windows versions.
typedef struct wom_header
{
	char mime_type[64];		// WOM_MIME_TYPE
	uint32_t binary_magic;		// WOM_BINARY_MAGIC
	uint32_t file_format_version;	// WOM_FORMAT_VERSION
	// data_set identifies the contents of the file. When multiple files with the same 'set' of articles are found,
	// the reader will only use the file with the most recent time stamp.
	uint32_t data_set;
	uint32_t data_timestamp;	// TBD: what is ufs_time_t - time_t is seconds since Jan1, 1970
	// tbd: add a field for md5 checksum?
	uint32_t page_size;		// WOM_PAGE_SIZE
	uint32_t index_first_page, index_num_pages, index_num_entries;
	uint32_t bitmaps_first_page, bitmaps_num_pages, bitmaps_num_entries;
	uint32_t articles_first_page, articles_num_pages;
	// Probably it's best to have the header followed by index, then bitmaps, then articles. That's because
	// index and bitmaps will be accessed most, and only occupy about 5% of a large file. Following the
	// FAT cluster chain for the large articles area will be slow.
} wom_header_t;

#define END_OF_INDEX_PAGE 0xFFFFFFFF

typedef struct wom_index_entry
{
	uint32_t offset_into_articles; // may be END_OF_INDEX_PAGE to signal that no more indices are coming on this page
	int16_t uri_len; // an index must fit into 1 page, i.e. uri_len <= WOM_PAGE_SIZE-6
	// Full URI would be:	http://en.wikipedia.org/wiki/Time_t
	// Abbreviated:		en/Time_t
	char abbreviated_uri[]; // not zero terminated, use uri_len instead
} wom_index_entry_t;

typedef struct wom_bitmap
{
	uint8_t width;
	uint8_t height;
	uint8_t bits[];
} wom_bitmap_t;

#define WOM_END_OF_ARTICLE	0xFF
#define WOM_Y_ADVANCE_ONLY	0xFE

typedef struct wom_page_element
{
	// x_delta is 'unsigned', but we assume a wrap-around at 256.
	// So if we want to go from 230 to 10, the x_delta value would be 36.
	// x_delta == 0xFF signals the end of the article, no other values in this structure follow anymore
	// x_delta == 0xFE means that this entry is only used to advance the y coordinate by 127 rows. No other values in this structure follow anymore.
	uint8_t x_delta;

	int8_t y_delta;

	// offset into the bitmaps, pointing to a wom_bitmap_t.
	uint32_t offset;

/* some preliminary ideas for links:
	// following members for links only
	uint8_t width, height;
	// TBD: a uint16_t identifier could be used to let multiple page elements form
	// one link together (for example when the link breaks around to the next line).
*/
} wom_page_element_t;
