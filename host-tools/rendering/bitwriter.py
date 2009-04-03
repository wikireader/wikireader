"""
 Simple tool to write a bitstream

 Copyright (C) 2008, 2009 Holger Hans Peter Freyther <zecke@openmoko.org>

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

class BitWriter:
    def __init__(self):
        self.bits = []

    def write_bit(self, bit):
        assert bit == (bit&0x1)
        if bit:
            self.bits.append(1)
        else:
            self.bits.append(0)

    def write_bits(self, bit_string):
        for bit in bit_string:
            if bit == '1':
                self.write_bit(1)
            elif bit == '0':
                self.write_bit(0)
            else:
                print "Wrong", bit_string
                assert False

    def _consume(self):
        "Consume bits to a packed 8byte"
        import struct

        consumed = []

        for index in range(0, len(self.bits), 8):
            operate = self.bits[index:index+8]
            data = (operate[0]<<7) | (operate[1]<<6) | (operate[2]<<5) | (operate[3]<<4) | (operate[4]<<3) | (operate[5]<<2) | (operate[6]<<1) | (operate[7]<<0)
            byte = struct.pack("<B", data)
            consumed.append(byte)

        return consumed

    def finish(self):
        "Consume everything that is still left"
        remainder = len(self.bits)%8
        for i in range(0,8-remainder):
            self.write_bit(0)
        assert len(self.bits)%8 == 0
        return self._consume()
