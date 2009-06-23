/***************************************************************************
                          cec/cec.h  -  description
                             -------------------
  Header for cec GPIB boards

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

#ifndef _CEC_GPIB_H
#define _CEC_GPIB_H

#include "nec7210.h"
#include "gpibP.h"
#include "plx9050.h"

typedef struct
{
	nec7210_private_t nec7210_priv;
	struct pci_dev *pci_device;
	// base address for plx9052 pci chip
	unsigned long plx_iobase;
	unsigned int irq;
} cec_private_t;

// interfaces
extern gpib_interface_t cec_pci_interface;
extern gpib_interface_t cec_pcmcia_interface;

// interface functions
int cec_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read);
int cec_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written);
ssize_t cec_command(gpib_board_t *board, uint8_t *buffer, size_t length);
int cec_take_control(gpib_board_t *board, int synchronous);
int cec_go_to_standby(gpib_board_t *board);
void cec_request_system_control( gpib_board_t *board, int request_control );
void cec_interface_clear(gpib_board_t *board, int assert);
void cec_remote_enable(gpib_board_t *board, int enable);
int cec_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits);
void cec_disable_eos(gpib_board_t *board);
unsigned int cec_update_status( gpib_board_t *board, unsigned int clear_mask );
void cec_primary_address(gpib_board_t *board, unsigned int address);
void cec_secondary_address(gpib_board_t *board, unsigned int address, int enable);
int cec_parallel_poll(gpib_board_t *board, uint8_t *result);
void cec_parallel_poll_configure( gpib_board_t *board, uint8_t configuration );
void cec_parallel_poll_response( gpib_board_t *board, int ist );
void cec_serial_poll_response( gpib_board_t *board, uint8_t status );
void cec_return_to_local( gpib_board_t *board );

// interrupt service routines
irqreturn_t cec_interrupt(int irq, void *arg PT_REGS_ARG);

// utility functions
void cec_free_private(gpib_board_t *board);
int cec_generic_attach(gpib_board_t *board);
void cec_init( cec_private_t *priv, const gpib_board_t *board );

// offset between consecutive nec7210 registers
static const int cec_reg_offset = 1;

#endif	// _CEC_GPIB_H
