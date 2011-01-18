#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Buffer for removing redundant escape sequences to reduce
#              extraneous whitespace
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import io
import struct


class EscapeBuffer(object):
    """buffer and reorder escape sequences"""

    def __init__(self, *args, **kw):
        """create new instance"""
        #super(EscapeBuffer, self).__init__(*args, **kw)

        try:
            self.callback = kw['callback']
        except KeyError:
            self.callback = lambda x: None

        try:
            self.max_length = kw['max_length']
        except KeyError:
            self.max_length = 'unlimited'

        self.limiting = False
        self.byte_count = 0
        self.output = io.BytesIO('')
        self.head = None


    def __del__(self):
        """delete instance"""
        self.output.close()


    def fetch(self):
        """return the buffer as a string and clear the buffer"""
        if None != self.head:
            self.output.write(self.head)
        self.output.flush()
        text = self.output.getvalue()
        self.output.seek(0)
        self.output.truncate(0)
        self.head = None
        self.limiting = False
        self.byte_count = 0
        return text


    def internal_write(self, data):
        """write data to buffer internal stream"""

        if None == data or self.limiting:
            return

        size = len(data)
        if 0 == size:
            return

        if 'unlimited' != self.max_length and self.byte_count + size > self.max_length:
            # truncate to fit in low capacity cards
            limited_data = '\x01\n\x07\x01\n\x04\x82\x08\x64* * *' + \
                '\x01\n\x07\x04\x82Article shortened to fit\x03into the compact database.\x02'
            self.output.write(limited_data)
            self.byte_count += len(limited_data)
            self.limiting = True
            return

        self.byte_count += size
        self.output.write(data)


    def write(self, data):
        """write data to buffer"""

        if None == data:
            return

        if '' == data and None != self.head:
            self.internal_write(self.head)
            self.head = None
            return

        if None == self.head:
            self.head = data

        elif chr(9) == data[0] and chr(1) == self.head[0]:
            self.internal_write(data)

        elif chr(10) == data[0] and chr(1) == self.head[0]:
            self.internal_write(data)

        elif chr(1) == data[0] and chr(1) == self.head[0]:
            inc = struct.unpack('<B', self.head[1])[0]
            self.callback(- inc)
            self.head = data

        else:
            self.internal_write(self.head)
            self.head = data


def main():
    global diff_values, diff_index

    diff_values = [0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 4]
    diff_index = 0

    def cb(diff):
        global diff_values, diff_index
        if diff != diff_values[diff_index]:
            print(u'FAIL: diff = {0:d} expected: {1:d}'.format(diff, diff_values[diff_index]))
        diff_index += 1

    output =  EscapeBuffer(callback=cb)

    output.write('Title')

    num_pixels = 10
    output.write(struct.pack('<BB', 1, num_pixels))
    output.write(struct.pack('<Bb', 10, 4))
    output.write(struct.pack('<BB', 9, 4))
    output.write(struct.pack('<BB', 1, num_pixels))
    output.write(struct.pack('<Bb', 10, 4))
    output.write(struct.pack('<BB', 9, 4))
    output.write(struct.pack('<BB', 1, num_pixels))
    output.write(struct.pack('<Bb', 10, 4))
    output.write(struct.pack('<BB', 9, 4))
    output.write(struct.pack('<BB', 1, num_pixels))
    output.write(struct.pack('<Bb', 10, 4))
    output.write(struct.pack('<BB', 9, 4))
    output.write('abcdefgh')
    output.write(struct.pack('<BB', 1, num_pixels))
    output.write(struct.pack('<Bb', 10, -4))
    output.write(struct.pack('<BB', 1, num_pixels))
    output.write(struct.pack('<Bb', 10, -4))
    output.write(struct.pack('<BB', 1, num_pixels))
    output.write(struct.pack('<Bb', 10, -4))
    output.write(struct.pack('<BB', 1, num_pixels))
    output.write(struct.pack('<Bb', 10, -4))

    output.write(struct.pack('<BB', 1, 16))
    output.write('Para 1')
    output.write(struct.pack('<BB', 1, 16))
    output.write('Para 2')
    output.write(struct.pack('<BB', 1, 16))
    output.write(struct.pack('<BB', 1, 16))
    output.write(struct.pack('<BB', 1, 16))
    output.write(struct.pack('<BB', 1, 16))
    output.write(struct.pack('<BB', 1, 16))
    output.write(struct.pack('<BB', 1, 20))

    data = output.fetch()
    expected = ('Title'
                '\x0a\x04\x09\x04\x0a\x04\x09\x04\x0a\x04\x09\x04\x0a\x04\x09\x04\x01\x0a'
                'abcdefgh'
                '\x0a\xfc\x0a\xfc\x0a\xfc\x0a\xfc\x01\x10'
                'Para 1'
                '\x01\x10'
                'Para 2'
                '\x01\x14')

    if data == expected:
        print('PASS: comparison match')
    else:
        print('FAIL:')
        print(u'data = {0!r:s}'.format(data))
        print(u'data = {0!r:s}'.format(struct.unpack('<' + str(len(data)) + 'B', data)))

# run the program
if __name__ == "__main__":
    main()
