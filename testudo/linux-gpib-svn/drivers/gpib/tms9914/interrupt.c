/***************************************************************************
                              tms9914/interrupt.c
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

/*
 *  interrupt service routine
 */
irqreturn_t tms9914_interrupt( gpib_board_t *board, tms9914_private_t *priv )
{
	int status0, status1;

	// read interrupt status (also clears status)
	status0 = read_byte( priv, ISR0 );
	status1 = read_byte( priv, ISR1 );
	return tms9914_interrupt_have_status( board, priv, status0, status1 );
}

irqreturn_t tms9914_interrupt_have_status(gpib_board_t *board, tms9914_private_t *priv, int status0,
		int status1)
{
	// record reception of END
	if(status0 & HR_END)
	{
		set_bit(RECEIVED_END_BN, &priv->state);
	}

	// get incoming data in PIO mode
	if((status0 & HR_BI))
	{
		set_bit(READ_READY_BN, &priv->state);
	}

	if((status0 & HR_BO))
	{
		if(read_byte(priv, ADSR) & HR_ATN)
		{
			set_bit(COMMAND_READY_BN, &priv->state);
		}else
		{
			set_bit(WRITE_READY_BN, &priv->state);
		}
	}

	if(status0 & HR_SPAS)
	{
		priv->spoll_status &= ~request_service_bit;
		write_byte(priv, priv->spoll_status, SPMR);
		//FIXME: set SPOLL status bit
	}

	// record service request in status
	if(status1 & HR_SRQ)
	{
		set_bit(SRQI_NUM, &board->status);
	}

	// have been addressed (with secondary addressing disabled)
	if(status1 & HR_MA)
	{
		// clear dac holdoff
		write_byte(priv, AUX_VAL, AUXCR);
	}

	// unrecognized command received
	if(status1 & HR_UNC)
	{
		unsigned short command_byte = read_byte(priv, CPTR);
// 		printk("tms9914: command pass thru 0x%x\n", command_byte);
		switch(command_byte)
		{
		case PPConfig:
			priv->ppoll_configure_state = 1;
			write_byte(priv, AUX_PTS, AUXCR);
			write_byte(priv, AUX_VAL, AUXCR);
			break;
		case PPU:
			tms9914_parallel_poll_configure(board, priv, PPD);
			write_byte(priv, AUX_VAL, AUXCR);	
		default:
			if(priv->ppoll_configure_state)
			{
				priv->ppoll_configure_state = 0;
				if(command_byte >= PPE && command_byte <= PPD + 0xd)
				{
					tms9914_parallel_poll_configure(board, priv, command_byte);
					write_byte(priv, AUX_VAL, AUXCR);
				}else
				{
					printk("tms9914: bad parallel poll configure byte, command pass thru 0x%x\n", command_byte);
					write_byte(priv, AUX_INVAL, AUXCR);
				}
				break;
			}
			printk("tms9914: unknown gpib command pass thru 0x%x\n", command_byte);
			// clear dac holdoff
			write_byte(priv, AUX_INVAL, AUXCR);
			break;
		}
	}

	if(status1 & HR_ERR)
	{
		GPIB_DPRINTK( "gpib bus error\n");
		set_bit( BUS_ERROR_BN, &priv->state );
	}

	if( status1 & HR_IFC )
	{
		push_gpib_event( board, EventIFC );
		clear_bit(CIC_NUM, &board->status);
	}

	if( status1 & HR_GET )
	{
		push_gpib_event( board, EventDevTrg );
		// clear dac holdoff
		write_byte(priv, AUX_VAL, AUXCR);
	}

	if( status1 & HR_DCAS )
	{
		push_gpib_event( board, EventDevClr );
		// clear dac holdoff
		write_byte(priv, AUX_VAL, AUXCR);
		set_bit( DEV_CLEAR_BN, &priv->state );
	}

	// check for being addressed with secondary addressing
	if( status1 & HR_APT )
	{
		if( board->sad < 0 )
		{
			printk( "tms9914: bug, APT interrupt without secondary addressing?\n" );
		}
		if( read_byte( priv, CPTR ) == MSA( board->sad ) )
		{
			write_byte(priv, AUX_VAL, AUXCR);
		}else
			write_byte(priv, AUX_INVAL, AUXCR);
	}

	if( ( status0 & priv->imr0_bits ) || ( status1 & priv->imr1_bits ) )
	{
		GPIB_DPRINTK("isr0 0x%x, imr0 0x%x, isr1 0x%x, imr1 0x%x\n",
			status0, priv->imr0_bits, status1, priv->imr1_bits );
		update_status_nolock( board, priv );
		wake_up_interruptible( &board->wait );
	}
	return IRQ_HANDLED;
}

EXPORT_SYMBOL(tms9914_interrupt);
EXPORT_SYMBOL(tms9914_interrupt_have_status);



