#include <string.h>
#include <malloc.h>
#include <msg.h>

#include "integer.h"
#include "diskio.h"
#include "tff.h"
#include "cache.h"

#define NO_ENTRY	(0xffffffff)
#define N_ENTRIES	(1000)
#define S_MAX_SIZ       (512)

static int cache_hits = 0, cache_updates = 0;
static struct cache_entry
{
	DWORD sector;
	BYTE *buf;
} cache[N_ENTRIES];

void dump_cache_stats(void)
{
	msg(0, "hits: %d, updates %d", cache_hits, cache_updates);
}

static void __cache_entry_move_up(DWORD entry)
{
	BYTE *b;
	DWORD s;

	if (entry == 0)
		return;
	
	b = cache[entry - 1].buf;
	s = cache[entry - 1].sector;

	cache[entry - 1].buf = cache[entry].buf;
	cache[entry - 1].sector = cache[entry].sector;

	cache[entry].buf = b;
	cache[entry].sector = s;
}

DSTATUS cache_read_sector (BYTE *buff, DWORD sector)
{
	DWORD i;

	for (i = 0; i < N_ENTRIES; i++) {
		if (cache[i].sector != sector)
			continue;

		cache_hits++;
		__cache_entry_move_up(i);
		memcpy(buff, cache[i].buf, S_MAX_SIZ);
		return RES_OK;
	}

	return RES_NOTRDY;
}

void cache_update_sector (const BYTE *buff, DWORD sector)
{
	DWORD i;

	for (i = 0; i < N_ENTRIES; i++) {
		if (cache[i].sector != sector)
			continue;

		memcpy(cache[i].buf, buff, S_MAX_SIZ);
		return;
	}
}

DSTATUS cache_write_sector (const BYTE *buff, DWORD sector)
{
	DWORD i;
	
	/* if already in cache, move the entry forward by swapping
	 * it with its neighbour */
	for (i = 0; i < N_ENTRIES; i++) {
		if (cache[i].sector == NO_ENTRY)
			break;
		
		if (cache[i].sector == sector) {
			__cache_entry_move_up(i);
			break;
		}
	}

	/* if not, kick last entry */
	if (i == N_ENTRIES)
		i--;

	cache_updates++;
	cache[i].sector = sector;
	if (!cache[i].buf)
		cache[i].buf = (BYTE *) malloc(S_MAX_SIZ);
	
	memcpy(cache[i].buf, buff, S_MAX_SIZ);
	return RES_OK;
}

DSTATUS cache_init (void)
{
	DWORD i;

	for (i = 0; i < N_ENTRIES; i++) {
		cache[i].sector = NO_ENTRY;
		cache[i].buf = NULL;
	}

	return RES_OK;
}

