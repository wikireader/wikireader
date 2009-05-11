LIBDIR=../../toolchain/mini-libc
LDFLAGS=-static --strip-all -s --no-gc-sections -N -L$(LIBDIR)
OBJCOPY_FLAGS=-I elf32-c33 -O binary
CFLAGS += -I../../common -I../../toolchain/mini-libc/include

TARGETS=rs232


all: $(TARGETS)

rs232.elf: rs232.o
	$(GCC) -o $@ $(LDFLAGS) $< -Wl,'-Ttext=0'

rs232: rs232.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@

clean:
	rm -fr $(TARGETS) *.o *.elf *.bin *.d *.asm33

include ../../common/Makefile

