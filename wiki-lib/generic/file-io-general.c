#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msg.h>
#include <wikilib.h>

extern int _wl_read(int fd, void *buf, unsigned int count);
extern int _wl_open(const char *filename, int flags);
extern int _wl_seek(int fd, unsigned int pos);
extern int _wl_tell(int fd);

#define BLOCK_SIZE	(512)
#define BLOCK_ALIGNMENT	(BLOCK_SIZE - 1)

struct wl_file {
	int eof;
	unsigned int bytes_read;
	unsigned int bytes_available;
	unsigned int l_offset;
	unsigned char block[BLOCK_SIZE];
};

static struct wl_file file_list[__FD_SETSIZE];

int wl_open(const char *filename, int flags)
{
	int r;
	r = _wl_open(filename, flags);
	if (r < 0)
		return r;
	memset(file_list + r, 0, sizeof(file_list[0]));
	return r;
}

static int read_block(int fd, struct wl_file *fp)
{
	int r;
	r = _wl_read(fd, fp->block, BLOCK_SIZE);
	if (r < 0)
		return r;
	fp->bytes_read = r;
	fp->bytes_available = r;
	fp->l_offset = (fp->l_offset + r) & ~BLOCK_ALIGNMENT;
	fp->eof = BLOCK_SIZE != r;
	return 0;
}

int wl_read(int fd, void *buf, unsigned int count)
{
	unsigned int left = count;
	struct wl_file *fp = file_list + fd;
	char *bufp = (char *) buf;

	while (left) {
		int r;

		if (fp->bytes_available != 0) {
			r = MIN(fp->bytes_available, left);
			memcpy(bufp, fp->block + fp->bytes_read - fp->bytes_available, r);
			fp->bytes_available -= r;
			left -= r;
			bufp += r;
			if (fp->eof)
				break;
		} else if (left >= BLOCK_SIZE) {
			/* read directly into buf */
			r = _wl_read(fd, bufp, BLOCK_SIZE);
			if (r < 0)
				return r;
			left -= r;
			bufp += r;
			if (r < BLOCK_SIZE)
				break;
		} else {
			if ((r = read_block(fd, fp)))
				return r;
		}
	}
	return count - left;
}

int wl_seek(int fd, unsigned int pos)
{
	int r;
	struct wl_file *fp = file_list + fd;
	unsigned int l_offset = pos & ~BLOCK_ALIGNMENT;

	if (l_offset == fp->l_offset)
		goto success;

	r = _wl_seek(fd, l_offset);
	if (r < 0)
		return r;

	fp->l_offset = l_offset;
	r = read_block(fd, fp);
	if (r)
		return r;

success:
	/* TODO: if seek beyond eof ? */
	fp->bytes_available -= pos & BLOCK_ALIGNMENT;
	return 0;
}

int wl_tell(int fd)
{
	return file_list[fd].l_offset - file_list[fd].bytes_available;
}
