#ifndef _CACHE_H

DSTATUS cache_read_sector (BYTE *buff, DWORD sector);
DSTATUS cache_write_sector (const BYTE *buff, DWORD sector);
DSTATUS cache_update_sector (const BYTE *buff, DWORD sector);
DSTATUS cache_init (void);

#endif /* _CACHE_H */

