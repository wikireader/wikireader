# test main board
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Main board test
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import Keithley
import Agilent
import RelayBoard
import communication
import process

import sys
import time


if 'serial_number' in global_args:
    serial_number = global_args['serial_number']
else:
    serial_number = 'No Serial Number'


psu = None
dvm = None
relay = None

RELAY_SERIAL = '/dev/USBrelay'
CPU_SERIAL = '/dev/USBconsole'

RELAY_VBATT = 1
RELAY_RESET = 2
RELAY_POWER_SWITCH = 3
RELAY_RANDOM_KEY = 4
RELAY_HISTORY_KEY = 5
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

# volts
LCD_V0 = 21.0

# power supply (volts, amps)
SUPPLY_STANDARD_VOLTAGE = 3.0
SUPPLY_CURRENT_LIMIT = 0.35

# specify the voltages actual and +/- percentages
# or minimum/maximum
VOLTAGE_LIST = (
#   ("text", relay_name,   required_value,       percent_low, percent_high)
#   ("text", relay_name,   None,                 minimum, maximum)
    ("1V8",  RELAY_1V8,    1.8,                  -5,   5),
    ("3V ",  RELAY_3V,     None,                  3.2, 3.4),
    ("V0 ",  RELAY_LCD_V0, LCD_V0,               -7.0, 7.0),
    ("V1 ",  RELAY_LCD_V1, LCD_V0 * 14.0 / 15.0, -7.0, 7.0),
    ("V2 ",  RELAY_LCD_V2, LCD_V0 * 13.0 / 15.0, -7.0, 7.0),
    ("V3 ",  RELAY_LCD_V3, LCD_V0 *  2.0 / 15.0, -7.0, 7.0),
    ("V4 ",  RELAY_LCD_V4, LCD_V0 *  1.0 / 15.0, -7.0, 7.0)
)

# amps
MAXIMUM_LEAKAGE_CURRENT = 0.002
MAXIMUM_SUSPEND_CURRENT = 0.02
MINIMUM_ON_CURRENT = 0.005
MAXIMUM_ON_CURRENT = 0.150

# seconds
MINIMUM_ON_TIME = 0.01
MAXIMUM_ON_TIME = 1.2
MINIMUM_OFF_TIME = 1.7
MAXIMUM_OFF_TIME = 4.0
ON_OFF_DELTA = 0.01
AVERAGING_DELAY = 0.02
SETTLING_TIME = 0.25
VOLTAGE_SAMPLE_TIME = 0.02
VOLTAGE_SAMPLE_OFF = 0.01
RESET_TIME = 0.5

# 1/10 seconds
ON_OFF_SCAN = int(5 / ON_OFF_DELTA)

# function keys
KEY_LIST = [
    ('Random',  RELAY_RANDOM_KEY,  '0x02'),
    ('History', RELAY_HISTORY_KEY, '0x04'),
    ('Search',  RELAY_SEARCH_KEY,  '0x01')
]


def setUp():
    """Set up power supply and turn on

       Also put a message on the PSU LCD to warn operator"""
    global debug, psu, dvm, relay
    info('setUp: **initialising**')

    relay = RelayBoard.PIC16F873A(port = RELAY_SERIAL)

    dvm = Agilent.DMM34401A()
    dvm.setVoltageDC()

    psu = Keithley.PSU2303()
    psu.setCurrent(SUPPLY_CURRENT_LIMIT)
    psu.setVoltage(SUPPLY_STANDARD_VOLTAGE)
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
    info('tearDown: **cleanup**')
    del psu
    psu = None
    del dvm
    dvm = None
    del relay
    relay = None


def test001_leakage():
    """Make sure power is off and no leakage"""
    global debug, psu, dvm, relay
    relay.off(RELAY_POWER_SWITCH)
    time.sleep(RESET_TIME)
    psu.powerOn()
    time.sleep(SETTLING_TIME)
    if debug:
        psu.settings()
        psu.measure()
    i = psu.current
    info('Leakage current = %7.3f mA @ %5.1f V' % (1000 * i, psu.voltage))
    fail_if(abs(i) > MAXIMUM_LEAKAGE_CURRENT, "Leakage current %7.3f mA is too high" % (i * 1000))


