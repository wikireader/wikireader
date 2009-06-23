/***************************************************************************
                             lib/local_lockout.c
                             -------------------

    copyright            : (C) 2002 by Frank Mori Hess
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

static int local_lockout( ibConf_t *conf, const Addr4882_t addressList[] )
{
	uint8_t cmd;
	int retval;

	retval = InternalEnableRemote( conf, addressList );
	if( retval < 0 ) return retval;

	cmd = LLO;
	retval = my_ibcmd( conf, &cmd, 1 );
	if( retval < 0 ) return retval;

	return 0;
}

void SendLLO( int boardID )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = local_lockout( conf, NULL );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}

void SetRWLS( int boardID, const Addr4882_t addressList[] )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	if( numAddresses( addressList ) == 0 )
	{
		setIberr( EARG );
		exit_library( boardID, 1 );
		return;
	}

	retval = local_lockout( conf, addressList );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}
