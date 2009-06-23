# language/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

SUBDIRS = $(PYTHON_SUBDIR) $(PHP_SUBDIR) $(GUILE_SUBDIR) $(TCL_SUBDIR)
EXTRA_DIST = $(PERL_DIST)

PERL_DIST = perl/Changes perl/LinuxGpib.pm perl/LinuxGpib.xs \
	perl/MANIFEST perl/Makefile.PL perl/README perl/test.pl \
	perl/typemap perl/examples/

if BIND_PERL
ALL_PERL = all-perl
CLEAN_PERL = clean-perl
else
ALL_PERL =
CLEAN_PERL =
endif

if BIND_PYTHON
PYTHON_SUBDIR=python
else
PYTHON_SUBDIR=
endif

if BIND_PHP
PHP_SUBDIR=php
else
PHP_SUBDIR=
endif

if BIND_GUILE
GUILE_SUBDIR=guile
else
GUILE_SUBDIR=
endif

if BIND_TCL
TCL_SUBDIR=tcl
else
TCL_SUBDIR=
endif

all-local: $(ALL_PERL)

clean-local: $(CLEAN_PERL)

.PHONY: all-perl
all-perl: perl/Makefile
	-$(MAKE) -C perl all

.PHONY: clean-perl
clean-perl: perl/Makefile
	-$(MAKE) -C perl clean

perl/Makefile:
	-(cd perl && $(PERL) Makefile.PL)
