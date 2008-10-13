#include <string.h>

#include "integer.h"
#include "diskio.h"
#include "ff.h"
#include "cache.h"

static BYTE sector_cache[_CACHE_SIZE * S_MAX_SIZ];
static DWORD sector_cache_index[_CACHE_SIZE];

DSTATUS cache_read_sector (BYTE *buff, DWORD sector)
{
	DWORD i;

	for (i = 0; i < _CACHE_SIZE; i++)
		if (sector_cache_index[i] == sector) {
			memcpy(buff, sector_cache + i, S_MAX_SIZ);
			return RES_OK;
		}

	return RES_NOTRDY;
}


DSTATUS cache_write_sector (const BYTE *buff, DWORD sector)
{
	DWORD i;

	/* if already in cache, move the entry forward */
	for (i = 0; i < _CACHE_SIZE; i++) {
		if (sector_cache_index[i] == -1)
			break;

		if (sector_cache_index[i] == sector) {
			DWORD tmp;

			if (i == 0)
				return RES_OK;

			tmp = sector_cache_index[i - 1];
			sector_cache_index[i - 1] = sector;
			sector_cache_index[i] = tmp;
			return RES_OK;
		}
	}

	/* if not, kick the last entry */
	if (i == _CACHE_SIZE)
		i--;

	sector_cache_index[i] = sector;
	memcpy(sector_cache + S_MAX_SIZ * i, buff, S_MAX_SIZ);

	return RES_OK;
}

DSTATUS cache_init (void)
{
	DWORD i;

	for (i = 0; i < _CACHE_SIZE; i++)
		sector_cache_index[i] = -1;
	
	return RES_OK;
}



