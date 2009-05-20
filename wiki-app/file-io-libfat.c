#include <stdio.h>
#include <wikilib.h>
#include <msg.h>
#include <file-io.h>
#include <tff.h>
#include <wl-time.h>

/* FIXME: define proper error codes */
/* FIXME: solve this with memory management */
#define MAX_FILES	(__FD_SETSIZE)

static FIL fil_list[MAX_FILES];
static int fil_used[MAX_FILES] = { 0 };
static unsigned int fil_offset[MAX_FILES] = { 0 };

int _wl_open(const char *filename, int flags)
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
	fil_offset[i] = 0;
	return i;
}

void wl_close(int fd)
{
	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return;

	f_close(fil_list + fd);
	fil_used[fd] = 0;
}

int _wl_read(int fd, void *buf, unsigned int count)
{
	int ret, rcount = -1;

	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;

	ret = f_read(fil_list + fd, buf, count, &rcount);
	if (ret)
		return -ret;

	fil_offset[fd] += rcount;
	return rcount;
}

#if _FS_READONLY
int wl_write(int fd, void *buf, unsigned int count)
{
	return -1;
}
#else
int wl_write(int fd, void *buf, unsigned int count)
{
	int ret, wcount;

	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;
	
	ret = f_write(fil_list + fd, buf, count, &wcount);
	if (ret)
		return -ret;

	fil_offset[fd] += wcount;

	if (wcount != count)
		return -1;

	return 0;
}
#endif /* _FS_READONLY */

int _wl_seek(int fd, unsigned int pos)
{
	int ret;

	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;

	ret = f_lseek(fil_list + fd, pos);
	if (ret)
		return -ret;

	fil_offset[fd] = pos;
	return 0;
}

int wl_fsize(int fd, unsigned int *size)
{
	if (fd < 0 || fd >= MAX_FILES || !fil_used[fd])
		return -1;

	*size = fil_list[fd].fsize;
	return 0;
}

unsigned int _wl_tell(int fd)
{
	return fil_offset[fd];
}