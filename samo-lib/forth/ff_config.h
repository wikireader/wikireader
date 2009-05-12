/*--------------------------------------------------------------------------/
/  FatFs - FAT file system module include file	R0.06	     (C)ChaN, 2008
/---------------------------------------------------------------------------/
/ FatFs module is an experimenal project to implement FAT file system to
/ cheap microcontrollers. This is a free software and is opened for education,
/ research and development under license policy of following trems.
/
/  Copyright (C) 2008, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is no warranty.
/ * You can use, modify and/or redistribute it for personal, non-profit or
/   commercial use without any restriction under your responsibility.
/ * Redistributions of source code must retain the above copyright notice.
/
/---------------------------------------------------------------------------*/

#define _MCU_ENDIAN		2
/* The _MCU_ENDIAN defines which access method is used to the FAT structure.
/  1: Enable word access.
/  2: Disable word access and use byte-by-byte access instead.
/  When the architectural byte order of the MCU is big-endian and/or address
/  miss-aligned access results incorrect behavior, the _MCU_ENDIAN must be set to 2.
/  If it is not the case, it can also be set to 1 for good code efficiency. */

#define _FS_READONLY	0
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename,
/  f_truncate and useless f_getfree. */

#define _FS_MINIMIZE	0
/* The _FS_MINIMIZE option defines minimization level to remove some functions.
/  0: Full function.
/  1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod, f_truncate and f_rename are removed.
/  2: f_opendir and f_readdir are removed in addition to level 1.
/  3: f_lseek is removed in addition to level 2. */

#define	_USE_STRFUNC	0
/* To enable string functions, set _USE_STRFUNC to 1 or 2. */

#define	_USE_MKFS	0
/* When _USE_MKFS is set to 1 and _FS_READONLY is set to 0, f_mkfs function is
/  enabled. */

#define _DRIVES		1
/* Number of logical drives to be used. This affects the size of internal table. */

#define	_MULTI_PARTITION	0
/* When _MULTI_PARTITION is set to 0, each logical drive is bound to same
/  physical drive number and can mount only 1st primaly partition. When it is
/  set to 1, each logical drive can mount a partition listed in Drives[]. */

#define _USE_FSINFO	0
/* To enable FSInfo support on FAT32 volume, set _USE_FSINFO to 1. */

#define	_USE_SJIS	0
/* When _USE_SJIS is set to 1, Shift-JIS code transparency is enabled, otherwise
/  only US-ASCII(7bit) code can be accepted as file/directory name. */

#define	_USE_NTFLAG	1
/* When _USE_NTFLAG is set to 1, upper/lower case of the file name is preserved.
/  Note that the files are always accessed in case insensitive. */

#define _FAT32 1
/* To enable FAT32 support in addition of FAT12/16, set _FAT32 to 1. For tinyfat*/
