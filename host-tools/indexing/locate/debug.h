#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdbool.h>
#include <errno.h>
#include <string.h>

#ifdef STRIP_DEBUG
#define debug(...)
#else
#define debug(fmt, ...) (debug ? fprintf(stderr, "debug: " fmt "\n", \
                                         ##__VA_ARGS__) : false)
#endif

#define fatal(error, ...) do { \
  fprintf(stderr, error, ##__VA_ARGS__); \
  fprintf(stderr, " (errno %d: %s)\n", errno, strerror(errno)); \
  exit(1); \
  } while(0)


#ifndef __GNUC__
#error I want GNU C plz
#endif

#ifdef DEBUG
static bool debug = true;
#else
static bool debug = false;
#endif

#endif
