/***************************************************************************
                          lib/ibRsv.c
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

int internal_ibrsv( ibConf_t *conf, int v )
{
	ibBoard_t *board;
	uint8_t status_byte = v;
	int retval;
	
	if( conf->is_interface == 0 )
	{
		setIberr( EARG );
		return -1;
	}

	board = interfaceBoard( conf );

	retval = ioctl( board->fileno, IBRSV, &status_byte );
	if( retval < 0 )
	{
		return retval;
	}

	return 0;
}

// should return old status byte in iberr on success
int ibrsv( int ud, int v )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	retval = internal_ibrsv( conf, v );
	if( retval < 0 )
	{
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}

