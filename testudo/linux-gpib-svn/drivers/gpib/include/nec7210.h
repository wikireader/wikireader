/***************************************************************************
                                   nec7210.h
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

#ifndef _NEC7210_H
#define _NEC7210_H

#include "gpib_state_machines.h"
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/interrupt.h>

#include "gpib_types.h"
#include "nec7210_registers.h"

/* struct used to provide variables local to a nec7210 chip */
typedef struct nec7210_private_struct nec7210_private_t;
struct nec7210_private_struct
{
	void *iobase;
	unsigned int offset;	// offset between successive nec7210 io addresses
	unsigned int dma_channel;
	uint8_t *dma_buffer;
	unsigned int dma_buffer_length;	// length of dma buffer
	dma_addr_t dma_buffer_addr;	// bus address of board->buffer for use with dma
	// software copy of bits written to registers
	volatile uint8_t reg_bits[ 8 ];
	volatile uint8_t auxa_bits;	// bits written to auxilliary register A
	volatile uint8_t auxb_bits;	// bits written to auxilliary register B
	// used to keep track of board's state, bit definitions given below
	volatile unsigned long state;
	/* lock for chips that extend the nec7210 registers by paging in alternate regs */
	spinlock_t register_page_lock;
	// wrappers for outb, inb, readb, or writeb
	uint8_t (*read_byte)(nec7210_private_t *priv, unsigned int register_number);
	void (*write_byte)(nec7210_private_t *priv, uint8_t byte, unsigned int register_number);
	enum nec7210_chipset type;
	enum talker_function_state talker_state;
	enum listener_function_state listener_state;
	void *private;
	unsigned srq_pending : 1;
};

static inline void init_nec7210_private( nec7210_private_t *priv )
{
	memset( priv, 0, sizeof( nec7210_private_t ) );
	spin_lock_init( &priv->register_page_lock );
}

// slightly shorter way to access read_byte and write_byte
static inline uint8_t read_byte(nec7210_private_t *priv, unsigned int register_number)
{
	return priv->read_byte(priv, register_number);
}
static inline void write_byte(nec7210_private_t *priv, uint8_t byte, unsigned int register_number)
{
	priv->write_byte(priv, byte, register_number);
}

// nec7210_private_t.state bit numbers
enum
{
	PIO_IN_PROGRESS_BN,	// pio transfer in progress
	DMA_READ_IN_PROGRESS_BN,	// dma read transfer in progress
	DMA_WRITE_IN_PROGRESS_BN,	// dma write transfer in progress
	READ_READY_BN,	// board has data byte available to read
	WRITE_READY_BN,	// board is ready to send a data byte
	COMMAND_READY_BN,	// board is ready to send a command byte
	RECEIVED_END_BN,	// received END
	BUS_ERROR_BN,	// output error has occurred
	RFD_HOLDOFF_BN,	// rfd holdoff in effect
	DEV_CLEAR_BN,	// device clear received
};

// interface functions
int nec7210_read(gpib_board_t *board, nec7210_private_t *priv,
	uint8_t *buffer, size_t length, int *end, size_t *bytes_read);
int nec7210_write(gpib_board_t *board, nec7210_private_t *priv,
	uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written);
ssize_t nec7210_command(gpib_board_t *board, nec7210_private_t *priv,
	uint8_t *buffer, size_t length);
int nec7210_take_control(gpib_board_t *board, nec7210_private_t *priv,
	int syncronous);
int nec7210_go_to_standby(gpib_board_t *board, nec7210_private_t *priv);
void nec7210_request_system_control( gpib_board_t *board,
	nec7210_private_t *priv, int request_control );
void nec7210_interface_clear(gpib_board_t *board, nec7210_private_t *priv, int assert);
void nec7210_remote_enable(gpib_board_t *board, nec7210_private_t *priv, int enable);
int nec7210_enable_eos(gpib_board_t *board, nec7210_private_t *priv,
	uint8_t eos_bytes, int compare_8_bits);
void nec7210_disable_eos(gpib_board_t *board, nec7210_private_t *priv);
unsigned int nec7210_update_status( gpib_board_t *board, nec7210_private_t *priv,
	unsigned int clear_mask );
void nec7210_primary_address( const gpib_board_t *board,
	nec7210_private_t *priv, unsigned int address);
void nec7210_secondary_address( const gpib_board_t *board, nec7210_private_t *priv,
	unsigned int address, int enable);
int nec7210_parallel_poll(gpib_board_t *board, nec7210_private_t *priv, uint8_t *result);
void nec7210_serial_poll_response(gpib_board_t *board, nec7210_private_t *priv, uint8_t status);
void nec7210_parallel_poll_configure( gpib_board_t *board,
	nec7210_private_t *priv, unsigned int configuration );
void nec7210_parallel_poll_response( gpib_board_t *board,
	nec7210_private_t *priv, int ist );
uint8_t nec7210_serial_poll_status( gpib_board_t *board,
	nec7210_private_t *priv );
unsigned int nec7210_t1_delay( gpib_board_t *board,
	nec7210_private_t *priv, unsigned int nano_sec );
void nec7210_return_to_local( const gpib_board_t *board, nec7210_private_t *priv );

// utility functions
void nec7210_board_reset( nec7210_private_t *priv, const gpib_board_t *board );
void nec7210_board_online( nec7210_private_t *priv, const gpib_board_t *board );
unsigned int nec7210_set_reg_bits( nec7210_private_t *priv, unsigned int reg,
	unsigned int mask, unsigned int bits );
void nec7210_set_handshake_mode( gpib_board_t *board, nec7210_private_t *priv, int mode );
void nec7210_release_rfd_holdoff( gpib_board_t *board, nec7210_private_t *priv );
uint8_t nec7210_read_data_in( gpib_board_t *board, nec7210_private_t *priv, int *end );

// wrappers for io functions
uint8_t nec7210_ioport_read_byte(nec7210_private_t *priv, unsigned int register_num);
void nec7210_ioport_write_byte(nec7210_private_t *priv, uint8_t data, unsigned int register_num);
uint8_t nec7210_iomem_read_byte(nec7210_private_t *priv, unsigned int register_num);
void nec7210_iomem_write_byte(nec7210_private_t *priv, uint8_t data, unsigned int register_num);
uint8_t nec7210_locking_ioport_read_byte(nec7210_private_t *priv, unsigned int register_num);
void nec7210_locking_ioport_write_byte(nec7210_private_t *priv, uint8_t data, unsigned int register_num);
uint8_t nec7210_locking_iomem_read_byte(nec7210_private_t *priv, unsigned int register_num);
void nec7210_locking_iomem_write_byte(nec7210_private_t *priv, uint8_t data, unsigned int register_num);

// interrupt service routine
irqreturn_t nec7210_interrupt(gpib_board_t *board, nec7210_private_t *priv);
irqreturn_t nec7210_interrupt_have_status( gpib_board_t *board,
	nec7210_private_t *priv, int status1, int status2 );

#endif	//_NEC7210_H
