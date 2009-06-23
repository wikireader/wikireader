/***************************************************************************
                              sys/ibcmd.c
                             -------------------

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

#include "gpibP.h"

/*
 * IBCMD
 * Write cnt command bytes from buf to the GPIB.  The
 * command operation terminates only on I/O complete.
 *
 * NOTE:
 *      1.  Prior to beginning the command, the interface is
 *          placed in the controller active state.
 *      2.  Before calling ibcmd for the first time, ibsic
 *          must be called to initialize the GPIB and enable
 *          the interface to leave the controller idle state.
 */
ssize_t ibcmd( gpib_board_t *board, uint8_t *buf, size_t length )
{
	size_t	count = 0;
	ssize_t ret = 0;
	int status = ibstatus( board );

	if(length == 0) return 0;

	if((status & CIC) == 0)
	{
		printk("gpib: cannot send command when not controller-in-charge\n");
		return -1;
	}

	osStartTimer( board, board->usec_timeout );

	ret = ibcac( board, 0 );
	if( ret == 0 )
	{
		ret = board->interface->command(board, buf, length - count);
		if(ret < 0)
		{
			printk("gpib: error writing gpib command bytes\n");
		}else
		{
			buf += ret;
			count += ret;
		}
	}

	osRemoveTimer(board);

	if( io_timed_out( board ) )
		ret = -ETIMEDOUT;

	return ret ? ret : count;
}


