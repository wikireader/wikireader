/***************************************************************************
                                 nec7210/aux.c
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

int nec7210_take_control(gpib_board_t *board, nec7210_private_t *priv, int syncronous)
{
	int i;
	const int timeout = 1000;
	int retval = 0;
	unsigned int adsr_bits = 0;

	if(syncronous)
	{
		write_byte(priv, AUX_TCS, AUXMR);
	}else
		write_byte(priv, AUX_TCA, AUXMR);
	// busy wait until ATN is asserted
	for(i = 0; i < timeout; i++)
	{
		adsr_bits = read_byte(priv, ADSR);
		if((adsr_bits & HR_NATN) == 0)
			break;
		udelay(1);
	}
	// if busy wait has failed, try sleeping
	if( i == timeout )
	{
		for(i = 0; i < HZ; i++)
		{
			set_current_state(TASK_INTERRUPTIBLE);
			if(schedule_timeout(1))
				return -ERESTARTSYS;
			adsr_bits = read_byte(priv, ADSR);
			if((adsr_bits & HR_NATN) == 0)
				break;
		}
		if(i == HZ)
		{
			printk("nec7210: error waiting for ATN\n");
			return -ETIMEDOUT;
		}
	}
	clear_bit( WRITE_READY_BN, &priv->state );
	return retval;
}

int nec7210_go_to_standby(gpib_board_t *board, nec7210_private_t *priv)
{
	int i;
	const int timeout = 1000;
	unsigned int adsr_bits = 0;
	int retval = 0;

	write_byte(priv, AUX_GTS, AUXMR);
	// busy wait until ATN is released
	for(i = 0; i < timeout; i++)
	{
		adsr_bits = read_byte(priv, ADSR);
		if(adsr_bits & HR_NATN)
			break;
		udelay(1);
	}
	// if busy wait has failed, try sleeping
	if(i == timeout)
	{
		for(i = 0; i < HZ; i++)
		{
			set_current_state(TASK_INTERRUPTIBLE);
			if(schedule_timeout(1))
				return -ERESTARTSYS;
			adsr_bits = read_byte(priv, ADSR);
			if(adsr_bits & HR_NATN)
				break;
		}
		if(i == HZ)
		{
			printk("nec7210: error waiting for NATN\n");
			return -ETIMEDOUT;
		}
	}
	clear_bit( COMMAND_READY_BN, &priv->state );
	return retval;
}

void nec7210_request_system_control( gpib_board_t *board, nec7210_private_t *priv,
	int request_control )
{
	if( request_control == 0 )
	{
		write_byte( priv, AUX_CREN, AUXMR );
		write_byte( priv, AUX_CIFC, AUXMR );
		write_byte( priv, AUX_DSC, AUXMR );
	}
}

void nec7210_interface_clear(gpib_board_t *board, nec7210_private_t *priv, int assert)
{
	if(assert)
		write_byte(priv, AUX_SIFC, AUXMR);
	else
		write_byte(priv, AUX_CIFC, AUXMR);
}

void nec7210_remote_enable(gpib_board_t *board, nec7210_private_t *priv, int enable)
{
	if(enable)
		write_byte(priv, AUX_SREN, AUXMR);
	else
		write_byte(priv, AUX_CREN, AUXMR);
}

void nec7210_release_rfd_holdoff( gpib_board_t *board, nec7210_private_t *priv )
{
	unsigned long flags;

	spin_lock_irqsave( &board->spinlock, flags );
	if( test_bit( RFD_HOLDOFF_BN, &priv->state ) &&
		test_bit( READ_READY_BN, &priv->state ) == 0 )
	{
		write_byte( priv, AUX_FH, AUXMR );
		clear_bit( RFD_HOLDOFF_BN, &priv->state );
	}
	spin_unlock_irqrestore( &board->spinlock, flags );
}

unsigned int nec7210_t1_delay( gpib_board_t *board, nec7210_private_t *priv,
	unsigned int nano_sec )
{
	unsigned int retval;

	if( nano_sec <= 500 )
	{
		priv->auxb_bits |= HR_TRI;
		retval = 500;
	}else
	{
		priv->auxb_bits &= ~HR_TRI;
		retval = 2000;
	}
	write_byte( priv, priv->auxb_bits, AUXMR );

	return retval;
}

void nec7210_return_to_local( const gpib_board_t *board, nec7210_private_t *priv )
{
	write_byte( priv, AUX_RTL, AUXMR );
}

EXPORT_SYMBOL( nec7210_t1_delay );
EXPORT_SYMBOL( nec7210_request_system_control );
EXPORT_SYMBOL( nec7210_take_control );
EXPORT_SYMBOL( nec7210_go_to_standby );
EXPORT_SYMBOL( nec7210_interface_clear );
EXPORT_SYMBOL( nec7210_remote_enable );
EXPORT_SYMBOL( nec7210_release_rfd_holdoff );
EXPORT_SYMBOL( nec7210_return_to_local );
