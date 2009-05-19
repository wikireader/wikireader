# Agilent
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Classes for Aglient GPIB instruments
# AUTHOR: Christopher Hall <hsw@openmoko.com>

try:
    from Gpib import *
except:
    print 'error: Missing python module: linux-gpib'
    exit(1)

from time import sleep


class DMM34401A(Gpib):

    def __init__(self, address = 22, gpib_card = 0):
        Gpib.__init__(self, gpib_card, address)
        self.clear()
        sleep(0.1)
        self.write('*rst\n')
        self.write('*cls\n')

    def __del__(self):
        sleep(0.1)
        Gpib.__del__(self)

    def setVoltageDC(self):
        self.write(':configure:voltage:dc def,def\n')

    def setCurrentDC(self):
        self.write(':configure:current:dc def,def\n')

    @property
    def voltage(self):
        self.write(':measure:voltage:dc?\n')
        return float(self.read().strip('\r\n \t'))

    @property
    def current(self):
        self.write(':measure:current:dc?\n')
        return float(self.read().strip('\r\n \t'))

    def messageOff(self):
        self.write(':display:window:text:state 0\n')


def main():
    dmm = DMM34401A()
    dmm.setVoltageDC()
    #sleep(1)
    v = dmm.voltage
    print 'V = %7.3f V' % v

if __name__ == '__main__':
    main()
