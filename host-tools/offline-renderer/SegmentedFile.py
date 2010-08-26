#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: segmented file handling
# AUTHORS: Christopher Hall <hsw@openmoko.com>

import sys, os
import os.path

class SegmentedFileReader(object):

    def __init__(self, filename_list, *args, **kw):
        """read a file that was split into segments"""

        #super(SegmentedFileReader, self).__init__(*args, **kw)

        if type(filename_list) is str:
            i = 0
            l = []
            while True:
                if 0 == i:
                    n = ''
                else:
                    n = '{0:d}'.format(i)
                path = filename_list.format(n)
                if not os.path.isfile(path):
                    break
                l += [path]
                i += 1
            filename_list = l

        if type(filename_list) is not list:
            raise ValueError('SegmentedFileReader: filename_list is not a list')

        self.start = {}
        self.size = {}
        i = 0
        start = 0
        for filename in filename_list:
            size = os.path.getsize(filename)
            self.size[i] = size
            self.start[i] = start
            start += size
            i += 1

        self.total_bytes = start
        self.filename_list = filename_list
        self.file_number = -1
        self.current_position = 0
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
        """get the filie position"""
        return self.current_position


    def seek(self, position):
        """set the current read position"""
        next = 0
        for i in range(0, len(self.start) + 1):
            next += self.size[i]
            if position >= self.start[i] and position < next:
                offset = position - self.start[i]
                self.file_number = i - 1
                self.open_next()
                self.file.seek(offset)
                return


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
        self.current_position += len(databuffer)
        return databuffer


class SegmentedFileWriter(object):

    def __init__(self, filename_format, segment_size, *args, **kw):
        """create a file that gets split into segments
        filename_format should be similar to: file{0:s}.ext"""

        #super(SegmentedFileWriter, self).__init__(*args, **kw)
        self.filename_format = filename_format
        self.file_number = -1
        self.segment_size = segment_size
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


    def open_next(self):
        """close current file and start a new one"""
        if self.file is not None:
            self.file.close()
        self.file_number += 1
        self.space_available = self.segment_size
        if self.file_number < 1:
            n = ''        # first file has no number
        else:
            n = '{0:d}'.format(self.file_number)
        self.current_filename = self.filename_format.format(n)
        self.file = open(self.current_filename, 'wb')


    def tell(self):
        return self.total_bytes


    def write(self, databuffer):
        """writes data respecting segment_size"""
        l = len(databuffer)
        self.total_bytes += l
        if l > self.space_available:
            avail = self.space_available
            self.file.write(databuffer[:avail])
            self.open_next()
            self.file.write(databuffer[avail:])
            self.space_available -= l - avail
        else:
            self.file.write(databuffer)
            self.space_available -= l
