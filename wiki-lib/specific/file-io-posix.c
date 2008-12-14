#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <wikilib.h>
#include <file-io.h>
#include <msg.h>
#include <wl-time.h>

#ifdef COLLECT_TIME
u32 init_file_io_time()
{
	time = 0;
}

u32 get_file_io_time()
{
	return time;
}
#endif

int wl_open(const char *filename, int flags)
{
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
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
	}

	int result = open(filename, f);
#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
	return result;
}

void wl_close(int fd)
{
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	close(fd);
#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
}

int wl_read(int fd, void *buf, unsigned int count)
{
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	int result = read(fd, buf, count);
#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
	return result;
}

int wl_write(int fd, void *buf, unsigned int count)
{
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	return write(fd, buf, count);
#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
}

int wl_seek(int fd, unsigned int pos)
{
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	int result = lseek(fd, pos, SEEK_SET);
#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
	return result;
}

int wl_ftell(int fd)
{
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	msg(MSG_ERROR, "%s() IS UNIMPLEMENTED!", __func__);
#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
	return -1;
}
