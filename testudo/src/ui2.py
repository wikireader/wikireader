#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Simple UI for running test
# AUTHOR: Christopher Hall <hsw@openmoko.com>

from __future__ import with_statement # This isn't required in Python 2.6

try:
    from serial.serialposix import *
except:
    print 'error: Missing python module: python-serial'
    print '       sudo apt-get install python-serial'
    exit(1)

import pygtk
pygtk.require('2.0')
import gtk
import pango

import sys
import datetime
import time
import os.path
import threading
import traceback

import Keithley

import sequencer


# power supply (volts, amps)
SUPPLY_STANDARD_VOLTAGE = 3.0
SUPPLY_CURRENT_LIMIT = 0.35
MAXIMUM_SUSPEND_CURRENT = 0.02
MINIMUM_ON_CURRENT = 0.012
MAXIMUM_ON_CURRENT = 0.150

# paths to files
SaveFilesFolder = '/log/files/stage2'
FileExt = '.2.text'

if not os.path.isdir(SaveFilesFolder):
    print 'error: %s is not a directory' % SaveFilesFolder
    print '       sudo mkdir -p %s' % SaveFilesFolder
    exit(1)


class SerialPort():

    def __init__(self, port = '/dev/USBconsole', bps = 19200, timeout = 0.2):
        self.s = Serial(port = port)
        self.s.xonxoff = False
        self.s.rtscts = False
        self.s.baudrate = bps
        self.s.timeout = timeout
        self.s.open()
        self.resetLow()
	self.s.flushOutput()
	self.s.flushInput()

    def __del__(self):
        self.s.close()

    def flush(self):
	self.s.flushInput()
        print 'flush:', self.s.inWaiting()
        if 0 < self.s.inWaiting():
            self.s.read(self.s.inWaiting())

    def write(self, text):
        self.s.write(text)
        self.s.flush()

    def readChar(self):
        return self.s.read(1)

    def resetLow(self):
        self.s.setDTR(True)
	self.s.flushInput()
	self.s.flushOutput()

    def resetHigh(self):
        self.s.setDTR(False)
	self.s.flushInput()
	self.s.flushOutput()

class ProgramPin():

    def __init__(self, port = '/dev/USBjtag', bps = 19200, timeout = 0.2):
        self.s = Serial(port = port)
        self.s.xonxoff = False
        self.s.rtscts = False
        self.s.baudrate = bps
        self.s.timeout = timeout
        self.s.open()
        self.setHigh()

    def __del__(self):
        self.s.close()

    def setHigh(self):
        self.s.setDTR(False)

    def setLow(self):
        self.s.setDTR(True)


def threaded(f):
    """Create a simple wrapper that allows a task to run in the background"""
    def wrapper(*args):
        t = threading.Thread(target = f, args = args)
        t.daemon = True
        t.start()
        return t
    return wrapper


class TestException(Exception):

    def __init__(self, value):
        self.value = value

    def __str__(self):
        return self.value


class StopTestException(TestException):
    pass


