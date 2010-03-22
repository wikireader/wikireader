#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Dump various files
# AUTHORS: Christopher Hall <hsw@openmoko.com>

import sys, os
import struct
import os.path
import pylzma
import getopt
import PrintLog
import pylzma
import locale


locale.setlocale(locale.LC_ALL, '')

verbose = False


def usage(message):
    if None != message:
        print('error: {0:s}'.format(message))
    print('usage: {0:s} <options> <indices...>'.format(os.path.basename(__file__)))
    print('       --help                  This message')
    print('       --verbose               Enable verbose output')
    print('       --dir=<dir>             Directory containing files [image/enpedia]')
    print('       --extract=<prefix>      Prefix for extracted data (no extraction)')
    exit(1)


def main():
    global verbose

    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   'hvd:e:',
                                   ['help',
                                    'verbose',
                                    'dir=',
                                    'extract=',
                                    ])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    dir = 'image/enpedia'
    extract = None

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-d', '--dir'):
            dir = arg
        elif opt in ('-e', '--extract'):
            extract = arg
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
    PrintLog.message('')

    for item in args:
        try:
            index_number = int(item)
        except ValueError:
            usage('"{0:s}" is not numeric'.format(item))

        if index_number < index_min or index_number > index_max:
            usage('index: {0:d} is outdide [{1:d} .. {2:d}]'.format(index_number, index_min, index_max))

        process(index_number, idx_file, fnd_file, dat_format, extract)


    idx_file.close()
    fnd_file.close()


def process(index_number, idx_file, fnd_file, dat_format, extract):
    """dump the index and fnd file entries"""

    PrintLog.message('Index number = {0:13n} [0x{0:08x}]'.format(index_number))
    PrintLog.message('')

    uint32_size = 4
    index_entry_size = 2 * uint32_size + 1

    index_offset = uint32_size + index_entry_size * (index_number - 1)
    idx_file.seek(index_offset)
    offset_dat, offset_fnd, file_id = struct.unpack('<2IB', idx_file.read(index_entry_size))

    data_file_name = dat_format.format(file_id)

    PrintLog.message('Index offset = {0:13n} [0x{0:08x}]'.format(index_offset))
    PrintLog.message('Data offset  = {0:13n} [0x{0:08x}]'.format(offset_dat))
    PrintLog.message('FND offset   = {0:13n} [0x{0:08x}]'.format(offset_fnd))
    PrintLog.message('File ID      = {0:13n} [0x{0:08x}] => "{1:s}"'.format(file_id, data_file_name))

    fnd_file.seek(offset_fnd)
    article_index_check = struct.unpack('<I', fnd_file.read(uint32_size))[0]

    index_match = '(Matches)' if article_index_check == index_number else '(**MISMATCHED INDEX**)'
    PrintLog.message('FND index    = {0:13n} [0x{0:08x}] {1:s}'.format(article_index_check, index_match))

    ignored = fnd_file.read(1)  # skip nul byte
    titles = fnd_file.read(1024).split('\0') # >= 2 * MAX_TITLE_SEARCH

    PrintLog.message('FND title    = "{0:s}"'.format(titles[1])) # actual title

    dat_file = open(data_file_name, 'rb')
    dat_file.seek(offset_dat)

    number_of_pages = struct.unpack('<B', dat_file.read(1))[0]
    PrintLog.message('Data Pages   = {0:13n} [0x{0:08x}]'.format(number_of_pages))
    PrintLog.message('')

    total_article_bytes = 0
    PrintLog.message('{0:>29s}{1:>25s}{2:>25s}'.format('Article Number', 'Article Offset', 'Uncompressed Length'))
    for i in range(0, number_of_pages):
        page_id, page_offset, page_length = struct.unpack('<3I', dat_file.read(3 * uint32_size))
        restricted = 'Restricted' if (0 != page_offset & 0x80000000) else ''
        page_offset = page_offset & 0x7fffffff
        total_article_bytes += page_length
        PrintLog.message('{0:3d}:  {1:10n} [0x{1:08x}]  {2:10n} [0x{2:08x}]  {3:10n} [0x{3:08x}]  {4:s}'
                         .format(i, page_id, page_offset, page_length, restricted))

    PrintLog.message('{0:<{1}s}{2:10n} [0x{2:08x}]'.format('Total bytes: ', 3+3+10+4+8+3+10+4+8+3, total_article_bytes))
    PrintLog.message('')


    data_length = struct.unpack('<I', dat_file.read(4))[0]
    PrintLog.message('DataLength  = {0:13n} [0x{0:08x}]'.format(data_length))

    article_data = dat_file.read(data_length)
    dat_file.close()
    if extract is not None:
        output_file_name = extract + '-I' + str(index_number) + '-b' + str(data_length) + '.articles'
        PrintLog.message('Extracting uncompressed articles to: {0:s}'.format(output_file_name))
        out = open(output_file_name, 'wb')
        out.write(pylzma.decompress(article_data))
        out.close()

    PrintLog.message('')


# run the program
if __name__ == "__main__":
    main()

