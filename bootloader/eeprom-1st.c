#include "regs.h"
#include "wikireader.h"

#define DEST 0x10000000
//#define DEST 0x200

static void spi_transmit(unsigned char b);

/* MRS command address for burst length=1, CAS latency = 2 */
#define MRSREG		(*(volatile unsigned char *) 0x10000442)
#define RAMDUMMY	(*(volatile unsigned char *) 0x10000000)


int main(void) {
	unsigned int len = EEPROM_PAYLOAD_SIZE;
	unsigned char *dest = (unsigned char *) DEST;
	int i;

	INIT_PINS();
	SDCARD_CS_HI();
	EEPROM_CS_HI();



	/*****************************************************/

	/* disable write protection of clock registers */
	REG_CMU_PROTECT = 0x96;

	/* switch on SDRAM clk */
	REG_CMU_GATEDCLK0 = 0x78;

	/* P20-P27 functions */
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;
	REG_P5_03_CFP = 0x80;
	
	/* enable write protection of clock registers */
	REG_CMU_PROTECT = 0x00;
	
	/* re-enable SDRAMC application core */
	REG_SDRAMC_APP = 0x8000000b;

	/* set up SDRAM controller */
	/* 4M x 16 bits x 1, 15 Trs/Trfc/Txsr cycles, 3 Tras, 2 Trp */
	REG_SDRAMC_CTL = 0x37e1;

	/* enable RAM self-refresh */
	//REG_SDRAMC_REF |= (1 << 25);

	REG_SDRAMC_INI = 0x10;	/* exit setup mode */

	/* SDRAM command sequence: PALL - REF - REF - MRS (for MT48LC16M16A2) */
	REG_SDRAMC_INI = 0x12;	/* INIPRE */
	RAMDUMMY = 0x0;		/* dummy write */
	
	REG_SDRAMC_INI = 0x12;	/* INIPRE */
	RAMDUMMY = 0x0;		/* dummy write */

	for (i = 0; i < 2; i++) {
		REG_SDRAMC_INI = 0x11;	/* INIREF */
		RAMDUMMY = 0x0;		/* dummy write */
	}

	REG_SDRAMC_INI = 0x14;	/* INIMRS */
	MRSREG = 0x0;		/* dummy write */

	REG_SDRAMC_INI = 0x10;	/* exit setup mode */

	/***************************************************/





	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x03 | (7 << 10);

	/* read the EEPROM payload, starting from page 2 (512 bytes) */
	EEPROM_CS_LO();

	spi_transmit(0x03);
	spi_transmit(0x00);
	spi_transmit(0x03);
	spi_transmit(0x00);

	while (len--) {
		spi_transmit(0x00);
		*dest = REG_SPI_RXD;
		dest++;
	}

	EEPROM_CS_HI();

	((void (*) (void)) DEST) ();

	return 0;
}

static void spi_transmit(unsigned char b)
{
	REG_SPI_TXD = b;
	do {} while (REG_SPI_STAT & (1 << 6));
}


