/***************************************************************************
                          tms9914/init.c  -  description
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

// size of modbus pci memory io region
static const int iomem_size = 0x2000;

void tms9914_board_reset( tms9914_private_t *priv )
{
	/* chip reset */
	write_byte(priv, AUX_CHIP_RESET | AUX_CS, AUXCR);

	/* disable all interrupts */
	priv->imr0_bits = 0;
	write_byte(priv, priv->imr0_bits, IMR0);
	priv->imr1_bits = 0;
	write_byte(priv, priv->imr1_bits, IMR1);
	write_byte(priv, AUX_DAI | AUX_CS, AUXCR);

	/* clear registers by reading */
	read_byte(priv, CPTR);
	read_byte(priv, ISR0);
	read_byte(priv, ISR1);

	write_byte(priv, 0, SPMR);

	/* parallel poll unconfigure */
	write_byte(priv, 0, PPR);
	// request for data holdoff
	tms9914_set_holdoff_mode(priv, TMS9914_HOLDOFF_ALL);
}

void tms9914_online( gpib_board_t *board, tms9914_private_t *priv )
{
	/* set GPIB address */
	tms9914_primary_address( board, priv, board->pad );
	tms9914_secondary_address( board, priv, board->sad, board->sad >=0 );

	// enable tms9914 interrupts
	priv->imr0_bits |= HR_MACIE | HR_RLCIE | HR_ENDIE | HR_BOIE | HR_BIIE |
		HR_SPASIE;
	priv->imr1_bits |= HR_MAIE | HR_SRQIE | HR_UNCIE | HR_ERRIE | HR_IFCIE |
		HR_GETIE| HR_DCASIE;
	write_byte(priv, priv->imr0_bits, IMR0);
	write_byte(priv, priv->imr1_bits, IMR1);
	write_byte(priv, AUX_DAI, AUXCR);

	// turn off reset state
	write_byte(priv, AUX_CHIP_RESET, AUXCR);
}

// wrapper for inb
uint8_t tms9914_ioport_read_byte(tms9914_private_t *priv, unsigned int register_num)
{
	return inb((unsigned long)(priv->iobase) + register_num * priv->offset);
}
// wrapper for outb
void tms9914_ioport_write_byte(tms9914_private_t *priv, uint8_t data, unsigned int register_num)
{
	outb(data, (unsigned long)(priv->iobase) + register_num * priv->offset);
	if(register_num == AUXCR)
		udelay(1);
}

// wrapper for readb
uint8_t tms9914_iomem_read_byte(tms9914_private_t *priv, unsigned int register_num)
{
	return readb(priv->iobase + register_num * priv->offset);
}
// wrapper for writeb
void tms9914_iomem_write_byte(tms9914_private_t *priv, uint8_t data, unsigned int register_num)
{
	writeb(data, priv->iobase + register_num * priv->offset);
	if(register_num == AUXCR)
		udelay(1);
}

static int __init tms9914_init_module(void)
{
	return 0;
}

static void __exit tms9914_exit_module(void)
{
}

module_init( tms9914_init_module );
module_exit( tms9914_exit_module );

EXPORT_SYMBOL_GPL(tms9914_board_reset);
EXPORT_SYMBOL_GPL(tms9914_online);
EXPORT_SYMBOL_GPL(tms9914_ioport_read_byte);
EXPORT_SYMBOL_GPL(tms9914_ioport_write_byte);
EXPORT_SYMBOL_GPL(tms9914_iomem_read_byte);
EXPORT_SYMBOL_GPL(tms9914_iomem_write_byte);

