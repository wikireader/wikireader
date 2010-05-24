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

CFLAGS = -g -Wall -Werror -MD -D_REENTRANT
LDFLAGS = -g -dynamic
C_OBJS = pcf.o

vpath %.c

TARGETS = pcf2bmf
OBJS = $(C_OBJS) pcf2bmf.o

.PHONY: all
all: $(TARGETS)

pcf2bmf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# just so that all the REQUIRED_xxx can be evaluated
.PHONY: requires
	true

.PHONY: clean
clean:
	$(RM) $(TARGETS) *.o *.d

-include $(wildcard *.d) dummy

