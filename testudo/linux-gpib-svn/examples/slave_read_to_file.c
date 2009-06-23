/***************************************************************************
                                 slave_read_to_file.c
                             -------------------

Example program which uses gpib c library.  I use this with
master_write_from_file in order to test read/write speed between two boards.

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
#include "gpib/ib.h"

int main( int argc, char *argv[] )
{
	int board = 0;
	int eos_mode = 0;
	char *file_path;
	int status;

	if( argc < 2 )
	{
		fprintf( stderr, "Must provide file path as arguement\n" );
		return -1;
	}

	file_path = argv[ 1 ];

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

	status = ibwait( board, LACS );
	if( ( status & LACS ) == 0 )
	{
		fprintf( stderr, "ibwait() for LACS failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	status = ibrdf( board, file_path );
	if( status & ERR )
	{
		fprintf( stderr, "ibrdf() failed\n" );
		fprintf( stderr, "%s\n", gpib_error_string( ThreadIberr() ) );
		return -1;
	}

	return 0;
}

