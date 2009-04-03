// interface to file system

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <tff.h>

#include "forth.h"

#define SizeOfArray(a) (sizeof(a) / sizeof((a)[0]))

// a type that can hole the path to the file
typedef unsigned char FilenameType[81];

// this determines the maximum files that can be open simultaneously
static struct {
	bool IsOpen;
	FIL file;
	FilenameType filename;
} FileControlBlock[10];

static FATFS TheFileSystem;

typedef struct {
	Forth_CellType result;
	Forth_CellType rc;
} ReturnType;


static bool ValidHandle(ReturnType *r, Forth_CellType handle)
{
	r->rc = FR_OK;
	r->result = 0;
	if (0 > handle || SizeOfArray(FileControlBlock) < handle) {
		r->rc = FR_DENIED;
		return false;
	}
	if (!FileControlBlock[handle].IsOpen) {
		r->rc = FR_DENIED;
		return false;
	}
	return true;
}


void FileSystem_initialise(void)
{
	size_t i = 0;
	for (i = 0; i < SizeOfArray(FileControlBlock); i++) {
		FileControlBlock[i].IsOpen = false;
	}
	f_mount(0, &TheFileSystem);  // only possible value is zero
}


ReturnType FileSystem_open(const char *filename, Forth_CellType length, Forth_CellType fam)
{
	ReturnType r = {0, FR_OK};
	if (NULL == filename || '\0' == *filename) {
		r.rc = FR_INVALID_NAME;
		return r;
	}
	{
		size_t i = 0;
		for (i = 0; i < SizeOfArray(FileControlBlock); i++) {
			if (!FileControlBlock[i].IsOpen) {
				if (sizeof(FileControlBlock[i].filename) <= length) {
					length = sizeof(FileControlBlock[i].filename) - 1;
				}
				memcpy(FileControlBlock[i].filename, filename, length);
				FileControlBlock[i].filename[length] = '\0';
				r.rc = f_open(&FileControlBlock[i].file, FileControlBlock[i].filename, fam);
				if (FR_OK == r.rc) {
					FileControlBlock[i].IsOpen = true;
					r.result = i;
					return r;
				}
				break;
			}
		}
	}
	r.rc = FR_DENIED;
	return r;
}


ReturnType FileSystem_delete(const char *filename, Forth_CellType length)
{
	ReturnType r = {0, FR_OK};
	FilenameType TempFilename;

	if (sizeof(TempFilename) <= length) {
		length = sizeof(TempFilename) - 1;
	}
	memcpy(TempFilename, filename, length);
	TempFilename[length] = '\0';
	r.rc = f_unlink(TempFilename);

	return r;
}


ReturnType FileSystem_create(const char *filename, Forth_CellType length, Forth_CellType fam)
{
	return FileSystem_open(filename, length, fam | FA_CREATE_NEW | FA_CREATE_ALWAYS);
}


ReturnType FileSystem_close(Forth_CellType handle)
{
	ReturnType r = {0, FR_OK};

	if (0 > handle || SizeOfArray(FileControlBlock) < handle) {
		r.rc = FR_DENIED;
		return r;
	}
	if (!FileControlBlock[handle].IsOpen) {
		return r;
	}

	r.rc = f_close(&FileControlBlock[handle].file);
	FileControlBlock[handle].IsOpen = false;
	return r;
}


ReturnType FileSystem_read(Forth_CellType handle, void *buffer, Forth_CellType length)
{
	ReturnType r = {0, FR_OK};
	UINT count = 0;

	if (!ValidHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_read(&FileControlBlock[handle].file, buffer, length, &count);
	r.result = count;
	return r;
}

ReturnType FileSystem_write(Forth_CellType handle, void *buffer, Forth_CellType length)
{
	ReturnType r = {0, FR_OK};
	UINT count = 0;

	if (!ValidHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_write(&FileControlBlock[handle].file, buffer, length, &count);
	r.result = count;
	return r;
}


ReturnType FileSystem_sync(Forth_CellType handle)
{
	ReturnType r = {0, FR_OK};

	if (!ValidHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_sync(&FileControlBlock[handle].file);
	return r;
}


ReturnType FileSystem_lseek(Forth_CellType handle, Forth_CellType pos)
{
	ReturnType r = {0, FR_OK};

	if (!ValidHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_lseek(&FileControlBlock[handle].file, pos);
	return r;
}


ReturnType FileSystem_ltell(Forth_CellType handle, Forth_CellType pos)
{
	ReturnType r = {0, FR_OK};

	if (!ValidHandle(&r, handle))
	{
		return r;
	}

	// not available :- just error
	//r.rc = f_ltell(&FileControlBlock[handle].file, pos);
	r.rc = FR_DENIED;
	return r;
}


ReturnType FileSystem_lsize(Forth_CellType handle, Forth_CellType pos)
{
	ReturnType r = {0, FR_OK};
	FILINFO stat;

	if (!ValidHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_stat(FileControlBlock[handle].filename, &stat);
	r.result = stat.fsize;
	return r;
}


Forth_CellType FileSystem_ReadOnly(void)
{
	return FA_READ;
}


Forth_CellType FileSystem_ReadWrite(void)
{
	return FA_READ | FA_WRITE;
}


Forth_CellType FileSystem_WriteOnly(void)
{
	return FA_WRITE;
}


Forth_CellType FileSystem_bin(Forth_CellType fam)
{
	return fam;
}


// 31-25: Year(0-127 +1980)
// 24-21: Month(1-12)
// 20-16: Day(1-31)
// 15-11: Hour(0-23)
// 10- 5: Minute(0-59)
//  4- 0: Second(0-29 *2)
#define YEAR   (2009 - 1980)
#define MONTH  3
#define DAY    30
#define HOUR   17
#define MINUTE 58
#define SECOND 41

DWORD get_fattime (void)
{
	return
		(YEAR	 << 25) |
		(MONTH	 << 21) |
		(DAY	 << 16) |
		(HOUR	 << 11) |
		(MINUTE	 <<  5) |
		(SECOND	 <<  0);
}
