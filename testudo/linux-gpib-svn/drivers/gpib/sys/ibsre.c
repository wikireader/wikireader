/***************************************************************************
                              ibsre.c
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
 * IBSRE
 * Send REN true if v is non-zero or false if v is zero.
 */
int ibsre( gpib_board_t *board, int enable )
{
	if(	board->master == 0 )
	{
		printk( "gpib: tried to set REN when not system controller\n" );
		return -1;
	}
	
	board->interface->remote_enable( board, enable );	/* set or clear REN */
	if( !enable )
		udelay(100);

	return 0;
}

