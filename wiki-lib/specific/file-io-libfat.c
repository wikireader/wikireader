#include <wikilib.h>
#include <msg.h>
#include <file-io.h>
#include <tff.h>
#include <wl-time.h>

/* FIXME: define proper error codes */
/* FIXME: solve this with memory management */
#define MAX_FILES 10

static FIL fil_list[MAX_FILES];
static int fil_used[MAX_FILES] = { 0 };

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
	FIL *fil;
	int i, ret, ff_flags = 0;
	switch (flags) {
	case WL_O_RDONLY:
		ff_flags = FA_READ;
		break;
	case WL_O_WRONLY:
#if !_FS_READONLY
		ff_flags = FA_WRITE;
#endif
		break;
	case WL_O_RDWR:
		ff_flags = FA_READ;
#if !_FS_READONLY
		ff_flags |= FA_WRITE;
#endif
		break;
	}

	for (i = 0; i < MAX_FILES; i++)
		if (!fil_used[i])
			break;
	
	if (i == MAX_FILES)
		return -1;

	fil = fil_list + i;
	ret = f_open(fil, filename, ff_flags);
	if (ret != 0)
		return -ret;

	fil_used[i] = 1;
#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
	return 0;
}

void wl_close(int fd)
{
	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return;
	
	f_close(fil_list + fd);
	fil_used[fd] = 0;
#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
}

int wl_read(int fd, void *buf, unsigned int count)
{
	int ret, rcount = -1;
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif

	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;

	ret = f_read(fil_list + fd, buf, count, &rcount);
	if (ret)
		return -ret;

#ifdef COLLECT_TIME
	time += get_timer() - time_start;
#endif
	return rcount;
}

int wl_write(int fd, void *buf, unsigned int count)
{
#if _FS_READONLY
	return -1;
#else
	int ret, wcount;

	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;
	
	ret = f_write(fil_list + fd, buf, count, &wcount);
	if (ret)
		return -ret;
	
	if (wcount != count)
		return -1;

	return 0;
#endif /* _FS_READONLY */
}

int wl_seek(int fd, unsigned int pos)
{
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	int ret;

	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;
	
	ret = f_lseek(fil_list + fd, pos);
	if (ret)
		return -ret;

#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	return 0;
}

int wl_ftell(int fd)
{
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;
	
#ifdef COLLECT_TIME
	time_start = get_timer();
#endif
	return fil_list[fd].fptr;
}


