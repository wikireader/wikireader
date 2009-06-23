/***************************************************************************
                          lib/ibRsp.c
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

static int serial_poll( ibBoard_t *board, unsigned int pad, int sad,
	unsigned int usec_timeout, char *result )
{
	serial_poll_ioctl_t poll_cmd;
	int retval;

	poll_cmd.pad = pad;
	poll_cmd.sad = sad;

	set_timeout( board, usec_timeout );

	retval = ioctl( board->fileno, IBRSP, &poll_cmd );
	if(retval < 0)
	{
		switch( errno )
		{
			case ETIMEDOUT:
				setIberr( EABO );
				break;
			case EPIPE:
				setIberr( ESTB );
				break;
			default:
				setIberr( EDVR );
				setIbcnt( errno );
				break;
		}
		return -1;
	}

	*result = poll_cmd.status_byte;

	return 0;
}

int ibrsp(int ud, char *spr)
{
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	if( conf->is_interface )
	{
		setIberr( EARG );
		return exit_library( ud, 1 );
	}
	board = interfaceBoard( conf );

	retval = serial_poll( board, conf->settings.pad, conf->settings.sad,
		conf->settings.spoll_usec_timeout, spr );
	if(retval < 0)
	{
		if( errno == ETIMEDOUT )
			conf->timed_out = 1;
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}

void AllSPoll( int boardID, const Addr4882_t addressList[], short resultList[] )
{
	int i;
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}
	if( addressListIsValid( addressList ) == 0 )
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

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		exit_library( boardID, 1 );
		return;
	}

	// XXX could use slightly more efficient ALLSPOLL protocol
	retval = 0;
	for( i = 0; i < numAddresses( addressList ); i++ )
	{
		char result;
		retval = serial_poll( board, extractPAD( addressList[ i ] ),
			extractSAD( addressList[ i ] ), conf->settings.spoll_usec_timeout, &result );
		if( retval < 0 )
		{
			if( errno == ETIMEDOUT )
				conf->timed_out = 1;
			break;
		}
		resultList[ i ] = result & 0xff;
	}
	setIbcnt( i );

	if( retval < 0 ) exit_library( boardID, 1 );
	else exit_library( boardID, 0 );
}

void AllSpoll( int boardID, const Addr4882_t addressList[], short resultList[] )
{
	AllSPoll( boardID, addressList, resultList );
}

void FindRQS( int boardID, const Addr4882_t addressList[], short *result )
{
	int i;
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}
	if( addressListIsValid( addressList ) == 0 )
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

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		exit_library( boardID, 1 );
		return;
	}

	retval = 0;
	for( i = 0; i < numAddresses( addressList ); i++ )
	{
		char spoll_byte;
		retval = serial_poll( board, extractPAD( addressList[ i ] ),
			extractSAD( addressList[ i ] ), conf->settings.usec_timeout, &spoll_byte );
		if( retval < 0 )
		{
			if( errno == ETIMEDOUT )
				conf->timed_out = 1;
			break;
		}
		if( spoll_byte & request_service_bit )
		{
			*result = spoll_byte & 0xff;
			break;
		}
	}
	setIbcnt( i );
	if( i == numAddresses( addressList ) )
	{
		setIberr( ETAB );
		retval = -1;
	}

	if( retval < 0 ) exit_library( boardID, 1 );
	else exit_library( boardID, 0 );
}

void ReadStatusByte( int boardID, Addr4882_t address, short *result )
{
	ibConf_t *conf;
	ibBoard_t *board;
	char byte_result;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}
	if( addressIsValid( address ) == 0 )
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

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		exit_library( boardID, 1 );
		return;
	}

	retval = serial_poll( board, extractPAD( address ),
		extractSAD( address ), conf->settings.spoll_usec_timeout, &byte_result );
	if( retval < 0 )
	{
		if( errno == ETIMEDOUT )
			conf->timed_out = 1;
		exit_library( boardID, 1 );
		return;
	}
	*result = byte_result & 0xff;

	exit_library( boardID, 0 );
}
