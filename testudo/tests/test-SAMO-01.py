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

import time


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

LCD_V0 = 23.0

# ("text", relay_name, required_value, percent_low, percent_high)
VOLTAGE_LIST = (
    ("1V8", RELAY_1V8, 1.8, -5, 5),
    ("3V", RELAY_3V, 3.0, -5, 5),
    ("V0", RELAY_LCD_V0, LCD_V0, -5, 5),
    ("V1", RELAY_LCD_V1, LCD_V0 * 14.0 / 15.0, -5, 5),
    ("V2", RELAY_LCD_V2, LCD_V0 * 13.0 / 15.0, -5, 5),
    ("V3", RELAY_LCD_V3, LCD_V0 * 2.0 / 15.0, -5, 5),
    ("V4", RELAY_LCD_V4, LCD_V0 * 1.0 / 15.0, -5, 5)
)

# amps
MAXIMUM_LEAKAGE_CURRENT = 0.002
MINIMUM_ON_CURRENT = 0.005

# seconds
MINIMUM_ON_TIME = 0.01
MAXIMUM_ON_TIME = 1.2
MINIMUM_OFF_TIME = 1.7
MAXIMUM_OFF_TIME = 4.0
ON_OFF_DELTA = 0.1

# 1/10 seconds
ON_OFF_SCAN = int(5 / ON_OFF_DELTA)


def setUp():
    """Set up power supply and turn on

       Also put a message on the PSU LCD to warn operator"""
    global debug, psu, dvm, relay
    if debug:
        print 'setUp: **initialising**'

    relay = RelayBoard.PIC16F873A()

    dvm = Agilent.DMM34401A()
    dvm.setVoltageDC()

    psu = Keithley.PSU2303()
    psu.setCurrent(0.35)
    psu.setVoltage(3.0)
    psu.powerOff()
    if debug:
        psu.settings()
        psu.measure()
    psu.message('Test in progress  Do NOT Touch  ')
    relay.on(RELAY_VBATT)


def tearDown():
    """Shutdown the power supply"""
    global debug, psu, dvm, relay
    relay.off(RELAY_VBATT)
    psu.powerOff()
    psu.setCurrent(0)
    psu.setVoltage(0)
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
    relay.off(RELAY_POWER_SWITCH)
    time.sleep(0.2)
    psu.powerOn()
    time.sleep(0.5)
    if debug:
        psu.settings()
        psu.measure()
    i = psu.current
    assert abs(i) < MAXIMUM_LEAKAGE_CURRENT, "Leakage current %7.3f mA is too high" % (i * 1000)


def test002_on():
    """Turn on power and wait for current to rise"""
    global debug, psu, dvm, relay
    relay.on(RELAY_POWER_SWITCH)
    t = time.time()
    for i in range(ON_OFF_SCAN):
        if psu.current >= MINIMUM_ON_CURRENT:
            break
        time.sleep(ON_OFF_DELTA)
    t = time.time() - t
    relay.off(RELAY_POWER_SWITCH)
    time.sleep(0.5)
    assert psu.current >= MINIMUM_ON_CURRENT, "Failed to Power On"
    assert t > MINIMUM_ON_TIME, "On too short, %5.1f s < %5.1f" % (t, MINIMUM_ON_TIME)
    assert t < MAXIMUM_ON_TIME, "On too long, %5.1f s > %5.1f" % (t, MAXIMUM_ON_TIME)


def test003_check_booted():
    """How to find out if booted?"""
    global debug, psu, dvm, relay
    for i in range(10):
        if debug:
            psu.measure()
        time.sleep(0.1)
        i = psu.current
        assert abs(i) > 0.01, "Device failed to power up"

def test004_measure_voltages():
    """Measure voltages"""
    global debug, psu, dvm, relay
    for item in VOLTAGE_LIST:
        v = item[0]
        r = item[1]
        min = item[2] * (100 + item[3]) / 100
        max = item[2] * (100 + item[4]) / 100
        relay.on(r)
        time.sleep(0.5)
        actual = dvm.voltage
        if debug:
            print '%s = %7.3f V' % (v, actual)
        assert actual >= min, "Low Voltage %s = %7.3f < %7.3f" % (v, actual, min)
        assert actual <= max, "High Voltage %s = %7.3f > %7.3f" % (v, actual, max)
        relay.off(r)
        time.sleep(0.5)


def test005_power_off():
    """Check power off function"""
    global debug, psu, dvm, relay
    relay.on(RELAY_POWER_SWITCH)
    t = time.time()
    for i in range(ON_OFF_SCAN):
        if psu.current < MINIMUM_ON_CURRENT:
            break
        time.sleep(ON_OFF_DELTA)
    t = time.time() - t
    relay.off(RELAY_POWER_SWITCH)
    time.sleep(2)
    i = psu.current
    assert abs(i) < MAXIMUM_LEAKAGE_CURRENT, "Failed to power off , %7.3f mA" % (i * 1000)
    assert t > MINIMUM_OFF_TIME, "Off too short, %5.1f s < %5.1f" % (t, MINIMUM_OFF_TIME)
    assert t < MAXIMUM_OFF_TIME, "Off too long, %5.1f s > %5.1f" % (t, MAXIMUM_OFF_TIME)


def test006_on():
    """Turn on power and wait for current to rise"""
    global debug, psu, dvm, relay
    test002_on()


def test007_program_flash():
    global debug, psu, dvm, relay
    relay.set(RELAY_RESET)
    relay.set(RELAY_PROGRAM_FLASH)
    relay.set(RELAY_RXD)
    relay.set(RELAY_TXD)
    relay.update()


    def callback(s):
        sys.stdout.flush()
        if 'Press Reset' == s.strip():
            relay.on(RELAY_RESET)
            time.sleep(0.2)
            relay.off(RELAY_RESET)

    p = process.Process('make flash-mbr', callback)

    rc = p.run()
    assert 0 == rc, 'Flashing failed'
    relay.off(RELAY_PROGRAM_FLASH)


def test008_keys():
    global debug, psu, dvm, relay
    p = communication.SerialPort()

    relay.on(RELAY_RESET)
    relay.off(RELAY_PROGRAM_FLASH)
    relay.off(RELAY_RANDOM_KEY)
    relay.off(RELAY_TREE_KEY)
    relay.off(RELAY_SEARCH_KEY)

    relay.on(RELAY_RXD)
    relay.on(RELAY_TXD)
    time.sleep(0.2)
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

def test009_power_off():
    """Check power off function"""
    global debug, psu, dvm, relay
    test005_power_off()
