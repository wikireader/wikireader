#ifndef WL_FILE_IO_H
#define WL_FILE_IO_H

enum {
	WL_O_RDONLY = 0,
	WL_O_WRONLY,
	WL_O_RDWR
};

/* this needs to be implemented by the host platform */
int wl_open(const char *filename, int flags);
void wl_close(int fd);
int wl_read(int fd, void *buf, unsigned int count);
int wl_write(int fd, void *buf, unsigned int count);
int wl_seek(int fd, unsigned int pos);

#endif /* WL_FILE_IO_H */

