/***************************************************************************
                          lib/ibFindLstn.c
                             -------------------

    copyright            : (C) 2002,2003 by Frank Mori Hess
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
#include <unistd.h>
#include <stdlib.h>

int listenerFound( ibConf_t *conf, const Addr4882_t addressList[] )
{
	uint8_t *cmd;
	int i, j;
	short line_status;
	int retval;

	if( addressList == NULL )
		return 0;
	if( addressListIsValid( addressList ) == 0 )
		return -1;

	cmd = malloc( 16 + 2 * numAddresses( addressList ) );
	if( cmd == NULL )
	{
		setIberr( EDVR );
		setIbcnt( ENOMEM );
		return -1;
	}

	j = 0;
	cmd[ j++ ] = UNL;
	for( i = 0; i < numAddresses( addressList ); i++ )
	{
		int pad, sad;

		pad = extractPAD( addressList[ i ] );
		sad = extractSAD( addressList[ i ] );
		cmd[ j++ ] = MLA( pad );
		if( sad >= 0 )
			cmd[ j++ ] = MSA( sad );
	}
	retval = my_ibcmd( conf, cmd, j );

	free( cmd );
	cmd = NULL;

	if( retval < 0 ) return retval;

	retval = internal_ibgts( conf, 0 );
	if( retval < 0 ) return -1;

	usleep( 1500 );

	retval = internal_iblines( conf, &line_status );
	if( retval < 0 ) return retval;

	if( ( line_status & ValidNDAC ) &&
		( line_status & BusNDAC ) )
	{
		return 1;
	}

	return 0;
}

int secondaryListenerFound( ibConf_t *conf, unsigned int pad )
{
	Addr4882_t testAddress[ 32 ];
	int j;

	for( j = 0; j <= gpib_addr_max; j++ )
		testAddress[ j ] = packAddress( pad, j );
	testAddress[ j ] = NOADDR;
	return listenerFound( conf, testAddress );
}

void FindLstn( int boardID, const Addr4882_t padList[],
	Addr4882_t resultList[], int maxNumResults )
{
	int i;
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;
	int resultIndex;
	short line_status;

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

	board = interfaceBoard( conf );

	retval = internal_iblines( conf, &line_status );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}
	if( ( line_status & ValidNDAC ) == 0 )
	{
		setIberr( ECAP );
		exit_library( boardID, 1 );
		return;
	}

	resultIndex = 0;
	for( i = 0; i < numAddresses( padList ); i++ )
	{
		Addr4882_t pad;
		Addr4882_t testAddress[ 2 ];

		pad = GetPAD( padList[ i ] );
		testAddress[ 0 ] = pad;
		testAddress[ 1 ] = NOADDR;
		retval = listenerFound( conf, testAddress );
		if( retval < 0 )
		{
			// XXX status/error settings
			exit_library( boardID, 1 );
			return;
		}
		if( retval > 0 )
		{
			if( resultIndex >= maxNumResults )
			{
				setIberr( ETAB );
				exit_library( boardID, 1 );
				return;
			}
			resultList[ resultIndex++ ] = testAddress[ 0 ];
			setIbcnt( resultIndex );
		}else
		{
			retval = secondaryListenerFound( conf, pad );
			if( retval < 0 )
			{
				exit_library( boardID, 1 );
				return;
			}
			if( retval > 0 )
			{
				int j;
				for( j = 0; j <= gpib_addr_max; j++ )
				{
					testAddress[ 0 ] = packAddress( pad, j );
					testAddress[ 1 ] = NOADDR;
					retval = listenerFound( conf, testAddress );
					if( retval < 0 )
					{
						exit_library( boardID, 1 );
						return;
					}
					if( retval > 1 )
					{
						if( resultIndex >= maxNumResults )
						{
							setIberr( ETAB );
							exit_library( boardID, 1 );
							return;
						}
						resultList[ resultIndex++ ] = testAddress[ 0 ];
						setIbcnt( resultIndex );
					}
				}
			}
		}
	}
	exit_library( boardID, 0 );
} /* FindLstn */

int ibln( int ud, int pad, int sad, short *found_listener )
{
	ibConf_t *conf;
	Addr4882_t addressList[ 2 ];
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	switch( sad )
	{
	case ALL_SAD:
		retval = secondaryListenerFound( conf, pad );
		break;
	case NO_SAD:
	default:
		addressList[ 0 ] = MakeAddr( pad, sad );
		addressList[ 1 ] = NOADDR;
		retval = listenerFound( conf, addressList );
		break;
	}
	if( retval < 0 ) return exit_library( ud, 1 );

	*found_listener = retval;

	return exit_library( ud, 0 );
}
