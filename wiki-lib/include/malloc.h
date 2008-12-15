#ifndef MALLOC_H
#define MALLOC_H

void malloc_init(void);
void *malloc(unsigned int size);
void free(void *ptr);

#endif /* MALLOC_H */

