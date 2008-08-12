#include "regs.h"
#include "wikireader.h"
#include "spi.h"

#define DEST 0x200

#ifdef LOAD_FROM_EEPROM
static void spi_transmit(unsigned char b);
#endif

//#define LOAD_FROM_EEPROM 1
#define LOAD_FROM_RS232 1

int main(void) {
#ifdef LOAD_FROM_RS232
	unsigned int len = 8192;
#endif
	unsigned char *dest = (unsigned char *) DEST;

	init_pins();
	SDCARD_CS_HI();
	EEPROM_CS_HI();

#ifdef LOAD_FROM_EEPROM
	/* read bytes from EEPROM and copy them to RAM */

	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x03 | (7 << 10);

	/* read the EEPROM payload, starting from page 2 (512 bytes) */
	read_eeprom(0x0300, dest, EEPROM_PAYLOAD_SIZE);
#endif

#ifdef LOAD_FROM_RS232
	init_rs232();

	/* read bytes from serial port and copy them to RAM */
	while (len--) {
		do {} while (!(REG_EFSIF0_STATUS & 0x1));
		*dest = REG_EFSIF0_RXD;
		dest++;
	}

	REG_EFSIF0_TXD = '!';
	do {} while (REG_EFSIF0_STATUS & (1 << 5));

#endif

	/* Gimme Van Halen */
	((void (*) (void)) DEST) ();

	return 0;
}

