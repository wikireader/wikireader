/***************************************************************************
                          lib/ibTrg.c
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

int my_trigger( ibConf_t *conf, const Addr4882_t addressList[] )
{
	int i, retval;
	uint8_t *cmd;

	if( addressListIsValid( addressList ) == 0 )
	{
		setIberr( EARG );
		return -1;
	}

	cmd = malloc( 16 + 2 * numAddresses( addressList ) );
	if( cmd == NULL )
	{
		setIberr( EDVR );
		setIbcnt( ENOMEM );
		return -1;
	}

	i = create_send_setup( interfaceBoard( conf ), addressList, cmd );
	cmd[ i++ ] = GET;

	retval = my_ibcmd( conf, cmd, i );

	free( cmd );
	cmd = NULL;

	if( retval != i )
	{
		return -1;
	}

	return 0;
}

int ibtrg( int ud )
{
	ibConf_t *conf;
	int retval;
	Addr4882_t addressList[ 2 ];

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	if( conf->is_interface )
	{
		setIberr( EARG );
		return exit_library( ud, 1 );
	}

	addressList[ 0 ] = packAddress( conf->settings.pad, conf->settings.sad );
	addressList[ 1 ] = NOADDR;

	retval = my_trigger( conf, addressList );
	if( retval < 0 )
	{
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}

void TriggerList( int boardID, const Addr4882_t addressList[] )
{
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

	retval = my_trigger( conf, addressList );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}

void Trigger( int boardID, Addr4882_t address )
{
	Addr4882_t addressList[ 2 ];

	addressList[ 0 ] = address;
	addressList[ 1 ] = NOADDR;

	TriggerList( boardID, addressList );
}
