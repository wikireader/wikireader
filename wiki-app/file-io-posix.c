#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wikilib.h>
#include <file-io.h>
#include <msg.h>
#include <wl-time.h>

int wl_open(const char *filename, int flags)
{
	int f = 0;

	switch (flags) {
	case WL_O_RDONLY:
		f = O_RDONLY;
		break;
	case WL_O_WRONLY:
		f = O_WRONLY;
		break;
	case WL_O_RDWR:
		f = O_RDWR;
		break;
	case WL_O_CREATE:
		f = O_WRONLY | O_TRUNC | O_CREAT;
		break;
	}

	return open(filename, f, S_IRWXU);
}

void wl_close(int fd)
{
	close(fd);
}

int wl_read(int fd, void *buf, unsigned int count)
{
	return read(fd, buf, count);
}

int wl_write(int fd, void *buf, unsigned int count)
{
	return write(fd, buf, count);
}

int wl_seek(int fd, unsigned int pos)
{
	return lseek(fd, pos, SEEK_SET);
}

int wl_fsize(int fd, unsigned int *size)
{
	int ret;
	struct stat stat_buf;

	ret = fstat(fd, &stat_buf);
	if (ret < 0)
		return ret;

	*size = (unsigned int) stat_buf.st_size;
	return 0;
}

unsigned int wl_tell(int fd)
{
	return lseek(fd, 0, SEEK_CUR);
}
