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


CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
AS=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar
HOSTCC=gcc
CC=$(GCC)
OBJCOPY=$(CROSS)objcopy
RM = rm -f
MKDIR = mkdir -p
TOUCH = touch


LDFLAGS=-static -s --no-gc-sections -N -L$(LIBDIR)
CFLAGS=-Wall -I. -Iinclude -gstabs -mlong-calls -fno-builtin -Os -mc33pe
ASFLAGS=-c -xassembler-with-cpp -Wa,--gstabs -medda32 -mc33pe -mc33_ext

C_STRING_OPERATIONS = \
    src/string/atol.o \
    src/string/bcmp.o \
    src/string/bcopy.o \
    src/string/bzero.o \
    src/string/ffs.o \
    src/string/index.o \
    src/string/isalnum.o \
    src/string/isalpha.o \
    src/string/isascii.o \
    src/string/isblank.o \
    src/string/iscntrl.o \
    src/string/isdigit.o \
    src/string/islower.o \
    src/string/isprint.o \
    src/string/ispunct.o \
    src/string/isspace.o \
    src/string/isupper.o \
    src/string/isxdigit.o \
    src/string/memccpy.o \
    src/string/memchr.o \
    src/string/memcmp.o \
    src/string/memcpy.o \
    src/string/memmove.o \
    src/string/memset.o \
    src/string/rindex.o \
    src/string/strcasecmp.o \
    src/string/strcat.o \
    src/string/strchr.o \
    src/string/strcmp.o \
    src/string/strcpy.o \
    src/string/strcspn.o \
    src/string/strdup.o \
    src/string/strlcat.o \
    src/string/strlcpy.o \
    src/string/strlen.o \
    src/string/strncasecmp.o \
    src/string/strncat.o \
    src/string/strncmp.o \
    src/string/strncpy.o \
    src/string/strpbrk.o \
    src/string/strrchr.o \
    src/string/strsep.o \
    src/string/strspn.o \
    src/string/strstr.o \
    src/string/strtok.o \
    src/string/swab.o \
    src/string/toascii.o \
    src/string/tolower.o \
    src/string/toupper.o

C_BSD_OPERATIONS = \
    src/bsd/qsort.o \
    src/bsd/strtol.o \
    src/bsd/strtoul.o

C_STD_LIB_OPERATIONS = \
    src/stdlib/abort.o \
    src/stdlib/abs.o \
    src/stdlib/atoi.o \
    src/stdlib/atol.o \
    src/stdlib/bsearch.o \
    src/stdlib/errno.o \
    src/stdlib/itoa.o \
    src/stdlib/labs.o \
    src/stdlib/ltoa.o \
    src/stdlib/printf.o \
    src/stdlib/puts.o \
    src/stdlib/rand.o \
    src/stdlib/snprintf.o \
    src/stdlib/sprintf.o \
    src/stdlib/strtol.o \
    src/stdlib/strtoul.o \
    src/stdlib/ultoa.o \
    src/stdlib/uprintf.o \
    src/stdlib/utoa.o \
    src/stdlib/vprintf.o \
    src/stdlib/vsnprintf.o \
    src/stdlib/vsprintf.o \
    src/stdlib/vuprintf.o



TARGETS=lib/libc.a
all: $(TARGETS)

.c.o:
	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

lib/libc.a: stamp-lib $(C_STRING_OPERATIONS) $(C_BSD_OPERATIONS) $(C_STD_LIB_OPERATIONS)
	$(AR) q lib/libc.a $^

stamp-lib:
	${MKDIR} "${@:stamp-%=%}"
	${TOUCH} "${@}"

clean::
	${RM} $(TARGETS) *.o *.elf *.d
	${RM} src/string/*.d src/string/*.o
	${RM} src/stdlib/*.d src/stdlib/*.o
	${RM} src/bsd/*.d src/bsd/*.o
	${RM} stamp-*
	${RM} -r lib

-include $(wildcard *.d) dummy

