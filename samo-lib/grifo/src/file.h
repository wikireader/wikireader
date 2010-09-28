/*
 * file - file system access
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
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

#if  !defined(_FILE_H_)
#define _FILE_H_ 1

#include "standard.h"

typedef enum {
//+MakeSystemCalls: access
	FILE_OPEN_READ = 1,
	FILE_OPEN_WRITE = 2,
	FILE_OPEN_CREATE = 4,
	FILE_OPEN_TRUNCATE = 8,
//-MakeSystemCalls: access
} File_AccessType;


typedef enum {
//+MakeSystemCalls: error
	FILE_ERROR_OK			= 0,
	FILE_ERROR_NOT_READY		= -1,
	FILE_ERROR_NO_FILE		= -2,
	FILE_ERROR_NO_PATH		= -3,
	FILE_ERROR_INVALID_NAME		= -4,
	FILE_ERROR_INVALID_DRIVE	= -5,
	FILE_ERROR_DENIED		= -6,
	FILE_ERROR_EXIST		= -7,
	FILE_ERROR_RW_ERROR		= -8,
	FILE_ERROR_WRITE_PROTECTED	= -9,
	FILE_ERROR_NOT_ENABLED		= -10,
	FILE_ERROR_NO_FILESYSTEM	= -11,
	FILE_ERROR_INVALID_OBJECT	= -12,
//-MakeSystemCalls: error
} File_ErrorType;

void File_initialise(void);

void File_PowerDown(void);
void File_CloseAll(void);

File_ErrorType File_rename(const char *OldFilename, const char *NewFilename);
File_ErrorType File_delete(const char *filename);
File_ErrorType File_size(const char *filename, unsigned long *length);

File_ErrorType File_create(const char *filename, File_AccessType fam);

File_ErrorType File_open(const char *filename, File_AccessType fam);
File_ErrorType File_close(int handle);
ssize_t File_read(int handle, void *buffer, size_t length);
ssize_t File_write(int handle, void *buffer, size_t length);
File_ErrorType File_sync(int handle);
File_ErrorType File_lseek(int handle, unsigned long pos);
//File_ErrorType File_ltell(int handle, unsigned long *pos); // not available yet

//File_ErrorType File_ChangeDirectory(const char *directoryname);
//File_ErrorType File_CurrentDirectory(char *directoryname, size_t length);
File_ErrorType File_CreateDirectory(const char *directoryname);
bool File_DirectoryExists(const char *directoryname);
File_ErrorType File_OpenDirectory(const char *directoryname);
File_ErrorType File_CloseDirectory(int handle);
ssize_t File_ReadDirectory(int handle, void *buffer, size_t length);

File_ErrorType File_AbsoluteRead(unsigned long sector, void *buffer, int count);
File_ErrorType File_AbsoluteWrite(unsigned long sector, const void *buffer, int count);

#endif
