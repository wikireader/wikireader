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
MAXIMUM_ON_CURRENT = 0.15

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

        print'bps =', self.s.baudrate

    def __del__(self):
        self.s.close()

    def flush(self):
        print 'flush:', self.s.inWaiting()
        if 0 < self.s.inWaiting():
            self.s.read(self.s.inWaiting())

    def write(self, text):
        self.s.write(text)

    def readChar(self):
        return self.s.read(1)


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
            psu = None

            s = SerialPort()
            if '' == self.serialNumber.get_text():
                raise StopTestException('Serial number invalid')
            serialNumber = self.serialNumber.get_text()

            psu = Keithley.PSU2303()
            psu.setCurrent(SUPPLY_CURRENT_LIMIT)
            psu.setVoltage(SUPPLY_STANDARD_VOLTAGE)
            psu.powerOn()

            self.write('\n*** Start of Test for device SN: %s ***\n\n' %
                       (serialNumber))
            t = time.time()

            self.write('*** Press device power on button to begin test ***\n\n')

            run = True
            while run:

                line = ''
                while not self.testStop:
                    c = s.readChar()
                    if '\n' == c:
                        break
                    if '' == c or ' ' > c:
                        continue
                    line = line + c

                if self.testStop:
                    raise StopTestException('Stop button pressed')

                self.write(line)
                self.write('\n')
                if '*SUSPEND*' == line:
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
                except Exception, e:
                    message = ' (ERROR: Save Data FAILED: %s)' % str(e)
                finally:
                    gtk.gdk.threads_leave()

            else:
                message = ''
            if self.testFailed:
                self.status.set_text('Stopped [FAILURE]' + message)
            else:
                self.status.set_text('Stopped [SUCCESS]' + message)

    def write(self, message):
        gtk.gdk.threads_enter()
        if 'FAIL:' == message[0:5]:
            self.testFailed = True
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
        sn = self.serialNumber.get_text()
        if '' == sn:
            sn = 'NO-SERIAL-NUMBER'

        fileName = sn + '-' + datetime.datetime.now().strftime('%Y%m%d-%H%M%S') + tag + FileExt

        if autoSave:
            fileName = os.path.join(SaveFilesFolder, fileName)
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
                    dialog = gtk.Dialog(title = 'Overwrite file:' + file, parent = None,
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
                f.write(self.buffer.get_text(start, end, include_hidden_chars = True))

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

        hbox = gtk.HBox(homogeneous = False, spacing = 5)

        self.autoSave = gtk.CheckButton("Auto-save")
        self.autoSave.set_active(True)
        hbox.pack_start(self.autoSave, expand = False, fill = True, padding = 0)

        label = gtk.Label('Serial Number')
        hbox.pack_start(label, expand = True, fill = True, padding = 0)

        self.serialNumber = gtk.Entry()
        self.serialNumber.set_max_length(32)
        hbox.pack_start(self.serialNumber, expand = True, fill = True, padding = 0)

        hbox.show_all()

        vbox1.pack_start(hbox, expand = False, fill = False, padding = 0)

        self.status = gtk.Label('')
        self.status.set_alignment(0, 0)
        vbox1.pack_start(self.status, expand = False, fill = False, padding = 0)
        self.status.show()

        self.window.add(vbox1)

        self.window.show()

    def main(self):
        gtk.gdk.threads_init()
        gtk.main()

if __name__ == '__main__':
    s = Sample()
    s.main()
