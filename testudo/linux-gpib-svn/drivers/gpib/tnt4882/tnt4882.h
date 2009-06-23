/***************************************************************************
                              tnt4882.h
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

#include "nec7210.h"
#include "gpibP.h"
#include "mite.h"
#include "tnt4882_registers.h"
#include <linux/init.h>
#include <linux/isapnp.h>
#include <linux/delay.h>

static const int ISAPNP_VENDOR_ID_NI = ISAPNP_VENDOR( 'N', 'I', 'C' );
static const int ISAPNP_ID_NI_ATGPIB_TNT = 0xc601;
enum
{
	PCI_DEVICE_ID_NI_GPIB = 0xc801,
	PCI_DEVICE_ID_NI_GPIB_PLUS = 0xc811,
	PCI_DEVICE_ID_NI_PXIGPIB = 0xc821,
	PCI_DEVICE_ID_NI_PMCGPIB = 0xc831,
	PCI_DEVICE_ID_NI_PCIEGPIB = 0x70cf
};

// struct which defines private_data for tnt4882 devices
typedef struct
{
	nec7210_private_t nec7210_priv;
	struct mite_struct *mite;
	struct pnp_dev *pnp_dev;
	unsigned int irq;
	volatile unsigned short imr0_bits;
	volatile unsigned short imr3_bits;
	unsigned short auxg_bits;	// bits written to auxilliary register G
	void (*io_writeb)(unsigned int value, void *address);
	void (*io_writew)(unsigned int value, void *address);
	unsigned int (*io_readb)(void *address);
	unsigned int (*io_readw)(void *address);
} tnt4882_private_t;

// interfaces
extern gpib_interface_t ni_isa_interface;
extern gpib_interface_t ni_isa_accel_interface;
extern gpib_interface_t ni_pci_interface;
extern gpib_interface_t ni_pci_accel_interface;
extern gpib_interface_t ni_pcmcia_interface;
extern gpib_interface_t ni_pcmcia_accel_interface;

// interface functions
int tnt4882_read(gpib_board_t *board, uint8_t *buffer, size_t length,
	int *end, size_t *bytes_read);
int tnt4882_accel_read(gpib_board_t *board, uint8_t *buffer, size_t length,
	int *end, size_t *bytes_read);
int tnt4882_write(gpib_board_t *board, uint8_t *buffer, size_t length,
	int send_eoi, size_t *bytes_written);
int tnt4882_accel_write(gpib_board_t *board, uint8_t *buffer, size_t length,
	int send_eoi, size_t *bytes_written);
ssize_t tnt4882_command(gpib_board_t *board, uint8_t *buffer, size_t length);
ssize_t tnt4882_command_unaccel(gpib_board_t *board, uint8_t *buffer, size_t length);
int tnt4882_take_control(gpib_board_t *board, int synchronous);
int tnt4882_go_to_standby(gpib_board_t *board);
void tnt4882_request_system_control( gpib_board_t *board, int request_control );
void tnt4882_interface_clear(gpib_board_t *board, int assert);
void tnt4882_remote_enable(gpib_board_t *board, int enable);
int tnt4882_enable_eos(gpib_board_t *board, uint8_t eos_byte, int
 compare_8_bits);
void tnt4882_disable_eos(gpib_board_t *board);
unsigned int tnt4882_update_status( gpib_board_t *board, unsigned int clear_mask );
void tnt4882_primary_address(gpib_board_t *board, unsigned int address);
void tnt4882_secondary_address(gpib_board_t *board, unsigned int address, int
 enable);
int tnt4882_parallel_poll(gpib_board_t *board, uint8_t *result);
void tnt4882_parallel_poll_configure( gpib_board_t *board, uint8_t config );
void tnt4882_parallel_poll_response( gpib_board_t *board, int ist );
void tnt4882_serial_poll_response(gpib_board_t *board, uint8_t status);
uint8_t tnt4882_serial_poll_status( gpib_board_t *board );
int tnt4882_line_status( const gpib_board_t *board );
unsigned int tnt4882_t1_delay( gpib_board_t *board, unsigned int nano_sec );
void tnt4882_return_to_local( gpib_board_t *board );

// pcmcia init/cleanup
int __init init_ni_gpib_cs(void);
void __exit exit_ni_gpib_cs(void);

// interrupt service routines
irqreturn_t tnt4882_internal_interrupt(gpib_board_t *board);
irqreturn_t tnt4882_interrupt(int irq, void *arg PT_REGS_ARG);

// utility functions
int tnt4882_allocate_private(gpib_board_t *board);
void tnt4882_free_private(gpib_board_t *board);
void tnt4882_init( tnt4882_private_t *tnt_priv, const gpib_board_t *board );
void tnt4882_board_reset( tnt4882_private_t *tnt_priv, gpib_board_t *board );

// register offset for nec7210 compatible registers
static const int atgpib_reg_offset = 2;

// number of ioports used
static const int atgpib_iosize = 32;
static const int pcmcia_gpib_iosize = 32;

/* paged io */
static inline unsigned int tnt_paged_readb( tnt4882_private_t *priv, unsigned long offset )
{
	priv->io_writeb(AUX_PAGEIN, priv->nec7210_priv.iobase + AUXMR * priv->nec7210_priv.offset);
	udelay(1);
	return priv->io_readb(priv->nec7210_priv.iobase + offset);
}
static inline void tnt_paged_writeb(tnt4882_private_t *priv, unsigned int value, unsigned long offset )
{
	priv->io_writeb(AUX_PAGEIN, priv->nec7210_priv.iobase + AUXMR * priv->nec7210_priv.offset);
	udelay(1);
	priv->io_writeb(value, priv->nec7210_priv.iobase + offset);
}

