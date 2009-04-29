#include <stdlib.h>
#include <file-io.h>
#include <minilzo.h>
#include <sys/types.h>

/*
   This function will decompress what's in <filename> and put it into
   a malloc block.  It's the caller's responsibility to free it.
 */
void *decompress(const char *filename, unsigned int *size)
{
    int fd, r;
    lzo_uint out_len;
    void *in = NULL, *out = NULL;
    unsigned int in_len;
    u_int32_t uint32;

    if (lzo_init() != LZO_E_OK)
	return NULL;

    fd = wl_open(filename, WL_O_RDONLY);
    if (fd < 0)
	return NULL;

    if (wl_fsize(fd, &in_len))
	goto error_exit;

    /* read the expected out_len */
    r = wl_read(fd, &uint32, sizeof(u_int32_t));
    if (r != sizeof(u_int32_t))
	goto error_exit;
    in_len -= r;

    /* malloc in_len last because we will free it first */
    out = malloc(uint32);
    if (!out)
	goto error_exit;

    in = malloc(in_len);
    if (!in)
	goto error_exit;

    r = wl_read(fd, in, in_len);
    if (r != in_len)
	goto error_exit;

    r = lzo1x_decompress(in, in_len, out, &out_len, NULL);
    if (r != LZO_E_OK || out_len != uint32)
	goto error_exit;

    free(in);
    wl_close(fd);

    *size = out_len;
    return out;

error_exit:
    wl_close(fd);
    if (in)
	free(in);

    if (out)
	free(out);

    return NULL;
}
