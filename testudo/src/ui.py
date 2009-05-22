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

import sequencer

class Sample:

    def delete_event(self, widget, event, data = None):
        #print 'delete event occurred'
        return False

    def destroy(self, widget, data=None):
        #print 'destroy signal occurred'
        gtk.main_quit()

    def run(self, widget, data=None):
        end = self.textbuffer.get_end_iter()
        self.textbuffer.insert(end, 'starting\n')
        sequencer.runOneTest(self, self.fileName, 0)

    def write(self, message):
        end = self.textbuffer.get_end_iter()
        self.textbuffer.insert(end, message)

    def press2(self, widget, data=None):
        print 'Click2'
        end = self.textbuffer.get_end_iter()
        self.textbuffer.insert(end, 'just a few sample words and the file is: %s\n' % self.fileName)

    def menuitem_response(self, event):
        print 'menu =', event
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
                start = self.textbuffer.get_start_iter()
                end = self.textbuffer.get_end_iter()
                f.write(self.textbuffer.get_text(start, end, include_hidden_chars = True))
        chooser.destroy()

    def __init__(self):
        self.fileName = ''

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
        textview = gtk.TextView()
        textview.set_editable(False)
        self.textbuffer = textview.get_buffer()
        scrolled.add(textview)
        scrolled.show()
        textview.show()
        vbox1.pack_start(scrolled, expand = True, fill = True, padding = 0)

        button_box = gtk.HButtonBox()
        button_box.set_layout(layout_style = gtk.BUTTONBOX_SPREAD)
        button_box.show()

        button1 = gtk.Button('Run')
        button1.connect('clicked', self.run, None)
        button1.show()

        button2 = gtk.Button('Push Me')
        button2.connect('clicked', self.press2, None)
        button2.show()

        button_box.add(button1)
        button_box.add(button2)

        vbox1.pack_end(button_box, expand = False, fill = False, padding = 0)

        self.status = gtk.Label('No test Loaded')
        self.status.set_alignment(0, 0)
        vbox1.pack_start(self.status, expand = False, fill = False, padding = 0)
        self.status.show()

        self.window.add(vbox1)

        self.window.show()

    def main(self):
        gtk.main()

if __name__ == '__main__':
    s = Sample()
    s.main()
