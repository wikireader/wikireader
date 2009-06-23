/***************************************************************************
                             lib/self_test.c
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
#include <stdlib.h>

int InternalTestSys( ibConf_t *conf, const Addr4882_t addressList[], short resultList[] )
{
	unsigned int failure_count = 0;
	ibBoard_t *board;
	int retval;
	int i;

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

	if( addressListIsValid( addressList ) == 0 )
	{
		setIberr( EARG );
		return -1;
	}

	if( numAddresses( addressList ) == 0 )
	{
		setIberr( EARG );
		return -1;
	}

	retval = InternalSendList( conf, addressList, "*TST?", 4, NLend );
	if( retval < 0 ) return retval;

	for( i = 0; i < numAddresses( addressList ); i++ )
	{
		char reply[ 16 ];

		retval = InternalReceive( conf, addressList[ i ], reply,
			sizeof( reply ) - 1, STOPend );
		if( retval < 0 )
			return -1;

		reply[ ThreadIbcnt() ] = 0;
		resultList[ i ] = strtol( reply, NULL, 0 );

		if( resultList[ i ] ) failure_count++;
	}

	setIbcnt( failure_count );

	return 0;
}


void TestSys( int boardID, const Addr4882_t addressList[], short resultList[] )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalTestSys( conf, addressList, resultList );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}
