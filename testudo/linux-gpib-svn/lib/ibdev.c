/***************************************************************************
                                 ibdev.c
                             -------------------
    begin                : Tues Feb 12 2002
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

#if 0
static int is_device_addr( int minor, int pad, int sad )
{
	ibBoard_t *board;
	unsigned int board_pad;
	int board_sad;

	board = &ibBoard[ minor ];

	if( query_pad( board, &board_pad ) < 0 )
	{
		fprintf( stderr, "failed to query pad\n" );
		return -1;
	}
	if( query_sad( board, &board_sad ) < 0 )
	{
		fprintf( stderr, "failed to query sad\n" );
		return -1;
	}

	if( gpib_address_equal( board_pad, board_sad, pad, sad ) == 0 )
	{
		return 1;
	}

	return 0;
}
#endif

int ibdev( int minor, int pad, int sad, int timo, int eot, int eosmode )
{
	int retval;
	ibConf_t new_conf;

	retval = ibParseConfigFile();
	if(retval < 0)
	{
		setIbsta( ERR );
		return -1;
	}

	sad -= sad_offset;

	init_ibconf( &new_conf );
	new_conf.settings.pad = pad;
	new_conf.settings.sad = sad;                        /* device address                   */
	new_conf.settings.board = minor;                         /* board number                     */
	new_conf.settings.eos = eosmode & 0xff;                           /* local eos modes                  */
	new_conf.settings.eos_flags = eosmode & 0xff00;
	new_conf.settings.usec_timeout = timeout_to_usec( timo );
	if( eot )
		new_conf.settings.send_eoi = 1;
	else
		new_conf.settings.send_eoi = 0;
	new_conf.defaults = new_conf.settings;
	new_conf.is_interface = 0;
	
	return my_ibdev( new_conf );
	// XXX check for address conflicts with boards
}

int my_ibdev( ibConf_t new_conf )
{
	int ud;
	ibConf_t *conf;

	ud = ibGetDescriptor(new_conf);
	if( ud < 0 )
	{
		fprintf( stderr, "libgpib: ibdev failed to get descriptor\n" );
		setIbsta( ERR );
		return -1;
	}

	conf = enter_library( ud );
	if( conf == NULL )
	{
		exit_library( ud, 1 );
		return -1;
	}
	// XXX do local lockout if appropriate

	exit_library( ud, 0 );
	return ud;
}
