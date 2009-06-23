# include/Makefile.am
# copyright (C) 2003 by Frank Mori Hess
# email : fmhess@users.sourceforge.net
#
#   This Makefile.am is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

BUILT_SOURCES = gpib gpib_ioctl.h gpib_types.h gpib_user.h ib.h ibConf.h ibP.h

gpib:
	ln -sf . gpib

gpib_ioctl.h:
	ln -sf $(top_srcdir)/drivers/gpib/include/gpib_ioctl.h

gpib_types.h:
	ln -sf $(top_srcdir)/drivers/gpib/include/gpib_types.h

gpib_user.h:
	ln -sf $(top_srcdir)/drivers/gpib/include/gpib_user.h

ib.h:
	ln -sf $(top_srcdir)/lib/ib.h

ibConf.h:
	ln -sf $(top_srcdir)/lib/ibConf.h

ibP.h:
	ln -sf $(top_srcdir)/lib/ibP.h

