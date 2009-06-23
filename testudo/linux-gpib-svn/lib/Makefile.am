# lib/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

SUBDIRS = . gpib_config

lib_LTLIBRARIES = libgpib.la

noinst_HEADERS = ibConf.h ib_internal.h
headersdir = $(includedir)/gpib
headers_HEADERS = ib.h

BUILT_SOURCES = ibConfLex.c ibConfLex.h ibConfYacc.c ibConfYacc.h
MAINTAINERCLEANFILES = ibConfLex.c ibConfLex.h ibConfYacc.c ibConfYacc.h

EXTRA_DIST = ibConfLex.l ibConfYacc.y gpib_version_script

libgpib_la_SOURCES = \
	ib.h ib_internal.h ibConf.h ibP.h parse.h \
	ibCac.c ibClr.c ibCmd.c ibEos.c ibEot.c \
	ibFind.c ibLines.c ibOnl.c ibPad.c ibRd.c ibRpp.c ibRsp.c ibRsv.c \
	ibSad.c ibSic.c ibSre.c ibTmo.c ibTrg.c ibWait.c ibWrt.c \
	ibGts.c ibBoard.c ibutil.c globals.c ibask.c ibppc.c \
	ibLoc.c ibDma.c ibdev.c ibbna.c async.c ibconfig.c ibFindLstn.c \
	ibEvent.c local_lockout.c self_test.c pass_control.c ibstop.c \
	ibConfLex.c ibConfLex.h ibConfYacc.c ibConfYacc.h

libgpib_la_CFLAGS = $(LIBGPIB_CFLAGS) -DDEFAULT_CONFIG_FILE=\"/etc/gpib.conf\"
libgpib_la_LDFLAGS = -version-info @GPIB_SO_VERSION@ -Wl,--version-script=$(srcdir)/gpib_version_script -lpthread

$(srcdir)/ibConfLex.c $(srcdir)/ibConfLex.h: $(srcdir)/ibConfLex.l
	$(LEX) $<

$(srcdir)/ibConfYacc.c $(srcdir)/ibConfYacc.h: $(srcdir)/ibConfYacc.y
	$(YACC) -d -p gpib_yy -o $(srcdir)/ibConfYacc.c $<


