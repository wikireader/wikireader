#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Count the articles in different size ranges
# AUTHORS: Christopher Hall <hsw@openmoko.com>

import sys, os
import struct
import os.path
import getopt
import PrintLog
import locale


locale.setlocale(locale.LC_ALL, '')

verbose = False
sizes = None
dist_list = None
distribution = None
byte_count = None
total = 0


def usage(message):
    if None != message:
        print('error: {0:s}'.format(message))
    print('usage: {0:s} <options>'.format(os.path.basename(__file__)))
    print('       --help                  This message')
    print('       --verbose               Enable verbose output')
    print('       --dir=<dir>             Directory containing files [image/enpedia]')
    exit(1)


def main():
    global verbose
    global sizes
    global distribution
    global dist_list
    global total
    global byte_count

    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   'hvd:',
                                   ['help',
                                    'verbose',
                                    'dir='])
    except getopt.GetoptError as err:
        usage(err)

    verbose = False
    dir = 'image/enpedia'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-d', '--dir'):
            dir = arg
        else:
            usage('unhandled option: ' + opt)

    if not os.path.isdir(dir):
        usage('{0:s} is not a directory'.format(dir))

    idx_file = open(os.path.join(dir, "wiki.idx"), "rb")
    fnd_file = open(os.path.join(dir, "wiki.fnd"), "rb")

    dat_format = os.path.join(dir, "wiki{0:d}.dat")

    index_min = 1
    index_max = struct.unpack('<I', idx_file.read(4))[0]

    PrintLog.message('Total index entries = {0:d}'.format(index_max))

    total = 0
    sizes = {}
    distribution = {}
    byte_count = {}
    dist_list = [100, 200, 300, 400, 500, 600, 700, 800, 900,
                 1000, 2000, 3000, 5000, 7500,
                 10000, 20000, 50000,
                 100000, 200000, 500000,
                 99999999]
    for d in dist_list:
        distribution[d] = 0
        byte_count[d] = 0

    for item in range(index_max):
        index_number = 1 + item

        if index_number not in sizes:
            process(index_number, idx_file, fnd_file, dat_format)

    PrintLog.message('{0:>10s}   {1:>20s} {2:>20s}  {3:>14s}'.format('Size(<=)', 'Articles', 'Accumulated', 'Bytes'))
    sum = 0
    for i in dist_list:
        sum += distribution[i]
        PrintLog.message('{0:10n} = {1:10n} {2:7.1f} % {3:10n} {4:7.1f} %  {5:14n}'
                         .format(i,
                                 distribution[i],
                                 100.0 * distribution[i] / index_max,
                                 sum,
                                 100.0 * sum / index_max,
                                 byte_count[i]))

    PrintLog.message('summed = {0:10n}'.format(sum))
    PrintLog.message('sizes  = {0:10n}'.format(len(sizes)))
    PrintLog.message('total  = {0:10n}'.format(total))
    idx_file.close()
    fnd_file.close()


def process(index_number, idx_file, fnd_file, dat_format):
    """dump the index and fnd file entries"""

    global verbose
    global sizes
    global distribution
    global dist_list
    global total
    global byte_count

    if verbose:
        PrintLog.message('Index number = {0:10n} 0x{0:08x}'.format(index_number))

    uint32_size = 4
    index_entry_size = 2 * uint32_size + 1

    index_offset = uint32_size + index_entry_size * (index_number - 1)

    idx_file.seek(index_offset)
    offset_dat, offset_fnd, file_id = struct.unpack('<2IB', idx_file.read(index_entry_size))

    data_file_name = dat_format.format(file_id)

    dat_file = open(data_file_name, 'rb')
    dat_file.seek(offset_dat)

    number_of_pages = struct.unpack('B', dat_file.read(1))[0]

    for i in range(0, number_of_pages):
        page_id, page_offset, page_length = struct.unpack('<3I', dat_file.read(12))
        restricted = 'Restricted' if (0 != page_offset & 0x80000000) else ''
        page_offset = page_offset & 0x7fffffff

        if page_id in sizes:
            PrintLog.message('DUP: {0:10n}'.format(page_id))

        sizes[page_id] = page_length
        for d in dist_list:
            if page_length <= d:
                distribution[d] += 1
                byte_count[d] += page_length
                total += 1
                break

    dat_file.close()


# run the program
if __name__ == "__main__":
    main()