def test002_on():
    """Turn on power and wait for current to rise"""
    global debug, psu, dvm, relay
    relay.set(RELAY_PROGRAM_FLASH)
    relay.set(RELAY_POWER_SWITCH)
    relay.update()
    t = time.time()
    for i in range(ON_OFF_SCAN):
        if psu.current >= MINIMUM_ON_CURRENT:
            break
        time.sleep(ON_OFF_DELTA)
    t = time.time() - t
    relay.off(RELAY_POWER_SWITCH)
    time.sleep(SETTLING_TIME)
    info('On current = %7.3f mA @ %5.1f V' % (1000 * psu.current, psu.voltage))
    fail_unless(psu.current >= MINIMUM_ON_CURRENT, "Failed to Power On")
    fail_if(t < MINIMUM_ON_TIME, "On too short, %5.1f s < %5.1f" % (t, MINIMUM_ON_TIME))
    fail_if(t > MAXIMUM_ON_TIME, "On too long, %5.1f s > %5.1f" % (t, MAXIMUM_ON_TIME))


def test003_check_current():
    """Monitor current to check if correct"""
    global debug, psu, dvm, relay
    averageCurrent = 0.0
    samples = 20
    for i in range(samples):
        if debug:
            psu.measure()
        time.sleep(AVERAGING_DELAY)
        i = psu.current
        averageCurrent = averageCurrent + i
        info('Supply current = %7.3f mA @ %5.1f V' % (1000 * i, psu.voltage))
        fail_unless(abs(i) > MINIMUM_ON_CURRENT, "Device failed to power up")
        fail_if(abs(i) > MAXIMUM_ON_CURRENT, "Device current too high")
    info('Average supply current = %7.3f mA @ %5.1f V' % (1000 * averageCurrent / samples, psu.voltage))


def test004_measure_voltages():
    """Measure voltages"""
    global debug, psu, dvm, relay
    for item in VOLTAGE_LIST:
        v = item[0]
        r = item[1]
        if None == item[2]:
            min = item[3]
            max = item[4]
        else:
            min = item[2] * (100 + item[3]) / 100
            max = item[2] * (100 + item[4]) / 100
        relay.on(r)
        time.sleep(VOLTAGE_SAMPLE_TIME)
        actual = dvm.voltage
        info('%s = %7.3f V' % (v, actual))
        fail_if(actual < min, "Low Voltage %s = %7.3f < %7.3f" % (v, actual, min))
        fail_if(actual > max, "High Voltage %s = %7.3f > %7.3f" % (v, actual, max))
        relay.off(r)
        time.sleep(VOLTAGE_SAMPLE_OFF)


def test005_power_off():
    """Check power off function"""
    global debug, psu, dvm, relay
    relay.clear(RELAY_PROGRAM_FLASH)
    relay.set(RELAY_POWER_SWITCH)
    relay.update()
    t = time.time()
    for i in range(ON_OFF_SCAN):
        if psu.current < MINIMUM_ON_CURRENT:
            break
        time.sleep(ON_OFF_DELTA)
    t = time.time() - t
    relay.off(RELAY_POWER_SWITCH)
    time.sleep(2)
    i = psu.current
    fail_unless(abs(i) < MAXIMUM_LEAKAGE_CURRENT, "Failed to power off , %7.3f mA" % (i * 1000))
    fail_if(t < MINIMUM_OFF_TIME, "Off too short, %5.1f s < %5.1f" % (t, MINIMUM_OFF_TIME))
    fail_if(t > MAXIMUM_OFF_TIME, "Off too long, %5.1f s > %5.1f" % (t, MAXIMUM_OFF_TIME))


def test006_on():
    """Turn on power and wait for current to rise"""
    global debug, psu, dvm, relay
    test002_on()


def test007_program_flash():
    """Program the boot loader into FLASH memory"""
    global debug, psu, dvm, relay
    p = communication.SerialPort(port = CPU_SERIAL)

    relay.on(RELAY_RESET)
    relay.clear(RELAY_PROGRAM_FLASH)
    relay.clear(RELAY_RANDOM_KEY)
    relay.clear(RELAY_HISTORY_KEY)
    relay.clear(RELAY_SEARCH_KEY)
    relay.set(RELAY_RXD)
    relay.set(RELAY_TXD)
    relay.update()
    time.sleep(RESET_TIME)
    relay.off(RELAY_RESET)

    p.waitFor('menu\?')
    p.send(' ')

    m_flash = p.waitFor('(.)\.\s+[fF][lL][aA][sS][hH]\s+[mM][bB][rR]')

    p.waitFor('[sS]election:')
    p.send(m_flash.group(1))

    m_flash = p.waitFor('(PASS|FAIL):\s+FLASH\s+MBR')
    fail_unless('PASS' == m_flash.group(1), 'FLASH MBR')
    info('Flash programming sucessful')