class Sample:

    def delete_event(self, widget, event, data = None):
        #print "delete event occurred"
        self.testStop = True
        if not self.testRunning:
            gtk.main_quit()
        return self.testRunning

    def destroy(self, widget, data=None):
        #print 'destroy signal occurred'
        if not self.testRunning:
            gtk.main_quit()

    def clearScreen(self, widget, data = None):
        """Initiate the test"""
        if not self.testRunning:
            start = self.buffer.get_start_iter()
            end = self.buffer.get_end_iter()
            self.buffer.delete(start, end)

    def startCallback(self, widget, data = None):
        """Initiate the test"""
        if not self.testRunning:
            # ensure the GPIB driver is started
            os.system('sudo /etc/init.d/gpib reload')

            self.testFailed = False
            self.testStop = False
            self.status.set_text('Running')
            self.testRunning = True
            self.runTest()

    def stopCallback(self, widget, data = None):
        """Request test stop"""
        if self.testRunning:
            self.testStop = True
            self.status.set_text('.....Stopping at end of this test cycle.....')

    @threaded
    def runTest(self):
        """main test routine"""

        auto = self.autoSave.get_active()
        if auto:
            try:
                gtk.gdk.threads_enter()
                start = self.buffer.get_start_iter()
                end = self.buffer.get_end_iter()
                self.buffer.delete(start, end)
            finally:
                gtk.gdk.threads_leave()

        try:
            s = None
            program = None
            psu = None

            s = SerialPort()
            serialNumber = self.serialNumber.get_text().strip()
            versionNumber = self.versionNumber.get_text().strip()

            if '' == serialNumber:
                raise StopTestException('Serial number invalid')

            program = ProgramPin()

            psu = Keithley.PSU2303()
            psu.setCurrent(SUPPLY_CURRENT_LIMIT)
            psu.setVoltage(SUPPLY_STANDARD_VOLTAGE)
            psu.powerOn()

            self.write('\n*** Start of Test for device SN: %s ***\n\n' %
                       (serialNumber))
            t = time.time()

            self.write('*** Press device power on button to begin test ***\n\n')

            starting = True
            run = True
            current_version = 'NONE'
            while run:

                line = ''
                while not self.testStop:
                    c = s.readChar()
                    if '\n' == c:
                        break
                    if '' == c or ' ' > c:
                        if starting:
                            i = psu.current
                            self.write('INFO: starting, current = %7.3f mA @ %5.1f V\n' %
                                   (1000 * i, psu.voltage))
                            if MINIMUM_ON_CURRENT < i:
                                starting = False
                                s.resetHigh()
                                s.write('  ')
				time.sleep(0.02)
                                s.write('                           ')
                        continue
                    line = line + c
                    if 'menu?' == line:
                        s.write(' ')
                        self.write('Menu detected\n')

                if self.testStop:
                    raise StopTestException('Stop button pressed')

                self.write(line)
                self.write('\n')

                # verify the serial number
                if line.startswith('S/N:'):
                    psn = line.split(' ')[-1]
                    if serialNumber == psn:
                        self.write('PASS: Programmed S/N matches\n', 'pass-text')
                    else:
                        self.write('FAIL: FLASH S/N(%s) != %s\n' % (psn, serialNumber), 'fail-text')
                        raise StopTestException('Serial Number mismatch')

                elif line.startswith('VERSION:'):
                    current_version = line.split(' ')[-1]

                if '. Boot Test Program' == line[1:]:
                    s.write(line[0:1])

                elif '*SUSPEND*' == line:
                    samples = 20
                    total = 0
                    for j in range(samples):
                        i = psu.current
                        total = total + i
                        self.write('INFO: suspend current = %7.3f mA @ %5.1f V\n' %
                                   (1000 * i, psu.voltage))
                        time.sleep(0.1)
                    self.write('INFO: average suspend current = %7.3f mA @ %5.1f V\n' %
                               (1000 * total / samples, psu.voltage))
                    if abs(i) > MAXIMUM_SUSPEND_CURRENT:
                        s.write('N')
                    else:
                        s.write('Y')

                elif '*VERSION*' == line:
                    self.write('INFO: check "%s" against "%s"\n' % (current_version, versionNumber))
                    if current_version == versionNumber:
                        s.write('Y')
                    else:
                        s.write('N')
                        raise StopTestException('Version Number mismatch')

                else:
                    i = psu.current
                    self.write('INFO: current = %7.3f mA @ %5.1f V\n' % (1000 * i, psu.voltage))
                    if abs(i) > MAXIMUM_ON_CURRENT:
                        raise StopTestException('Device overcurrent: %7.3f mA' % (1000 * i))

                if '*END-TEST*' == line:
                    run = False

            t = time.time() - t

            self.write('\n*** End of Test [%7.3f seconds] ***\n' % (t))

        except StopTestException, e:
            # no save on manual abort
            auto = False
            self.write('\n*** Test stop exception ***\n')
            self.write('FAIL: %s\n' % str(e))
        except TestException, e:
            self.write('\n*** Test terminated ***\n')
            self.write('FAIL: %s\n' % str(e))
        except Exception, e:
            self.write('\n*** Test aborted ***\n')
            self.write('FAIL: Python Exception: %s\n' % str(e))
            self.write('TRACE:\n')
            self.write(traceback.format_exc())
            self.write('END_TRACE:\n')
        finally:
            if None != s:
                del s
            if None != program:
                del program
            if None != psu:
                psu.powerOff()
                del psu
            self.write('\n*** End of Test ***\n')
            self.testStop = False
            self.testRunning = False
            if auto:
                self.status.set_text('Saving log file...')
                message = ' (Data Saved)'
                try:
                    gtk.gdk.threads_enter()
                    self.save_file(True)
                    self.serialNumber.set_text('')
                    self.serialNumber.grab_focus()
                except Exception, e:
                    message = ' (ERROR: Save Data FAILED: %s)' % str(e)
                finally:
                    gtk.gdk.threads_leave()

            else:
                message = ''
            if self.testFailed:
                self.write('\nFAIL: one or more tests FAILED\n', 'fail-text')
                self.status.set_text('Stopped [FAILURE]' + message)
            else:
                self.write('\nPASS: all tests completed\n', 'pass-text')
                self.status.set_text('Stopped [SUCCESS]' + message)

    def write(self, message, style = None):
        gtk.gdk.threads_enter()
        if 'FAIL:' == message[0:5]:
            style = 'fail-text'
            self.testFailed = True
        elif 'PASS:' == message[0:5]:
            style = 'pass-text'
        if style:
            self.buffer.insert_with_tags(self.buffer.get_end_iter(), message,
                                  self.buffer.get_tag_table().lookup(style))
        else:
            self.buffer.insert(self.buffer.get_end_iter(), message)
        e = self.buffer.create_mark('*End*', self.buffer.get_end_iter())
        self.view.scroll_to_mark(e, 0.0, True, 0.0, 0.0)
        self.buffer.delete_mark(e)
        gtk.gdk.threads_leave()

    def menuitem_response(self, event):
        if self.testRunning:
            return
        if 'file.save' == event:
            self.save_file()

    def save_file(self, autoSave = False):
        global SaveFilesFolder, FileExt

        if self.testFailed:
            tag = '.FAIL'
        else:
            tag = '.OK'
        sn = self.serialNumber.get_text().strip()
        if '' == sn:
            sn = 'NO-SERIAL-NUMBER'

        t = datetime.datetime.now()
        theDate = t.strftime('%Y%m%d')
        theTime = t.strftime('%H%M%S')
        fileName = sn + '-' + theDate + '-' + theTime + tag + FileExt

        if autoSave:
            datedFolder = os.path.join(SaveFilesFolder, theDate)
            if not os.path.isdir(datedFolder):
                os.mkdir(datedFolder)
            fileName = os.path.join(datedFolder, fileName)
            response = gtk.RESPONSE_YES
        else:
            chooser = gtk.FileChooserDialog(title = 'Save As...', action = gtk.FILE_CHOOSER_ACTION_SAVE,
                                            buttons = (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                                       gtk.STOCK_SAVE_AS, gtk.RESPONSE_OK))
            chooser.set_default_response(gtk.RESPONSE_OK)
            chooser.set_select_multiple(select_multiple = False)
            chooser.set_current_folder(SaveFilesFolder)
            chooser.set_current_name(fileName)

            response = chooser.run()
            if gtk.RESPONSE_OK == response:
                fileName = chooser.get_filename()
                if os.path.exists(fileName):
                    dialog = gtk.Dialog(title = 'Overwrite file:' + fileName, parent = None,
                                        flags = gtk.DIALOG_MODAL,
                                        buttons = (gtk.STOCK_NO, gtk.RESPONSE_NO,
                                                   gtk.STOCK_YES, gtk.RESPONSE_YES))
                    response = dialog.run()
                    dialog.destroy()
                else:
                    response = gtk.RESPONSE_YES
            chooser.destroy()


        if gtk.RESPONSE_YES == response:
            with open(fileName, 'w') as f:
                start = self.buffer.get_start_iter()
                end = self.buffer.get_end_iter()
                text = self.buffer.get_text(start, end, include_hidden_chars = True)
                f.write('\r\n'.join(text.split('\n')))

    def __init__(self):
        self.fileName = ''
        self.testRunning = False
        self.testStop = False
        self.testFailed = False

        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

        self.window.set_title('Production Test Stage 2 - Full Set')

        self.window.connect('delete_event', self.delete_event)
        self.window.connect('destroy', self.destroy)

        self.window.set_border_width(10)
        self.window.set_geometry_hints(None, min_width=800, min_height=600)

        vbox1 = gtk.VBox(homogeneous = False, spacing = 5)
        vbox1.show()

        file_menu = gtk.Menu()    # Don't need to show menus

        # Create the menu items
        save_item = gtk.MenuItem('Save')
        exit_item = gtk.MenuItem('Exit')

        # Add them to the menu
        file_menu.append(save_item)
        file_menu.append(exit_item)

        # Attach the callback functions to the activate signal
        save_item.connect_object('activate', self.menuitem_response, 'file.save')

        # We can attach the Exit menu item to our exit function
        exit_item.connect_object ('activate', self.destroy, 'file.exit')

        # We do need to show menu items
        save_item.show()
        exit_item.show()

        menu_bar = gtk.MenuBar()
        #self.window.add(menu_bar)
        menu_bar.show()

        vbox1.pack_start(menu_bar, expand = False, fill = False, padding = 0)

        file_item = gtk.MenuItem('File')
        file_item.show()

        file_item.set_submenu(file_menu)

        menu_bar.append(file_item)

        #menu_item.set_right_justified(right_justified)


        scrolled = gtk.ScrolledWindow()
        scrolled.set_policy(hscrollbar_policy = gtk.POLICY_AUTOMATIC, vscrollbar_policy = gtk.POLICY_AUTOMATIC)
        self.view = gtk.TextView()
        self.view.set_editable(False)
        self.buffer = self.view.get_buffer()
        tag_fail = self.buffer.create_tag("fail-text",
                                         size_points = 24.0,
                                         foreground = "red")
        tag_pass = self.buffer.create_tag("pass-text",
                                          size_points = 24.0,
                                          foreground = "green")
        self.buffer.create_tag('fixed-text',
                               size_points = 10.0,
                               family = 'Monospace',
                               underline = pango.UNDERLINE_NONE,
                               foreground = 'black')
        self.buffer.create_tag('fixed-title',
                               size_points = 10.0,
                               family = 'Monospace',
                               underline = pango.UNDERLINE_DOUBLE,
                               foreground = 'blue')

        scrolled.add(self.view)
        scrolled.show()
        self.view.show()
        vbox1.pack_start(scrolled, expand = True, fill = True, padding = 0)

        button_box = gtk.HButtonBox()
        button_box.set_layout(layout_style = gtk.BUTTONBOX_SPREAD)
        button_box.show()

        button1 = gtk.Button('Run')
        button1.connect('clicked', self.startCallback, None)
        button1.show()

        button2 = gtk.Button('Stop')
        button2.connect('clicked', self.stopCallback, None)
        button2.show()

        button3 = gtk.Button('Clear Screen')
        button3.connect('clicked', self.clearScreen, None)
        button3.show()

        button_box.add(button1)
        button_box.add(button2)
        button_box.add(button3)

        vbox1.pack_end(button_box, expand = False, fill = False, padding = 0)

        table1 = gtk.Table(rows = 2, columns = 3, homogeneous = False)
        table1.set_col_spacings(3)
        table1.set_row_spacings(2)

        self.autoSave = gtk.CheckButton("Auto-save")
        self.autoSave.set_active(True)
        self.autoSave.show()
        table1.attach(self.autoSave, 0, 1, 0, 1,
                      xoptions = gtk.EXPAND | gtk.FILL, yoptions = 0,
                      xpadding = 0, ypadding = 1)

        label = gtk.Label('Serial Number:')
        label.set_alignment(xalign = 1.0, yalign = 0.5)
        label.show()
        table1.attach(label, 1, 2, 0, 1,
                      xoptions = gtk.FILL, yoptions = 0,
                      xpadding = 0, ypadding = 1)

        self.serialNumber = gtk.Entry()
        self.serialNumber.set_max_length(32)
        self.serialNumber.set_width_chars(32)
        self.serialNumber.show()
        table1.attach(self.serialNumber, 2, 3, 0, 1,
                      xoptions = gtk.EXPAND | gtk.FILL, yoptions = 0,
                      xpadding = 0, ypadding = 1)

        label = gtk.Label('SD Card Version:')
        label.set_alignment(xalign = 1.0, yalign = 0.5)
        label.set_justify(gtk.JUSTIFY_RIGHT)
        label.show()
        table1.attach(label, 1, 2, 1, 2,
                      xoptions = gtk.FILL, yoptions = 0,
                      xpadding = 0, ypadding = 1)

        self.versionNumber = gtk.Entry()
        self.versionNumber.set_max_length(32)
        self.versionNumber.set_width_chars(32)
        self.versionNumber.show()
        table1.attach(self.versionNumber, 2, 3, 1, 2,
                      xoptions = gtk.EXPAND | gtk.FILL, yoptions = 0,
                      xpadding = 0, ypadding = 1)

        table1.show()
        vbox1.pack_start(table1, expand = False, fill = False, padding = 0)


        self.status = gtk.Label('')
        self.status.set_alignment(0, 0)
        vbox1.pack_start(self.status, expand = False, fill = False, padding = 0)
        self.status.show()

        self.window.add(vbox1)

        self.window.show()

        self.serialNumber.grab_focus()
        self.serialNumber.set_activates_default(True)
        button1.set_flags(gtk.CAN_DEFAULT)
        button1.grab_default()

    def main(self):
        gtk.gdk.threads_init()
        gtk.main()

if __name__ == '__main__':
    s = Sample()
    s.main()
