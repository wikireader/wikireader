# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

CC = gcc
CFLAGS = -Wall -I. -g 

C_SRCS = $(wildcard ./*.c) 
C_OBJS = $(patsubst %.c,%.o, $(C_SRCS))

TARGETS=a.out

%.o: %.c
	@$(CC) $(CFLAGS) -o $@  -c $<

all: $(TARGETS)

$(C_OBJS): $(C_SRCS)

a.out:$(C_OBJS)
	$(CC) -o $(TARGETS) $(C_OBJS)

lsearch: $(TARGETS)
	./a.out ../indexfile.index L

bsearch: $(TARGETS)
	./a.out ../indexfile.index B

search: $(TARGETS)
	./a.out ../indexfile.index L > linear_result.txt
	./a.out ../indexfile.index B > binary_result.txt

clean:
	rm -fr $(TARGETS) *.o *~
