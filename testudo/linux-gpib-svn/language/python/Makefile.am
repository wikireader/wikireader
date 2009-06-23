# python/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

EXTRA_DIST = gpibtest.py setup.py Gpib.py gpibinter.c

all-local: build

build: gpibinter.c
	-{ $(PYTHON) setup.py build && touch build; } || { $(RM) -r build; exit 1; }

install-data-local:
	-$(PYTHON) setup.py install --prefix=$(DESTDIR)$(prefix)

clean-local:
	-$(RM) -r build

