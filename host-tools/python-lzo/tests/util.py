##
## vi:ts=4:et
##
##---------------------------------------------------------------------------##
##
## This file is part of the LZO real-time data compression library.
##
## Copyright (C) 1998-2002 Markus Franz Xaver Johannes Oberhumer
## All Rights Reserved.
##
## The LZO library is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; either version 2 of
## the License, or (at your option) any later version.
##
## The LZO library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with the LZO library; see the file COPYING.
## If not, write to the Free Software Foundation, Inc.,
## 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##
## Markus F.X.J. Oberhumer
## <markus@oberhumer.com>
## http://www.oberhumer.com/opensource/lzo/
##
##---------------------------------------------------------------------------##


import os, sys
from distutils.util import get_platform

#
# prepare sys.path in case we are still in the build directory
# see also: distutils/command/build.py (build_platlib)
#

def get_sys_path(p=None):
    if p is None: p = sys.path
    p = p[:]
    p0 = ""
    if p: p0 = p[0]
    #
    plat = get_platform()
    plat_specifier = "%s-%s" % (plat, sys.version[:3])
    ##print plat, plat_specifier
    #
    for prefix in (p0, os.curdir, os.pardir,):
        if not prefix:
            continue
        d = os.path.join(prefix, "build")
        for subdir in ("lib", "lib." + plat_specifier, "lib." + plat):
            dir = os.path.normpath(os.path.join(d, subdir))
            if os.path.isdir(dir):
                if dir not in p:
                    p.insert(1, dir)
    #
    return p


