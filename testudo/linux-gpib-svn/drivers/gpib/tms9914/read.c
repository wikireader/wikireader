/***************************************************************************
                              tms9914/read.c
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
#include <linux/spinlock.h>

static int check_for_eos( tms9914_private_t *priv, uint8_t byte )
{
	static const uint8_t sevenBitCompareMask = 0x7f;
	
	if( ( priv->eos_flags & REOS ) == 0 ) return 0;

	if( priv->eos_flags & BIN )
	{
		if( priv->eos == byte )
			return 1;
	}else
	{
		if((priv->eos & sevenBitCompareMask) == (byte & sevenBitCompareMask))
			return 1;
	}
	return 0;
}

static int wait_for_read_byte(gpib_board_t *board, tms9914_private_t *priv)
{
	if(wait_event_interruptible(board->wait,
		test_bit(READ_READY_BN, &priv->state) ||
		test_bit(DEV_CLEAR_BN, &priv->state) ||
		test_bit(TIMO_NUM, &board->status)))
	{
		printk("gpib: pio read wait interrupted\n");
		return -ERESTARTSYS;
	};
	if( test_bit( TIMO_NUM, &board->status ) )
	{
		return -ETIMEDOUT;
	}
	if( test_bit( DEV_CLEAR_BN, &priv->state ) )
	{
		return -EINTR;
	}
	return 0;
}

static inline uint8_t tms9914_read_data_in(gpib_board_t *board, tms9914_private_t *priv, int *end)
{
	unsigned long flags;
	uint8_t data;

	spin_lock_irqsave(&board->spinlock, flags);
	clear_bit(READ_READY_BN, &priv->state);
	data = read_byte( priv, DIR );
	if(test_and_clear_bit(RECEIVED_END_BN, &priv->state))
		*end = 1;
	else
		*end = 0;	
	switch(priv->holdoff_mode)
	{
	case TMS9914_HOLDOFF_EOI:
		if(*end)
			priv->holdoff_active = 1;
		break;
	case TMS9914_HOLDOFF_ALL:
		priv->holdoff_active = 1;
		break;
	case TMS9914_HOLDOFF_NONE:
		break;
	default:
		printk("%s: bug! bad holdoff mode %i\n", __FUNCTION__, priv->holdoff_mode);
		break;
	};
	spin_unlock_irqrestore(&board->spinlock, flags);

	return data;
}

static int pio_read(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	ssize_t retval = 0;

	*bytes_read = 0;
	*end = 0;
	while(*bytes_read < length && *end == 0)
	{
		tms9914_release_holdoff(priv);
		retval = wait_for_read_byte(board, priv);
		if(retval < 0) return retval;;
		buffer[(*bytes_read)++] = tms9914_read_data_in(board, priv, end);

		if(check_for_eos(priv, buffer[*bytes_read - 1]))
			*end = 1;
	}

	return retval;
}

int tms9914_read(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	ssize_t retval = 0;
	size_t num_bytes;
	
	*end = 0;
	*bytes_read = 0;
	if(length == 0) return 0;

	clear_bit( DEV_CLEAR_BN, &priv->state );

	// transfer data (except for last byte)
	if(length > 1)
	{
		if( priv->eos_flags & REOS )
			tms9914_set_holdoff_mode(priv, TMS9914_HOLDOFF_ALL);
		else
			tms9914_set_holdoff_mode(priv, TMS9914_HOLDOFF_EOI);
		// PIO transfer
		retval = pio_read(board, priv, buffer, length - 1, end, &num_bytes);
		*bytes_read += num_bytes;
		if(retval < 0)
			return retval;
		buffer += num_bytes;
		length -= num_bytes;
	}
	// read last bytes if we havn't received an END yet
	if(*end == 0)
	{
		// make sure we holdoff after last byte read
		tms9914_set_holdoff_mode(priv, TMS9914_HOLDOFF_ALL);
		retval = pio_read(board, priv, buffer, length, end, &num_bytes);
		*bytes_read += num_bytes;
		if(retval < 0)
			return retval;
	}
	return 0;
}

EXPORT_SYMBOL(tms9914_read);






