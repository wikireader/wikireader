# language/php/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

SUBDIRS = TESTS

EXTRA_DIST = sample-php.ini run

lib_LTLIBRARIES = gpib_php.la

gpib_php_la_SOURCES = gpib.c

EXTENSIONDIR = `$(PHP_CONFIG) --extension-dir`
PHP_INCLUDES = `$(PHP_CONFIG) --includes`

gpib_php_la_CPPFLAGS = $(PHP_INCLUDES) -DUSE_LINUX_GPIB -DCOMPILE_DL=1
gpib_php_la_CFLAGS = $(LIBGPIB_CFLAGS)
gpib_php_la_LDFLAGS = -release $(VERSION) $(LIBGPIB_LDFLAGS) -module -rdynamic
libdir = $(EXTENSIONDIR)

