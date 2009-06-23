/***************************************************************************
                                    tms9914/cmd.c
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
#include <linux/delay.h>

void check_my_address_state( gpib_board_t *board, tms9914_private_t *priv, int cmd_byte )
{
	if( cmd_byte == MLA( board->pad ) )
	{
		priv->primary_listen_addressed = 1;
		// become active listener
		if( board->sad < 0 )
			write_byte(priv, AUX_LON | AUX_CS, AUXCR);
	}else if( board->sad >= 0 && priv->primary_listen_addressed &&
		cmd_byte == MSA( board->sad ) )
	{
		// become active listener
		write_byte(priv, AUX_LON | AUX_CS, AUXCR);
	}else if( cmd_byte != MLA( board->pad ) && ( cmd_byte & 0xf0 ) == LAD )
	{
		priv->primary_listen_addressed = 0;
	}else if( cmd_byte == UNL )
	{
		priv->primary_listen_addressed = 0;
		write_byte(priv, AUX_LON, AUXCR);
	}else if( cmd_byte == MTA( board->pad ) )
	{
		priv->primary_talk_addressed = 1;
		if( board->sad < 0 )
			//make active talker
			write_byte(priv, AUX_TON | AUX_CS, AUXCR);
	}else if( board->sad >= 0 && priv->primary_talk_addressed &&
		cmd_byte == MSA( board->sad ) )
	{
		// become active talker
		write_byte(priv, AUX_TON | AUX_CS, AUXCR);
	}else if( cmd_byte != MTA( board->pad ) && ( cmd_byte & 0xf0 ) == TAD )
	{
		priv->primary_talk_addressed = 0;
	}else if( cmd_byte == UNT )
	{
		priv->primary_talk_addressed = 0;
		write_byte(priv, AUX_TON, AUXCR);
	}
}

ssize_t tms9914_command(gpib_board_t *board, tms9914_private_t *priv, uint8_t *buffer, size_t length)
{
	size_t count = 0;
	ssize_t retval = 0;
	unsigned long flags;

	while(count < length)
	{
		if(wait_event_interruptible(board->wait, test_bit(COMMAND_READY_BN,
			&priv->state) || test_bit(TIMO_NUM, &board->status)))
		{
			printk("gpib command wait interrupted\n");
			break;
		}
		if( test_bit( TIMO_NUM, &board->status ) ) break;

		spin_lock_irqsave(&board->spinlock, flags);
		clear_bit(COMMAND_READY_BN, &priv->state);
		write_byte(priv, buffer[count], CDOR);
		spin_unlock_irqrestore(&board->spinlock, flags);

		check_my_address_state( board, priv, buffer[ count ] );

		count++;
	}
	// wait until last command byte is written
	if(wait_event_interruptible(board->wait, test_bit(COMMAND_READY_BN,
		&priv->state) || test_bit(TIMO_NUM, &board->status)))
	{
		retval = -ERESTARTSYS;
	}
	if(test_bit(TIMO_NUM, &board->status))
	{
		retval = -ETIMEDOUT;
	}

	return count ? count : retval;
}

EXPORT_SYMBOL(tms9914_command);








