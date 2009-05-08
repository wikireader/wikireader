
#include <inttypes.h>
#include <malloc-simple.h>
#include <file-io.h>
#include <minilzo.h>


void *decompress_block(const int fd, unsigned int in_len, unsigned int *out_size)
{
    int r;
    lzo_uint out_len;
    void *in = NULL, *out = NULL;
    uint32_t uint32;

    if (lzo_init() != LZO_E_OK)
	return NULL;

    /* read the expected out_len */
    r = wl_read(fd, &uint32, sizeof(uint32_t));
    if (r != sizeof(uint32_t))
	goto error_exit;
    in_len -= r;

    /* malloc in_len last because we will free it first */
    out = malloc_simple(uint32, MEM_TAG_DECOMP_M1);
    if (!out)
	goto error_exit;

    in = malloc_simple(in_len, MEM_TAG_DECOMP_M2);
    if (!in)
	goto error_exit;

    r = wl_read(fd, in, in_len);
    if (r < 0 || (unsigned int)r != in_len)
	goto error_exit;

    r = lzo1x_decompress(in, in_len, out, &out_len, NULL);
    if (r != LZO_E_OK || out_len != uint32)
	goto error_exit;

    free_simple(in, MEM_TAG_DECOMP_F3);
    *out_size = out_len;
    return out;

error_exit:
    if (in)
	free_simple(in, MEM_TAG_DECOMP_F2);

    if (out)
	free_simple(out, MEM_TAG_DECOMP_F1);

    return NULL;
}
/*
   This function will decompress what's in <filename> and put it into
   a malloc block.  It's the caller's responsibility to free it.
 */
void *decompress(const char *filename, unsigned int *size)
{
    int fd;
    void *out = NULL;
    unsigned int in_len;

    fd = wl_open(filename, WL_O_RDONLY);
    if (fd < 0)
	return NULL;

    if (wl_fsize(fd, &in_len))
	goto error_exit;
    out = decompress_block(fd, in_len, size);

    wl_close(fd);
    return out;

error_exit:
    wl_close(fd);
    return NULL;
}
