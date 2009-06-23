/***************************************************************************
                          lib/ibstop.c
                             -------------------

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
#include "ib_internal.h"
#include <pthread.h>

int internal_ibstop( ibConf_t *conf )
{
	int retval;

	pthread_mutex_lock( &conf->async.lock );
	if( conf->async.in_progress == 0 )
	{
		pthread_mutex_unlock( &conf->async.lock );
		return 0;
	}
	retval = pthread_cancel( conf->async.thread );
	if( retval )
	{
		pthread_mutex_unlock( &conf->async.lock );
		return 0;
	}
	pthread_mutex_unlock( &conf->async.lock );
	retval = gpib_aio_join( &conf->async );
	if( retval )
	{
		return -1;
	}
	pthread_mutex_lock( &conf->async.lock );
	conf->async.in_progress = 0;
	pthread_mutex_unlock( &conf->async.lock );

	setIberr( EABO );

	return 1;
}

int ibstop( int ud )
{
	ibConf_t *conf;
	int retval;

	conf = general_enter_library( ud, 1, 1 );
	if( conf == NULL )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	retval = internal_ibstop( conf );
	if( retval < 0 )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	return general_exit_library( ud, 0, 0, 0, 0, CMPL, 1 );
}
