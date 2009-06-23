/***************************************************************************
                              hp82335/hp82335.h
                             -------------------

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

#ifndef _HP82335_H
#define _HP82335_H

#include "tms9914.h"
#include "gpibP.h"


// struct which defines private_data for board
typedef struct
{
	tms9914_private_t tms9914_priv;
	unsigned int irq;
	unsigned long raw_iobase;
} hp82335_private_t;

// interfaces
extern gpib_interface_t hp82335_interface;

// interface functions
int hp82335_read( gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read);
int hp82335_write( gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written);
ssize_t hp82335_command( gpib_board_t *board, uint8_t *buffer, size_t length );
int hp82335_take_control( gpib_board_t *board, int synchronous );
int hp82335_go_to_standby( gpib_board_t *board );
void hp82335_request_system_control( gpib_board_t *board, int request_control );
void hp82335_interface_clear( gpib_board_t *board, int assert );
void hp82335_remote_enable( gpib_board_t *board, int enable );
int hp82335_enable_eos( gpib_board_t *board, uint8_t eos_byte, int
	compare_8_bits );
void hp82335_disable_eos( gpib_board_t *board );
unsigned int hp82335_update_status( gpib_board_t *board, unsigned int clear_mask );
void hp82335_primary_address( gpib_board_t *board, unsigned int address );
void hp82335_secondary_address( gpib_board_t *board, unsigned int address, int
	enable );
int hp82335_parallel_poll( gpib_board_t *board, uint8_t *result );
void hp82335_parallel_poll_configure( gpib_board_t *board, uint8_t config );
void hp82335_parallel_poll_response( gpib_board_t *board, int ist );
void hp82335_serial_poll_response( gpib_board_t *board, uint8_t status );
void hp82335_return_to_local( gpib_board_t *board );

// interrupt service routines
irqreturn_t hp82335_interrupt(int irq, void *arg PT_REGS_ARG);

// utility functions
int hp82335_allocate_private(gpib_board_t *board);
void hp82335_free_private(gpib_board_t *board);

// size of io memory region used
static const int hp82335_rom_size = 0x2000;
static const int hp82335_upper_iomem_size = 0x2000;

// hp82335 register offsets
enum hp_read_regs
{
	HPREG_CSR = 0x37f8,
	HPREG_STATUS = 0x3ffc,
};
enum hp_write_regs
{
	HPREG_INTR_CLEAR = 0x37f7,
	HPREG_CCR = HPREG_CSR,
};

enum ccr_bits
{
	DMA_ENABLE = ( 1 << 0 ),   /* DMA enable                  */
	DMA_CHAN_SELECT = ( 1 << 1 ),   /* DMA channel select  O=3,1=2 */
	INTR_ENABLE = ( 1 << 2 ),   /* interrupt enable            */
	SYS_DISABLE = ( 1 << 3 ),   /* system controller disable   */
};

enum csr_bits
{
	SWITCH6 = ( 1 << 0 ),   /* switch 6 position           */
	SWITCH5 = ( 1 << 1 ),   /* switch 5 position           */
	SYS_CONTROLLER = ( 1 << 2 ),   /* system controller bit       */
	DMA_ENABLE_STATUS = ( 1 << 4 ),   /* DMA enabled                 */
	DMA_CHAN_STATUS = ( 1 << 5 ),   /* DMA channel   0=3,1=2       */
	INTR_ENABLE_STATUS = ( 1 << 6 ),   /* Interrupt enable            */
	INTR_PENDING = ( 1 << 7 ),   /* Interrupt Pending           */
};

#endif	// _HP82335_H
