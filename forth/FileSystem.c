// interface to file system

//	Copyright 2009 Christopher Hall <hsw@openmoko.com>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in
//     the documentation and/or other materials provided with the
//     distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <tff.h>

#include "forth.h"

#define SizeOfArray(a) (sizeof(a) / sizeof((a)[0]))


// a type that can hold the path to the file
typedef char FilenameType[81];

// this determines the maximum files that can be open simultaneously
static struct {
	bool IsOpen;
	FIL file;
	FilenameType filename;
} FileControlBlock[20];

// this determines the maximum directoriess that can be open simultaneously
static struct {
	bool IsOpen;
	DIR directory;
	FilenameType directoryname;
} DirectoryControlBlock[10];

// state for the entire file system
static FATFS TheFileSystem;


static bool ForthBufferToCString(char *buffer, size_t length,
				 Forth_PointerType SourceBuffer, Forth_CellType SourceLength)
{
	if (NULL == buffer || NULL == SourceBuffer) {
		return false;
	}

	if (SourceLength >= length) {
		--length;
	} else {
		length = SourceLength;
	}
	if (length > 0) {
		memcpy(buffer, SourceBuffer, length);
		buffer[length] = '\0';
		return true;
	}
	return false;
}

static bool ValidFileHandle(Forth_ReturnType *r, Forth_CellType handle)
{
	r->rc = FR_OK;
	r->result = 0;
	if (0 > handle || SizeOfArray(FileControlBlock) < handle) {
		r->rc = FR_INVALID_OBJECT;
		return false;
	}
	if (!FileControlBlock[handle].IsOpen) {
		r->rc = FR_DENIED;
		return false;
	}
	return true;
}


