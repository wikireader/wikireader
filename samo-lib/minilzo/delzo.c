#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "minilzo.h"

/* LZO may expand uncompressible data by a small amount */
#define MAX_COMPRESSED_SIZE(x)  ((x) + (x) / 16 + 64 + 3)

#define msg(level, template...)	\
    fprintf(stderr, ##template)

enum {
	MSG_ERROR = 0,
	MSG_WARNING,
	MSG_INFO,
	MSG_DEBUG,
	MSG_LEVEL_MAX
};

static void usage(char *argv0)
{
    printf("%s <lzo file>\n", argv0);
    exit(-1);
}

static void error_exit(const char *msg)
{
    perror(msg);
    exit(-1);
}

static int wl_fsize(int fd, unsigned int *size)
{
    int ret;
    struct stat stat_buf;

    ret = fstat(fd, &stat_buf);
    if (ret < 0)
	return ret;

    *size = (unsigned int) stat_buf.st_size;
    return 0;
}

int main(int argc, char *argv[])
{
    int fd, r;
    lzo_uint out_len;
    char *lzo_name;
    void *in, *out;
    unsigned int in_len;
    uint32_t uint32;

    if (argc < 2)
	usage(argv[0]);

    lzo_name = argv[1];
    fd = open(lzo_name, O_RDONLY);
    if (fd < 0)
	error_exit("open");

    r = wl_fsize(fd, &in_len);
    if (r)
	error_exit("fstat");

    in = mmap(NULL, in_len, PROT_READ, MAP_SHARED, fd, 0);
    if (in == (void *) -1)
	error_exit("mmap");

    uint32 = *((uint32_t *) in);
    msg(MSG_DEBUG, "decompress size: %u\n", uint32);
    in += sizeof(uint32_t);
    in_len -= sizeof(uint32_t);

    out = malloc(uint32);
    if (!out)
	error_exit("malloc");

    if (lzo_init() != LZO_E_OK)
    {
	msg(MSG_ERROR, "lzo_init failed\n");
	exit(-1);
    }

    r = lzo1x_decompress(in, in_len, out, &out_len, NULL);
    if (r != LZO_E_OK || out_len != uint32)
    {
	msg(MSG_ERROR, "lzo1x_decompress failed\n");
	exit(-1);
    }

    r = fwrite(out, 1, out_len, stdout);
    if (r != out_len)
    {
	msg(MSG_ERROR, "fwrite failed\n");
	exit(-1);
    }
    return 0;
}
