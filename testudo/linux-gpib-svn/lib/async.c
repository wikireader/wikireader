/***************************************************************************
                          lib/async.c
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
#include <sys/ioctl.h>
#include <stdlib.h>
#include <pthread.h>

static void* do_aio( void *varg );

struct gpib_aio_arg
{
	int ud;
	ibConf_t *conf;
	int gpib_aio_type;
};

void init_async_op( struct async_operation *async )
{
	pthread_mutex_init( &async->lock, NULL );
	pthread_mutex_init( &async->join_lock, NULL );
	pthread_cond_init(&async->condition, NULL);
	async->buffer = NULL;
	async->buffer_length = 0;
	async->iberr = 0;
	async->ibsta = 0;
	async->ibcntl = 0;
	async->in_progress = 0;
	async->abort = 0;
}

static void cleanup_aio( void *varg )
{
	struct gpib_aio_arg arg = *((struct gpib_aio_arg*) varg);
	ibBoard_t *board = interfaceBoard(arg.conf);
	ibstatus( arg.conf, 0, 0, CMPL);
	int retval = unlock_board_mutex(board);
	assert(retval == 0);
}

int gpib_aio_launch( int ud, ibConf_t *conf, int gpib_aio_type,
	void *buffer, long cnt )
{
	int retval;
	struct gpib_aio_arg *arg;
	pthread_attr_t attributes;

	arg = malloc( sizeof( *arg ) );
	if( arg == NULL )
	{
		setIberr( EDVR );
		setIbcnt( ENOMEM );
		return -1;
	}
	arg->ud = ud;
	arg->conf = conf;
	arg->gpib_aio_type = gpib_aio_type;

	pthread_mutex_lock( &conf->async.lock );
	if( conf->async.in_progress )
	{
		pthread_mutex_unlock( &conf->async.lock );
		setIberr( EOIP );
		return -1;
	}
	conf->async.ibsta = 0;
	conf->async.ibcntl = 0;
	conf->async.iberr = 0;
	conf->async.buffer = buffer;
	conf->async.buffer_length = cnt;
	conf->async.abort = 0;

	pthread_attr_init( &attributes );
	pthread_attr_setstacksize( &attributes, 0x10000 );
	retval = pthread_create( &conf->async.thread,
		&attributes, do_aio, arg );
	pthread_attr_destroy( &attributes );
	conf->async.in_progress = (retval == 0);
	pthread_cond_wait(&conf->async.condition, &conf->async.lock);
	pthread_mutex_unlock( &conf->async.lock );
	if( retval )
	{
		free( arg ); arg = NULL;
		setIberr( EDVR );
		setIbcnt( retval );
		return -1;
	}

	return 0;
}

static void* do_aio( void *varg )
{
	size_t count;
	struct gpib_aio_arg arg;
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;

	arg = *((struct gpib_aio_arg*) varg);
	free( varg ); varg = NULL;

	conf = arg.conf;
	board = interfaceBoard(conf);
	retval = lock_board_mutex(board);
	if(retval == 0)
	{
		ibstatus(conf, 0, CMPL, 0);
	}
	pthread_mutex_lock(&conf->async.lock);
	pthread_cond_broadcast(&conf->async.condition);
	pthread_mutex_unlock(&conf->async.lock);
	if( retval < 0 ) return NULL;

	pthread_cleanup_push( cleanup_aio, &arg );
	pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
	pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );

	switch( arg.gpib_aio_type )
	{
	case GPIB_AIO_COMMAND:
		count = retval = my_ibcmd( conf, conf->async.buffer, conf->async.buffer_length );
		break;
	case GPIB_AIO_READ:
		retval = my_ibrd( conf, conf->async.buffer, conf->async.buffer_length, &count);
		break;
	case GPIB_AIO_WRITE:
		retval = my_ibwrt(conf, conf->async.buffer, conf->async.buffer_length, &count);
		break;
	default:
		retval = -1;
		fprintf( stderr, "libgpib: bug! in %s\n", __FUNCTION__ );
		break;
	}
	pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
	pthread_mutex_lock( &conf->async.lock );
	if(retval < 0)
	{
		if(ThreadIberr() != EDVR)
			conf->async.ibcntl = count;
		else
			conf->async.ibcntl = ThreadIbcntl();
		conf->async.iberr = ThreadIberr();
		conf->async.ibsta = CMPL | ERR;
	}else
	{
		conf->async.ibcntl = count;
		conf->async.iberr = 0;
		conf->async.ibsta = CMPL;
	}
	pthread_mutex_unlock( &conf->async.lock );
	pthread_cleanup_pop( 1 );
	return NULL;
}

int gpib_aio_join( struct async_operation *async )
{
	int retval;

	pthread_mutex_lock( &async->join_lock );
	retval = pthread_join( async->thread, NULL );
	switch( retval )
	{
	case 0:
		break;
	case ESRCH:	/* thread has already been joined */
		retval = 0;
		break;
	default:
		fprintf( stderr, "libgpib: pthread_join() returned %i in %s\n",
			retval, __FUNCTION__ );
		setIberr( EDVR );
		setIbcnt( retval );
		break;
	}
	pthread_mutex_unlock( &async->join_lock );
	return retval;
}
