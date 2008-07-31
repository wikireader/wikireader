#include "interfaces/s1c33e07_spi.h"
#include "interfaces/sd.h"
#include <regs.h>
#include <wikireader.h>

esint8 if_initInterface(hwInterface* file, eint8* opts)
{
	if_spiInit(file);
	if(sd_Init(file) < 0)	{
		DBG((TXT("Card failed to init, breaking up...\n")));
		return -1;
	}
	if(sd_State(file) < 0){
		DBG((TXT("Card didn't return the ready state, breaking up...\n")));
		return -2;
	}
	file->sectorCount = 4; /* FIXME ASAP!! */
	DBG((TXT("Init done...\n")));
	return 0;
}

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	return sd_readSector(file, address, buf, 512);
}

esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
	return sd_writeSector(file, address, buf);
}

esint8 if_setPos(hwInterface* file,euint32 address)
{
	return 0;
}

void if_spiInit(hwInterface *iface)
{
	/* empty - this done in another early stage */
}

euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
	euint8 incoming=0;

	SDCARD_CS_LO();

	REG_SPI_TXD = outgoing;
	/* wait for transfer to finish */
	do {} while (REG_SPI_STAT & (1 << 6));

	/* read back */
	incoming = REG_SPI_RXD;
	
	SDCARD_CS_HI();
	return incoming;
}

