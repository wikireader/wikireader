/***************************************************************************
                          lib/ibGts.c
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

int internal_ibgts( ibConf_t *conf, int shadow_handshake )
{
	ibBoard_t *board;
	int retval;

	board = interfaceBoard( conf );

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return -1;
	}

	retval = ioctl( board->fileno, IBGTS, &shadow_handshake );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return -1;
	}

	return 0;
}

// incomplete XXX need to implement acceptor handshake stuff in drivers
int ibgts( int ud, int v )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	if( conf->is_interface == 0 )
	{
		setIberr( EARG );
		return exit_library( ud, 1 );
	}

	retval = internal_ibgts( conf, v );
	if( retval < 0 )
	{
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}

