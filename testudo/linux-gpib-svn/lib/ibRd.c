/***************************************************************************
                          lib/ibRd.c
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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "ib_internal.h"

// sets up bus to receive data from device with address pad/sad
int InternalReceiveSetup( ibConf_t *conf, Addr4882_t address )
{
	ibBoard_t *board;
	uint8_t cmdString[8];
	unsigned int i = 0;
	unsigned int pad, board_pad;
	int sad, board_sad;

	if( addressIsValid( address ) == 0 ||
		address == NOADDR )
	{
		setIberr( EARG );
		return -1;
	}
	board = interfaceBoard( conf );

	if( query_pad( board, &board_pad ) < 0 ) return -1;
	if( query_sad( board, &board_sad ) < 0 ) return -1;

	pad = extractPAD( address );
	sad = extractSAD( address );

	cmdString[ i++ ] = UNL;

	cmdString[ i++ ] = MLA( board_pad );	/* controller's listen address */
	if ( board_sad >= 0 )
		cmdString[ i++ ] = MSA( board_sad );
	cmdString[ i++ ] = MTA( pad );
	if( sad >= 0 )
		cmdString[ i++ ] = MSA( sad );

	if ( my_ibcmd( conf, cmdString, i ) < 0)
	{
		fprintf(stderr, "%s: command failed\n", __FUNCTION__ );
		return -1;
	}

	return 0;
}

ssize_t read_data(ibConf_t *conf, uint8_t *buffer, size_t count, size_t *bytes_read)
{
	ibBoard_t *board;
	read_write_ioctl_t read_cmd;
	int retval;

	board = interfaceBoard( conf );

	assert(sizeof(buffer) <= sizeof(read_cmd.buffer_ptr));
	read_cmd.buffer_ptr = (uintptr_t)buffer;
	read_cmd.count = count;
	read_cmd.handle = conf->handle;
	read_cmd.end = 0;

	set_timeout( board, conf->settings.usec_timeout );
	conf->end = 0;

	retval = ioctl( board->fileno, IBRD, &read_cmd );
	if( retval < 0 )
	{
		switch( errno )
		{
			case ETIMEDOUT:
				conf->timed_out = 1;
				setIberr(EABO);
				break;
			default:
				setIberr(EDVR);
				setIbcnt(errno);
				break;
		}
	}

	if( read_cmd.end ) conf->end = 1;

	*bytes_read = read_cmd.count;

	return retval;
}

ssize_t my_ibrd( ibConf_t *conf, uint8_t *buffer, size_t count, size_t *bytes_read)
{
	*bytes_read = 0;
	// set eos mode
	iblcleos( conf );

	if( conf->is_interface == 0 )
	{
		// set up addressing
		if( InternalReceiveSetup( conf, packAddress( conf->settings.pad, conf->settings.sad ) ) < 0 )
		{
			return -1;
		}
	}

	return read_data(conf, buffer, count, bytes_read);
}

int ibrd(int ud, void *rd, long cnt)
{
	ibConf_t *conf;
	ssize_t retval;
	size_t bytes_read;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	retval = my_ibrd(conf, rd, cnt, &bytes_read);
	if(retval < 0)
	{
		if(ThreadIberr() != EDVR)
			setIbcnt(bytes_read);
		return exit_library( ud, 1 );
	}else
	{
		setIbcnt(bytes_read);
	}

	return general_exit_library( ud, 0, 0, 0, DCAS, 0, 0 );
}

int ibrda( int ud, void *buffer, long cnt )
{
	ibConf_t *conf;
	int retval;

	conf = general_enter_library( ud, 1, 0 );
	if( conf == NULL )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	retval = gpib_aio_launch( ud, conf, GPIB_AIO_READ,
		buffer, cnt );
	if( retval < 0 )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	return general_exit_library( ud, 0, 0, 0, 0, 0, 1 );
}

int ibrdf(int ud, const char *file_path )
{
	ibConf_t *conf;
	int retval;
	uint8_t buffer[ 0x4000 ];
	unsigned long byte_count;
	FILE *save_file;
	int error;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	save_file = fopen( file_path, "a" );
	if( save_file == NULL )
	{
		setIberr( EFSO );
		setIbcnt( errno );
		return exit_library( ud, 1 );
	}

	if( conf->is_interface == 0 )
	{
		// set up addressing
		if( InternalReceiveSetup( conf, packAddress( conf->settings.pad, conf->settings.sad ) ) < 0 )
		{
			return exit_library( ud, 1 );
		}
	}

	// set eos mode
	iblcleos( conf );

	byte_count = error = 0;
	do
	{
		int fwrite_count;
		size_t bytes_read;

		retval = read_data(conf, buffer, sizeof(buffer), &bytes_read);
		fwrite_count = fwrite( buffer, 1, bytes_read, save_file );
		if( fwrite_count != bytes_read )
		{
			setIberr( EFSO );
			setIbcnt( errno );
			error++;
		}
		byte_count += fwrite_count;
		if( retval < 0 )
		{
			error++;
			break;
		}
	}while( conf->end == 0 && error == 0 );

	setIbcnt( byte_count );

	if( fclose( save_file ) )
	{
		setIberr( EFSO );
		setIbcnt( errno );
		return exit_library( ud, 1 );
	}
	if( error ) return exit_library( ud, error );

	return general_exit_library( ud, 0, 0, 0, DCAS, 0, 0 );
}

int InternalRcvRespMsg( ibConf_t *conf, void *buffer, long count, int termination )
{
	ibBoard_t *board;
	int retval;
	int use_eos;
	size_t bytes_read;

	if( conf->is_interface == 0 )
	{
		setIberr(EARG);
		return -1;
	}

	board = interfaceBoard( conf );

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return -1;
	}

	if( termination != ( termination & 0xff ) &&
		termination != STOPend )
	{
		setIberr( EARG );
		return -1;
	}
	// XXX check for listener active state

	//XXX detect no listeners (EBUS) error
	use_eos = ( termination != STOPend );
	retval = config_read_eos( board, use_eos, termination, 1 );
	if( retval < 0 )
	{
		return retval;
	}

	retval = read_data(conf, buffer, count, &bytes_read);
	setIbcnt(bytes_read);
	if(retval < 0)
	{
		return -1;
	}

	return 0;
}

void RcvRespMsg( int boardID, void *buffer, long count, int termination )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalRcvRespMsg( conf, buffer, count, termination );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	general_exit_library( boardID, 0, 0, 0, DCAS, 0, 0 );
}

void ReceiveSetup( int boardID, Addr4882_t address )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalReceiveSetup( conf, address );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );
}

int InternalReceive( ibConf_t *conf, Addr4882_t address,
	void *buffer, long count, int termination )
{
	int retval;

	retval = InternalReceiveSetup( conf, address );
	if( retval < 0 ) return retval;

	retval = InternalRcvRespMsg( conf, buffer, count, termination );
	if( retval < 0 )return retval;

	return 0;
}

void Receive( int boardID, Addr4882_t address,
	void *buffer, long count, int termination )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalReceive( conf, address, buffer, count, termination );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	general_exit_library( boardID, 0, 0, 0, DCAS, 0, 0 );
}
