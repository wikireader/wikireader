# language/tcl/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

EXTRA_DIST = gpib.n examples

lib_LTLIBRARIES = libgpib_tcl.la

libgpib_tcl_la_SOURCES = ibCmds.c

TCL_CONFIG = $(TCL_BIN_DIR)/tclConfig.sh
TCL_LIB_SPEC = `$(SHELL) -c '. $(TCL_CONFIG); eval echo $${TCL_LIB_SPEC};'`
TCL_INC = `$(SHELL) -c '. $(TCL_CONFIG); eval echo $${TCL_INCLUDE_SPEC};'`

#fix tcl directory for version independence
libgpib_tcl_la_CFLAGS = $(LIBGPIB_CFLAGS) $(TCL_INC)
libgpib_tcl_la_LDFLAGS = -release $(VERSION) $(LIBGPIB_LDFLAGS) $(TCL_LIB_SPEC)

