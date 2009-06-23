/***************************************************************************
                              tms9914/write.c
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

static int pio_write_wait( gpib_board_t *board, tms9914_private_t *priv )
{
	// wait until next byte is ready to be sent
	if(wait_event_interruptible( board->wait,
		test_bit( WRITE_READY_BN, &priv->state ) ||
		test_bit( BUS_ERROR_BN, &priv->state ) ||
		test_bit( DEV_CLEAR_BN, &priv->state ) ||
		test_bit( TIMO_NUM, &board->status ) ) )
	{
		GPIB_DPRINTK( "gpib write interrupted!\n" );
		return -ERESTARTSYS;
	}
	if( test_bit( TIMO_NUM, &board->status ) )
		return -ETIMEDOUT;
	if( test_bit( BUS_ERROR_BN, &priv->state ) )
		return -EIO;
	if( test_bit( DEV_CLEAR_BN, &priv->state ) )
		return -EINTR;

	return 0;
}

static int pio_write(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length, size_t *bytes_written)
{
	ssize_t retval = 0;
	unsigned long flags;

	*bytes_written = 0;
	while(*bytes_written < length)
	{
		retval = pio_write_wait( board, priv );
		if( retval < 0 ) break;

		spin_lock_irqsave(&board->spinlock, flags);
		clear_bit( WRITE_READY_BN, &priv->state );
		write_byte( priv, buffer[(*bytes_written)++], CDOR );
		spin_unlock_irqrestore(&board->spinlock, flags);
	}
	retval = pio_write_wait( board, priv );
	if( retval < 0 ) return retval;

	return length;
}


int tms9914_write(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length,
	int send_eoi, size_t *bytes_written)
{
	ssize_t retval = 0;

	*bytes_written = 0;
	if(length == 0) return 0;

	clear_bit( BUS_ERROR_BN, &priv->state );
	clear_bit( DEV_CLEAR_BN, &priv->state );

	if(send_eoi)
	{
		length-- ; /* save the last byte for sending EOI */
	}

	if(length > 0)
	{
		size_t num_bytes;
		// PIO transfer
		retval = pio_write(board, priv, buffer, length, &num_bytes);
		*bytes_written += num_bytes;
		if(retval < 0)
			return retval;
	}
	if(send_eoi)
	{
		size_t num_bytes;
		/*send EOI */
		write_byte(priv, AUX_SEOI, AUXCR);

		retval = pio_write(board, priv, &buffer[*bytes_written], 1, &num_bytes);
		*bytes_written += num_bytes;
	}
	return retval;
}

EXPORT_SYMBOL(tms9914_write);











