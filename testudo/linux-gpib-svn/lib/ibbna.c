/***************************************************************************
                          lib/ibbna.c
                             -------------------

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

int my_ibbna( ibConf_t *conf, unsigned int new_board_index )
{
	ibBoard_t *board;
	ibConf_t *board_conf;
	int retval;
	int old_board_index;

	board = interfaceBoard( conf );

	if( conf->is_interface )
	{
		setIberr( EARG );
		return -1;
	}
	retval = close_gpib_handle( conf );
	if( retval < 0 )
	{
		setIberr( EDVR );
		return -1;
	}

	board_conf = &ibFindConfigs[ new_board_index ];
	if( board_conf->is_interface == 0 )
	{
		setIberr( EARG );
		return -1;
	}
	if( is_cic( interfaceBoard( board_conf ) ) == 0 )
	{
		setIberr( ECIC );
		return -1;
	}

	old_board_index = conf->settings.board;
	conf->settings.board = board_conf->settings.board;

	if( ibBoardOpen( interfaceBoard( conf ) ) < 0 )
	{
		setIberr( EDVR );
		return -1;
	}

	retval = open_gpib_handle( conf );
	if( retval < 0 )
	{
		setIberr( EDVR );
		return -1;
	}

	setIberr( old_board_index );
	return 0;
}

int ibbna( int ud, char *board_name )
{
	ibConf_t *conf;
	int retval;
	int find_index;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	if( ( find_index = ibFindDevIndex( board_name ) ) < 0 )
	{
		setIberr( EARG );
		return exit_library( ud, 1 );
	}

	retval = my_ibbna( conf, find_index );
	if( retval < 0 )
		return exit_library( ud, 1 );

	return exit_library( ud, 0 );
}
