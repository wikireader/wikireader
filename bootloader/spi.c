#include "regs.h"
#include "spi.h"
#include "wikireader.h"

unsigned char spi_transmit(unsigned char out)
{
	REG_SPI_TXD = out;
	do {} while (REG_SPI_STAT & (1 << 6));
	return REG_SPI_RXD;
}

