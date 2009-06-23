/***************************************************************************
                          lib/ibWrt.c
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

#include <assert.h>
#include "ib_internal.h"
#include <stdint.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int find_eos( const uint8_t *buffer, size_t length, int eos, int eos_flags )
{
	unsigned int i;
	unsigned int compare_mask;

	if( eos_flags & BIN ) compare_mask = 0xff;
	else compare_mask = 0x7f;

	for( i = 0; i < length; i++ )
	{
		if( ( buffer[i] & compare_mask ) == ( eos & compare_mask ) )
		return i;
	}

	return -1;
}

int send_data(ibConf_t *conf, const void *buffer, size_t count, int send_eoi, size_t *bytes_written)
{
	ibBoard_t *board;
	read_write_ioctl_t write_cmd;
	int retval;
	
	board = interfaceBoard( conf );

	set_timeout( board, conf->settings.usec_timeout );

	assert(sizeof(buffer) <= sizeof(write_cmd.buffer_ptr));
	write_cmd.buffer_ptr = (uintptr_t)buffer;
	write_cmd.count = count;
	write_cmd.end = send_eoi;
	write_cmd.handle = conf->handle;
	
	retval = ioctl( board->fileno, IBWRT, &write_cmd);
	if(retval < 0)
	{
		switch( errno )
		{
			case ETIMEDOUT:
				conf->timed_out = 1;
				setIberr( EABO );
				break;
			case EINTR:
				setIberr( EABO );
				break;
			case EIO:
				setIberr( ENOL );
				break;
			case EFAULT:
				write_cmd.count = 0;
				//fall-through
			default:
				setIberr( EDVR );
				setIbcnt( errno );
				break;
		}
	}
	*bytes_written = write_cmd.count;
	conf->end = send_eoi && (*bytes_written == count);
	if(retval < 0) return retval;
	return 0;
}

int send_data_smart_eoi(ibConf_t *conf, const void *buffer, size_t count,
	int force_eoi, size_t *bytes_written)
{
	int eoi_on_eos;
	int eos_found = 0;
	int send_eoi;
	unsigned long block_size;
	int retval;

	eoi_on_eos = conf->settings.eos_flags & XEOS;

	block_size = count;

	if( eoi_on_eos )
	{
		retval = find_eos( buffer, count, conf->settings.eos, conf->settings.eos_flags );
		if( retval < 0 ) eos_found = 0;
		else
		{
			block_size = retval;
			eos_found = 1;
		}
	}

	send_eoi = force_eoi || ( eoi_on_eos && eos_found );
	if(send_data(conf, buffer, block_size, send_eoi, bytes_written) < 0)
	{
		return -1;
	}
	return 0;
}

int my_ibwrt( ibConf_t *conf,
	const uint8_t *buffer, size_t count, size_t *bytes_written)
{
	ibBoard_t *board;
	size_t block_size;
	int retval;
	
	*bytes_written = 0;
	board = interfaceBoard( conf );

	set_timeout( board, conf->settings.usec_timeout );

	if( conf->is_interface == 0 )
	{
		// set up addressing
		if( send_setup( conf ) < 0 )
		{
			return -1;
		}
	}

	while( count )
	{
		retval = send_data_smart_eoi( conf, buffer, count, conf->settings.send_eoi, &block_size);
		*bytes_written += block_size;
		if(retval < 0)
		{
			return -1;
		}
		count -= block_size;
		buffer += block_size;
	}
	return 0;
}

int ibwrt( int ud, const void *rd, long cnt )
{
	ibConf_t *conf;
	size_t count;
	int retval;
	
	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	conf->end = 0;

	retval = my_ibwrt(conf, rd, cnt, &count);
	if(retval < 0)
	{
		if(ThreadIberr() != EDVR) setIbcnt(count);
		return exit_library( ud, 1 );
	}
	setIbcnt(count);
	return general_exit_library( ud, 0, 0, 0, DCAS, 0, 0 );
}

int ibwrta( int ud, const void *buffer, long cnt )
{
	ibConf_t *conf;
	int retval;

	conf = general_enter_library( ud, 1, 0 );
	if( conf == NULL )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	retval = gpib_aio_launch( ud, conf, GPIB_AIO_WRITE,
		(void*)buffer, cnt );
	if( retval < 0 )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	return general_exit_library( ud, 0, 0, 0, 0, 0, 1 );
}

int my_ibwrtf( ibConf_t *conf, const char *file_path, size_t *bytes_written)
{
	ibBoard_t *board;
	long count;
	size_t block_size;
	int retval;
	FILE *data_file;
	struct stat file_stats;
	uint8_t buffer[ 0x4000 ];

	*bytes_written = 0;
	board = interfaceBoard( conf );

	data_file = fopen( file_path, "r" );
	if( data_file == NULL )
	{
		setIberr( EFSO );
		setIbcnt( errno );
		return -1;
	}

	retval = fstat( fileno( data_file ), &file_stats );
	if( retval < 0 )
	{
		setIberr( EFSO );
		setIbcnt( errno );
		return -1;
	}

	count = file_stats.st_size;

	if( conf->is_interface == 0 )
	{
		// set up addressing
		if( send_setup( conf ) < 0 )
		{
			return -1;
		}
	}

	set_timeout( board, conf->settings.usec_timeout );

	while( count )
	{
		size_t fread_count;
		int send_eoi;
		size_t buffer_offset = 0;
		
		fread_count = fread( buffer, 1, sizeof( buffer ), data_file );
		if( fread_count == 0 )
		{
			setIberr( EFSO );
			setIbcnt( errno );
			return -1;
		}
		while(buffer_offset < fread_count)
		{
			send_eoi = conf->settings.send_eoi && (count == fread_count - buffer_offset);
			retval = send_data_smart_eoi(conf, buffer + buffer_offset,
				fread_count - buffer_offset, send_eoi, &block_size);
			count -= block_size;
			buffer_offset += block_size;
			*bytes_written += block_size;
			if(retval < 0)
			{
				return -1;
			}
		}
	}
	return 0;
}

int ibwrtf( int ud, const char *file_path )
{
	ibConf_t *conf;
	size_t count;
	int retval;
	
	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	conf->end = 0;

	retval = my_ibwrtf(conf, file_path, &count);
	if(retval < 0)
	{
		if(ThreadIberr() != EDVR) setIbcnt(count);
		return exit_library( ud, 1 );
	}
	setIbcnt( count );

	return general_exit_library( ud, 0, 0, 0, DCAS, 0, 0 );
}

int InternalSendDataBytes( ibConf_t *conf, const void *buffer,
	size_t count, int eotmode)
{
	int retval;
	size_t num_bytes;
	size_t bytes_written = 0;
	
	if( conf->is_interface == 0 )
	{
		setIberr(EARG);
		return -1;
	}

	switch( eotmode )
	{
		case DABend:
		case NLend:
		case NULLend:
			break;
		default:
			setIberr( EARG );
			return -1;
			break;
	}

	retval = send_data( conf, buffer, count, eotmode == DABend, &num_bytes);
	bytes_written += num_bytes;
	if( retval < 0 )
	{
		setIbcnt(bytes_written);
		return retval;
	}
	if( eotmode == NLend )
	{
		retval = send_data( conf, "\n", 1, 1, &num_bytes);
		bytes_written += num_bytes;
		if( retval < 0 )
		{
			setIbcnt(bytes_written);
			return retval;
		}
	}
	setIbcnt(bytes_written);
	return 0;
}

void SendDataBytes( int boardID, const void *buffer,
	long count, int eotmode )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalSendDataBytes( conf, buffer, count, eotmode );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	general_exit_library( boardID, 0, 0, 0, DCAS, 0, 0 );
}

int InternalSendList( ibConf_t *conf, const Addr4882_t addressList[],
	const void *buffer, long count, int eotmode )
{
	ibBoard_t *board;
	int retval;

	if( addressListIsValid( addressList ) == 0 ||
		numAddresses( addressList ) == 0 )
	{
		setIberr(EARG);
		return -1;
	}

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

	retval = InternalSendSetup( conf, addressList );
	if( retval < 0 ) return retval;

	retval = InternalSendDataBytes( conf, buffer, count, eotmode );
	if( retval < 0 ) return retval;

	return 0;
}

void SendList( int boardID, const Addr4882_t addressList[],
	const void *buffer, long count, int eotmode )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalSendList( conf, addressList, buffer, count, eotmode );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	general_exit_library( boardID, 0, 0, 0, DCAS, 0, 0 );
}

void Send( int boardID, Addr4882_t address, const void *buffer, long count,
	int eotmode )
{
	Addr4882_t addressList[ 2 ];

	addressList[ 0 ] = address;
	addressList[ 1 ] = NOADDR;

	SendList( boardID, addressList, buffer, count, eotmode );
}


