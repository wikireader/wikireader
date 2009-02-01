RM = rm -f
CC = gcc
LD = ld
GUILIB_DIR = ../../../gui-lib
WIKILIB_DIR = ../../../wiki-lib

CFLAGS = -g -O3 -Wall -MD -D_REENTRANT -I $(WIKILIB_DIR)/include -I $(GUILIB_DIR)
LDFLAGS = -g -dynamic -lncurses
C_OBJS = guilib.o glyph.o file-io-posix.o wikilib.o time-posix.o	\
	fontfile.o

vpath %.c $(WIKILIB_DIR)/generic $(WIKILIB_DIR)/specific $(GUILIB_DIR)

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

