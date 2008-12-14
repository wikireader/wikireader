#ifndef WL_FILE_IO_H
#define WL_FILE_IO_H
#include <wikilib.h>

enum {
	WL_O_RDONLY = 0,
	WL_O_WRONLY,
	WL_O_RDWR
};

/* get the file io time function */
#ifdef COLLECT_TIME
static u32 time = 0;
static u32 time_start = 0;

u32 init_file_io_time();
u32 get_file_io_time();

#endif

/* generic functions */
char *wl_fgets(char *dst, unsigned int size, int fd);

/* those are implemented by the specific/file-io-*.c */
int wl_open(const char *filename, int flags);
void wl_close(int fd);
int wl_read(int fd, void *buf, unsigned int count);
int wl_write(int fd, void *buf, unsigned int count);
int wl_seek(int fd, unsigned int pos);
int wl_ftell(int fd);

#endif /* WL_FILE_IO_H */

