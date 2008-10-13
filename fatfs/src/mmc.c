/*
 * MMC layer implementation taken from
 * 	http://www.acmesystems.it/download/mmc_drv.tar.bz2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 */ 
 

#include "ff.h"
#include "diskio.h"
#include "cache.h"

// mmc needs to be attached as follows
// SCK		23	O	OG1  
// MISO		24	I	IG1
// MOSI		25	O	OG4
// CS		26	O	OG3

// these pins are I/O
#define OG1 1<<1
#define IG1 1<<1
#define OG4 1<<4
#define OG3 1<<3

// SPI port defines
#define SCK		OG1
#define SI		OG4
#define SO		IG1
#define CS		OG3

#define MMC_CMD_0_GO_IDLE			0
#define MMC_CMD_1_SEND_OP_COND			1
#define MMC_CMD_9_SEND_CSD			9
#define MMC_CMD_10_SEND_CID			10
#define MMC_CMD_12_STOP				12
#define MMC_CMD_13_SEND_STATUS			13
#define MMC_CMD_16_BLOCKLEN			16
#define MMC_CMD_17_READ_SINGLE			17
#define MMC_CMD_18_READ_MULTIPLE		18
#define MMC_CMD_24_WRITE_SINGLE			24

// bit definitions for R1
#define MMC_R1_IN_IDLE 	0x01
#define MMC_R1B_BUSY_BYTE 0x00

// different defines for tokens etc ...
#define MMC_START_TOKEN_SINGLE  0xfe
#define MMC_START_TOKEN_MULTI   0xfc

// return values from a write operation
#define MMC_DATA_ACCEPT 		0x2
#define MMC_DATA_CRC	 		0x5
#define MMC_DATA_WRITE_ERROR	0x6


typedef struct _VOLUME_INFO{
  unsigned char   	size_MB;
  unsigned long int size;
  unsigned char   	sector_multiply;
  unsigned int   	sector_count;
  unsigned int 	sector_size;
  unsigned char	name[6];
} VOLUME_INFO;



// software delay
void msDelay(int ms)
{
	int i,a;
	int delayvar = 10;
  
	for (a = 0; a < ms; a++) {
		for (i = 0 ;i < 1500;i++) {
			delayvar *= 2;
			delayvar /= 2;
		}
	}
}

// write a byte to the spi bus
// data is send MSB first
unsigned char SPI_io(unsigned char byte)
{
	// XXX
#if 0
	int i;
	unsigned char byte_out = 0;
	for(i = 7; i>=0; i--){
		if(byte & (1<<i)){
			bit_set(SI);
		} else {
			bit_clear(SI);
		};	
		SPI_clock();
		byte_out += bit_get(SO)<<i;
	};
	return byte_out;
#endif
	return 0;
};


// sends a block of data from the mem over the spi bus
void spi_io_mem(unsigned char *data, int length)
{
	// transmit 'length' bytes over spi
	while(length){
		SPI_io(*data);
		data++;
		length--;
	};
};


// send cmd + arguments + default crc for init command. once in spi mode 
// we dont need crc so we can keep this constant
void MMC_send_cmd(unsigned char cmd, unsigned long int data)
{
	// default command sequence
	static unsigned char buffer[6] ;
	// fill sequence with our specific data
	buffer[0]=0x40 + cmd;
	buffer[1]=(data>>24)&0xff;
	buffer[2]=(data>>16)&0xff;
	buffer[3]=(data>>8)&0xff;
	buffer[4]=data&0xff;
	buffer[5]=0x95;
	// dispach data
	spi_io_mem(buffer,6);
};


// gets a 1 byte long R1
unsigned char MMC_get_R1(void)
{
	unsigned char retval;
	unsigned int max_errors = 1024;
	// wait for first valid response byte
	do{
		retval = SPI_io(0xff);
		max_errors--;
	}while(  (retval & 0x80) && (max_errors>0));
	
	return retval;
};


// selects the CS for spi xfer
void MMC_CS_select(void)
{
	// pull down the MMC CS line
	// XXX bit_clear(CS);
}

// deselects the CS for spi xfer
void MMC_CS_deselect(void)
{
	// pull up the MMC CS card
	// XXX bit_set(CS);
}

