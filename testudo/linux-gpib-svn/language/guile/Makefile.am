# language/guile/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

EXTRA_DIST = gpib.scm

lib_LTLIBRARIES = libgpib-guile.la

noinst_HEADERS = gpib.h

libgpib_guile_la_SOURCES = gpib.c

libgpib_guile_la_CFLAGS = $(LIBGPIB_CFLAGS)
libgpib_guile_la_LDFLAGS = -release $(VERSION) $(LIBGPIB_LDFLAGS)

