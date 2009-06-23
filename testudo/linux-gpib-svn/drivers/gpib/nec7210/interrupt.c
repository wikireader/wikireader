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

#include "board.h"
#include <asm/bitops.h>
#include <asm/dma.h>

static inline short nec7210_atn_has_changed(gpib_board_t *board, nec7210_private_t *priv)
{
	short address_status_bits = read_byte(priv, ADSR);

	if(address_status_bits & HR_NATN)
	{
		if(test_bit(ATN_NUM, &board->status))
			return 1;
		else return 0;
	}else
	{
		if(test_bit(ATN_NUM, &board->status))
			return 0;
		else return 1;
	}
	return -1;
}
/*
 *  interrupt service routine
 */
irqreturn_t nec7210_interrupt( gpib_board_t *board, nec7210_private_t *priv )
{
	int status1, status2;

	// read interrupt status (also clears status)
	status1 = read_byte(priv, ISR1);
	status2 = read_byte(priv, ISR2);

	return nec7210_interrupt_have_status( board, priv, status1, status2 );
}

irqreturn_t nec7210_interrupt_have_status( gpib_board_t *board,
	nec7210_private_t *priv, int status1, int status2 )
{
	unsigned long dma_flags;
	int retval = IRQ_NONE;
	
	// record service request in status
	if(status2 & HR_SRQI)
	{
		set_bit(SRQI_NUM, &board->status);
	}

	// change in lockout status
	if(status2 & HR_LOKC)
	{
		if(status2 & HR_LOK)
			set_bit(LOK_NUM, &board->status);
		else
			clear_bit(LOK_NUM, &board->status);
	}

	// change in remote status
	if(status2 & HR_REMC)
	{
		if(status2 & HR_REM)
			set_bit(REM_NUM, &board->status);
		else
			clear_bit(REM_NUM, &board->status);
	}

	// record reception of END
	if(status1 & HR_END)
	{
		set_bit(RECEIVED_END_BN, &priv->state);
		if( ( priv->auxa_bits & HR_HANDSHAKE_MASK ) == HR_HLDE )
			set_bit( RFD_HOLDOFF_BN, &priv->state);
	}

	// get incoming data in PIO mode
	if((status1 & HR_DI))
	{
		set_bit(READ_READY_BN, &priv->state);
		if( ( priv->auxa_bits & HR_HANDSHAKE_MASK ) == HR_HLDA )
			set_bit( RFD_HOLDOFF_BN, &priv->state);
	}
#if 0
	// check for dma read transfer complete
	if(test_bit(DMA_READ_IN_PROGRESS_BN, &priv->state))
	{
		dma_flags = claim_dma_lock();
		disable_dma(priv->dma_channel);
		clear_dma_ff(priv->dma_channel);
		if((status1 & HR_END) || get_dma_residue(priv->dma_channel) == 0)
		{
			clear_bit(DMA_READ_IN_PROGRESS_BN, &priv->state);
		}else
			enable_dma(priv->dma_channel);
		release_dma_lock( dma_flags );
	}
#endif
	if((status1 & HR_DO))
	{
		if(test_bit(DMA_WRITE_IN_PROGRESS_BN, &priv->state) == 0)
			set_bit(WRITE_READY_BN, &priv->state);
#if 0
		if(test_bit(DMA_WRITE_IN_PROGRESS_BN, &priv->state))	// write data, isa dma mode
		{
			// check if dma transfer is complete
			dma_flags = claim_dma_lock();
			disable_dma(priv->dma_channel);
			clear_dma_ff(priv->dma_channel);
			if(get_dma_residue(priv->dma_channel) == 0)
			{
				clear_bit(DMA_WRITE_IN_PROGRESS_BN, &priv->state);
				// XXX race? byte may still be in CDOR reg
			}else
			{
				clear_bit(WRITE_READY_BN, &priv->state);
				enable_dma(priv->dma_channel);
			}
			release_dma_lock( dma_flags );
		}
#endif
	}

	// outgoing command can be sent
	if(status2 & HR_CO)
	{
		set_bit(COMMAND_READY_BN, &priv->state);
	}

	// command pass through received
	if(status1 & HR_CPT)
	{
		unsigned int command;

		command = read_byte(priv, CPTR);
		write_byte(priv, AUX_NVAL, AUXMR);
		printk( "gpib: command pass through 0x%x\n", command );
	}

	if(status1 & HR_ERR)
	{
		set_bit( BUS_ERROR_BN, &priv->state );
		printk("nec7210: bus error\n");
	}

	if( status1 & HR_DEC )
	{
		unsigned short address_status_bits = read_byte(priv, ADSR);

		// ignore device clear events if we are controller in charge
		if((address_status_bits & HR_CIC) == 0)
		{
			push_gpib_event( board, EventDevClr );
			set_bit( DEV_CLEAR_BN, &priv->state );
		}
	}

	if( status1 & HR_DET )
	{
		push_gpib_event( board, EventDevTrg );
	}

	if((status1 & priv->reg_bits[ IMR1 ]) ||
		(status2 & (priv->reg_bits[ IMR2 ] & IMR2_ENABLE_INTR_MASK)) ||
		nec7210_atn_has_changed(board, priv))
	{
		GPIB_DPRINTK( "minor %i, isr1 0x%x, imr1 0x%x, isr2 0x%x, imr2 0x%x\n",
			board->minor, status1, priv->reg_bits[ IMR1 ], status2, priv->reg_bits[ IMR2 ] );
		update_status_nolock(board, priv);
		wake_up_interruptible(&board->wait); /* wake up sleeping process */
		retval = IRQ_HANDLED;
	}
	return retval;
}

EXPORT_SYMBOL(nec7210_interrupt);
EXPORT_SYMBOL(nec7210_interrupt_have_status);


