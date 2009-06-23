/***************************************************************************
                          lib/ibCac.c
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

int ibcac( int ud, int synchronous )
{
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	if( conf->is_interface == 0 )
	{
		setIberr( EARG );
		return exit_library( ud, 1 );
	}

	board = interfaceBoard( conf );

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return exit_library( ud, 1 );
	}

	retval = ioctl( board->fileno, IBCAC, &synchronous );
	// if synchronous failed, fall back to asynchronous
	if( retval < 0 && synchronous  )
	{
		synchronous = 0;
		retval = ioctl( board->fileno, IBCAC, &synchronous );
	}
	if(retval < 0)
	{
		switch( errno )
		{
			default:
				setIberr( EDVR );
				break;
		}
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}
