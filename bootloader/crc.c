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

