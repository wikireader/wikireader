/***************************************************************************
                              nec7210/read.c
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
#include <asm/dma.h>
#include <linux/spinlock.h>

static int pio_read( gpib_board_t *board, nec7210_private_t *priv, uint8_t *buffer,
	size_t length, int *end, size_t *bytes_read)
{
	ssize_t retval = 0;

	*bytes_read = 0;	
	*end = 0;

	while( *bytes_read < length )
	{
		if(wait_event_interruptible(board->wait,
			test_bit(READ_READY_BN, &priv->state) ||
			test_bit(DEV_CLEAR_BN, &priv->state) ||
			test_bit(TIMO_NUM, &board->status)))
		{
			printk("nec7210: pio read wait interrupted\n");
			retval = -ERESTARTSYS;
			break;
		}
		if( test_bit(READ_READY_BN, &priv->state) )
		{
			buffer[ (*bytes_read)++ ] = nec7210_read_data_in( board, priv, end );
			if( *end )
				break;
		}
		if( test_bit( TIMO_NUM, &board->status ) )
		{
			GPIB_DPRINTK("interrupted by timeout\n");
			retval = -ETIMEDOUT;
			break;
		}
		if( test_bit( DEV_CLEAR_BN, &priv->state) )
		{
			GPIB_DPRINTK("interrupted by device clear\n");
			retval = -EINTR;
			break;
		}

		if(*bytes_read < length)
			nec7210_release_rfd_holdoff( board, priv );

		if(need_resched())
			schedule();
	}
	return retval;
}
#if 0
static ssize_t __dma_read(gpib_board_t *board, nec7210_private_t *priv, size_t length)
{
	ssize_t retval = 0;
	size_t count = 0;
	unsigned long flags, dma_irq_flags;

	if(length == 0)
		return 0;

	spin_lock_irqsave(&board->spinlock, flags);

	dma_irq_flags = claim_dma_lock();
	disable_dma(priv->dma_channel);
	/* program dma controller */
	clear_dma_ff(priv->dma_channel);
	set_dma_count(priv->dma_channel, length);
	set_dma_addr (priv->dma_channel, priv->dma_buffer_addr);
	set_dma_mode(priv->dma_channel, DMA_MODE_READ);
	release_dma_lock(dma_irq_flags);

	enable_dma(priv->dma_channel);

	set_bit(DMA_READ_IN_PROGRESS_BN, &priv->state);
	clear_bit(READ_READY_BN, &priv->state);

	// enable nec7210 dma
	nec7210_set_reg_bits( priv, IMR2, HR_DMAI, HR_DMAI );

	spin_unlock_irqrestore(&board->spinlock, flags);

	// wait for data to transfer
	if(wait_event_interruptible(board->wait,
		test_bit( DMA_READ_IN_PROGRESS_BN, &priv->state ) == 0 ||
		test_bit( DEV_CLEAR_BN, &priv->state ) ||
		test_bit( TIMO_NUM, &board->status ) ) )
	{
		printk("nec7210: dma read wait interrupted\n");
		retval = -ERESTARTSYS;
	}
	if( test_bit( TIMO_NUM, &board->status ) )
		retval = -ETIMEDOUT;
	if( test_bit( DEV_CLEAR_BN, &priv->state ) )
		retval = -EINTR;

	// disable nec7210 dma
	nec7210_set_reg_bits( priv, IMR2, HR_DMAI, 0 );

	// record how many bytes we transferred
	flags = claim_dma_lock();
	clear_dma_ff(priv->dma_channel);
	disable_dma(priv->dma_channel);
	count += length - get_dma_residue(priv->dma_channel);
	release_dma_lock(flags);

	return retval ? retval : count;
}

static ssize_t dma_read(gpib_board_t *board, nec7210_private_t *priv, uint8_t *buffer, size_t length)
{
	size_t remain = length;
	size_t transfer_size;
	ssize_t retval = 0;

	while(remain > 0)
	{
		transfer_size = (priv->dma_buffer_length < remain) ? priv->dma_buffer_length : remain;
		retval = __dma_read(board, priv, transfer_size);
		if(retval < 0) break;
		memcpy(buffer, priv->dma_buffer, transfer_size);
		remain -= retval;
		buffer += retval;
		if(test_bit(RECEIVED_END_BN, &priv->state)) break;
	}

	if(retval < 0) return retval;

	return length - remain;
}
#endif
int nec7210_read(gpib_board_t *board, nec7210_private_t *priv, uint8_t *buffer,
	size_t length, int *end, size_t *bytes_read)
{
	ssize_t retval = 0;
	
	*end = 0;
	*bytes_read = 0;
	
	if( length == 0 ) return 0;

	clear_bit( DEV_CLEAR_BN, &priv->state ); // XXX wrong

	nec7210_set_handshake_mode( board, priv, HR_HLDA );
	nec7210_release_rfd_holdoff( board, priv );

	retval = pio_read(board, priv, buffer, length, end, bytes_read);
	
	return retval;
}

EXPORT_SYMBOL(nec7210_read);






