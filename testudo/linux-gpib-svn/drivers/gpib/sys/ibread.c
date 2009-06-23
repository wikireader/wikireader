/***************************************************************************
                              ibread.c
                             -------------------

    begin                : Dec 2001
    copyright            : (C) 2001, 2002, 2004 by Frank Mori Hess
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
 * IBRD
 * Read up to 'length' bytes of data from the GPIB into buf.  End
 * on detection of END (EOI and or EOS) and set 'end_flag'.
 *
 * NOTE:
 *      1.  The interface is placed in the controller standby
 *          state prior to beginning the read.
 *      2.  Prior to calling ibrd, the intended devices as well
 *          as the interface board itself must be addressed by
 *          calling ibcmd.
 */

int ibrd(gpib_board_t *board, uint8_t *buf, size_t length, int *end_flag, size_t *nbytes)
{
	ssize_t ret = 0;
	int retval;
	size_t bytes_read;

	*nbytes = 0;
	*end_flag = 0;	
	if( length == 0 )
	{
		printk( "gpib: ibrd() called with zero length?\n");
		return 0;
	}

	if( board->master )
	{
		retval = ibgts( board );
		if( retval < 0 ) return retval;
	}
	/* XXX reseting timer here could cause timeouts take longer than they should,
	 * since read_ioctl calls this
	 * function in a loop, there is probably a similar problem with writes/commands */
	osStartTimer( board, board->usec_timeout );

	do
	{
		ret = board->interface->read(board, buf, length - *nbytes, end_flag, &bytes_read);
		if(ret < 0)
		{
/*			printk("gpib read error\n");*/
		}
		buf += bytes_read;
		*nbytes += bytes_read;
		if(need_resched())
		{
			schedule();
		}
	}while(ret == 0 && *nbytes > 0 && *nbytes < length && *end_flag == 0);

	osRemoveTimer(board);

	return ret;
}

