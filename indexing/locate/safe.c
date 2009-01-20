#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/param.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "safe.h"
#include "debug.h"

#define TRUE !0
#define FALSE 0

void *xalloc(size_t size) {
  void *ptr = malloc(size);
  
  if(!ptr) fatal("Couldn't allocate %zu bytes", size);
  
  memset(ptr, 0, size);
  return ptr;
}

FILE *xfopen(const char *path, const char *mode) {
  debug("opening %s mode %s", path, mode);
  
  FILE *fp = fopen(path, mode);
  
  if(!fp) fatal("Couldn't open %s", path);
  
  return fp;
}

int32_t xgetc(FILE *fp) {
  int32_t ret = getc(fp);
  
  if(ret == EOF && errno != 0) {
    fatal("Couldn't getc");
  }
  
  return ret;
}

int32_t xputc(u_char c, FILE *fp) {
  int32_t ret = putc(c, fp);
  
  if(ret == EOF)
    fatal("Couldn't putc");
    
  return ret;
}

void xfflush(FILE *fp) {
  if(fflush(fp) == EOF)
    fatal("Error on fflush");
}

void xfclose(FILE *fp) {
  if(fclose(fp) == EOF)
    fatal("Error on fclose");
}

void *xmmap(void *addr, size_t len, int prot, int flags, int fildes, off_t offset) {
  void *ptr;
  if((ptr = mmap(addr, len, prot, flags, fildes, offset)) < 0)
    fatal("Error on mmap");

  return ptr;
}

void *xmmapf(char *path, size_t *size) {
  int fd;
  struct stat s;

  if((fd = open(path, O_RDONLY)) < 0) fatal("Error open()ing %s", path);
  if(fstat(fd, &s) < 0) fatal("fstat");

  if(size) *size = s.st_size;

  return xmmap(NULL, (size_t) s.st_size, PROT_READ, MAP_PRIVATE, fd, (off_t) 0);
}
