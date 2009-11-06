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


define FIND_SAMO_LIB
d='.' ; \
while [ X"/" != X"$${d}" ] ; \
do \
  s="$${d}/samo-lib" ; \
  [ -d "$${s}" ] && echo $${s} && exit 0 ; \
  d=$$(readlink -es "$${d}/..") ; \
done ; \
echo samo_lib_NOT_FOUND ; \
exit 1
endef
SAMO_LIB := $(shell ${FIND_SAMO_LIB})

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
