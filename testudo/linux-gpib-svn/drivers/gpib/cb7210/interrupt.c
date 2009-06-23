/***************************************************************************
                              nec7210/interrupt.c
                             -------------------

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

#include "cb7210.h"
#include <asm/bitops.h>
#include <asm/dma.h>
#include "quancom_pci.h"

irqreturn_t cb7210_locked_internal_interrupt(gpib_board_t *board);

/*
 * GPIB interrupt service routines
 */

irqreturn_t cb_pci_interrupt(int irq, void *arg PT_REGS_ARG)
{
	int bits;
	gpib_board_t *board = arg;
	cb7210_private_t *priv = board->private_data;

	// first task check if this is really our interrupt in a shared irq environment
	switch(priv->pci_chip)
	{
	case PCI_CHIP_AMCC_S5933:
		if((inl(priv->amcc_iobase + INTCSR_REG) & (INBOX_INTR_CS_BIT | INTR_ASSERTED_BIT)) == 0)
			return IRQ_NONE;
		
		// read incoming mailbox to clear mailbox full flag
		inl(priv->amcc_iobase + INCOMING_MAILBOX_REG(3));
		// clear amccs5933 interrupt
		bits = INBOX_FULL_INTR_BIT | INBOX_BYTE_BITS(3) | INBOX_SELECT_BITS(3) |
			INBOX_INTR_CS_BIT;
		outl(bits, priv->amcc_iobase + INTCSR_REG );
		break;
	case PCI_CHIP_QUANCOM:
		if((inb(nec7210_iobase(priv) + QUANCOM_IRQ_CONTROL_STATUS_REG) & QUANCOM_IRQ_ASSERTED_BIT))
			outb(QUANCOM_IRQ_ENABLE_BIT, nec7210_iobase(priv) + QUANCOM_IRQ_CONTROL_STATUS_REG );
		break;
	default:
		break;
	}
	return cb7210_locked_internal_interrupt(arg);
}

irqreturn_t cb7210_internal_interrupt( gpib_board_t *board )
{
	int hs_status, status1, status2;
	cb7210_private_t *priv = board->private_data;
	nec7210_private_t *nec_priv = &priv->nec7210_priv;
	int clear_bits;

	if((priv->hs_mode_bits & HS_ENABLE_MASK))
	{
		status1 = 0;
		hs_status = cb7210_read_byte(priv, HS_STATUS);
	}else
	{
		hs_status = 0;
		status1 = read_byte( nec_priv, ISR1 );
	}
	status2 = read_byte( nec_priv, ISR2 );
	nec7210_interrupt_have_status( board, nec_priv, status1, status2 );

	GPIB_DPRINTK( "cb7210: status 0x%x, mode 0x%x\n", hs_status, priv->hs_mode_bits );

	clear_bits = 0;

	if( hs_status & HS_HALF_FULL )
	{
		if( priv->hs_mode_bits & HS_TX_ENABLE )
			priv->out_fifo_half_empty = 1;
		else if( priv->hs_mode_bits & HS_RX_ENABLE )
			priv->in_fifo_half_full = 1;
		clear_bits |= HS_CLR_HF_INT;
	}

	if( hs_status & HS_SRQ_INT )
	{
		set_bit(SRQI_NUM, &board->status);
		clear_bits |= HS_CLR_SRQ_INT;
	}
	
	if( ( hs_status & HS_EOI_INT ) )
	{
		clear_bits |= HS_CLR_EOI_EMPTY_INT;
		set_bit( RECEIVED_END_BN, &nec_priv->state );
		if( ( nec_priv->auxa_bits & HR_HANDSHAKE_MASK ) == HR_HLDE )
			set_bit( RFD_HOLDOFF_BN, &nec_priv->state );
	}

	if( ( priv->hs_mode_bits & HS_TX_ENABLE ) &&
		( hs_status & ( HS_TX_MSB_NOT_EMPTY | HS_TX_LSB_NOT_EMPTY ) ) == 0 )
		clear_bits |= HS_CLR_EOI_EMPTY_INT;

	if( clear_bits )
	{
		cb7210_write_byte(priv, priv->hs_mode_bits | clear_bits, HS_MODE);
		cb7210_write_byte(priv, priv->hs_mode_bits, HS_MODE);
		wake_up_interruptible(&board->wait);
	}
	return IRQ_HANDLED;
}

irqreturn_t cb7210_locked_internal_interrupt(gpib_board_t *board)
{
	unsigned long flags;
	irqreturn_t retval;
	
	spin_lock_irqsave(&board->spinlock, flags);
	retval = cb7210_internal_interrupt(board);
	spin_unlock_irqrestore(&board->spinlock, flags);
	return retval;
}

irqreturn_t cb7210_interrupt(int irq, void *arg PT_REGS_ARG)
{
	return cb7210_internal_interrupt(arg);
}
