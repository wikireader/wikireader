/***************************************************************************
                                 master_read_to_file.c
                             -------------------

Example program which uses gpib c library.  I use this with
slave_write_from_file in order to test read/write speed between two boards.
Unlike master_write_from_file, we don't use ibrdf() here because I want
to separate gpib transfer speed and disk io speed in my benchmarking.

    copyright            : (C) 2003 by Frank Mori Hess
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

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "gpib/ib.h"

int main( int argc, char *argv[] )
{
	int dev;
	int board_index = 0;
	int pad = 1;
	int sad = 0;
	int send_eoi = 1;
	int eos_mode = 0;
	char *file_path;
	int status;
	struct timeval start_time, end_time;
	float elapsed_time;
	FILE *filep;
	uint8_t *buffer;
	static const unsigned long buffer_length = 10000000;

	if( argc < 2 )
	{
		fprintf( stderr, "Must provide file path as arguement\n" );
		return -1;
	}

	file_path = argv[ 1 ];
	filep = fopen( file_path, "w" );
	if( filep == NULL )
	{
		perror( "fopen()");
		return -1;
	}

	buffer = malloc( buffer_length );
	if( buffer == NULL )
	{
		perror( "malloc()");
		return -1;
	}

	dev = ibdev( board_index, pad, sad, TNONE, send_eoi, eos_mode );
	if( dev < 0 )
	{
		fprintf( stderr, "ibdev() failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	printf( "Device online: board index=%i, pad=%i, sad=%i\n"
		"\tfile path=%s\n", board_index, pad, sad, file_path );

	gettimeofday( &start_time, NULL );

	status = ibrd( dev, buffer, buffer_length );
	if( status & ERR )
	{
		fprintf( stderr, "ibrd() failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	gettimeofday( &end_time, NULL );

	elapsed_time = end_time.tv_sec - start_time.tv_sec +
		( end_time.tv_usec - start_time.tv_usec ) / 1e6;
	printf( "Transferred %lu bytes in %g seconds: %g bytes/sec\n",
		ThreadIbcntl(), elapsed_time, ThreadIbcntl() / elapsed_time );

	if( fwrite( buffer, 1, ThreadIbcntl(), filep ) != ThreadIbcntl() )
	{
		perror( "fwrite()" );
		return -1;
	}

	fclose( filep );
	free( buffer );

	return 0;
}

