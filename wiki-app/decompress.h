#ifndef WL_DECOMPRESS_H
#define WL_DECOMPRESS_H

#include "file-io.h"


void *decompress(const char *filename, unsigned int *size);
void *decompress_block(const int file, unsigned int in_size, unsigned int *out_size);
#endif /* WL_DECOMPRESSION_H */
