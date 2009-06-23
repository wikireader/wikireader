/***************************************************************************
                          agilent_82350b/agilent_82350b_read.c  -  description
                             -------------------

    copyright            : (C) 2002, 2004 by Frank Mori Hess
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

#include "agilent_82350b.h"
#include <linux/sched.h>
#include <linux/wait.h>

int agilent_82350b_accel_read( gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	agilent_82350b_private_t *a_priv = board->private_data;
	tms9914_private_t *tms_priv = &a_priv->tms9914_priv;
	int retval = 0;
	unsigned short event_status;
	int i, num_fifo_bytes;
	//hardware doesn't support checking for end-of-string character when using fifo
	if(tms_priv->eos_flags & REOS) 
	{
		return tms9914_read( board, tms_priv, buffer, length, end, bytes_read);
	}
	clear_bit( DEV_CLEAR_BN, &tms_priv->state );
	read_and_clear_event_status(board);
	*end = 0;
	*bytes_read = 0;
	if(length == 0) return 0;
	//disable fifo for the moment
	writeb(DIRECTION_GPIB_TO_HOST, a_priv->gpib_base + SRAM_ACCESS_CONTROL_REG);
	// handle corner case of board not in holdoff and one byte might slip in early
	if(tms_priv->holdoff_active == 0 && length > 1)
	{
		size_t num_bytes;
		retval = tms9914_read(board, tms_priv, buffer, 1, end, &num_bytes);
		*bytes_read += num_bytes;
		if(retval < 0)
			printk("tms9914_read failed retval=%i\n", retval);
		if(retval < 0 || *end)
		{
			return retval;
		}
		++buffer;
		--length;
	}
	tms9914_set_holdoff_mode(tms_priv, TMS9914_HOLDOFF_EOI);
	tms9914_release_holdoff(tms_priv);
	i = 0;
	num_fifo_bytes = length - 1;
	while(i < num_fifo_bytes && *end == 0)
	{
		int block_size;
		int j;
		int count;
		if(num_fifo_bytes - i < agilent_82350b_fifo_size)
			block_size = num_fifo_bytes - i;
		else
			block_size = agilent_82350b_fifo_size;
		set_transfer_counter(a_priv, block_size);
		writeb(ENABLE_TI_TO_SRAM | DIRECTION_GPIB_TO_HOST, a_priv->gpib_base + SRAM_ACCESS_CONTROL_REG); 
		if(agilent_82350b_fifo_is_halted(a_priv))
			writeb(RESTART_STREAM_BIT, a_priv->gpib_base + STREAM_STATUS_REG); 
		clear_bit(READ_READY_BN, &tms_priv->state);
		if(wait_event_interruptible(board->wait, 
			((event_status = read_and_clear_event_status(board)) & (TERM_COUNT_STATUS_BIT | BUFFER_END_STATUS_BIT)) ||
			test_bit(DEV_CLEAR_BN, &tms_priv->state) ||
			test_bit(TIMO_NUM, &board->status)))
		{
			printk("%s: write wait interrupted\n", __FILE__);
			retval = -ERESTARTSYS;
			break;
		}
		count = block_size - read_transfer_counter(a_priv);
		for(j = 0; j < count && i < num_fifo_bytes; ++j)
			buffer[i++] = readb(a_priv->sram_base + j); 
		if(event_status & BUFFER_END_STATUS_BIT)
		{
			clear_bit(RECEIVED_END_BN, &tms_priv->state);
			tms_priv->holdoff_active = 1;
			*end = 1;
		}
		if(test_bit(TIMO_NUM, &board->status))
		{
			printk("%s: minor %i: read timed out\n", __FILE__, board->minor);
			retval = -ETIMEDOUT;
			break;
		}
		if(test_bit(DEV_CLEAR_BN, &tms_priv->state))
		{
			printk("%s: device clear interrupted write\n", __FILE__);
			retval = -EINTR;
			break;
		}
	}
	*bytes_read += i;
	buffer += i;
	length -= i;
	writeb(DIRECTION_GPIB_TO_HOST, a_priv->gpib_base + SRAM_ACCESS_CONTROL_REG);
	if(retval < 0) return retval;
	// read last bytes if we havn't received an END yet
	if(*end == 0)
	{
		size_t num_bytes;
		// try to make sure we holdoff after last byte read
		retval = tms9914_read(board, tms_priv, buffer, length, end, &num_bytes);
		*bytes_read += num_bytes;
		if(retval < 0)
			return retval;
	}
	return 0;
}
