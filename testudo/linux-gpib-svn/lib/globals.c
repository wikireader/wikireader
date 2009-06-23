/***************************************************************************
                              lib/globals.c
                             -------------------

    begin                : Oct 2002
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
#include <pthread.h>
#include <stdlib.h>


volatile int iberr = 0;
volatile int ibsta = 0;
volatile int ibcnt = 0;
volatile long ibcntl = 0;

static pthread_key_t ibsta_key;
static pthread_key_t iberr_key;
static pthread_key_t ibcntl_key;

static pthread_once_t global_keys_once = PTHREAD_ONCE_INIT;

static void ibsta_destroy( void *thread_ibsta )
{
	if( thread_ibsta )
	{
		free( thread_ibsta );
		thread_ibsta = NULL;
	}
}

static void iberr_destroy( void *thread_iberr )
{
	if( thread_iberr )
	{
		free( thread_iberr );
		thread_iberr = NULL;
	}
}

static void ibcntl_destroy( void *thread_ibcntl )
{
	if( thread_ibcntl )
	{
		free( thread_ibcntl );
		thread_ibcntl = NULL;
	}
}

static void global_keys_alloc()
{
	int retval;

	retval = pthread_key_create( &ibsta_key, ibsta_destroy );
	if( retval ) fprintf( stderr, "libgpib: failed to allocate TSD key!\n" );
	retval = pthread_key_create( &iberr_key, iberr_destroy );
	if( retval ) fprintf( stderr, "libgpib: failed to allocate TSD key!\n" );
	retval = pthread_key_create( &ibcntl_key, ibcntl_destroy );
	if( retval ) fprintf( stderr, "libgpib: failed to allocate TSD key!\n" );
}

void globals_alloc( void )
{
	int *ibsta_p, *iberr_p, *ibcntl_p;

	pthread_once( &global_keys_once, global_keys_alloc );
	if( pthread_getspecific( ibsta_key ) == NULL )
	{
		ibsta_p = malloc( sizeof( int ) );
		if( ibsta_p == NULL )
		fprintf( stderr, "libgpib: failed to allocate ibsta!\n" );
		iberr_p = malloc( sizeof( int ) );
		if( iberr_p == NULL )
		fprintf( stderr, "libgpib: failed to allocate iberr!\n" );
		ibcntl_p = malloc( sizeof( long ) );
		if( ibcntl_p == NULL )
		fprintf( stderr, "libgpib: failed to allocate ibcntl!\n" );
		*ibsta_p = 0;
		*iberr_p = 0;
		*ibcntl_p = 0;
		pthread_setspecific( ibsta_key, ibsta_p );
		pthread_setspecific( iberr_key, iberr_p );
		pthread_setspecific( ibcntl_key, ibcntl_p );
	}
}

void setIbsta( int status )
{
	int *thread_ibsta;

	globals_alloc();
	thread_ibsta = pthread_getspecific( ibsta_key );
	if( thread_ibsta == NULL )
	{
		fprintf( stderr, "libgpib: failed to set ibsta TSD\n" );
		return;
	}
	*thread_ibsta = status;
}

void setIberr( int error )
{
	int *thread_iberr;

	globals_alloc();
	thread_iberr = pthread_getspecific( iberr_key );
	if( thread_iberr == NULL )
	{
		fprintf( stderr, "libgpib: failed to set iberr TSD\n" );
		return;
	}
	*thread_iberr = error;
}

void setIbcnt( long count )
{
	int *thread_ibcntl;

	globals_alloc();
	thread_ibcntl = pthread_getspecific( ibcntl_key );
	if( thread_ibcntl == NULL )
	{
		fprintf( stderr, "libgpib: failed to set ibcntl TSD\n" );
		return;
	}
	*thread_ibcntl = count;
}

int ThreadIbsta( void )
{
	int *thread_ibsta;

	globals_alloc();

	thread_ibsta = pthread_getspecific( ibsta_key );
	if( thread_ibsta == NULL )
	{
		fprintf( stderr, "libgpib: failed to get ibsta TSD\n" );
		return ERR;
	}

	return *thread_ibsta;
}

int ThreadIberr( void )
{
	int *thread_iberr;

	globals_alloc();

	thread_iberr = pthread_getspecific( iberr_key );
	if( thread_iberr == NULL )
	{
		fprintf( stderr, "libgpib: failed to get iberr TSD\n" );
		return EDVR;
	}

	return *thread_iberr;
}

int ThreadIbcnt( void )
{
	return ThreadIbcntl();
}

long ThreadIbcntl( void )
{
	int *thread_ibcntl;

	globals_alloc();
	thread_ibcntl = pthread_getspecific( ibcntl_key );
	if( thread_ibcntl == NULL )
	{
		fprintf( stderr, "libgpib: failed to get ibcntl TSD\n" );
		return 0;
	}

	return *thread_ibcntl;
}

void sync_globals( void )
{
	ibsta = ThreadIbsta();
	iberr = ThreadIberr();
	ibcntl = ThreadIbcnt();
	ibcnt = ibcntl;
}
