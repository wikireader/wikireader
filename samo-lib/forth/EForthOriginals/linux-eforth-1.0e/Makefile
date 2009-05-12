# Customize here:
INSTALLDIR	=	/home/fare/bin/i386
SAVEDIR		=	/home/arc/lang/forth/Linux
TARFILE		=	$(SAVEDIR)/$(PACKAGE_NAME).tar.gz
BINMODE		=	0755
INSTALL		=	install --strip -c -m $(BINMODE)
CPP		=	/usr/bin/gcc -E
RM		=	/bin/rm -f
AS		=	as
LD		=	ld
FORMAT		=	__ELF__

# If you still have an a.out compiler, you may try the following (untested):
#AS		=	/usr/i486-linuxaout/bin/as
#LD		=	/usr/i486-linuxaout/bin/ld -m i386linux
#FORMAT		=	__AOUT__


### Maintainer-only stuff ###
EFORTH_VERSION	=	1.0
LINUX_PORT_VER	=	e
PACKAGE_NAME	=	linux-eforth-$(EFORTH_VERSION)$(LINUX_PORT_VER)

FILES		=	Makefile eforth.S test.4th \
			README FILES README.eforth README.linux \
			Changelog TODO \
			Glossary eforth.4th


DEFINES		=	-Dlinux -D$(FORMAT)

.SUFFIXES:	.o .s .S .l
.PHONY:	clean veryclean install tar test

%.o %.l:	%.s
	$(AS) -adhls=$*.l -o $@ $<

%.s:	%.S
	$(CPP) $(DEFINES) $< > $@

eforth:		eforth.o
	$(LD) -N -e __init__ -o $@ $<

eforth.o:	eforth.s

eforth.s:	eforth.S

clean:
	[ -n "$(wildcard *.o *.s core*)" ] && \
	$(RM) $(wildcard *.[loss] core*) || :

veryclean:	clean
	[ -f eforth ] && $(RM) eforth || :

install:	eforth
	$(INSTALL) eforth $(INSTALLDIR)

tar:
	ln -sf . $(PACKAGE_NAME) && \
	tar zcvf $(TARFILE) \
		`for i in $(FILES) ; do echo $(PACKAGE_NAME)/$$i ; done` ; \
	rm -f $(PACKAGE_NAME)

test:	eforth
	./eforth foo bar baz quux < test.4th

stripped:	eforth
	strip eforth
