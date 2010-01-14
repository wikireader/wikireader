#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import sys
import time

import Dump


COMMAND_BOARD_REVISION = 'A'
COMMAND_SPI_WRITE = 'W'
COMMAND_SPI_READ = 'R'
COMMAND_SPI_DESELECT = 'H'
COMMAND_SPI_SELECT= 'L'

def local_len(item):
    if str == type(item):
        return len(item)
    elif list == type(item):
        return sum(map(local_len, item))
    elif int == type(item):
        return 1
    return 0


class SPI:

    def __init__(self, port):
        self.s = port

    def version(self):
        self.deselect()
        self.internal_send(COMMAND_BOARD_REVISION)
        time.sleep(0.1)
        buffer = self.s.read(2)
        rev = ord(buffer[1])
        if rev < 5:
            return 'A%d' % rev
        return 'V%d' % (rev - 4)

    def command(self, buffer):
        self.select()
        self.internal_write(buffer)
        self.deselect()

    def transact(self, write_buffer, skip_length, read_length):
        self.select()
        self.internal_write(write_buffer)
        self.internal_read(skip_length)
        result = self.internal_read(read_length)
        self.deselect()
        return result

    # private functions below here

    def select(self):
        self.internal_send(COMMAND_SPI_SELECT)

    def deselect(self):
        self.internal_send(COMMAND_SPI_DESELECT)


    def internal_write(self, buffer):
        length = local_len(buffer)
        if 0 >= length:
            return
        assert length < 65536, "Write Length: %d too big" % length

        self.internal_send([COMMAND_SPI_WRITE, length, (length >> 8), buffer])


    def internal_read(self, length):
        if 0 >= length:
            return
        assert length < 65536, "Read Length: %d too big" % length

        self.internal_send([COMMAND_SPI_READ, length, length >> 8])
        buffer = self.s.read(length)
        return buffer


    def internal_send(self, item):
        if str == type(item):
            for c in item:
                self.s.write(c)
        elif list == type(item):
            for x in item:
                self.internal_send(x)
        elif int == type(item):
            self.s.write(chr(item & 0xff))
        else:
            assert True, 'Invalid send item: %s' % type(item)
