# test SAMO
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Sample test
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import Keithley
import Agilent
import RelayBoard
import communication
import process

from time import sleep


psu = None
dvm = None
relay = None


RELAY_VBATT = 1
RELAY_RESET = 2
RELAY_POWER_SWITCH = 3
RELAY_RANDOM_KEY = 4
RELAY_TREE_KEY = 5
RELAY_SEARCH_KEY = 6
RELAY_RXD  = 7
RELAY_TXD = 8
RELAY_PROGRAM_FLASH = 9
RELAY_1V8 = 10
RELAY_3V = 11
RELAY_LCD_V0 = 12
RELAY_LCD_V1 = 13
RELAY_LCD_V2 = 14
RELAY_LCD_V3 = 15
RELAY_LCD_V4 = 16

LCD_V0 = 21.0

# (relay_name, required_value, percent_low, percent_high)
VOLTAGE_LIST = {
    "1V8": (RELAY_1V8, 1.8, -5, 5),
    "3V": (RELAY_3V, 3.0, -5, 5),
    "V0": (RELAY_LCD_V0, LCD_V0, -5, 5),
    "V1": (RELAY_LCD_V1, LCD_V0 * 14.0 / 15.0, -5, 5),
    "V2": (RELAY_LCD_V2, LCD_V0 * 13.0 / 15.0, -5, 5),
    "V3": (RELAY_LCD_V3, LCD_V0 * 2.0 / 15.0, -5, 5),
    "V4": (RELAY_LCD_V4, LCD_V0 * 1.0 / 15.0, -5, 5)
}

def setUp():
    """Set up power supply and turn on

       Also put a message on the PSU LCD to warn operator"""
    global debug, psu, dvm, relay
    if debug:
        print 'setUp: **initialising**'

    relay = RelayBoard()

    dvm = Agilent.DMM34401A()
    dvn.setVoltageDC();

    psu = Keithley.PSU2303()
    psu.setCurrent(0.35)
    psu.setVoltage(3.0)
    psu.powerOff()
    if debug:
        psu.settings()
        psu.measure()
    psu.message('Test in progress  Do NOT Touch  ')

def tearDown():
    """Shutdown the power supply"""
    global debug, psu, dvm, relay
    psu.setCurrent(0)
    psu.setVoltage(0)
    psu.powerOff()
    psu.messageOff()
    if debug:
        print 'tearDown: **cleanup**'
    del psu
    psu = None
    del dvm
    dvm = None
    del relay
    relay = None

def testZzz():
    """Run this last"""
    pass

def test001_leakage():
    """Make sure power is off and no leakage"""
    global debug, psu, dvm, relay
    relay.on(RELAY_VBATT)
    i = psu.current
    assert abs(i) < 0.001, "Leakage current too high"

def test002_on():
    """Turn on power and wait for current to rise"""
    global debug, psu, dvm, relay
    relay.on(RELAY_POWER_SWITCH)
    sleep(0.25)
    relay.off(RELAY_POWER_SWITCH)

    for i in range(20):
        if psu.current > 0.01:
            break
        if debug:
            psu.measure()
        sleep(0.1)
    sleep(0.5)
    assert psu.current > 0.01, "Failed to Power On"

def test003_check_booted():
    """How to find out if booted?"""
    global debug, psu, dvm, relay
    for i in range(10):
        if debug:
            psu.measure()
        sleep(0.1)
        i = psu.current
        assert abs(i) > 0.01, "Device failed to power up"

def test004_measure_voltages():
    """Measure voltages"""
    global debug, psu, dvm, relay
    for v in VOLTAGE_LIST:
        r = VOLTAGE_LIST[v][0]
        min = VOLTAGE_LIST[v][1] * (100 + VOLTAGE_LIST[v][2]) / 100
        max = VOLTAGE_LIST[v][1] * (100 + VOLTAGE_LIST[v][3]) / 100
        relay.on(r)
        sleep(0.25)
        actual = dvm.voltage()
        if debug:
            print 'V[%s] = %7.3f V' % (v, actual)
        assert actual >= min, "Low Voltage %s = %7.3f < %f7.3" % (v, actual, min)
        assert actual <= man, "High Voltage %s = %7.3f > %f7.3" % (v, actual, max)
        relay.off(r)
        sleep(0.25)

def test005_power_off():
    """Check power off function"""
    global debug, psu, dvm, relay
    relay.on(RELAY_POWER_SWITCH)
    sleep(3.0)
    relay.off(RELAY_POWER_SWITCH)
    sleep(0.25)
    i = psu.current
    assert abs(i) < 0.001, "Device failed to turn off"


def test006_on():
    """Turn on power and wait for current to rise"""
    global debug, psu, dvm, relay
    relay.on(RELAY_POWER_SWITCH)
    sleep(0.25)
    relay.off(RELAY_POWER_SWITCH)

    for i in range(20):
        if psu.current > 0.01:
            break
        if debug:
            psu.measure()
        sleep(0.1)
    sleep(0.5)
    assert psu.current > 0.01, "Failed to Power On"


def test007_program_flash():
    global debug, psu, dvm, relay
    relay.on(RELAY_RESET)
    relay.on(RELAY_PROGRAM_FLASH)
    relay.on(RELAY_RXD)
    relay.on(RELAY_TXD)


    def callback(s):
        sys.stdout.flush();
        if 'Press Reset' == s.strip():
            relay.on(RELAY_RESET)
            sleep(0.2)
            relay.off(RELAY_RESET)

    p = process.Process('make flash-mbr')

    rc = p.run()
    assert 0 == rc, 'Flashing failed'
    relay.off(RELAY_PROGRAM_FLASH)


def test007_keys():
    global debug, psu, dvm, relay
    p = communication.SerialPort()

    relay.on(RELAY_RESET)
    relay.off(RELAY_PROGRAM_FLASH)
    relay.off(RELAY_RANDOM_KEY)
    relay.off(RELAY_TREE_KEY)
    relay.off(RELAY_SEARCH_KEY)

    relay.on(RELAY_RXD)
    relay.on(RELAY_TXD)
    sleep(0.2)
    relay.off(RELAY_RESET)

    p.waitFor('menu?')
    p.send('d')

    for r, k in [(RELAY_RANDOM_KEY, '0x02'), (RELAY_TREE_KEY, '0x04'), (RELAY_SEARCH_KEY, '0x04')]:
        relay.off(r)
        p.waitFor('keys = ')
        key = p.read(4)
        assert '0x00' == key, 'Invalid keys: wanted, got %s' % ('0x00', key)
        relay.on(r)
        p.waitFor('keys = ')
        key = p.read(4)
        assert k == key, 'Invalid keys: wanted, got %s' % (k, key)
        relay.off(r)
        p.waitFor('keys = ')
        key = p.read(4)
        assert '0x00' == key, 'Invalid keys: wanted, got %s' % ('0x00', key)

    del p
    p = None

def test008_power_off():
    """Check power off function"""
    global debug, psu, dvm, relay
    relay.on(RELAY_POWER_SWITCH)
    sleep(3.0)
    relay.off(RELAY_POWER_SWITCH)
    sleep(0.25)
    i = psu.current
    assert abs(i) < 0.001, "Device failed to turn off"


