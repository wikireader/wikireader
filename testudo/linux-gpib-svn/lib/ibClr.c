/***************************************************************************
                          lib/ibClr.c
                             -------------------

    copyright            : (C) 2001,2002 by Frank Mori Hess
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
#include <stdlib.h>

int ibclr( int ud )
{
	uint8_t cmd[ 16 ];
	ibConf_t *conf;
	ibBoard_t *board;
	ssize_t count;
	int i;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	board = interfaceBoard( conf );

	if( conf->is_interface )
	{
		setIberr( EARG );
		return exit_library( ud, 1 );
	}

	i = send_setup_string( conf, cmd );
	cmd[ i++ ] = SDC;

	//XXX detect no listeners (EBUS) error
	count = my_ibcmd( conf, cmd, i );
	if(count != i)
	{
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}


int InternalDevClearList( ibConf_t *conf, const Addr4882_t addressList[] )
{
	int i;
	ibBoard_t *board;
	uint8_t *cmd;
	int count;

	if( addressListIsValid( addressList ) == 0 )
	{
		return -1;
	}

	if( conf->is_interface == 0 )
	{
		setIberr( EDVR );
		return -1;
	}

	board = interfaceBoard( conf );

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return -1;
	}

	cmd = malloc( 16 + 2 * numAddresses( addressList ) );
	if( cmd == NULL )
	{
		setIberr( EDVR );
		setIbcnt( ENOMEM );
		return -1;
	}

	i = 0;
	if( numAddresses( addressList ) )
	{
		i += create_send_setup( board, addressList, cmd );
		cmd[ i++ ] = SDC;
	}
	else
	{
		cmd[ i++ ] = DCL;
	}
	//XXX detect no listeners (EBUS) error
	count = my_ibcmd( conf, cmd, i );

	free( cmd );
	cmd = NULL;

	if(count != i)
	{
		return -1;
	}

	return 0;
}

void DevClearList( int boardID, const Addr4882_t addressList[] )
{
	int retval;
	ibConf_t *conf;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalDevClearList( conf, addressList );
	if( retval < 0 )
		exit_library( boardID, 1 );

	exit_library( boardID, 0 );
}

void DevClear( int boardID, Addr4882_t address )
{
	Addr4882_t addressList[2];

	addressList[0] = address;
	addressList[1] = NOADDR;

	DevClearList( boardID, addressList );
}
