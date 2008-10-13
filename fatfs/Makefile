CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
AS=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar
HOSTCC=gcc
CC=$(GCC)
OBJCOPY=$(CROSS)objcopy

LDFLAGS=-static -s --no-gc-sections -N -L$(LIBDIR)
CFLAGS=-Wall -I../mini-libc/include -gstabs -mlong-calls -fno-builtin -Os -mc33pe
ASFLAGS=-c -xassembler-with-cpp -Wa,--gstabs -medda32 -mc33pe -mc33_ext

# For the c33
CFLAGS+=-Iconfig/c33

TARGETS=all

all: lib/libfat.a lib/libtinyfat.a

.c.o: $<
	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

lib/libfat.a: src/ff.o src/cache.o src/mmc.o
	$(AR) q $@ $^

lib/libtinyfat.a: src/tff.o
	$(AR) q $@ $^

clean:
	$(RM) src/*.d
	$(RM) src/*.o
	$(RM) lib/*.a
