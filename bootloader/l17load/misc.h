#ifndef MISC_H
#define MISC_H

extern int verbose_flag;

#define msg(x...)                                                       \
do {                                                                    \
        fprintf(stderr, x);                                             \
        fflush(stderr);                                                 \
} while (0)

#define debug(x...)                                                     \
if (verbose_flag) {                                                     \
        fprintf(stderr, x);                                             \
        fflush(stderr);                                                 \
} while (0)

#define error(x...)                                                     \
do {                                                                    \
        fprintf(stderr, "\033[31m");            /* red */               \
        fprintf(stderr, "%s (%s: %d): ",                                \
                __FUNCTION__, __FILE__, __LINE__);                      \
        fprintf(stderr, x);                                             \
        fprintf(stderr, "\033[0m");                                     \
        fflush(stderr);                                                 \
} while (0)

void hex_dump (const unsigned char *buf, unsigned int addr, unsigned int len);
void read_blocking(int fd, char *dest, size_t size);
void strchomp(char *s);

#endif /* MISC_H */

