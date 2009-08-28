RM = rm -f
CC = gcc
LD = ld

WIKIAPP_DIR = ../../wiki-app
BZLIB_DIR = ../../samo-lib/bzlib
LZO_SRC_DIR = ../../samo-lib/lzo/src
LZO_INC_DIR = ../../samo-lib/lzo/include
LZMA_DIR = ../../samo-lib/lzma

CFLAGS = -g -O3 -Wall -MD -D_REENTRANT -DWIKIPCF -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DBZ_NO_STDIO -DWIKIPCF
CFLAGS += -I. -I$(WIKIAPP_DIR) -I$(BZLIB_DIR) -I$(LZO_INC_DIR) -I$(LZMA_DIR)
LDFLAGS = -g -dynamic -L/usr/lib/mysql -lmysqlclient
C_OBJS = wiki_render.o lcd_buf_draw.o bmf.o bigram.o \
	LzmaEnc.o Alloc.o Bra.o LzFind.o


vpath %.c $(WIKIAPP_DIR) $(BZLIB_DIR) $(LZO_SRC_DIR) $(LZMA_DIR)

TARGETS = wiki-xml-parser
OBJS = $(C_OBJS) main.o

all: $(TARGETS)

wiki-xml-parser: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(RM) $(TARGETS) *.o *.d

-include $(wildcard *.d) dummy