static bool ValidDirectoryHandle(Forth_ReturnType *r, Forth_CellType handle)
{
	r->rc = FR_OK;
	r->result = 0;
	if (0 > handle || SizeOfArray(DirectoryControlBlock) < handle) {
		r->rc = FR_INVALID_OBJECT;
		return false;
	}
	if (!DirectoryControlBlock[handle].IsOpen) {
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
	for (i = 0; i < SizeOfArray(DirectoryControlBlock); i++) {
		DirectoryControlBlock[i].IsOpen = false;
	}
	f_mount(0, &TheFileSystem);  // only possible value is zero
}


Forth_ReturnType FileSystem_open(const Forth_PointerType filename, Forth_CellType length, Forth_CellType fam)
{
	Forth_ReturnType r = {0, FR_OK};
	size_t i = 0;
	for (i = 0; i < SizeOfArray(FileControlBlock); i++) {
		if (!FileControlBlock[i].IsOpen) {
			if (!ForthBufferToCString(FileControlBlock[i].filename,
						  sizeof(FileControlBlock[i].filename),
						  filename, length)) {
				r.rc = FR_INVALID_NAME;
				return r;
			}
			r.rc = f_open(&FileControlBlock[i].file, FileControlBlock[i].filename, fam);
			if (FR_OK == r.rc) {
				FileControlBlock[i].IsOpen = true;
				r.result = i;
				return r;
			}
			break;
		}
	}
	r.rc = FR_DENIED;
	return r;
}


Forth_ReturnType FileSystem_rename(const Forth_PointerType OldFilename, Forth_CellType OldLength,
			     const Forth_PointerType NewFilename, Forth_CellType NewLength)
{
	Forth_ReturnType r = {0, FR_OK};
	FilenameType TempOldFilename;
	FilenameType TempNewFilename;

	if (!ForthBufferToCString(TempOldFilename, sizeof(TempOldFilename), OldFilename, OldLength)
	    || !ForthBufferToCString(TempNewFilename, sizeof(TempNewFilename), NewFilename, NewLength)) {
		r.rc = FR_INVALID_NAME;
		return r;
	}

	r.rc = f_rename(TempOldFilename, TempNewFilename);

	return r;
}


Forth_ReturnType FileSystem_delete(const Forth_PointerType filename, Forth_CellType length)
{
	Forth_ReturnType r = {0, FR_OK};
	FilenameType TempFilename;

	if (!ForthBufferToCString(TempFilename, sizeof(TempFilename), filename, length)) {
		r.rc = FR_INVALID_NAME;
		return r;
	}

	r.rc = f_unlink(TempFilename);

	return r;
}


Forth_ReturnType FileSystem_create(const Forth_PointerType filename, Forth_CellType length, Forth_CellType fam)
{
	return FileSystem_open(filename, length, fam | FA_CREATE_NEW | FA_CREATE_ALWAYS);
}


Forth_ReturnType FileSystem_close(Forth_CellType handle)
{
	Forth_ReturnType r = {0, FR_OK};

	if (0 > handle || SizeOfArray(FileControlBlock) < handle) {
		r.rc = FR_DENIED;
		return r;
	}

	if (FileControlBlock[handle].IsOpen) {
		r.rc = f_close(&FileControlBlock[handle].file);
		FileControlBlock[handle].IsOpen = false;
	}
	return r;
}


Forth_ReturnType FileSystem_read(Forth_CellType handle, void *buffer, Forth_CellType length)
{
	Forth_ReturnType r = {0, FR_OK};
	UINT count = 0;

	if (!ValidFileHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_read(&FileControlBlock[handle].file, buffer, length, &count);
	r.result = count;
	return r;
}

Forth_ReturnType FileSystem_write(Forth_CellType handle, void *buffer, Forth_CellType length)
{
	Forth_ReturnType r = {0, FR_OK};
	UINT count = 0;

	if (!ValidFileHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_write(&FileControlBlock[handle].file, buffer, length, &count);
	r.result = count;
	return r;
}


Forth_ReturnType FileSystem_sync(Forth_CellType handle)
{
	Forth_ReturnType r = {0, FR_OK};

	if (!ValidFileHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_sync(&FileControlBlock[handle].file);
	return r;
}


Forth_ReturnType FileSystem_lseek(Forth_CellType handle, Forth_CellType pos)
{
	Forth_ReturnType r = {0, FR_OK};

	if (!ValidFileHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_lseek(&FileControlBlock[handle].file, pos);
	return r;
}


Forth_ReturnType FileSystem_ltell(Forth_CellType handle, Forth_CellType pos)
{
	Forth_ReturnType r = {0, FR_OK};

	if (!ValidFileHandle(&r, handle))
	{
		return r;
	}

	// not available :- just error
	//r.rc = f_ltell(&FileControlBlock[handle].file, pos);
	r.rc = FR_DENIED;
	return r;
}


Forth_ReturnType FileSystem_lsize(Forth_CellType handle, Forth_CellType pos)
{
	Forth_ReturnType r = {0, FR_OK};
	FILINFO stat;

	if (!ValidFileHandle(&r, handle))
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


Forth_ReturnType FileSystem_OpenDirectory(const Forth_PointerType directoryname, Forth_CellType length)
{
	Forth_ReturnType r = {0, FR_OK};
	size_t i = 0;
	for (i = 0; i < SizeOfArray(DirectoryControlBlock); i++) {
		if (!DirectoryControlBlock[i].IsOpen) {
			if (!ForthBufferToCString(DirectoryControlBlock[i].directoryname,
						  sizeof(DirectoryControlBlock[i].directoryname),
						  directoryname, length)) {
				r.rc = FR_INVALID_NAME;
				return r;
			}
			r.rc = f_opendir(&DirectoryControlBlock[i].directory, DirectoryControlBlock[i].directoryname);
			if (FR_OK == r.rc) {
				DirectoryControlBlock[i].IsOpen = true;
				r.result = i;
				return r;
			}
			break;
		}
	}
	r.rc = FR_DENIED;
	return r;
}


Forth_ReturnType FileSystem_CloseDirectory(Forth_CellType handle)
{
	Forth_ReturnType r = {0, FR_OK};

	if (0 > handle || SizeOfArray(DirectoryControlBlock) < handle) {
		r.rc = FR_DENIED;
		return r;
	}

	if (DirectoryControlBlock[handle].IsOpen) {
		//r.rc = f_dirclose(&DirectoryControlBlock[handle].directory);
		DirectoryControlBlock[handle].IsOpen = false;
	}
	return r;
}


Forth_ReturnType FileSystem_ReadDirectory(Forth_CellType handle, void *buffer, Forth_CellType length)
{
	Forth_ReturnType r = {0, FR_OK};
	FILINFO info;

	if (!ValidDirectoryHandle(&r, handle))
	{
		return r;
	}

	r.rc = f_readdir(&DirectoryControlBlock[handle].directory, &info);
	if (FR_OK == r.rc){
		size_t count = strlen(info.fname);
		if (count > length) {
			count = length;
		}
		if (0 != count) {
			memcpy(buffer, info.fname, count);
		}
		r.result = count;
	}
	return r;
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
