include ../common/Makefile

INCLUDES=-I../toolchain/mini-libc/include -I../common 
LDFLAGS=-static -s --no-gc-sections -N -L$(LIBDIR)
CFLAGS=-Wall $(INCLUDES) -gstabs -mlong-calls -fno-builtin -Os -mc33pe

# For the c33
INCLUDES+=-Iconfig/c33

TARGETS=all

all: lib/libtinyfat.a

lib/libtinyfat.a: src/tff.o src/mmc.o
	$(AR) r $@ $^

clean:
	$(RM) src/*.d
	$(RM) src/*.o
	$(RM) lib/*.a
