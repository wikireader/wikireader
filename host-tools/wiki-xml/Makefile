# Copyright (c) 2009 Openmoko Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


RM = rm -f
CC = gcc
LD = ld

WIKIAPP_DIR = ../../wiki-app
LZMA_DIR = ../../samo-lib/lzma

CFLAGS = -g -O3 -Wall -MD -D_REENTRANT -DWIKIPCF -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DBZ_NO_STDIO -DWIKIPCF
CFLAGS += -I. -I$(WIKIAPP_DIR) -I$(LZMA_DIR)

LDFLAGS = -g -dynamic -L/usr/lib/mysql -lmysqlclient

C_OBJS = wiki_render.o
C_OBJS += lcd_buf_draw.o
C_OBJS += bmf.o
C_OBJS += bigram.o
C_OBJS += LzmaEnc.o
C_OBJS += LzmaDec.o
C_OBJS += Alloc.o
C_OBJS += Bra.o
C_OBJS += LzFind.o
C_OBJS += search_hash.o


vpath %.c $(WIKIAPP_DIR) $(LZMA_DIR)

TARGETS = wiki-xml-parser
OBJS = $(C_OBJS) main.o

all: $(TARGETS)

wiki-xml-parser: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGETS) *.o *.d

-include $(wildcard *.d) dummy
