#
# Authors:	Wolfgang Spraul <wolfgang@openmoko.com>
#
#		This program is free software; you can redistribute it and/or
#		modify it under the terms of the GNU General Public License
#		as published by the Free Software Foundation; either version
#		3 of the License, or (at your option) any later version.
#           

RM = rm -f
CC = gcc
LD = ld
WIKIAPP_DIR = ../
SAMOLIB_DIR = ../../samo-lib
CFLAGS = -g -O3 -Wall -MD -D_REENTRANT -I $(WIKIAPP_DIR) -I $(SAMOLIB_DIR)
LDFLAGS = -g -dynamic
OBJS = wom_creator.o hexdump.o msg-printf.o malloc-simple.o
TARGETS = wom_creator

vpath %.c $(WIKIAPP_DIR)

all: $(TARGETS)

wom_creator: $(OBJS)
	gcc $(OBJS) -o wom_creator

#test:
#	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o: 
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) $(OBJS:.o=.d) $(TARGETS) wom_creator.c.*

