GCC=$(CROSS)gcc
AS=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar
HOSTCC=gcc
CC=$(GCC)
OBJCOPY=$(CROSS)objcopy

ifneq ($(CROSS),)
	LDFLAGS = -static -s --no-gc-sections -N -L$(LIBDIR)
	CFLAGS = -Wall -I. -Iinclude -I../zlibc/include -nostdinc -gstabs -mlong-calls -fno-builtin -Os -mc33pe
	ASFLAGS = -c -xassembler-with-cpp -Wa,--gstabs -medda32 -mc33pe -mc33_ext
else
	CFLAGS = -Wall -I. -Iinclude -O2
endif

FB_SOURCES= \
	fb/fb.o

LIB_SOURCES= \
	lib/main.o

SOURCES=$(FB_SOURCES) $(LIB_SOURCES)

TARGETS=wikilib.a
all: $(TARGETS)

.c.o:
	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

wikilib.a: $(SOURCES)
	$(AR) q wikilib.a $^

clean::
	rm -rf $(TARGETS) *.o *.elf *.d
	rm -fr $(SOURCES)

-include $(wildcard *.d) dummy

