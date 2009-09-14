#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import os, sys, re
import os.path
import struct


def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s [--verbose] [--prefix=name]' % os.path.basename(__file__)
    print '       --verbose      Enable verbose output'
    print '       --prefix=name  Device file name portion for .idx [pedia]'
    exit(1)

def main():
    global verbose
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvo:f:p:', ['help', 'verbose', 'out=', 'offsets=', 'prefix='])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    in_format = 'pedia%d.idx-tmp'
    out_name = 'pedia.idx'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
            off_name = arg
        elif opt in ('-p', '--prefix'):
            in_format = arg + '%d.idx-tmp'
            out_name = arg + '.idx'
        else:
            usage('unhandled option: ' + opt)

    out = open(out_name, 'wb')

    article_count = 0
    i = 0
    data = {}
    while True:
        in_name = in_format % i
        if not os.path.isfile(in_name):
            break
        if verbose:
            print 'combining: %s' % in_name
        data[i] = open(in_name, 'rb').read()
        article_count += len(data[i]) / 12 # sizeof(struct)
        i += 1

    out.write(struct.pack('L', article_count))

    for j in range(i):
        out.write(data[j])

    out.close()


# run the program
if __name__ == "__main__":
    main()