def test008_keys():
    """Test the three function keys"""
    global debug, psu, dvm, relay
    p = communication.SerialPort(port = CPU_SERIAL)

    relay.on(RELAY_RESET)
    relay.clear(RELAY_PROGRAM_FLASH)
    relay.clear(RELAY_RANDOM_KEY)
    relay.clear(RELAY_HISTORY_KEY)
    relay.clear(RELAY_SEARCH_KEY)
    relay.set(RELAY_RXD)
    relay.set(RELAY_TXD)
    relay.update()
    time.sleep(RESET_TIME)
    relay.off(RELAY_RESET)

    p.waitFor('menu\?')
    p.send(' ')

    m_mem = p.waitFor('(.)\.\s+[mM]emory\s+[cC]heck')
    m_key = p.waitFor('(.)\.\s+[kK]ey\s+[tT]est')

    p.waitFor('[sS]election:')
    p.send(m_mem.group(1))

    m_mem = p.waitFor('[mM]emory:[^\]]+\]')
    info(m_mem.group(0))

    m_mem = p.waitFor('(PASS|FAIL):\s+(.*)\n')
    fail_unless('PASS' == m_mem.group(1), m_mem.group(2))
    info(m_mem.group(2) + ': ' + m_mem.group(2))

    p.waitFor('[sS]election:')
    p.send(m_key.group(1))

    for desc, r, k in KEY_LIST:
        relay.off(r)
        p.waitFor('keys = ')
        key = p.read(4)
        info('key (none) = %s' % key)
        fail_unless('0x00' == key, 'Invalid keys: wanted %s, got %s' % ('0x00', key))
        relay.on(r)
        p.waitFor('keys = ')
        key = p.read(4)
        i = psu.current
        info('Supply current = %7.3f mA' % (1000 * i))
        info('key (%s)[%s] = %s' % (desc, k, key))
        fail_unless(k == key, 'Invalid keys: wanted %s, got %s' % (k, key))
        relay.off(r)

    p.waitFor('keys = ')
    key = p.read(4)
    info('key (none) = %s' % key)
    fail_unless('0x00' == key, 'Invalid keys: wanted %s, got %s' % ('0x00', key))

    test004_measure_voltages()

    info('sending auto power off sequence')
    p.send('\n0\n')
    del p
    p = None

    for n in range(5):
        time.sleep(SETTLING_TIME)
        i = psu.current
        info('Supply current = %7.3f mA' % (1000 * i))
        if abs(i) < MAXIMUM_LEAKAGE_CURRENT:
            break
    fail_if(abs(i) > MAXIMUM_LEAKAGE_CURRENT, "Failed auto power off, current %7.3f mA is too high" % (i * 1000))


def test009_on():
    """Turn on power and wait for current to rise"""
    global debug, psu, dvm, relay
    test002_on()


def test010_boot_sd_card():
    """Boot the test program from the SD Card"""
    global debug, psu, dvm, relay
    p = communication.SerialPort(port = CPU_SERIAL)

    relay.on(RELAY_RESET)
    relay.clear(RELAY_PROGRAM_FLASH)
    relay.clear(RELAY_RANDOM_KEY)
    relay.clear(RELAY_HISTORY_KEY)
    relay.clear(RELAY_SEARCH_KEY)
    relay.set(RELAY_RXD)
    relay.set(RELAY_TXD)
    relay.update()
    time.sleep(RESET_TIME)
    relay.off(RELAY_RESET)

    p.waitFor('menu\?')
    p.send(' ')

    m_boot = p.waitFor('(.)\.\s+[bB]oot\s+[tT]est\s+[pP]rogram')

    p.waitFor('[sS]election:')
    p.send(m_boot.group(1))

    info('Starting the test program')

    while True:
        m_boot = p.waitFor('(\*SUSPEND\*|(PASS|FAIL):\s+(.*)(\r|\n))')
        if  m_boot.group(1) == '*SUSPEND*':
            break
        fail_unless('PASS' == m_boot.group(2), m_boot.group(3))
        info(m_boot.group(2) + ': ' + m_boot.group(3))

    samples = 20
    total = 0
    for j in range(samples):
        i = psu.current
        total = total + i
        info('suspend current = %7.3f mA @ %5.1f V\n' %
             (1000 * i, psu.voltage))
        time.sleep(0.1)
        info('average suspend current = %7.3f mA @ %5.1f V\n' %
             (1000 * total / samples, psu.voltage))

    if abs(i) > MAXIMUM_SUSPEND_CURRENT:
        p.send('N')
    else:
        p.send('Y')

    m_boot = p.waitFor('(PASS|FAIL):\s+(.*)\n')
    fail_unless('PASS' == m_boot.group(1), m_boot.group(2))
    info(m_boot.group(2) + ': ' + m_boot.group(2))

    p.waitFor('\*END-TEST\*')


def test011_power_off():
    """Check power off function"""
    global debug, psu, dvm, relay
    test005_power_off()
