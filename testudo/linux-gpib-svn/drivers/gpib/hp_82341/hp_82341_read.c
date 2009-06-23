/***************************************************************************
                          hp_82341/hp_82341_read.c  -  description
                             -------------------

    copyright            : (C) 2002, 2004, 2005 by Frank Mori Hess
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

#include "hp_82341.h"
#include <linux/sched.h>

int hp_82341_accel_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	hp_82341_private_t *hp_priv = board->private_data;
	tms9914_private_t *tms_priv = &hp_priv->tms9914_priv;
	int retval = 0;
	unsigned short event_status;
	int i;
	int num_fifo_bytes;
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
	outb(DIRECTION_GPIB_TO_HOST_BIT, hp_priv->iobase[3] + BUFFER_CONTROL_REG);
	// Handle corner case of board not in holdoff and one byte has slipped in already.
	// Also, board sometimes has problems (spurious 1 byte reads) when read fifo is 
	// started up with board in
	// TACS under certain data holdoff conditions.  Doing a 1 byte tms9914-style
	// read avoids these problems.
	if(/*tms_priv->holdoff_active == 0 && */length > 1)
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
	outb(0x00, hp_priv->iobase[3] + BUFFER_FLUSH_REG);
	i = 0;
	num_fifo_bytes = length - 1;
	while(i < num_fifo_bytes && *end == 0)
	{
		int block_size;
		int j;
		int count;
		if(num_fifo_bytes - i < hp_82341_fifo_size)
			block_size = num_fifo_bytes - i;
		else
			block_size = hp_82341_fifo_size;
		set_transfer_counter(hp_priv, block_size);
		outb(ENABLE_TI_BUFFER_BIT | DIRECTION_GPIB_TO_HOST_BIT, hp_priv->iobase[3] + BUFFER_CONTROL_REG);
		if(inb(hp_priv->iobase[0] + STREAM_STATUS_REG) & HALTED_STATUS_BIT) 
			outb(RESTART_STREAM_BIT, hp_priv->iobase[0] + STREAM_STATUS_REG); 
		clear_bit(READ_READY_BN, &tms_priv->state);
		if(wait_event_interruptible(board->wait, 
			((event_status = read_and_clear_event_status(board)) & (TERMINAL_COUNT_EVENT_BIT | BUFFER_END_EVENT_BIT)) ||
			test_bit(DEV_CLEAR_BN, &tms_priv->state) ||
			test_bit(TIMO_NUM, &board->status)))
		{
			printk("%s: read wait interrupted\n", __FILE__);
			retval = -ERESTARTSYS;
			break;
		}
		// have to disable buffer before we can read from buffer port
		outb(DIRECTION_GPIB_TO_HOST_BIT, hp_priv->iobase[3] + BUFFER_CONTROL_REG);
		count = block_size - read_transfer_counter(hp_priv);
		j = 0;
		while(j < count && i < num_fifo_bytes)
		{
			unsigned short data_word = inw(hp_priv->iobase[3] + BUFFER_PORT_LOW_REG);
			buffer[i++] = data_word & 0xff;
			++j;
			if(j < count && i < num_fifo_bytes)
			{
				buffer[i++] = (data_word >> 8) & 0xff;
				++j;
			}
		}
		if(event_status & BUFFER_END_EVENT_BIT)
		{
			clear_bit(RECEIVED_END_BN, &tms_priv->state);
			*end = 1;
			tms_priv->holdoff_active = 1;
		}
		if(test_bit(TIMO_NUM, &board->status))
		{
			printk("%s: minor %i: read timed out\n", __FILE__, board->minor);
			retval = -ETIMEDOUT;
			break;
		}
		if(test_bit(DEV_CLEAR_BN, &tms_priv->state))
		{
			printk("%s: device clear interrupted read\n", __FILE__);
			retval = -EINTR;
			break;
		}
	}
	*bytes_read += i;
	buffer += i;
	length -= i;
	if(retval < 0) return retval;
	// read last byte if we havn't received an END yet
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
