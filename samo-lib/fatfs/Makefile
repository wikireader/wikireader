INCLUDES = -I../../toolchain/mini-libc/include -I../../common
LDFLAGS = -static -s --no-gc-sections -N -L$(LIBDIR)
CFLAGS = -Wall $(INCLUDES) -gstabs -mlong-calls -fno-builtin -Os -mc33pe
VPATH = src

TYPE_LIST = read-only read-write

TARGETS = all

.PHONY: all
all: $(foreach type, ${TYPE_LIST}, lib/${type}/libtinyfat.a)

define BUILD
lib/$(strip ${1}):
	$${RM} -r $$@
	mkdir -p $$@

lib/$(strip ${1})/libtinyfat.a: lib/$(strip ${1})

build-$(strip ${1}): Makefile
	$${RM} -r $$@
	mkdir -p $$@
	@echo INCLUDES = $${INCLUDES} | sed 's@\.\./\.\./@../../../@g' >> $$@/Makefile
	@echo LDFLAGS = $${LDFLAGS} | sed 's@\.\./\.\./@../../../@g' >> $$@/Makefile
	@echo CFLAGS = $${CFLAGS} | sed 's@\.\./\.\./@../../../@g' >> $$@/Makefile
	@echo VPATH = ../src >> $$@/Makefile
	@echo INCLUDES += -I../config/c33/$(strip ${1})/ >> $$@/Makefile
	@echo TARGETS = all >> $$@/Makefile
	@echo include ../../../common/Makefile >> $$@/Makefile
endef

# macros to generate rules

MAKE_RULE = $(eval $(call MAKE_RULE1,${1},${2}))

define MAKE_RULE1
lib/$(strip ${1})/libtinyfat.a: build-$(strip ${1})/$(strip ${2})

build-$(strip ${1})/$(strip ${2}): build-$(strip ${1})
	$${MAKE} -C build-$(strip ${1}) ${2}
	$${AR} r lib/$(strip ${1})/libtinyfat.a $$@
endef


# master rules

$(foreach type, ${TYPE_LIST}, $(eval $(call BUILD,${type})))

# create all the individual object file rules

$(call MAKE_RULE,read-only,tff.o)
$(call MAKE_RULE,read-only,mmc.o)

$(call MAKE_RULE,read-write,tff.o)
$(call MAKE_RULE,read-write,mmc.o)
$(call MAKE_RULE,read-write,GetFATTime.o)


clean:
	$(RM) *.[do] *.asm33
	$(RM) -r $(foreach type, ${TYPE_LIST}, build-${type})

include ../../common/Makefile
