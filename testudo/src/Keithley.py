# Keithley
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Classes for Keithley GPIB instruments
# AUTHOR: Christopher Hall <hsw@openmoko.com>

try:
    from Gpib import *
except:
    print 'error: Missing python module: linux-gpib'
    exit(1)

from time import sleep


class PSU2303(Gpib):

    def __init__(self, address = 5, gpib_card = 0):
        Gpib.__init__(self, gpib_card, address)
        self.clear()
        sleep(0.1)
        self.write('*rst\n')
        self.write('*cls\n')
        self.write(':display:window:text:state 0\n')
        self.powerOff()
        self.messageOff()

    def __del__(self):
        self.powerOff()
        self.messageOff()
        sleep(0.25)
        self.clear()
        sleep(0.1)
        Gpib.__del__(self)

    def setVoltage(self, v, max = 4):
        if v < 0:
            v = 0
        elif v > max:
            v = max
        self.write(':source:voltage:amplitude %f\n' % v)


    def setCurrent(self, i, max = 1):
        if i < 0.01:
            i = 0.01
        elif i > max:
            i = max
        self.write(':source:current:limit:value %f\n' % i)

    def powerOff(self):
        self.write(':output off\n')

    def powerOn(self):
        self.write(':output on\n')

    def settings(self):
        self.write(':source:voltage:amplitude?\n')
        print 'Vs   = %7.3f V' % float(self.read().strip('\r\n \t'))
        self.write(':source:current:limit:value?\n')
        print 'Ilim = %7.3f mA' % (float(self.read().strip('\r\n \t')) * 1000)

    def measure(self):
        print 'V = %7.3f V' % self.voltage
        print 'I = %7.3f mA' % (self.current * 1000)

    @property
    def voltage(self):
        self.write(':measure:voltage:dc?\n')
        return float(self.read().strip('\r\n \t'))

    @property
    def current(self):
        self.write(':measure:current:dc?\n')
        return float(self.read().strip('\r\n \t'))

    def message(self, text):
        self.write(':display:window:text:data "%s"\n' % text)
        self.write(':display:window:text:state 1\n')

    def messageOff(self):
        self.write(':display:window:text:state 0\n')


def main():
    psu = PSU2303()
    psu.message('just testing')
    psu.setVoltage(2)
    psu.setCurrent(0.2)
    psu.powerOn()
    sleep(1)
    psu.settings()
    psu.measure()
    psu.powerOff()


if __name__ == '__main__':
    main()
