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
import pango

import sys
import datetime
import time
import os.path

# paths to files
SaveFilesFolder = '/log/files/stage3'
FileExt = '.3.text'

if not os.path.isdir(SaveFilesFolder):
    print 'error: %s is not a directory' % SaveFilesFolder
    print '       sudo mkdir -p %s' % SaveFilesFolder
    exit(1)


class Sample:

    def delete_event(self, widget, event, data = None):
        #print 'delete event occurred'
        gtk.main_quit()

    def destroy(self, widget, data=None):
        #print 'destroy signal occurred'
        gtk.main_quit()

    def clearCounter(self, widget, data = None):
        """Reset counters"""
        self.count = 0
        self.sequence = 0
        self.title = False
        self.clearSerialNumber()
        self.counter.set_text('%10d' % self.count)

    def clearScreen(self, widget, data = None):
        """Clear the screen"""
        start = self.buffer.get_start_iter()
        end = self.buffer.get_end_iter()
        self.buffer.delete(start, end)
        self.title = False
        self.clearSerialNumber()

    def clearSerialNumber(self):
        self.serialNumber.set_text('')
        self.serialNumber.grab_focus()


    def enterCallback(self, widget, data = None):
        """save serial number"""
        sn = self.serialNumber.get_text()
        self.clearSerialNumber()

        if '' ==  sn:
            return

        t = datetime.datetime.now()
        theDate = t.strftime('%Y%m%d')
        theTime = t.strftime('%H%M%S')

        fileName = sn + FileExt
        fileData = sn + ' ' + t.strftime('%Y-%m-%d') + ' ' + t.strftime('%H:%M:%S')

        datedFolder = os.path.join(SaveFilesFolder, theDate)
        if not os.path.isdir(datedFolder):
            os.mkdir(datedFolder)
        fileName = os.path.join(datedFolder, fileName)

        if os.path.exists(fileName):
            repeat = '*Duplicate*'
            c = '%10s' % '-'
        else:
            self.count += 1
            c = '%10d' % self.count
            self.counter.set_text('%10d' % self.count)
            repeat = ''

        with open(fileName, 'a') as f:
            f.write(fileData + '\n')

        self.sequence += 1
        if not self.title:
            self.title = True
            self.write('\n%10s %10s   %-25s %s\n\n' % ('Count', 'Sequence', 'Serial Number', 'Duplicates'), 'fixed-title')

        self.write('%10s %10d   %-25s %s\n' % (c, self.sequence, sn, repeat), 'fixed-text')

    def write(self, message, style = None):
        #gtk.gdk.threads_enter()
        if 'FAIL:' == message[0:5]:
            self.testFailed = True
        if style:
            self.buffer.insert_with_tags(self.buffer.get_end_iter(), message,
                                  self.buffer.get_tag_table().lookup(style))
        else:
            self.buffer.insert(self.buffer.get_end_iter(), message)
        e = self.buffer.create_mark('*End*', self.buffer.get_end_iter())
        self.view.scroll_to_mark(e, 0.0, True, 0.0, 0.0)
        self.buffer.delete_mark(e)
        #gtk.gdk.threads_leave()

    def menuitem_response(self, event):
        if 'file.save' == event:
            self.save_file()

    def save_file(self):
        global SaveFilesFolder, FileExt

        t = datetime.datetime.now()
        theDate = t.strftime('%Y%m%d')
        theTime = t.strftime('%H%M%S')
        fileName = 'log' + '-' + theDate + '-' + theTime + FileExt

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
        self.sequence = 0
        self.count = 0
        self.title = False

        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

        self.window.set_title('Production Test Stage 3 - Final Inspection')

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
        self.view.set_justification(gtk.JUSTIFY_LEFT)

        self.buffer = self.view.get_buffer()
        tag_fail = self.buffer.create_tag('fail-text',
                                         size_points = 24.0,
                                         foreground = 'red')
        tag_pass = self.buffer.create_tag('pass-text',
                                          size_points = 24.0,
                                          foreground = 'green')
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

        button1 = gtk.Button('Enter')
        button1.connect('clicked', self.enterCallback, None)
        button1.show()

        button2 = gtk.Button('Clear Counter')
        button2.connect('clicked', self.clearCounter, None)
        button2.show()

        button3 = gtk.Button('Clear Screen')
        button3.connect('clicked', self.clearScreen, None)
        button3.show()

        button_box.add(button1)
        button_box.add(button2)
        button_box.add(button3)

        vbox1.pack_end(button_box, expand = False, fill = False, padding = 0)

        hbox = gtk.HBox(homogeneous = False, spacing = 5)

        label = gtk.Label('Count')
        hbox.pack_start(label, expand = True, fill = True, padding = 0)

        self.counter = gtk.Label('0')
        hbox.pack_start(self.counter, expand = True, fill = True, padding = 0)

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