// stops the MMC transmission and sends the 8 clock cycles needed by the mmc for cleanup
void MMC_cleanup(void)
{
	// deselect the MMC card
	MMC_CS_deselect();
	// pulse the SCK 8 times
	SPI_io(0xff);
}

// waits for the card to send the start data block token
void MMC_wait_for_start_token(unsigned int max_errors)
{
	unsigned char retval;
	do {
		// get a byte from the spi bus
		retval = SPI_io(0xff);
		// keep track of the trys
		max_errors--;   
	} while ((retval != MMC_START_TOKEN_SINGLE));
}

// gets n bytes plus crc from spi bus
void MMC_get_data(unsigned char *ptr_data, unsigned int length)
{
	MMC_wait_for_start_token(1024);
	while(length){
		*ptr_data = SPI_io(0xff);
		//USART_sendint(*ptr_data);
		//USART_send(' ');
		length--;
		ptr_data++;
	}
	// get the 2 CRC bytes
	SPI_io(0xff);
	SPI_io(0xff);
}

// reads the CID reg from the card
void MMC_get_CID(unsigned char *ptr_data)
{
  // select card
	MMC_CS_select();
	// tell the MMC card that we want to know its status
	MMC_send_cmd(MMC_CMD_10_SEND_CID,0x0);
	// get the response
	MMC_get_R1();
	// get the register data
	MMC_get_data(ptr_data, 16);
	// cleanup behind us
	MMC_cleanup();
};

// reads the CSD reg from the card
void MMC_get_CSD(unsigned char *ptr_data)
{
	// select card
	MMC_CS_select();
	// tell the MMC card that we want to know its status
	MMC_send_cmd(MMC_CMD_9_SEND_CSD,0x0);
	// get the response
	MMC_get_R1();
	// get the register data
	MMC_get_data(ptr_data, 16);
	// cleanup behind us
	MMC_cleanup();
};

// returns the :
// 		size of the card in MB ( ret * 1024^2) == bytes
// 		sector count and multiplier MB are in unsigned char == C_SIZE / (2^(9-C_SIZE_MULT))
// 		name of the media 
void MMC_get_volume_info(VOLUME_INFO* vinf)
{
	unsigned char data[16];
	// read the CSD register
	MMC_get_CSD(data);
	// get the C_SIZE value. bits [73:62] of data
	// [73:72] == data[6] && 0x03
	// [71:64] == data[7]
	// [63:62] == data[8] && 0xc0
	vinf->sector_count = data[6] & 0x03;
	vinf->sector_count <<= 8;
	vinf->sector_count += data[7];
	vinf->sector_count <<= 2;
	vinf->sector_count += (data[8] & 0xc0) >> 6;
		
	// get the val for C_SIZE_MULT. bits [49:47] of data
	// [49:48] == data[5] && 0x03
	// [47]    == data[4] && 0x80
	vinf->sector_multiply = data[9] & 0x03;
	vinf->sector_multiply <<= 1;
	vinf->sector_multiply += (data[10] & 0x80) >> 7;

	// work out the MBs
	// mega bytes in unsigned char == C_SIZE / (2^(9-C_SIZE_MULT))
	vinf->size_MB = vinf->sector_count >> (9-vinf->sector_multiply);
	vinf->size    = (vinf->sector_count * 512 )<< (vinf->sector_multiply+2);
	vinf->sector_size = 512;
	
	// get the name of the card
	MMC_get_CID(data);
	vinf->name[0] = data[3];
	vinf->name[1] = data[4];
	vinf->name[2] = data[5];
	vinf->name[3] = data[6];
	vinf->name[4] = data[7];
	vinf->name[5] = '\0';
	
}


// get a whole sector and put it in the data buffer
static unsigned char MMC_get_block(unsigned long sector, unsigned char *data)
{
	// turn sectors into byte addr
	sector = sector << 9;
	// select card
	MMC_CS_select();
	// tell the MMC card that we want to know its status
	MMC_send_cmd(MMC_CMD_17_READ_SINGLE, sector);
	// get the response
	MMC_get_R1();
	// wait till the mmc starts sending data
	MMC_wait_for_start_token(255);
	// loop 512 times
	int length = 512;
	while(length--){
		*data = SPI_io(0xff);
		data++;
	};
	// get the 2 CRC bytes
	SPI_io(0xff);
	SPI_io(0xff);
	// give enough time
	MMC_cleanup();
	return 0;
}

