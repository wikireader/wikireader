INCLUDES=-I../toolchain/mini-libc/include -I../common 
LDFLAGS=-static -s --no-gc-sections -N -L$(LIBDIR)
CFLAGS=-Wall $(INCLUDES) -gstabs -mlong-calls -fno-builtin -Os -mc33pe
VPATH=src

# For the c33
INCLUDES+=-Iconfig/c33

TARGETS=all

all: lib/libtinyfat.a

lib/libtinyfat.a: tff.o mmc.o
	$(AR) r $@ $^

clean:
	$(RM) *.[do] *.asm33
	$(RM) lib/*.a

include ../common/Makefile
