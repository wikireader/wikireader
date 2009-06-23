/***************************************************************************
                             cb7210_read.c
                             -------------------

    copyright            : (C) 2003 by Frank Mori Hess
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
#include <linux/delay.h>

static inline int have_fifo_word(const cb7210_private_t *cb_priv)
{
	if(((cb7210_read_byte(cb_priv, HS_STATUS)) &
			(HS_RX_MSB_NOT_EMPTY | HS_RX_LSB_NOT_EMPTY)) ==
			(HS_RX_MSB_NOT_EMPTY | HS_RX_LSB_NOT_EMPTY))
		return 1;
	else
		return 0;
}

static inline void input_fifo_enable( gpib_board_t *board, int enable )
{
	cb7210_private_t *cb_priv = board->private_data;
	nec7210_private_t *nec_priv = &cb_priv->nec7210_priv;
	unsigned long flags;

	spin_lock_irqsave( &board->spinlock, flags );

	if( enable )
	{
		cb_priv->in_fifo_half_full = 0;
		nec7210_set_reg_bits( nec_priv, IMR2, HR_DMAI, 0 );

		cb7210_write_byte(cb_priv, HS_RX_ENABLE | HS_TX_ENABLE | HS_CLR_SRQ_INT |
			HS_CLR_EOI_EMPTY_INT | HS_CLR_HF_INT | cb_priv->hs_mode_bits,
			HS_MODE);

		cb_priv->hs_mode_bits &= ~HS_ENABLE_MASK;
		cb7210_write_byte(cb_priv, cb_priv->hs_mode_bits, HS_MODE);

		cb7210_write_byte(cb_priv, irq_bits( cb_priv->irq ), HS_INT_LEVEL);

		cb_priv->hs_mode_bits |= HS_RX_ENABLE;
		cb7210_write_byte(cb_priv, cb_priv->hs_mode_bits, HS_MODE);
	}else
	{
		nec7210_set_reg_bits( nec_priv, IMR2, HR_DMAI, 0 );

		cb_priv->hs_mode_bits &= ~HS_ENABLE_MASK;
		cb7210_write_byte(cb_priv, cb_priv->hs_mode_bits, nec7210_iobase(cb_priv) + HS_MODE);

		clear_bit( READ_READY_BN, &nec_priv->state );
	}

	spin_unlock_irqrestore( &board->spinlock, flags );
}

static int fifo_read(gpib_board_t *board, cb7210_private_t *cb_priv, uint8_t *buffer,
	size_t length, int *end, size_t *bytes_read)
{
	ssize_t retval = 0;
	nec7210_private_t *nec_priv = &cb_priv->nec7210_priv;
	int hs_status;
	uint16_t word;
	unsigned long flags;
	
	*bytes_read = 0;
	if(cb_priv->fifo_iobase == 0)
	{
		printk("cb7210: fifo iobase is zero!\n");
		return -EIO;
	}
	*end = 0;
	if( length <= cb7210_fifo_size )
	{
		printk("cb7210: bug! fifo_read() with length < fifo size\n" );
		return -EINVAL;
	}

	input_fifo_enable( board, 1 );

	while( *bytes_read + cb7210_fifo_size < length )
	{
		nec7210_set_reg_bits( nec_priv, IMR2, HR_DMAI, HR_DMAI );

		if(wait_event_interruptible(board->wait,
			(cb_priv->in_fifo_half_full && have_fifo_word(cb_priv)) ||
			test_bit(RECEIVED_END_BN, &nec_priv->state) ||
			test_bit(DEV_CLEAR_BN, &nec_priv->state) ||
			test_bit(TIMO_NUM, &board->status)))
		{
			printk("cb7210: fifo half full wait interrupted\n");
			retval = -ERESTARTSYS;
			nec7210_set_reg_bits( nec_priv, IMR2, HR_DMAI, 0 );
			break;
		}

		spin_lock_irqsave( &board->spinlock, flags );

		nec7210_set_reg_bits( nec_priv, IMR2, HR_DMAI, 0 );

		while(have_fifo_word(cb_priv))
		{
			word = inw(cb_priv->fifo_iobase + DIR );
			buffer[ (*bytes_read)++ ] = word & 0xff;
			buffer[ (*bytes_read)++ ] = ( word >> 8 ) & 0xff;
		}

		cb_priv->in_fifo_half_full = 0;

		hs_status = cb7210_read_byte(cb_priv, HS_STATUS);

		spin_unlock_irqrestore( &board->spinlock, flags );

		if( test_and_clear_bit( RECEIVED_END_BN, &nec_priv->state ) )
		{
			*end = 1;
			break;
		}
		if( hs_status & HS_FIFO_FULL )
			break;
		if( test_bit( TIMO_NUM, &board->status ) )
		{
			retval = -ETIMEDOUT;
			break;
		}
		if( test_bit( DEV_CLEAR_BN, &nec_priv->state ) )
		{
			retval = -EINTR;
			break;
		}
	}
	hs_status = cb7210_read_byte(cb_priv, HS_STATUS);
	if( hs_status & HS_RX_LSB_NOT_EMPTY )
	{
		word = inw(cb_priv->fifo_iobase + DIR );
		buffer[ (*bytes_read)++ ] = word & 0xff;
	}

	input_fifo_enable( board, 0 );

	if( wait_event_interruptible( board->wait,
		test_bit( READ_READY_BN, &nec_priv->state ) ||
		test_bit( RECEIVED_END_BN, &nec_priv->state ) ||
		test_bit( DEV_CLEAR_BN, &nec_priv->state ) ||
		test_bit( TIMO_NUM, &board->status ) ) )
	{
		printk("cb7210: fifo half full wait interrupted\n");
		retval = -ERESTARTSYS;
	}
	if( test_bit( TIMO_NUM, &board->status ) )
	{
		retval = -ETIMEDOUT;
	}
	if( test_bit( DEV_CLEAR_BN, &nec_priv->state ) )
	{
		retval = -EINTR;
	}
	if( test_bit( READ_READY_BN, &nec_priv->state ) )
	{
		nec7210_set_handshake_mode( board, nec_priv, HR_HLDA );
		buffer[ (*bytes_read)++ ] = nec7210_read_data_in( board, nec_priv, end );
	}

	return retval;
}

int cb7210_accel_read( gpib_board_t *board, uint8_t *buffer,
	size_t length, int *end, size_t *bytes_read)
{
	ssize_t retval;
	cb7210_private_t *cb_priv = board->private_data;
	nec7210_private_t *nec_priv = &cb_priv->nec7210_priv;
	size_t num_bytes;
	
	*bytes_read = 0;
	// deal with limitations of fifo
	if( length < cb7210_fifo_size + 3 || ( nec_priv->auxa_bits & HR_REOS ) )
	{
		return cb7210_read(board, buffer, length, end, bytes_read);
	}
	*end = 0;

	nec7210_set_handshake_mode( board, nec_priv, HR_HLDA );
	nec7210_release_rfd_holdoff( board, nec_priv);

	if( wait_event_interruptible( board->wait,
		test_bit( READ_READY_BN, &nec_priv->state ) ||
		test_bit( DEV_CLEAR_BN, &nec_priv->state ) ||
		test_bit( TIMO_NUM, &board->status ) ) )
	{
		printk("cb7210: read ready wait interrupted\n");
		return -ERESTARTSYS;
	}
	if( test_bit( TIMO_NUM, &board->status ) )
		return -ETIMEDOUT;
	if( test_bit( DEV_CLEAR_BN, &nec_priv->state ) )
		return -EINTR;

	buffer[ (*bytes_read)++ ] = nec7210_read_data_in( board, nec_priv, end );
	if( *end ) return 0;

	nec7210_set_handshake_mode( board, nec_priv, HR_HLDE );
	nec7210_release_rfd_holdoff( board, nec_priv );

	retval = fifo_read(board, cb_priv, &buffer[*bytes_read], length - *bytes_read - 1, end, &num_bytes);
	*bytes_read += num_bytes;
	if( retval < 0 )
		return retval;
	if( *end ) return 0;

	retval = cb7210_read(board, &buffer[*bytes_read], 1, end, &num_bytes);
	*bytes_read += num_bytes;
	if( retval < 0 ) return retval;

	return 0;
}






