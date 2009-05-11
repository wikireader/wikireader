/*
 * a.lp_mp3 - Open Source MP3 Player - FOX BOARD MMC KERNEL MODULE
 * Copyright (c) 2003-2005 K. John '2B|!2B' Crispin
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
 *
 * Feedback, Bugs.... mail john{AT}phrozen.org
 *
 */ 
/*
#ifndef MMC_H__
#define MMC_H__

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

#define MMC_CMD_0_GO_IDLE				0
#define MMC_CMD_1_SEND_OP_COND			1
#define MMC_CMD_9_SEND_CSD				9
#define MMC_CMD_10_SEND_CID				10
#define MMC_CMD_12_STOP					12
#define MMC_CMD_13_SEND_STATUS			13
#define MMC_CMD_16_BLOCKLEN				16
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


static unsigned char MMC_init(void);

static unsigned char MMC_get_block(unsigned long sector, unsigned char *data);

static unsigned char MMC_put_block(unsigned long sector,unsigned char *data);

void MMC_get_volume_info(VOLUME_INFO* vinf)



#endif

*/
