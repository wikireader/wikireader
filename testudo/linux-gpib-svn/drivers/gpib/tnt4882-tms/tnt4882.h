/***************************************************************************
                              tms9914/tnt4882.h
                             -------------------

    begin                : Jan 2002
    copyright            : (C) 2002 by Frank Mori Hess
    email                : fmhess@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _TNT4882_H
#define _TNT4882_H

#include "tms9914.h"
#include "gpibP.h"
#include "mite.h"
#include <linux/delay.h>

enum
{
	PCI_DEVICE_ID_NI_GPIB = 0xc801,
};

// struct which defines private_data for tnt4882 devices
typedef struct
{
	tms9914_private_t tms9914_priv;
	struct mite_struct *mite;
	unsigned int irq;
} tnt4882_private_t;

// interfaces
extern gpib_interface_t ni_isa_interface;

// interface functions
ssize_t tnt4882_read(gpib_board_t *board, uint8_t *buffer, size_t length, int
 *end);
ssize_t tnt4882_write(gpib_board_t *board, uint8_t *buffer, size_t length, int
 send_eoi);
ssize_t tnt4882_command(gpib_board_t *board, uint8_t *buffer, size_t length);
int tnt4882_take_control(gpib_board_t *board, int synchronous);
int tnt4882_go_to_standby(gpib_board_t *board);
void tnt4882_interface_clear(gpib_board_t *board, int assert);
void tnt4882_remote_enable(gpib_board_t *board, int enable);
void tnt4882_enable_eos(gpib_board_t *board, uint8_t eos_byte, int
 compare_8_bits);
void tnt4882_disable_eos(gpib_board_t *board);
unsigned int tnt4882_update_status( gpib_board_t *board, unsigned int clear_mask );
void tnt4882_primary_address(gpib_board_t *board, unsigned int address);
void tnt4882_secondary_address(gpib_board_t *board, unsigned int address, int
 enable);
int tnt4882_parallel_poll(gpib_board_t *board, uint8_t *result);
void tnt4882_parallel_poll_response( gpib_board_t *board, uint8_t config );
void tnt4882_serial_poll_response(gpib_board_t *board, uint8_t status);
void tnt4882_return_to_local( gpib_board_t *board );

// interrupt service routines
void tnt4882_interrupt(int irq, void *arg, struct pt_regs *registerp);

// utility functions
int tnt4882_allocate_private(gpib_board_t *board);
void tnt4882_free_private(gpib_board_t *board);

// register offset for tms9914 compatible registers
static const int atgpib_reg_offset = 2;

// number of ioports used
static const int atgpib_iosize = 32;

// tnt4882 register offsets
enum
{
	ACCWR = 0x5,
	INTRT = 0x7,
	// register number for auxilliary command register when swap bit is set (9914 mode)
	SWAPPED_AUXCR = 0xa,
	// offset of auxilliary mode register in 7210 mode
	AUXMR = 0xa,
	HSSEL = 0xd,	// handshake select register
	CFG = 0x10,
	IMR3 = 0x12,
	CNT0 = 0x14,
	CNT1 = 0x16,
	KEYREG = 0x17,	// key control register (7210 mode only)
	FIFOB = 0x18,
	FIFOA = 0x19,
	CCRG = 0x1a,	// carry cycle register
	CMDR = 0x1c,	// command register
	TIMER = 0x1e,	// timer register

	STS1 = 0x10,		/* T488 Status Register 1 */
	STS2 = 0x1c,	        /* T488 Status Register 2 */
	ISR3 = 0x1a,		/* T488 Interrupt Status Register 3 */
};

/*============================================================*/

/* TURBO-488 registers bit definitions */

/* HSSEL -- handshake select register (write only) */
#define NODMA 0x10

/* STS1 -- Status Register 1 (read only) */
#define S_DONE          (0x80)	/* DMA done                           */
#define S_SC            (0x40)	/* is system contoller                */
#define S_IN            (0x20)	/* DMA in (to memory)                 */
#define S_DRQ           (0x10)	/* DRQ line (for diagnostics)         */
#define S_STOP          (0x08)	/* DMA stopped                        */
#define S_NDAV          (0x04)	/* inverse of DAV                     */
#define S_HALT          (0x02)	/* status of transfer machine         */
#define S_GSYNC         (0x01)	/* indicates if GPIB is in sync w I/O */

/* CFG -- Configuration Register (write only) */
#define	C_CMD	        (1<<7)	/* FIFO 'bcmd' in progress            */
#define	C_TLCH	     (1<<6)	/* halt DMA on TLC interrupt          */
#define	C_IN	        (1<<5)	/* DMA is a GPIB read                 */
#define	C_A_B	        (1<<4)	/* fifo order 1=motorola, 0=intel     */
#define	C_CCEN	     (1<<3)	/* enable carry cycle                 */
#define	C_TMOE	     (1<<2)	/* enable CPU bus time limit          */
#define	C_T_B	        (1<<1)  	/* tmot reg is: 1=125ns clocks,       */
						/* 0=num bytes                        */
#define	C_B16	        (1<<0)  	/* 1=FIFO is 16-bit register, 0=8-bit */

/* ISR3 -- Interrupt Status Register (read only) */
#define	HR_INTR	        (1<<7)	/* 1=board is interrupting	*/
#define	HR_SRQI_CIC      (1<<5)	/* SRQ asserted and we are CIC	*/
#define	HR_STOP          (1<<4)	/* fifo empty or STOP command	*/
						/* issued			*/
#define	HR_NFF	        (1<<3)	/* NOT full fifo		*/
#define	HR_NEF	        (1<<2)	/* NOT empty fifo		*/
#define	HR_TLCI	        (1<<1)	/* TLC interrupt asserted	*/
#define	HR_DONE          (1<<0)	/* DMA done			*/

/* CMDR -- Command Register */
#define	CLRSC		(unsigned char)0x2	/* clear the SC bit 		*/
#define	SETSC		(unsigned char)0x3	/* set the SC bit 		*/
#define	GO		(unsigned char)(1<<2)	/* start DMA 			*/
#define	STOP		(unsigned char)(1<<3)	/* stop DMA 			*/
#define	RSTFIFO		(unsigned char)(1<<4)	/* reset the FIFO 		*/
#define SFTRST		(unsigned char)(1<<5)	/* issue a software reset 	*/
#define	DU_ADD		(unsigned char)(1<<6)	/* Motorola mode dual 	  	*/
#define	DDU_ADD		(unsigned char)(1<<7)	/* Disable dual addressing 	*/

/* STS2 -- Status Register 2 */
#define AFFN		(1<<3)	/* "A full FIFO NOT"  (0=FIFO full)  */
#define AEFN		(1<<2)	/* "A empty FIFO NOT" (0=FIFO empty) */
#define BFFN		(1<<1)	/* "B full FIFO NOT"  (0=FIFO full)  */
#define BEFN		(1<<0)	/* "B empty FIFO NOT" (0=FIFO empty) */

// Auxilliary commands
enum
{
	AUX_7210 = 0x99,	// switch to 7210 mode
	AUX_9914 = 0x15,	// switch to 9914 mode
};

#endif	// _TNT4882_H
