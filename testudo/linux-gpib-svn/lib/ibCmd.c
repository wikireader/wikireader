/***************************************************************************
                          lib/ibCmd.c
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
#include <assert.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <stdlib.h>

int ibcmd(int ud, const void *cmd_buffer, long cnt)
{
	ibConf_t *conf;
	ssize_t count;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	// check that ud is an interface board
	if( conf->is_interface == 0 )
	{
		setIberr( EARG );
		return exit_library( ud, 1 );
	}

	count = my_ibcmd( conf, cmd_buffer, cnt);
	if(count < 0)
	{
		// report no listeners error XXX
		return exit_library( ud, 1);
	}

	if(count != cnt)
	{
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}

int ibcmda( int ud, const void *cmd_buffer, long cnt )
{
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;

	conf = general_enter_library( ud, 1, 0 );
	if( conf == NULL )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	// check that ud is an interface board
	if( conf->is_interface == 0 )
	{
		setIberr( EARG );
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );
	}

	board = interfaceBoard( conf );

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );
	}

	retval = gpib_aio_launch( ud, conf, GPIB_AIO_COMMAND,
		(void*)cmd_buffer, cnt );
	if( retval < 0 )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	return general_exit_library( ud, 0, 0, 0, 0, 0, 1 );
}

ssize_t my_ibcmd( ibConf_t *conf, const uint8_t *buffer, size_t count)
{
	read_write_ioctl_t cmd;
	int retval;
	ibBoard_t *board;

	board = interfaceBoard( conf );

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return -1;
	}

	assert(sizeof(buffer) <= sizeof(cmd.buffer_ptr));
	cmd.buffer_ptr = (uintptr_t)buffer;
	cmd.count = count;
	cmd.handle = conf->handle;
	cmd.end = 0;
	
	set_timeout( board, conf->settings.usec_timeout);

	retval = ioctl( board->fileno, IBCMD, &cmd );
	if( retval < 0 )
	{
		switch( errno )
		{
			case ETIMEDOUT:
				setIberr( EBUS );
				conf->timed_out = 1;
				break;
			default:
				setIberr( EDVR );
				setIbcnt( errno );
				break;
		}
		return -1;
	}

	return cmd.count;
}

unsigned int create_send_setup( const ibBoard_t *board,
	const Addr4882_t addressList[], uint8_t *cmdString )
{
	unsigned int i, j;
	unsigned int board_pad;
	int board_sad;

	if( addressList == NULL )
	{
		fprintf(stderr, "libgpib: bug! addressList NULL in create_send_setup()\n");
		return 0;
	}
	if( addressListIsValid( addressList ) == 0 )
	{
		fprintf(stderr, "libgpib: bug! bad address list\n");
		return 0;
	}

	i = 0;
	/* controller's talk address */
	if(query_pad(board, &board_pad) < 0) return 0;
	cmdString[i++] = MTA(board_pad);
	if(query_sad(board, &board_sad) < 0) return 0;
	if(board_sad >= 0 )
		cmdString[i++] = MSA(board_sad);
	cmdString[ i++ ] = UNL;
	for( j = 0; j < numAddresses( addressList ); j++ )
	{
		unsigned int pad;
		int sad;

		pad = extractPAD( addressList[ j ] );
		sad = extractSAD( addressList[ j ] );
		cmdString[ i++ ] = MLA( pad );
		if( sad >= 0)
			cmdString[ i++ ] = MSA( sad );
	}

	return i;
}

unsigned int send_setup_string( const ibConf_t *conf,
	uint8_t *cmdString )
{
	ibBoard_t *board;
	Addr4882_t addressList[ 2 ];

	board = interfaceBoard( conf );

	addressList[ 0 ] = packAddress( conf->settings.pad, conf->settings.sad );
	addressList[ 1 ] = NOADDR;

	return create_send_setup( board, addressList, cmdString );
}

int send_setup( ibConf_t *conf )
{
	uint8_t cmdString[8];
	int retval;

	retval = send_setup_string( conf, cmdString );

	if( my_ibcmd( conf, cmdString, retval ) < 0 )
		return -1;

	return 0;
}

int InternalSendSetup( ibConf_t *conf, const Addr4882_t addressList[] )
{
	int i;
	ibBoard_t *board;
	uint8_t *cmd;
	int count;

	if( addressListIsValid( addressList ) == 0 ||
		numAddresses( addressList ) == 0 )
	{
		setIberr( EARG );
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

	i = create_send_setup( board, addressList, cmd );

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

void SendSetup( int boardID, const Addr4882_t addressList[] )
{
	int retval;
	ibConf_t *conf;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalSendSetup( conf, addressList );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}

void SendCmds( int boardID, const void *buffer, long count )
{
	ibcmd( boardID, buffer, count );
}
