/***************************************************************************
                          lib/ibRpp.c
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

int internal_ibrpp( ibConf_t *conf, char *result )
{
	uint8_t poll_byte;
	ibBoard_t *board;
	int retval;
	
	board = interfaceBoard( conf );

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return -1;
	}

	set_timeout( board, conf->settings.ppoll_usec_timeout );

	retval = ioctl( board->fileno, IBRPP, &poll_byte );
	if( retval < 0 )
	{
		switch( errno )
		{
			case ETIMEDOUT:
				conf->timed_out = 1;
				break;
			default:
				setIberr( EDVR );
				setIbcnt( errno );
				break;
		}
		return -1;
	}

	*result = poll_byte;

	return 0;
}

int ibrpp( int ud, char *ppr )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	retval = internal_ibrpp( conf, ppr );
	if( retval < 0 )
	{
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}

void PPoll( int boardID, short *result )
{
	char byte_result;
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	if( conf->is_interface == 0 )
	{
		setIberr( EDVR );
		exit_library( boardID, 1 );
		return;
	}

	retval = internal_ibrpp( conf, &byte_result );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	*result = byte_result & 0xff;

	exit_library( boardID, 0 );
}

