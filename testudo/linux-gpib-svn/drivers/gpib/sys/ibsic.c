/***************************************************************************
                              ibsic.c
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
#include <linux/delay.h>

/*
 * IBSIC
 * Send IFC for at least 100 microseconds.
 *
 * NOTE:
 *      1.  Ibsic must be called prior to the first call to
 *          ibcmd in order to initialize the bus and enable the
 *          interface to leave the controller idle state.
 */
int ibsic( gpib_board_t *board, unsigned int usec_duration )
{
	if( board->master == 0 )
	{
		printk( "gpib: tried to assert IFC when not system controller\n" );
		return -1;
	}

	if( usec_duration < 100 ) usec_duration = 100;
	if( usec_duration > 1000 )
	{
		usec_duration = 1000;
		printk( "gpib: warning, shortening long udelay\n");
	}

	GPIB_DPRINTK( "sending interface clear\n" );
	board->interface->interface_clear(board, 1);
	udelay( usec_duration );
	board->interface->interface_clear(board, 0);

	return 0;
}

void ibrsc( gpib_board_t *board, int request_control )
{
	board->master = request_control != 0;
	if( board->interface->request_system_control == NULL )
	{
		printk( "gpib: bug! driver does not implement request_system_control()\n" );
		return;
	}
	board->interface->request_system_control( board, request_control );
}

