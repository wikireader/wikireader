#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Serial port commands to the in flash tests
# AUTHOR: Christopher Hall <hsw@openmoko.com>


try:
    from serial.serialposix import *
except:
    print 'error: Missing python module: python-serial'
    print '       sudo apt-get install python-serial'
    exit(1)

import sys
import time
import re


class SerialPort():

    def __init__(self, port = '/dev/USBconsole', bps = 19200, timeout = 0.2):
        self.s = Serial(port = port)
        self.s.xonxoff = False
        self.s.rtscts = False
        self.s.baudrate = bps
        self.s.timeout = timeout
        self.s.open()

    def __del__(self):
        self.s.close()

    def flush(self):
        if 0 < self.s.inWaiting():
            self.s.read(self.s.inWaiting())

    def waitFor(self, s, timeout = 30):
        regexp = re.compile(s)
        buffer = ''
        while True:
            m = regexp.search(buffer)
            if m:
                return m
            while 0 == self.s.inWaiting():
                time.sleep(0.01)

            buffer = buffer + self.s.read(1)

    def send(self, s):
        self.s.write(s)

    def read(self, length):
        return self.s.read(length)


def main():
    p = SerialPort()
    p.waitFor('menu?')
    p.send('d')

    while True:
        p.waitFor('keys = ')
        key = p.read(4)

        if '0x00' == key:
            print 'none'
        elif '0x01' == key:
            print 'one'
        elif '0x02' == key:
            print 'two'
        elif '0x04' == key:
            print 'four'
        else:
            print '?key =', key
    del p
    p = None

if __name__ == '__main__':
    main()
