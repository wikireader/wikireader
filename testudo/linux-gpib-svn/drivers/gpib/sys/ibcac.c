/***************************************************************************
                               sys/ibcac.c
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
 * IBCAC
 * Return to the controller active state from the
 * controller standby state, i.e., turn ATN on.  Note
 * that in order to enter the controller active state
 * from the controller idle state, ibsic must be called.
 * If v is non-zero, take control synchronously, if
 * possible.  Otherwise, take control asynchronously.
 */
int ibcac( gpib_board_t *board, int sync )
{
	int status = ibstatus( board );
	int retval;

	if( ( status & CIC ) == 0 )
	{
		printk("gpib: not CIC during ibcac()\n");
		return -1;
	}

	retval = board->interface->take_control( board, sync );
	if( retval < 0 )
		printk("gpib: error while becoming active controller\n");

	board->interface->update_status( board, 0 );

	return retval;
}




