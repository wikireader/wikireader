#include <string.h>

#include "integer.h"
#include "diskio.h"
#include "ff.h"
#include "cache.h"

#define NO_ENTRY	(0xffff)
#define INDEX(i)	((i) & 0xffff)
#define SECTOR(s)	((s) >> 16 & 0xffff)
#define MKENTRY(i,s)	(((s) & 0xffff) << 16 | ((i) & 0xffff))

static BYTE sector_cache[_CACHE_SIZE * S_MAX_SIZ];
static DWORD sector_cache_index[_CACHE_SIZE];

DSTATUS cache_read_sector (BYTE *buff, DWORD sector)
{
	DWORD i;

	for (i = 0; i < _CACHE_SIZE; i++) {
		if (SECTOR(sector_cache_index[i]) == NO_ENTRY)
			break;

		if (SECTOR(sector_cache_index[i]) == sector) {
			DWORD index = INDEX(sector_cache_index[i]);
			memcpy(buff, sector_cache + index, S_MAX_SIZ);
			return RES_OK;
		}
	}

	return RES_NOTRDY;
}


DSTATUS cache_update_sector (const BYTE *buff, DWORD sector)
{
	DWORD i, index;

	/* if already in cache, move the entry forward */
	for (i = 0; i < _CACHE_SIZE; i++) {
		if (SECTOR(sector_cache_index[i]) == NO_ENTRY)
			return;

		if (SECTOR(sector_cache_index[i]) == sector) {
			index = INDEX(sector_cache_index[i]);
			memcpy(sector_cache + S_MAX_SIZ * index, buff, S_MAX_SIZ);
			return;
		}
	}
}

DSTATUS cache_write_sector (const BYTE *buff, DWORD sector)
{
	DWORD i, index;

	/* if already in cache, move the entry forward */
	for (i = 0; i < _CACHE_SIZE; i++) {
		if (SECTOR(sector_cache_index[i]) == NO_ENTRY)
			break;

		if (SECTOR(sector_cache_index[i]) == sector) {
			DWORD tmp;

			if (i != 0) {
				tmp = sector_cache_index[i - 1];
				sector_cache_index[i - 1] = sector_cache_index[i];
				sector_cache_index[i] = tmp;
			}

			index = INDEX(sector_cache_index[i]);
			memcpy(sector_cache + S_MAX_SIZ * index, buff, S_MAX_SIZ);
			return RES_OK;
		}
	}

	/* if not, kick the last entry */
	if (i == _CACHE_SIZE)
		i--;

	index = INDEX(sector_cache_index[i]);
	sector_cache_index[i] = MKENTRY(index, sector);
	memcpy(sector_cache + S_MAX_SIZ * index, buff, S_MAX_SIZ);
	return RES_OK;
}

DSTATUS cache_init (void)
{
	DWORD i;

	for (i = 0; i < _CACHE_SIZE; i++)
		sector_cache_index[i] = MKENTRY(i, NO_ENTRY);
	
	return RES_OK;
}



