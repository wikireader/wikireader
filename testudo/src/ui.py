#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Simple UI for running test
# AUTHOR: Christopher Hall <hsw@openmoko.com>

from __future__ import with_statement # This isn't required in Python 2.6

import pygtk
pygtk.require('2.0')
import gtk

import os.path
import threading

import sequencer


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
        if '' == self.fileName:
            self.status.set_text('load a file first')
            return
        if not self.testRunning:
            self.testStop = False
            self.status.set_text('Running')
            self.testRunning = True
            count = self.repeat.get_value_as_int()
            self.runTest(count)

    def stopCallback(self, widget, data = None):
        """Request test stop"""
        if self.testRunning:
            self.testStop = True
            self.status.set_text('.....Stopping at end of this test cycle.....')

    @threaded
    def runTest(self, count):
        """main test routine2"""

        try:
            for cycle in range(1, count + 1):
                self.write('\n*** Start of Test %d of %d ***\n\n' % (cycle, count))

                t = time.time()
                sequencer.runOneTest(self, self.fileName, 0)
                t = time.time() - t

                self.write('\n*** End of Test %d of %d [%7.3f seconds]***\n' % (cycle, count, t))

                if self.testStop:
                    raise StopTestException('Stop button pressed')

        except StopTestException, e:
            self.write('\n*** Test stop exception ***\n')
            self.write('FAIL: %s\n' % str(e))
        except TestException, e:
            self.write('\n*** Test terminated ***\n')
            self.write('FAIL: %s\n' % str(e))
        except Exception, e:
            self.write('\n*** Test aborted ***\n')
            self.write('FAIL: Python Exception: %s\n' % str(e))
        finally:
            self.write('\n*** End of Test ***\n')
            self.testStop = False
            self.testRunning = False
            self.status.set_text('Stopped')

    def write(self, message):
        gtk.gdk.threads_enter()
        self.buffer.insert(self.buffer.get_end_iter(), message)
        e = self.buffer.create_mark('*End*', self.buffer.get_end_iter())
        self.view.scroll_to_mark(e, 0.0, True, 0.0, 0.0)
        self.buffer.delete_mark(e)
        gtk.gdk.threads_leave()

    def menuitem_response(self, event):
        if self.testRunning:
            return
        if 'file.open' == event:
            self.open_file()
        elif 'file.save' == event:
            self.save_file()

    def open_file(self):
        chooser = gtk.FileChooserDialog(title = 'Open...', action = gtk.FILE_CHOOSER_ACTION_OPEN,
                                        buttons = (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                                   gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        chooser.set_default_response(gtk.RESPONSE_OK)
        chooser.set_select_multiple(select_multiple = False)
        #chooser.set_current_folder('/')
        #chooser.set_current_name('')

        filter = gtk.FileFilter()
        filter.set_name("Python files")
        filter.add_pattern("*.py")
        chooser.add_filter(filter)

        filter = gtk.FileFilter()
        filter.set_name("All files")
        filter.add_pattern("*")
        chooser.add_filter(filter)

        response = chooser.run()
        if response == gtk.RESPONSE_OK:
            self.fileName = chooser.get_filename()
            self.status.set_text('Current test: %s' % self.fileName)
            start = self.buffer.get_start_iter()
            end = self.buffer.get_end_iter()
            self.buffer.delete(start, end)
            end = self.buffer.get_end_iter()
            self.buffer.insert(end, 'Loaded: %s\n\n' % self.fileName)
        else:
            self.fileName = ''
            self.status.set_text('No test loaded')
        chooser.destroy()

    def save_file(self):
        chooser = gtk.FileChooserDialog(title = 'Save As...', action = gtk.FILE_CHOOSER_ACTION_SAVE,
                                        buttons = (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                                   gtk.STOCK_SAVE_AS, gtk.RESPONSE_OK))
        chooser.set_default_response(gtk.RESPONSE_OK)
        chooser.set_select_multiple(select_multiple = False)
        chooser.set_current_folder('/tmp')
        chooser.set_current_name('test-log.text')
        response = chooser.run()
        if gtk.RESPONSE_OK == response:
            file = chooser.get_filename()
            if os.path.exists(file):
                dialog = gtk.Dialog(title = 'Overwrite file:' + file, parent = None,
                                    flags = gtk.DIALOG_MODAL,
                                    buttons = (gtk.STOCK_NO, gtk.RESPONSE_NO,
                                               gtk.STOCK_YES, gtk.RESPONSE_YES))
                response = dialog.run()
                dialog.destroy()
            else:
                response = gtk.RESPONSE_YES
        if gtk.RESPONSE_YES == response:
            with open(chooser.get_filename(), 'w') as f:
                start = self.buffer.get_start_iter()
                end = self.buffer.get_end_iter()
                f.write(self.buffer.get_text(start, end, include_hidden_chars = True))
        chooser.destroy()

    def __init__(self):
        self.fileName = ''
        self.testRunning = False
        self.testStop = False

        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

        self.window.connect('delete_event', self.delete_event)
        self.window.connect('destroy', self.destroy)

        self.window.set_border_width(10)
        self.window.set_geometry_hints(None, min_width=800, min_height=600)

        vbox1 = gtk.VBox(homogeneous = False, spacing = 5)
        vbox1.show()

        file_menu = gtk.Menu()    # Don't need to show menus

        # Create the menu items
        open_item = gtk.MenuItem('Open')
        save_item = gtk.MenuItem('Save')
        exit_item = gtk.MenuItem('Exit')

        # Add them to the menu
        file_menu.append(open_item)
        file_menu.append(save_item)
        file_menu.append(exit_item)

        # Attach the callback functions to the activate signal
        open_item.connect_object('activate', self.menuitem_response, 'file.open')
        save_item.connect_object('activate', self.menuitem_response, 'file.save')

        # We can attach the Exit menu item to our exit function
        exit_item.connect_object ('activate', self.destroy, 'file.exit')

        # We do need to show menu items
        open_item.show()
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

        label = gtk.Label('Auto-Repeat')
        hbox.pack_start(label, expand = False, fill = True, padding = 0)

        adj = gtk.Adjustment(value = 1, lower = 1, upper = 1000,
                             step_incr = 1, page_incr = 10, page_size = 0)
        self.repeat = gtk.SpinButton(adjustment = adj, climb_rate = 0.0, digits = 0)
        self.repeat.set_wrap(False)
        self.repeat.set_numeric(True)

        hbox.pack_start(self.repeat, expand = False, fill = True, padding = 0)

        hbox.show_all()

        vbox1.pack_start(hbox, expand = False, fill = False, padding = 0)

        self.status = gtk.Label('No test Loaded')
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
