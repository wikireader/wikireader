#include <msg.h>
#include <file-io.h>
#include <tff.h>

/* FIXME: define proper error codes */
/* FIXME: solve this with memory management */
#define MAX_FILES 10

static FIL fil_list[MAX_FILES];
static int fil_used[MAX_FILES] = { 0 };

int wl_open(const char *filename, int flags)
{
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
	return 0;
}

void wl_close(int fd)
{
	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return;
	
	f_close(fil_list + fd);
	fil_used[fd] = 0;
}

int wl_read(int fd, void *buf, unsigned int count)
{
	int ret, rcount;

	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;

	ret = f_read(fil_list + fd, buf, count, &rcount);
	if (ret)
		return -ret;
	
	if (rcount != count)
		return -1;

	return 0;
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
	int ret;

	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;
	
	ret = f_lseek(fil_list + fd, pos);
	if (ret)
		return -ret;

	return 0;
}

