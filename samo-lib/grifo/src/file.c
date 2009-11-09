/*
 * file - file system access
 *
 * Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "standard.h"

#include <string.h>

#include <tff.h>
#include <diskio.h>

#include "serial.h" //debug
#include "file.h"


// a type that can hold the path to the file
typedef char FilenameType[128 + 1];

// this determines the maximum files that can be open simultaneously
typedef struct {
	bool IsOpen;
	FIL file;
} FileType;

static FileType FileControlBlock[512];

// this determines the maximum directoriess that can be open simultaneously
typedef struct {
	bool IsOpen;
	DIR directory;
} DirectoryType;

static DirectoryType DirectoryControlBlock[64];

// state for the entire file system
static FATFS TheFileSystem;


void File_initialise(void)
{
	size_t i = 0;

	for (i = 0; i < SizeOfArray(FileControlBlock); i++) {
		FileControlBlock[i].IsOpen = false;
	}
	for (i = 0; i < SizeOfArray(DirectoryControlBlock); i++) {
		DirectoryControlBlock[i].IsOpen = false;
	}
	memset(&TheFileSystem, 0, sizeof(TheFileSystem));
	{
		uint8_t b = 0;
		disk_ioctl(0, CTRL_POWER, &b);
		disk_initialize(0);
	}
	f_mount(0, &TheFileSystem);  // only possible value is zero
}


// ensure: 0 <= handle < array size
static FileType *ValidateFileHandle(int handle)
{
	if (0 > handle || SizeOfArray(FileControlBlock) <= handle) {
		return NULL;
	}
	if (!FileControlBlock[handle].IsOpen) {
		return NULL;
	}
	return &FileControlBlock[handle];
}


// ensure: 1 <= handle <= array size
static DirectoryType *ValidateDirectoryHandle(int handle)
{
	if (0 > handle || SizeOfArray(DirectoryControlBlock) <= handle) {
		return NULL;
	}
	if (!DirectoryControlBlock[handle].IsOpen) {
		return NULL;
	}
	return &DirectoryControlBlock[handle];
}


void File_CloseAll(void)
{
	size_t i = 0;

	for (i = 0; i < SizeOfArray(FileControlBlock); i++) {
		File_close(i + 1);
	}
	for (i = 0; i < SizeOfArray(DirectoryControlBlock); i++) {
		File_CloseDirectory(i + 1);
	}
	File_initialise();
}


File_ErrorType File_rename(const char *OldFilename, const char *NewFilename)
{
	if (NULL == OldFilename || NULL == NewFilename) {
		return FILE_ERROR_INVALID_NAME;
	}
	return -f_rename(OldFilename, NewFilename);
}

File_ErrorType File_delete(const char *filename)
{
	if (NULL == filename) {
		return FILE_ERROR_INVALID_NAME;
	}
	return -f_unlink(filename);
}


File_ErrorType File_size(const char *filename, unsigned long *length)
{
	if (NULL == filename) {
		return FILE_ERROR_INVALID_NAME;
	}
	FILINFO stat;

	File_ErrorType rc = -f_stat(filename, &stat);
	*length = stat.fsize;
	return rc;
}


File_ErrorType File_create(const char *filename, File_AccessType fam)
{
	return File_open(filename, fam | FILE_OPEN_CREATE | FILE_OPEN_TRUNCATE);
}


File_ErrorType File_open(const char *filename, File_AccessType fam)
{
	if (NULL == filename) {
		return FILE_ERROR_INVALID_NAME;
	}

	Serial_printf("File_open '%s'\n", filename);
	size_t i = 0;
	for (i = 0; i < SizeOfArray(FileControlBlock); i++) {
		if (!FileControlBlock[i].IsOpen) {
			File_ErrorType rc = -f_open(&FileControlBlock[i].file, filename, fam);
			if (FILE_ERROR_OK == rc) {
				FileControlBlock[i].IsOpen = true;
	Serial_printf("File_open handle = %d\n", i);
				return i;  // handle 0...
			}
			break;
		}
	}
	return FILE_ERROR_DENIED;
}


File_ErrorType File_close(int handle)
{
	FileType *file = ValidateFileHandle(handle);

	if (NULL == file) {
		return FILE_ERROR_INVALID_OBJECT;
	}
	return -f_close(&file->file);
}


ssize_t File_read(int handle, void *buffer, size_t length)
{
	FileType *file = ValidateFileHandle(handle);

	if (NULL == file) {
		return FILE_ERROR_INVALID_OBJECT;
	}

	unsigned int count;
	File_ErrorType rc = -f_read(&file->file, buffer, length, &count);
	if (FILE_ERROR_OK == rc) {
		return count;
	}
	return rc;
}


ssize_t File_write(int handle, void *buffer, size_t length)
{
	FileType *file = ValidateFileHandle(handle);

	if (NULL == file) {
		return FILE_ERROR_INVALID_OBJECT;
	}

	unsigned int count;
	File_ErrorType rc = -f_write(&file->file, buffer, length, &count);
	if (FILE_ERROR_OK == rc) {
		return count;
	}
	return rc;
}


File_ErrorType File_sync(int handle)
{
	FileType *file = ValidateFileHandle(handle);

	if (NULL == file) {
		return FILE_ERROR_INVALID_OBJECT;
	}

	return -f_sync(&file->file);
}


File_ErrorType File_lseek(int handle, unsigned long pos)
{
	FileType *file = ValidateFileHandle(handle);

	if (NULL == file) {
		return FILE_ERROR_INVALID_OBJECT;
	}
	return -f_lseek(&file->file, pos);
}


//File_ErrorType File_ltell(int handle, unsigned long *pos); // not available yet


File_ErrorType File_CreateDirectory(const char *directoryname)
{
	if (NULL == directoryname) {
		return FILE_ERROR_INVALID_NAME;
	}

	return -f_mkdir(directoryname);
}


File_ErrorType File_OpenDirectory(const char *directoryname)
{
	if (NULL == directoryname) {
		return FILE_ERROR_INVALID_NAME;
	}

	size_t i;
	for (i = 0; i < SizeOfArray(DirectoryControlBlock); i++) {
		if (!DirectoryControlBlock[i].IsOpen) {
			File_ErrorType rc = -f_opendir(&DirectoryControlBlock[i].directory, directoryname);
			if (FILE_ERROR_OK == rc) {
				DirectoryControlBlock[i].IsOpen = true;
				return i; // 0...
			}
			break;
		}
	}
	return FILE_ERROR_DENIED;
}


File_ErrorType File_CloseDirectory(int handle)
{
	DirectoryType *directory = ValidateDirectoryHandle(handle);

	if (NULL == directory) {
		return FILE_ERROR_INVALID_OBJECT;
	}

	// rc = -f_dirclose(&directory->directory);
	directory->IsOpen = false;

	return FILE_ERROR_OK;
}


ssize_t File_ReadDirectory(int handle, void *buffer, size_t length)
{
	DirectoryType *directory = ValidateDirectoryHandle(handle);

	if (NULL == directory) {
		return FILE_ERROR_INVALID_OBJECT;
	}

	FILINFO info;

	File_ErrorType rc = -f_readdir(&directory->directory, &info);
	if (FILE_ERROR_OK == rc){
		size_t count = strlen(info.fname);

		if (count > length) {
			count = length;
		}
		if (0 != count) {
			memcpy(buffer, info.fname, count);
		}
		return count;
	}
	return rc;
}


File_ErrorType File_AbsoluteRead(unsigned long sector, void *buffer, int count)
{
	return -disk_read(0, buffer, sector, count);
}


File_ErrorType File_AbsoluteWrite(unsigned long sector, const void *buffer, int count)
{
	return -disk_write(0, buffer, sector, count);
}
