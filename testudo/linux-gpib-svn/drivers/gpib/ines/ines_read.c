/***************************************************************************
                          ines_read.c  -  description
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
#include "ines.h"
#include <linux/sched.h>

static const int in_fifo_size = 0xff;

static inline unsigned short num_in_fifo_bytes( ines_private_t *ines_priv )
{
	return ines_inb( ines_priv, IN_FIFO_COUNT );
}

static ssize_t pio_read( gpib_board_t *board, ines_private_t *ines_priv, uint8_t *buffer, size_t length, size_t *nbytes)
{
	ssize_t retval = 0;
	unsigned int num_fifo_bytes, i;
	nec7210_private_t *nec_priv = &ines_priv->nec7210_priv;

	*nbytes = 0;
	while( *nbytes < length )
	{
		if( wait_event_interruptible( board->wait,
			num_in_fifo_bytes( ines_priv ) ||
			test_bit(RECEIVED_END_BN, &nec_priv->state) ||
			test_bit(DEV_CLEAR_BN, &nec_priv->state) ||
			test_bit(TIMO_NUM, &board->status)))
		{
			printk("gpib: pio read wait interrupted\n");
			return -ERESTARTSYS;
		}
		if( test_bit( TIMO_NUM, &board->status ) )
			return -ETIMEDOUT;
		if( test_bit( DEV_CLEAR_BN, &nec_priv->state ) )
			return -EINTR;

		num_fifo_bytes = num_in_fifo_bytes( ines_priv );
		if( num_fifo_bytes + *nbytes > length )
		{
			printk( "ines: counter allowed %li extra byte(s)\n", (long)(num_fifo_bytes - (length - *nbytes)));
			num_fifo_bytes = length - *nbytes;
		}
		for( i = 0; i < num_fifo_bytes; i++ )
		{
			buffer[(*nbytes)++] = read_byte( nec_priv, DIR );
		}
		if( test_bit( RECEIVED_END_BN, &nec_priv->state ) && num_in_fifo_bytes( ines_priv ) == 0 )
		{
			break;
		}
		if(need_resched()) schedule();
	}
	/* make sure RECEIVED_END is in sync */
	ines_interrupt(board);
	return retval;
}

int ines_accel_read( gpib_board_t *board, uint8_t *buffer,
	size_t length, int *end, size_t *bytes_read)
{
	ssize_t retval = 0;
	ines_private_t *ines_priv = board->private_data;
	nec7210_private_t *nec_priv = &ines_priv->nec7210_priv;
	int counter_setting;

	*end = 0;
	*bytes_read = 0;
	if(length == 0) return 0;

	clear_bit( DEV_CLEAR_BN, &nec_priv->state );

	write_byte( nec_priv, INES_RFD_HLD_IMMEDIATE, AUXMR );

	//clear in fifo
	nec7210_set_reg_bits(nec_priv, ADMR, IN_FIFO_ENABLE_BIT, 0);
	nec7210_set_reg_bits(nec_priv, ADMR, IN_FIFO_ENABLE_BIT, IN_FIFO_ENABLE_BIT);
	
	ines_priv->extend_mode_bits |= LAST_BYTE_HANDLING_BIT;
	ines_priv->extend_mode_bits &= ~XFER_COUNTER_OUTPUT_BIT & ~XFER_COUNTER_ENABLE_BIT;
	ines_outb( ines_priv, ines_priv->extend_mode_bits, EXTEND_MODE );

	counter_setting = length - num_in_fifo_bytes(ines_priv);
	if(counter_setting > 0)
	{
		ines_set_xfer_counter(ines_priv, length);
		ines_priv->extend_mode_bits |= XFER_COUNTER_ENABLE_BIT;
		ines_outb( ines_priv, ines_priv->extend_mode_bits, EXTEND_MODE );

		// holdoff on END
		nec7210_set_handshake_mode( board, nec_priv, HR_HLDE );
		/* release rfd holdoff */
		write_byte( nec_priv, AUX_FH, AUXMR );
	}

	retval = pio_read(board, ines_priv, buffer, length, bytes_read);
	ines_priv->extend_mode_bits &= ~XFER_COUNTER_ENABLE_BIT;
	ines_outb( ines_priv, ines_priv->extend_mode_bits, EXTEND_MODE );
	if( retval < 0 )
	{
		write_byte( nec_priv, INES_RFD_HLD_IMMEDIATE, AUXMR );
		return retval;
	}
	if( test_and_clear_bit( RECEIVED_END_BN, &nec_priv->state ) )
		*end = 1;

	return retval;
}






