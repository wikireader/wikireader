/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

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
*/

#include "crc.h"

unsigned char crc_byte(unsigned int crc, unsigned char byte, unsigned char nbits)
{
	signed char bit;

	for (bit = nbits - 1; bit >= 0; bit--) {
		crc <<= 1;
		crc |= (byte >> bit) & 1;
		if ((crc & 0x80) ^ (byte & 0x80))
			crc ^= 0x09;
	
	}
	
	return crc & 0xff;
}

unsigned char crc7(const unsigned char *buffer, int len)
{
	unsigned char crc = 0;

        while (len--)
		crc = crc_byte(crc, *buffer++, 8);

	return crc_byte(crc, 0, 7);
}

