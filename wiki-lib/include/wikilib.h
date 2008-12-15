#ifndef WIKILIB_H
#define WIKILIB_H

#ifndef NULL
#define NULL 0
#endif

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

#ifdef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;


/* function prototypes */
int wikilib_init (void);
int wikilib_run (void);

#endif /* WIKILIB_H */
