/***************************************************************************
                          lib/ibSre.c
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
#include <stdlib.h>

int remote_enable( const ibBoard_t *board, int enable )
{
	int retval;

	if( is_system_controller( board ) == 0 )
	{
		setIberr( ESAC );
		return -1;
	}

	retval = ioctl( board->fileno, IBSRE, &enable );
	if( retval < 0 )
	{
		// XXX other error types?
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	return 0;
}

int internal_ibsre( ibConf_t *conf, int v )
{
	ibBoard_t *board;
	int retval;

	board = interfaceBoard( conf );

	if( conf->is_interface == 0 )
	{
		setIberr( EARG );
		return -1;
	}

	retval = remote_enable( board, v );
	if( retval < 0 )
		return retval;

	return 0;
}

int ibsre(int ud, int v)
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	retval = internal_ibsre( conf, v );
	if( retval < 0 )
	{
		fprintf( stderr, "libgpib: ibsre error\n");
		return exit_library( ud, 1 );
	}
//XXX supposed to set iberr to old REN setting
	return exit_library( ud, 0 );
}

int InternalEnableRemote( ibConf_t *conf, const Addr4882_t addressList[] )
{
	int i;
	ibBoard_t *board;
	uint8_t *cmd;
	int count;
	int retval;

	if( addressListIsValid( addressList ) == 0 )
		return -1;

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

	retval = remote_enable( board, 1 );
	if( retval < 0 ) return -1;

	if( numAddresses( addressList ) == 0 )
		return 0;

	cmd = malloc( 16 + 2 * numAddresses( addressList ) );
	if( cmd == NULL )
	{
		setIberr( EDVR );
		setIbcnt( ENOMEM );
		return -1;
	}

	i = create_send_setup( board, addressList, cmd );

	//XXX detect no listeners (EBUS) error
	count = my_ibcmd( conf, cmd, i );

	free( cmd );
	cmd = NULL;

	if( count != i )
		return -1;

	return 0;
}


void EnableRemote( int boardID, const Addr4882_t addressList[] )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalEnableRemote( conf, addressList );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}


