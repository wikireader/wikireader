/***************************************************************************
                              lib/ibppc.c
                             -------------------

    begin                : Oct 2002
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

int ppoll_configure_device( ibConf_t *conf, const Addr4882_t addressList[],
	int ppc_configuration )
{
	uint8_t *cmd;
	int i;
	int retval;

	if( is_cic( interfaceBoard( conf ) ) == 0 )
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

	i = create_send_setup( interfaceBoard( conf ), addressList, cmd );

	cmd[ i++ ] = PPC;
	cmd[ i++ ] = ppc_configuration;

	retval = my_ibcmd( conf, cmd, i );

	free( cmd );
	cmd = NULL;

	if( retval < 0 )
	{
		return -1;
	}

	return 0;
}

int device_ppc( ibConf_t *conf, int ppc_configuration )
{
	Addr4882_t addressList[ 2 ];

	addressList[ 0 ] = packAddress( conf->settings.pad, conf->settings.sad );
	addressList [ 1 ] = NOADDR;

	return ppoll_configure_device( conf, addressList, ppc_configuration );
}

int board_ppc( ibConf_t *conf, int ppc_configuration )
{
	ibBoard_t *board;
	int retval;
	ppoll_config_ioctl_t cmd;

	board = interfaceBoard( conf );

	if( conf->settings.local_ppc == 0 )
	{
		setIberr( ECAP );
		return -1;
	}

	retval = query_ppc( board );
	if( retval < 0 ) return retval;
	conf->settings.ppoll_config = retval;	// store old value

	cmd.config = ppc_configuration;
	cmd.set_ist = 0;
	cmd.clear_ist = 0;
	retval = ioctl( board->fileno, IBPPC, &cmd );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return -1;
	}

	return 0;
}

int internal_ibppc( ibConf_t *conf, int v )
{
	static const int ppc_mask = 0xe0;
	int retval;

	if( v && ( v & ppc_mask ) != PPE )
	{
		fprintf( stderr, "libgpib: illegal parallel poll configuration\n" );
		setIberr( EARG );
		return -1;
	}

	if( !v || (v & PPC_DISABLE) )
		v = PPD;

	if( conf->is_interface )
	{
		retval = board_ppc( conf, v );
		if( retval < 0 )
			return retval;
	}else
	{
		retval = device_ppc( conf, v );
		if( retval < 0 ) return retval;
	}

	setIberr( conf->settings.ppoll_config );
	conf->settings.ppoll_config = v;

	return 0;
}

int ibppc( int ud, int v )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	retval = internal_ibppc( conf, v );
	if( retval < 0 ) return exit_library( ud, 1 );

	return exit_library( ud, 0 );
}

void PPollConfig( int boardID, Addr4882_t address,
	int dataLine, int lineSense )
{
	ibConf_t *conf;
	int retval;
	int ppoll_config;
	Addr4882_t addressList[ 2 ];

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

	if( dataLine < 1 || dataLine > 8 ||
		addressIsValid( address ) == 0 || address == NOADDR )
	{
		setIberr( EARG );
		exit_library( boardID, 1 );
		return;
	}

	ppoll_config = PPE_byte( dataLine, lineSense );

	addressList[ 0 ] = address;
	addressList[ 1 ]= NOADDR;
	retval = ppoll_configure_device( conf,
		addressList, ppoll_config );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}

void PPollUnconfig( int boardID, const Addr4882_t addressList[] )
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

	if( addressListIsValid( addressList ) == 0 )
	{
		setIberr( EARG );
		exit_library( boardID, 1 );
		return;
	}

	if( numAddresses( addressList ) )
	{
		retval = ppoll_configure_device( conf,
			addressList, PPD );
	}else
	{
		uint8_t cmd = PPU;

		retval = my_ibcmd( conf, &cmd, 1 );
	}
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}

int internal_ibist( ibConf_t *conf, int ist )
{
	int retval;
	ppoll_config_ioctl_t cmd;

	if( conf->is_interface == 0 )
	{
		setIberr( EARG );
		return -1;
	}

	retval = query_ist( interfaceBoard( conf ) );
	if( retval < 0 ) return retval;
	setIberr( retval );	// set iberr to old ist value

	cmd.config = 0;
	cmd.set_ist = 0;
	cmd.clear_ist = 0;
	if( ist )
		cmd.set_ist = 1;
	else
		cmd.clear_ist = 1;
	retval = ioctl( interfaceBoard( conf )->fileno, IBPPC, &cmd );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return -1;
	}

	return 0;
}

int ibist( int ud, int ist )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	retval = internal_ibist( conf, ist );
	if( retval < 0 ) return exit_library( ud, 1 );

	return exit_library( ud, 0 );
}
