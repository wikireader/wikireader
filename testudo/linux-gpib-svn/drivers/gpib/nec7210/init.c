/***************************************************************************
                          nec7210/init.c  -  description
                             -------------------
 board specific initialization stuff

    begin                : Dec 2001
    copyright            : (C) 2001, 2002 by Frank Mori Hess
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

#include "board.h"
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/dma.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/string.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

void nec7210_board_reset( nec7210_private_t *priv, const gpib_board_t *board )
{
	/* 7210 chip reset */
	write_byte(priv, AUX_CR, AUXMR);

	/* disable all interrupts */
	priv->reg_bits[ IMR1 ] = 0;
	write_byte(priv, priv->reg_bits[ IMR1 ], IMR1);
	priv->reg_bits[ IMR2 ] = 0;
	write_byte(priv, priv->reg_bits[ IMR2 ], IMR2);
	write_byte(priv, 0, SPMR);

	/* clear registers by reading */
	read_byte(priv, CPTR);
	read_byte(priv, ISR1);
	read_byte(priv, ISR2);

	/* parallel poll unconfigure */
	write_byte(priv, PPR | HR_PPU, AUXMR);

	priv->reg_bits[ ADMR ] = HR_TRM0 | HR_TRM1;

	priv->auxa_bits = AUXRA | HR_HLDA;
	write_byte(priv, priv->auxa_bits, AUXMR);

	write_byte( priv, AUXRE | 0, AUXMR );

	/* set INT pin to active high, enable command pass through of unknown commands */
	priv->auxb_bits = AUXRB | HR_CPTE;
	write_byte(priv, priv->auxb_bits, AUXMR);
	write_byte(priv, AUXRE, AUXMR);
}

void nec7210_board_online( nec7210_private_t *priv, const gpib_board_t *board )
{
	/* set GPIB address */
	nec7210_primary_address( board, priv, board->pad );
	nec7210_secondary_address( board, priv, board->sad, board->sad >= 0 );

	// enable interrupts
	priv->reg_bits[ IMR1 ] = HR_ERRIE | HR_DECIE | HR_ENDIE |
		HR_DETIE | HR_CPTIE | HR_DOIE | HR_DIIE;
	priv->reg_bits[ IMR2 ] = IMR2_ENABLE_INTR_MASK;
	write_byte( priv, priv->reg_bits[ IMR1 ], IMR1);
	write_byte( priv, priv->reg_bits[ IMR2 ], IMR2);

	write_byte( priv, AUX_PON, AUXMR);
}

/* wrappers for io */
uint8_t nec7210_ioport_read_byte(nec7210_private_t *priv, unsigned int register_num)
{
	return inb((unsigned long)(priv->iobase) + register_num * priv->offset);
}
void nec7210_ioport_write_byte(nec7210_private_t *priv, uint8_t data, unsigned int register_num)
{
	if(register_num == AUXMR)
	{
		/* locking makes absolutely sure noone accesses the
		 * AUXMR register faster than once per microsecond */
		nec7210_locking_ioport_write_byte( priv, data, register_num );
	}else
		outb(data, (unsigned long)(priv->iobase) + register_num * priv->offset);
}
uint8_t nec7210_iomem_read_byte(nec7210_private_t *priv, unsigned int register_num)
{
	return readb(priv->iobase + register_num * priv->offset);
}
void nec7210_iomem_write_byte(nec7210_private_t *priv, uint8_t data, unsigned int register_num)
{
	if(register_num == AUXMR)
	{
		/* locking makes absolutely sure noone accesses the
		 * AUXMR register faster than once per microsecond */
		nec7210_locking_iomem_write_byte( priv, data, register_num );
	}else
		writeb(data, priv->iobase + register_num * priv->offset);
}
/* locking variants of io wrappers, for chips that page-in registers */
uint8_t nec7210_locking_ioport_read_byte(nec7210_private_t *priv, unsigned int register_num)
{
	uint8_t retval;
	unsigned long flags;

	spin_lock_irqsave( &priv->register_page_lock, flags );
	retval = inb((unsigned long)(priv->iobase) + register_num * priv->offset);
	spin_unlock_irqrestore( &priv->register_page_lock, flags );
	return retval;
}
void nec7210_locking_ioport_write_byte(nec7210_private_t *priv, uint8_t data, unsigned int register_num)
{
	unsigned long flags;

	spin_lock_irqsave( &priv->register_page_lock, flags );
	if(register_num == AUXMR)
		udelay(1);
	outb(data, (unsigned long)(priv->iobase) + register_num * priv->offset);
	spin_unlock_irqrestore( &priv->register_page_lock, flags );
}
uint8_t nec7210_locking_iomem_read_byte(nec7210_private_t *priv, unsigned int register_num)
{
	uint8_t retval;
	unsigned long flags;

	spin_lock_irqsave( &priv->register_page_lock, flags );
	retval = readb(priv->iobase + register_num * priv->offset);
	spin_unlock_irqrestore( &priv->register_page_lock, flags );
	return retval;
}
void nec7210_locking_iomem_write_byte(nec7210_private_t *priv, uint8_t data, unsigned int register_num)
{
	unsigned long flags;

	spin_lock_irqsave( &priv->register_page_lock, flags );
	if(register_num == AUXMR)
		udelay(1);
	writeb(data, priv->iobase + register_num * priv->offset);
	spin_unlock_irqrestore( &priv->register_page_lock, flags );
}

static int __init nec7210_init_module( void )
{
	return 0;
}

static void __exit nec7210_exit_module( void )
{
}

module_init( nec7210_init_module );
module_exit( nec7210_exit_module );

EXPORT_SYMBOL(nec7210_board_reset);
EXPORT_SYMBOL(nec7210_board_online);

EXPORT_SYMBOL(nec7210_ioport_read_byte);
EXPORT_SYMBOL(nec7210_ioport_write_byte);
EXPORT_SYMBOL(nec7210_iomem_read_byte);
EXPORT_SYMBOL(nec7210_iomem_write_byte);
EXPORT_SYMBOL(nec7210_locking_ioport_read_byte);
EXPORT_SYMBOL(nec7210_locking_ioport_write_byte);
EXPORT_SYMBOL(nec7210_locking_iomem_read_byte);
EXPORT_SYMBOL(nec7210_locking_iomem_write_byte);

