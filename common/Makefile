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
CFLAGS=-Wall -I. -gstabs -mlong-calls -fno-builtin -Os -mc33pe $(INCLUDES)