/* readb/writeb wrappers */
static inline unsigned short tnt_readb( tnt4882_private_t *priv, unsigned long offset )
{
	void *address = priv->nec7210_priv.iobase + offset;
	unsigned long flags;
	unsigned short retval;
	spinlock_t *register_lock = &priv->nec7210_priv.register_page_lock;

	spin_lock_irqsave( register_lock, flags );
	switch( offset )
	{
	case CSR:
	case SASR:
	case ISR0:
	case BSR:
		switch(priv->nec7210_priv.type)
		{
		case TNT4882:
			retval = priv->io_readb(address);
			break;
		case NAT4882:
			retval = tnt_paged_readb( priv, offset - tnt_pagein_offset );
			break;
		case NEC7210:
			retval = 0;
			break;
		default:
			printk( "tnt4882: bug! unsupported ni_chipset\n" );
			retval = 0;
			break;
		}
		break;
	default:
		retval = priv->io_readb( address );
		break;
	}
	spin_unlock_irqrestore( register_lock, flags );
	return retval;
}

static inline void tnt_writeb( tnt4882_private_t *priv, unsigned short value, unsigned long offset)
{
	void *address = priv->nec7210_priv.iobase + offset;
	unsigned long flags;
	spinlock_t *register_lock = &priv->nec7210_priv.register_page_lock;

	spin_lock_irqsave( register_lock, flags );
	switch( offset )
	{
	case KEYREG:
	case IMR0:
	case BCR:
		switch(priv->nec7210_priv.type)
		{
		case TNT4882:
			priv->io_writeb( value, address );
			break;
		case NAT4882:
			tnt_paged_writeb( priv, value, offset - tnt_pagein_offset );
			break;
		case NEC7210:
			break;
		default:
			printk( "tnt4882: bug! unsupported ni_chipset\n" );
			break;
		}
		break;
	default:
		priv->io_writeb( value, address );
		break;
	}
	spin_unlock_irqrestore( register_lock, flags );
}

#endif	// _TNT4882_H
