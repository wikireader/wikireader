#!/usr/bin/env python
"""
 Extract image into a bytearray...

 Copyright (C) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import gd, sys

def write_image(image, file):
    file.write("""/* This file is auto generated... */
static struct guilib_image image_data = {
\t.width = %d,
\t.height = %d,
\t.data = %s,
};
""" % (image['width'], image['height'], "".join(image['formated_data'])))

def str_to_byte(str):
    """
    Convert 11111111 to a byte
    """
    byte = 0
    for i in range(0, len(str)):
        if str[i] == '1':
            val = 1
        else:
            val = 0

        byte = byte | (val << 7-i)
    return "0x%.2x" % byte

def load_image(filename):
    img_data = {}
    img_data['data'] = []
    image = open(filename)
    line_no = 0
    data = False
    for line in image:
        if line_no == 0:
            if line[:-1] != "P1":
                raise Exception("Wrong Image")
        elif not data and line[0] == '#':
            pass
        elif not data:
            data = True
            split = line[:-1].split(" ")
            img_data['width'] = int(split[0])
            img_data['height'] = int(split[1])
        elif data:
            img_data['data'].append(line[:-1])
        else:
            raise Exception("Something wrong with the state")

        line_no = line_no + 1

    # build formatted_type..
    img_data['data'] = "".join(img_data['data'])
    img_data['formated_data'] = []
    img_data['formated_data'].append('{')
    bytes = 0
    for i in range(0, len(img_data['data']), 8):
        if bytes % 12 == 0:
            img_data['formated_data'].append('\n\t')
        img_data['formated_data'].append(str_to_byte(img_data['data'][i:i+8]))
        img_data['formated_data'].append(', ')
        bytes = bytes + 1

    img_data['formated_data'].append('}')
    return img_data

write_image(load_image(sys.argv[1]), sys.stdout)

