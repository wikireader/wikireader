/***************************************************************************
                              sys/ibrpp.c
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
 * IBRPP
 * Conduct a parallel poll and return the byte in buf.
 *
 * NOTE:
 *      1.  Prior to conducting the poll the interface is placed
 *          in the controller active state.
 */
int ibrpp(gpib_board_t *board, uint8_t *result )
{
	int retval = 0;

	osStartTimer( board, board->usec_timeout );
	retval = ibcac( board, 0 );
	if( retval ) return -1;

	if(board->interface->parallel_poll( board, result ) )
	{
		printk("gpib: parallel poll failed\n");
		retval = -1;
	}
	osRemoveTimer(board);
	return retval;
}

int ibppc( gpib_board_t *board, uint8_t configuration )
{

	configuration &= 0x1f;
	board->interface->parallel_poll_configure( board, configuration );
	board->parallel_poll_configuration = configuration;
	
	return 0;
}







