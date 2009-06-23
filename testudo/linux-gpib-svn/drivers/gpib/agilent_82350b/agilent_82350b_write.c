/***************************************************************************
                          agilent_82350b/agilent_82350b_write.c  -  description
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

static int translate_wait_return_value(gpib_board_t *board, int retval)
{
	agilent_82350b_private_t *a_priv = board->private_data;
	tms9914_private_t *tms_priv = &a_priv->tms9914_priv;

	if(retval)
	{
		printk("%s: write wait interrupted\n", __FILE__);
		return -ERESTARTSYS;
	}
	if(test_bit(TIMO_NUM, &board->status))
	{
		printk("%s: minor %i: write timed out\n", __FILE__, board->minor);
		return -ETIMEDOUT;
	}
	if(test_bit(DEV_CLEAR_BN, &tms_priv->state))
	{
		printk("%s: device clear interrupted write\n", __FILE__);
		return -EINTR;
	}
	return 0;
}

int agilent_82350b_accel_write( gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	agilent_82350b_private_t *a_priv = board->private_data;
	tms9914_private_t *tms_priv = &a_priv->tms9914_priv;
	int i, j;
	unsigned short event_status;
	int retval = 0;
	int fifoTransferLength = length;
	int block_size = 0;
	size_t num_bytes;
	*bytes_written = 0;
	if(send_eoi)
	{
		--fifoTransferLength;
	}
	clear_bit(DEV_CLEAR_BN, &tms_priv->state);
	read_and_clear_event_status(board);
	writeb(0, a_priv->gpib_base + SRAM_ACCESS_CONTROL_REG);

 // wait for previous BO to complete if any
	retval = wait_event_interruptible(board->wait,
					  test_bit(DEV_CLEAR_BN, &tms_priv->state) ||
					  test_bit(WRITE_READY_BN, &tms_priv->state) ||
					  test_bit(TIMO_NUM, &board->status));
	retval = translate_wait_return_value(board, retval);

	if(retval) return retval;

	for(i = 0; i < fifoTransferLength;)
	{
		clear_bit(WRITE_READY_BN, &tms_priv->state);

		if(fifoTransferLength - i < agilent_82350b_fifo_size)
			block_size = fifoTransferLength - i;
		else
			block_size = agilent_82350b_fifo_size;
		set_transfer_counter(a_priv, block_size);
		for(j = 0; j < block_size; ++j, ++i)
		{
			// load data into board's sram
			writeb(buffer[i], a_priv->sram_base + j);
		}
		writeb(ENABLE_TI_TO_SRAM, a_priv->gpib_base + SRAM_ACCESS_CONTROL_REG);
		if(agilent_82350b_fifo_is_halted(a_priv))
			writeb(RESTART_STREAM_BIT, a_priv->gpib_base + STREAM_STATUS_REG);
		retval = wait_event_interruptible(board->wait,
			((event_status = read_and_clear_event_status(board)) & TERM_COUNT_STATUS_BIT) ||
			test_bit(DEV_CLEAR_BN, &tms_priv->state) ||
			test_bit(TIMO_NUM, &board->status));
		writeb(0, a_priv->gpib_base + SRAM_ACCESS_CONTROL_REG);
		num_bytes = block_size - read_transfer_counter(a_priv);
		*bytes_written += num_bytes;
		retval = translate_wait_return_value(board, retval);
		if(retval) break;
	}
	if(retval) return retval;

	if(send_eoi)
	{
		retval = agilent_82350b_write(board, buffer + fifoTransferLength, 1, 1, &num_bytes);
		*bytes_written += num_bytes;
		if(retval < 0) return retval;
	}
	return 0;
}



