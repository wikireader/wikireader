#include <wikilib.h>
#include <file-io.h>

/* functions that are common for all implementations */

char *wl_fgets(char *dst, unsigned int size, int fd)
{
	char *orig_dst = dst;

	while (size--) {
		if (wl_read(fd, dst, 1) != 1)
			return NULL;

		if (*dst == '\n' || *dst == '\r') {
			*dst = '\0';
			break;
		}

		dst++;
	}

	return orig_dst;
}

