/***************************************************************************
                              eastwood/eastwood_gpib.h
                             -------------------

    Author: Frank Mori Hess <fmhess@users.sourceforge.net>
    copyright: (C) 2006 Fluke Corporation
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _EASTWOOD_GPIB_H
#define _EASTWOOD_GPIB_H

#include <asm/io.h>
#include <linux/delay.h>
#include "nec7210.h"

#define AVALON_DMA_FIFO_SIZE (8)

typedef struct
{
	nec7210_private_t nec7210_priv;
	int irq;
	int dma_channel;
	uint8_t *dma_buffer;
	int dma_buffer_size;
	void *dma_port;
	void *write_transfer_counter;
	unsigned fifo_dirty : 1;
} eastwood_private_t;


// cb7210 specific registers and bits
enum cb7210_regs
{
	BUS_STATUS = 0x7,
};
enum cb7210_page_in
{
	BUS_STATUS_PAGE = 1,
};

static inline int cb7210_page_in_bits(unsigned int page)
{
	return 0x50 | (page & 0xf);
}
// don't use without locking nec_priv->register_page_lock
static inline uint8_t eastwood_read_byte_nolock(nec7210_private_t *nec_priv,
	int register_num)
{
	uint8_t retval;

	retval = readl(nec_priv->iobase + register_num * nec_priv->offset);
	return retval;
}
// don't use without locking nec_priv->register_page_lock
static inline void eastwood_write_byte_nolock(nec7210_private_t *nec_priv,
	uint8_t data, int register_num)
{
	writel(data, nec_priv->iobase + register_num * nec_priv->offset);
}
static inline uint8_t eastwood_paged_read_byte(eastwood_private_t *e_priv,
	unsigned int register_num, unsigned int page)
{
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	uint8_t retval;
	unsigned long flags;

	spin_lock_irqsave(&nec_priv->register_page_lock, flags);
	eastwood_write_byte_nolock(nec_priv, cb7210_page_in_bits(page), AUXMR);
	udelay(1);
	retval = eastwood_read_byte_nolock(nec_priv, register_num);
	spin_unlock_irqrestore(&nec_priv->register_page_lock, flags);
	return retval;
}
static inline void eastwood_paged_write_byte(eastwood_private_t *e_priv,
	uint8_t data, unsigned int register_num, unsigned int page)
{
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	unsigned long flags;

	spin_lock_irqsave(&nec_priv->register_page_lock, flags);
	eastwood_write_byte_nolock(nec_priv, cb7210_page_in_bits(page), AUXMR);
	udelay(1);
	eastwood_write_byte_nolock(nec_priv, data, register_num);
	spin_unlock_irqrestore(&nec_priv->register_page_lock, flags);
}

enum bus_status_bits
{
	BSR_ATN_BIT = 0x1,
	BSR_EOI_BIT = 0x2,
	BSR_SRQ_BIT = 0x4,
	BSR_IFC_BIT = 0x8,
	BSR_REN_BIT = 0x10,
	BSR_DAV_BIT = 0x20,
	BSR_NRFD_BIT = 0x40,
	BSR_NDAC_BIT = 0x80,
};


enum cb7210_aux_cmds
{
/* AUX_RTL2 is an undocumented aux command which causes cb7210 to assert 
	(and keep asserted) local rtl message.  This is used in conjunction 
	with the (stupid) cb7210 implementation
	of the normal nec7210 AUX_RTL aux command, which
	causes the rtl message to toggle between on and off. */
	AUX_RTL2 = 0xd,
	AUX_NBAF = 0xe,	// new byte available false (also clears seoi)
	AUX_LO_SPEED = 0x40,
	AUX_HI_SPEED = 0x41,
};

enum avalon_dma_registers
{
	AVALON_STATUS_REG = 0,
};

enum avalon_dma_status_bits
{
	AVALON_DONE_BIT = 0x1,
	AVALON_BUSY_BIT = 0x2,
	AVALON_REOP_BIT = 0x4,
	AVALON_WEOP_BIT = 0x8,
	AVALON_LEN_BIT = 0x10
};

static const int eastwood_reg_offset = 4;
static const int eastwood_num_regs = 8;
static const unsigned write_transfer_counter_mask = 0x7ff;

#endif	// _EASTWOOD_GPIB_H
