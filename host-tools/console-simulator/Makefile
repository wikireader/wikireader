RM = rm -f
CC = gcc
LD = ld
GUILIB_DIR = ../../../gui-lib
WIKILIB_DIR = ../../../wiki-lib
INDEX_DIR = ../../indexing/locate
COMMON_DIR = ../../../common
MINILZO_DIR = ../../../minilzo
FATFS_DIR = ../../../fatfs

CFLAGS = -g -O3 -Wall -MD -D_REENTRANT -Werror
CFLAGS += -I $(WIKILIB_DIR)/include
CFLAGS += -I $(GUILIB_DIR)
CFLAGS += -I $(INDEX_DIR)
CFLAGS += -I $(COMMON_DIR)
CFLAGS += -I $(MINILZO_DIR)
CFLAGS += -I $(FATFS_DIR)/src
CFLAGS += -I $(FATFS_DIR)/config/c33/read-write
LDFLAGS = -g -dynamic -lncurses
C_OBJS = guilib.o glyph.o file-io-general.o file-io-posix.o wikilib.o time-posix.o	\
	fontfile.o search.o lsearcher.o fast_search.o article.o history.o decompress.o \
	minilzo.o keyboard.o wom_reader.o fatfs_stdio.o

vpath %.c $(WIKILIB_DIR)/generic $(WIKILIB_DIR)/specific $(GUILIB_DIR) \
	  $(INDEX_DIR) $(MINILZO_DIR) $(COMMON_DIR)

TARGETS = wikireader
OBJS = $(C_OBJS) main.o

all: $(TARGETS)

wikireader: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(RM) $(TARGETS) *.o *.d

-include $(wildcard *.d) dummy

