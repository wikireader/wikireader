// interface to file system

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <tff.h>

#define SizeOfArray(a) (sizeof(a) / sizeof((a)[0]))

static struct {
	bool IsOpen;
	FIL file;
	char filename[80]; // 8(name) + 1('.') + 3(ext) + 1('\0')
} FileControlBlock[10];

static FATFS TheFileSystem;

typedef struct {
	unsigned int result;
	unsigned int rc;
} ReturnType;

// from sio.asm
void sio_put_string(const char *s);
void sio_put_hex(unsigned int x);
void sio_put_hex(unsigned int x);
#define puts(s) sio_put_string((s))
#define puthex(x) sio_put_hex((x))

void FileSystem_initialise(void)
{
	size_t i = 0;
	puts("FileSystem_initialise 1\n");
	for (i = 0; i < SizeOfArray(FileControlBlock); i++) {
		FileControlBlock[i].IsOpen = false;
	}
	puts("FileSystem_initialise 2\n");
	f_mount(0, &TheFileSystem);  // only possible value is zero
	puts("FileSystem_initialise 3\n");
}


ReturnType FileSystem_open(const char *filename, unsigned int length, unsigned int fam)
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

				puts(" open = "); puthex(length);
				puts(" : ");
				puts(FileControlBlock[i].filename);
				puts("\n");

				r.rc = f_open(&FileControlBlock[i].file, FileControlBlock[i].filename, fam);

				puts("rc = ");
				puthex(r.rc);
				puts("\nresult = ");
				puthex(r.result);
				puts("\n");

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


ReturnType FileSystem_create(const char *filename, unsigned int length, unsigned int fam)
{
	return FileSystem_open(filename, length, fam | FA_CREATE_NEW | FA_CREATE_ALWAYS);
}


ReturnType FileSystem_close(int handle)
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


ReturnType FileSystem_read(int handle, void *buffer, unsigned int length)
{
	ReturnType r = {0, FR_OK};
	UINT count = 0;

	if (0 > handle || SizeOfArray(FileControlBlock) < handle) {
		r.rc = FR_DENIED;
		return r;
	}
	if (!FileControlBlock[handle].IsOpen) {
		r.rc = FR_DENIED;
		return r;
	}

	r.rc = f_read(&FileControlBlock[handle].file, buffer, length, &count);
	r.result = count;
	return r;
}

ReturnType FileSystem_write(int handle, void *buffer, unsigned int length)
{
	ReturnType r = {0, FR_OK};
	UINT count = 0;

	if (0 > handle || SizeOfArray(FileControlBlock) < handle) {
		r.rc = FR_DENIED;
		return r;
	}
	if (!FileControlBlock[handle].IsOpen) {
		r.rc = FR_DENIED;
		return r;
	}

	r.rc = f_write(&FileControlBlock[handle].file, buffer, length, &count);
	r.result = count;
	return r;
}


unsigned int FileSystem_ReadOnly(void)
{
	return FA_READ;
}


unsigned int FileSystem_ReadWrite(void)
{
	return FA_READ | FA_WRITE;
}


unsigned int FileSystem_WriteOnly(void)
{
	return FA_WRITE;
}


unsigned int FileSystem_bin(unsigned int fam)
{
	return fam;
}


void JustTesting(void)
{

	puts("bye from C\n");
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
