/***************************************************************************
                                 slave_write_from_file.c
                             -------------------

Example program which uses gpib c library.  I use this with
master_read_to_file in order to test read/write speed between two boards.
Unlike slave_read_to_file, this program does not use ibwrtf() because I want
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

#define _GNU_SOURCE

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
	int board = 0;
	int eos_mode = 0;
	char *file_path;
	int status;
	int retval;
	FILE *filep;
	struct stat file_stats;
	uint8_t *buffer;
	unsigned long buffer_length;

	if( argc < 2 )
	{
		fprintf( stderr, "Must provide file path as arguement\n" );
		return -1;
	}

	file_path = argv[ 1 ];
	filep = fopen( file_path, "r" );
	if( filep == NULL )
	{
		perror( "fopen()");
		return -1;
	}

	retval = fstat( fileno( filep ), &file_stats );
	if( retval < 0 )
	{
		perror( "fstat()");
		return -1;
	}
	buffer_length = file_stats.st_size;

	buffer = malloc( buffer_length );
	if( buffer == NULL )
	{
		perror( "malloc()");
		return -1;
	}

	if( fread( buffer, 1, buffer_length, filep ) != buffer_length )
	{
		perror( "fread()" );
		return -1;
	}

	fclose( filep );

	status = ibeos( board, eos_mode );
	if( status & ERR )
	{
		fprintf( stderr, "ibeos() failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	status = ibtmo( board, TNONE );
	if( status & ERR )
	{
		fprintf( stderr, "ibeos() failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	status = ibconfig( board, IbcTIMING, T1_DELAY_350ns );
	if( status & ERR )
	{
		fprintf( stderr, "ibconfig() failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	status = ibwait( board, TACS );
	if( ( status & TACS ) == 0 )
	{
		fprintf( stderr, "ibwait() for TACS failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	status = ibwrt( board, buffer, buffer_length );
	if( status & ERR )
	{
		fprintf( stderr, "ibwrt() failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	free( buffer );

	return 0;
}

