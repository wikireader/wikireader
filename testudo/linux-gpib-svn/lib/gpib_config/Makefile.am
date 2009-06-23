# gpib_config/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

sbin_PROGRAMS = gpib_config

gpib_config_SOURCES = gpib_config.c
gpib_config_CFLAGS = $(LIBGPIB_CFLAGS) -I$(top_srcdir)/lib -DDEFAULT_CONFIG_FILE=\"/etc/gpib.conf\"
gpib_config_LDADD = $(LIBGPIB_LDFLAGS) -lpthread

