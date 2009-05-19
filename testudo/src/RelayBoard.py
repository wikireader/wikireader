# RelayBoard
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Classes for Serial Relay Board
# AUTHOR: Christopher Hall <hsw@openmoko.com>


try:
    from serial.serialposix import *
except:
    print 'error: Missing python module: python-serial'
    print '       sudo apt-get install python-serial'
    exit(1)

from time import sleep


class RelayBoard():

    def __init__(self, port = '/dev/ttyUSB0', bps = 19200, timeout = 0.1):
        self.relay = Serial(port = port)
        self.relay.xonxoff = False
        self.relay.rtscts = False
        self.relay.baudrate = bps
        self.relay.timeout = timeout
        self.relay.open()
        self.allOff()

    def __del__(self):
        self.relay.close()

    def allOff(self):
        """Swich off all realays immediately"""
        self.now = 0
        self.map = 0
        self.update()

    def update(self):
        """Set relay pending state to current state"""
        s = '{%04x}' % (self.map & 0xffff)
        self.now = self.map
        for c in s:
            self.relay.write(c)
            while 0 == self.relay.inWaiting():
                sleep(0.001)
            self.relay.read(1)

    def state(self):
        """Return the state of the relays"""
        return self.now

    def pending(self):
        """Return the pending state of the relays"""
        return self.map

    def set(self, n):
        """Prepare to turn relay on"""
        if 1 > n or 16 < n:
            return
        self.map |= 1 << (n - 1)

    def clear(self, n):
        """Prepare to turn relay off"""
        if 1 > n or 16 < n:
            return
        self.map &= ~(1 << (n - 1))

    def on(self, n):
        """Turn relay on (and update pending relays)"""
        self.set(n):
        self.update();

    def off(self, n):
        """Turn relay off (and update pending relays)"""
        self.clear(n):
        self.update();


def main():
    r = RelayBoard()
    r.set(4)
    r.set(7)
    r.update()
    sleep(0.25)
    r.allOff()
    sleep(0.1)

    for x in range(5):
        for i in range(1, 17):
            r.set(i)
            r.update()
            sleep(0.02)
            r.clear(i)

    r = None

if __name__ == '__main__':
    main()
