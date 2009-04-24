#! /usr/bin/env python
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

import sys, string

# update sys.path when running in the build directory
from util import get_sys_path
sys.path = get_sys_path()

import lzo


# /***********************************************************************
# // a very simple test driver...
# ************************************************************************/

def print_modinfo():
    #print sys.modules
    mod = sys.modules['lzo']
    #print mod
    d = mod.__dict__
    for k in d.keys():
        print k, d[k]


def test(src, level=1):
    a0 = lzo.adler32(src)
    c =  lzo.compress(src, level)
    u1 = lzo.decompress(c)
    a1 = lzo.adler32(u1)
    o =  lzo.optimize(c)
    u2 = lzo.decompress(o)
    a2 = lzo.adler32(u2)
    if src != u1 or src != u2:
        raise lzo.error, "internal error 1"
    if a0 != a1 or a0 != a2:
        raise lzo.error, "internal error 2"
    print "compressed %6d -> %6d" % (len(src), len(c))


def main(args):
    # display version information and module documentation
    print "LZO version %s (0x%x), %s" % (lzo.LZO_VERSION_STRING, lzo.LZO_VERSION, lzo.LZO_VERSION_DATE)
    print lzo.__file__
    print
    print lzo.__doc__

    # display additional module information
    ## print dir(lzo)
    ## print_modinfo()

    # compress some simple strings
    test("aaaaaaaaaaaaaaaaaaaaaaaa")
    test("abcabcabcabcabcabcabcabc")
    test("abcabcabcabcabcabcabcabc", level=9)
    test(" " * 131072)
    test("")
    print "Simple compression test passed."

    # force an exception (because of invalid compressed data)
    assert issubclass(lzo.error, Exception)
    try:
        x = lzo.decompress("xx")
    except lzo.error, ex:
        ## print ex
        pass
    else:
        print "Exception handling does NOT work !"
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))