static unsigned char MMC_put_block(unsigned long sector, const unsigned char *data){
	// turn sectors into byte addr
	sector = sector << 9;
	// select card
	MMC_CS_select();
	// tell the MMC card that we want to write a sector
	MMC_send_cmd(MMC_CMD_24_WRITE_SINGLE, sector);
	// get the response
	MMC_get_R1();
	// send the start token
	SPI_io(MMC_START_TOKEN_SINGLE);
	
	int length = 512;
	while (length--){
		SPI_io(*data);	
		data++;
	};	
	// send 2 crcs
	SPI_io(0xff);
	SPI_io(0xff);
	// get the data response token
	/*
	can be one of the following :
		MMC_DATA_ACCEPT
		MMC_DATA_CRC
		MMC_DATA_WRITE_ERROR
	*/
	unsigned char tmp = (SPI_io(0xff) & 0xf) >> 1;
	if(tmp != MMC_DATA_ACCEPT){
		//print("mmc_drv.ko : ERROR : MMC_write_sector %d\n", tmp);
		return tmp;
	} 
	
	// all ok, wait while busy
	while (SPI_io(0xff) == MMC_R1B_BUSY_BYTE) {};
	
	return tmp;
}

static unsigned char MMC_init(void)
{
	unsigned char i, j;
	unsigned char res;

	for(j = 0; j < 4; j++){
		MMC_CS_deselect();
		// the data sheet says that the MMC needs 74 clock pulses to startup
		for(i = 0; i < 100; i++){
			// XXX SPI_clock();
			msDelay(100);
		};
		
		msDelay(1000);
		// select card
		MMC_CS_select();
		// put MMC in idle
		MMC_send_cmd(MMC_CMD_0_GO_IDLE,0x0);
		// get the response
		res = MMC_get_R1();
		
		//printk("mmc_drv.ko : response : %d\n", res);
		if (res == 1)
			j = 100;
	}

	if(res != 0x01){
		// we need to indicate the exact error
		if(res == 0xff){
			//print("mmc_drv.ko : card not found\n");
			return 1;
		} else {
			//print("mmc_drv.ko : invalid response\n");	
			return 2;
		}
	}

	//print("mmc_drv.ko : Card Found\n");
	
	// tell the MMC to start its init process by sending the MMC_CMD_1_SEND_OP_COND comand
	// until the response has the idle bit set to 0
	while(res == 0x01){
		// deselect card
		MMC_CS_deselect();
		// send 8 clock pulses
		SPI_io(0xff);
		// select card
		MMC_CS_select();
		// send wake up signal s.t. MMC leaves idle state and switches to operation mode
		MMC_send_cmd(MMC_CMD_1_SEND_OP_COND,0x0);
		// get response
		res = MMC_get_R1();
	}

	// cleanup behind us
	MMC_cleanup();
	
	
	VOLUME_INFO vinf;
	MMC_get_volume_info(&vinf);
	//print("mmc_drv.ko : SIZE : %d, nMUL : %d, COUNT : %d, NAME : %s\n", vinf.size_MB, vinf.sector_multiply, vinf.sector_count, vinf.name);
	return 0;

}


/* libfat glue layer */

DSTATUS disk_initialize (BYTE drv)
{
	cache_init();
	MMC_init();
	return RES_OK;
}

DSTATUS disk_status (BYTE drv)
{
	// FIXME
	return RES_OK;
}

DRESULT disk_read (BYTE drv, BYTE *buf, DWORD sector, BYTE count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (cache_read_sector (buf, sector + i)) {
			if (MMC_get_block (sector + i, buf) != 0)
				return RES_ERROR;

			cache_write_sector (buf, sector + i);
		}

		buf += S_MAX_SIZ;
	}

	return RES_OK;
}

DRESULT disk_write (BYTE drv, const BYTE *buf, DWORD sector, BYTE count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (MMC_put_block (sector + i, buf) != 0)
			return RES_ERROR;

		cache_update_sector (buf, sector + i);
		buf += S_MAX_SIZ;
	}

	return RES_OK;
}

DRESULT disk_ioctl (BYTE drv, BYTE ctrl, void *buf)
{
	// FIXME
	return RES_OK;
}

