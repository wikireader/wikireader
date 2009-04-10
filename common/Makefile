# Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
# Copyright (c) 2008 Holger Hans Peter Freyther <zecke@openmoko.org>
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
#
CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
AS=$(CROSS)as
LD=$(CROSS)ld
HOSTCC=gcc
CC=$(GCC)
OBJCOPY=$(CROSS)objcopy
OBJDUMP=$(CROSS)objdump
STRIP=$(CROSS)strip

CFLAGS=-Wall -Werror -I. -gstabs -mlong-calls -fno-builtin -Os -mc33pe $(INCLUDES)

# protection in case some Makefile includes this too early
.PHONY: this-is-included-too-early
this-is-included-too-early:
	@echo This is common/Makefile reporting an error
	@echo move the '"include"' to the bottom of the Makefile.
	@echo Otherwise the dependencies are not built in the correct order
	@exit 1

.c.o: $<
	$(GCC) -M $(CFLAGS) $< > $(notdir $(<:.c=.d))
	$(GCC) $(CFLAGS) -c $< -o $(notdir $(<:.c=.o)) -Wa,-ahl=$(notdir $(<:.c=.asm33))

-include $(wildcard *.d) dummy
