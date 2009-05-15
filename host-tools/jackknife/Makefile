SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)

include ${SAMO_LIB}/Mk/definitions.mk

LDFLAGS = -static --strip-all -s --no-gc-sections -N -L${MINI_LIBC}
OBJCOPY_FLAGS = -I elf32-c33 -O binary

CFLAGS += -I${SAMO_LIB_INCLUDE} -I${MINI_LIBC_INCLUDE}

TARGETS = jackknife

all: $(TARGETS)

jackknife.elf: jackknife.o
	$(GCC) -o $@ $(LDFLAGS) $< -Wl,'-Ttext=0'

jackknife: jackknife.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@
	@c=$$(wc --bytes < "$@") ; \
	echo mbr size = $${c} bytes, max = 512 bytes ; \
	if [ "$${c}" -gt 512 ]; then $(RM) "$@"; exit 99; fi

clean:
	rm -fr $(TARGETS) *.o *.elf *.bin *.d *.asm33

include ${SAMO_LIB}/Mk/rules.mk
