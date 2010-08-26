#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: dump the fnd file - for debugging
# AUTHORS: Christopher Hall <hsw@openmoko.com>

import sys, os
import struct
import os.path
import getopt
import PrintLog
import locale


locale.setlocale(locale.LC_ALL, '')

verbose = False


def usage(message):
    if None != message:
        print('error: {0:s}'.format(message))
    print('usage: {0:s} <options> <fnd-files>'.format(os.path.basename(__file__)))
    print('       --help                  This message')
    print('       --verbose               Enable verbose output')
    exit(1)


class SegmentedFileReader(object):

    def __init__(self, filename_list, *args, **kw):
        """read a file that was split into segments"""

        #super(SegmentedFileWriter, self).__init__(*args, **kw)
        self.filename_list = filename_list
        self.file_number = -1
        self.total_bytes = 0
        self.file = None
        self.open_next()

    def __del__(self):
        """close any open file"""
        self.close()


    def close(self):
        """close down the stream"""
        if self.file is not None:
            self.file.close()
        self.file = None


    def tell(self):
        return self.total_bytes


    def open_next(self):
        """close current file and start a new one"""
        if self.file is not None:
            self.file.close()
        self.file_number += 1
        if self.file_number >= len(self.filename_list):
            self.file = None
        else:
            self.current_filename = self.filename_list[self.file_number]
            self.file = open(self.current_filename, 'rb')


    def internal_read(self, byte_count):
        """read data crossing over to next segment"""
        if self.file is None:
            return ''
        databuffer = self.file.read(byte_count)
        l = len(databuffer)
        if l < byte_count:
            self.open_next()
            databuffer += self.internal_read(byte_count - l)
        return databuffer


    def read(self, byte_count):
        """read data crossing over to next segment"""
        if self.file is None:
            return ''
        databuffer = self.internal_read(byte_count)
        self.total_bytes += len(databuffer)
        return databuffer


def main():
    global verbose

    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   'hv',
                                   ['help',
                                    'verbose',
                                    ])
    except getopt.GetoptError, err:
        usage(err)

    verbose = False
    uint32_size = 4

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        else:
            usage('unhandled option: ' + opt)

    if len(args) < 1:
        usage('missing arguments')

    fnd_file = SegmentedFileReader(args)

    total_entries = 0

    bigram_table = {}
    for i in range(128,256):
        bigram_table[i] = fnd_file.read(2)

    previous_title1 = ''
    previous_title2 = ''

    while True:
        fnd_offset = fnd_file.tell()
        header = fnd_file.read(uint32_size + 1)

        if 0 == len(header):
            break

        article_number, nul_byte = struct.unpack('<IB', header)

        title1 = get_title(fnd_file)
        title2 = get_title(fnd_file)
        total_entries += 1

        length1 = len(title1)
        length2 = len(title2)

        if 0 != length1 and title1[0] < ' ':
            prefix_length = ord(title1[0]) + 1
            title1 = previous_title1[:prefix_length] + title1[1:]

        if 0 != length2 and title2[0] < ' ':
            prefix_length = ord(title2[0]) + 1
            title2 = previous_title2[:prefix_length] + title2[1:]

        full_length1 = len(title1)
        full_length2 = len(title2)

        decoded_title1 = ''
        for c in title1:
            i = ord(c)
            if i in bigram_table:
                decoded_title1 += bigram_table[i]
            else:
                decoded_title1 += c

        PrintLog.message(u'Index: {an:13n} @ Offset: {of:13n} [0x{of:08x}]\n'
                         u'{pad1:s}[{l1:3d}/{fl1:3d}]:{t1!r:s}\n'
                         u'{pad1:s}{pad2}{dt1!r:s}\n'
                         u'{pad1:s}[{fl1:3d}/{fl2:3d}]:"{t2:s}"\n'
                         .format(of = fnd_offset, an = article_number,
                                 l1 = length1, fl1 = full_length1, t1 = title1, dt1 = decoded_title1,
                                 pad1 = ' ' * 2, pad2 = ' ' * (2 * 3 + 4),
                                 l2 = length2, fl2 = full_length2, t2 = truncated_utf8(title2)))

        previous_title1 = title1
        previous_title2 = title2

    fnd_file.close()

    PrintLog.message(u'Total entries  = {0:13n}'.format(total_entries))


def truncated_utf8(text):
    """converted text to unicode even if the string is truncated"""
    while len(text) > 0:
        try:
            return unicode(text, 'utf-8')
        except UnicodeDecodeError:
            pass
        text = text[:-1]
    return u''


def get_title(f):
        c = 'X'
        title = ''
        while '\0' != c:
            c = f.read(1)
            title += c
        return title[:-1]


# run the program
if __name__ == "__main__":
    main()

