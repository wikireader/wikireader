/***************************************************************************
                          lib/ibEvent.c
                             -------------------

    copyright            : (C) 2001,2002,2003 by Frank Mori Hess
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

#include "ib_internal.h"

int ibevent(int ud, short *event )
{
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;
	event_ioctl_t user_event;

	conf = general_enter_library( ud, 1, 1 );
	if( conf == NULL )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	if( conf->is_interface == 0 )
	{
		setIberr( EARG );
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );
	}

	board = interfaceBoard( conf );

	retval = ioctl( board->fileno, IBEVENT, &user_event );
	if( retval < 0 )
	{
		switch( errno )
		{
			case EPIPE:
				setIberr( ETAB );
				break;
			default:
				setIberr( EDVR );
				setIbcnt( errno );
				break;
		}
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );
	}

	*event = user_event;

	return general_exit_library( ud, 0, 0, 0, 0, 0, 1 );
}

