/***************************************************************************
                          lib/ibWait.c
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

#include "ib_internal.h"
#include <pthread.h>

void fixup_status_bits( const ibConf_t *conf, int *status )
{
	const int board_wait_mask = board_status_mask & ~ERR;
	const int device_wait_mask = device_status_mask & ~ERR;

	if( conf->is_interface == 0 )
	{
		*status &= device_wait_mask;
	}else
	{
		*status &= board_wait_mask;
		if( interfaceBoard(conf)->use_event_queue )
		{
			*status &= ~DTAS & ~DCAS;
		}else
		{
			*status &= ~EVENT;
		}
	}
}

int my_wait( ibConf_t *conf, int wait_mask, int clear_mask, int set_mask, int *status )
{
	ibBoard_t *board;
	int retval;
	wait_ioctl_t cmd;

	board = interfaceBoard( conf );

	if( conf->is_interface == 0 &&
		is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return -1;
	}

	cmd.handle = conf->handle;
	cmd.usec_timeout = conf->settings.usec_timeout;
	cmd.wait_mask = wait_mask;
	cmd.clear_mask = clear_mask;
	cmd.set_mask = set_mask;
	cmd.set_mask = 0;
	cmd.ibsta = 0;
	fixup_status_bits( conf, &cmd.wait_mask );
	if( conf->is_interface == 0 )
	{
		cmd.pad = conf->settings.pad;
		cmd.sad = conf->settings.sad;
	}else
	{
		cmd.pad = NOADDR;
		cmd.sad = NOADDR;
//XXX additionally, clear wait mask depending on event queue enabled, etc */
	}

	if( wait_mask != cmd.wait_mask )
	{
		setIberr( EARG );
		return -1;
	}

	retval = ioctl(board->fileno, IBWAIT, &cmd);
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return -1;
	}
	fixup_status_bits( conf, &cmd.ibsta );
	if( conf->end ) //XXX
		cmd.ibsta |= END;
	setIbsta( cmd.ibsta );
	*status = cmd.ibsta;
	return 0;
}

int ibwait( int ud, int mask )
{
	ibConf_t *conf;
	int retval;
	int status;
	int clear_mask;
	int error = 0;
	
	conf = general_enter_library( ud, 1, 0 );
	if( conf == NULL )
		return general_exit_library(ud, 1, 0, 0, 0, 0, 1);

	clear_mask = mask & ( DTAS | DCAS | SPOLL);
	retval = my_wait( conf, mask, clear_mask, 0, &status );
	if( retval < 0 )
		return general_exit_library(ud, 1, 0, 0, 0, 0, 1);

//XXX
	if(conf->async.in_progress && (status & CMPL))
	{
		if( gpib_aio_join( &conf->async ) )
			error++;
		pthread_mutex_lock( &conf->async.lock );
		if( conf->async.ibsta & CMPL )
		{
			conf->async.in_progress = 0;
			setIbcnt( conf->async.ibcntl );
			setIberr( conf->async.iberr );
			if( conf->async.ibsta & ERR )
			{
				error++;
			}
		}
		pthread_mutex_unlock( &conf->async.lock );
		if(error && (ThreadIbsta() & ERR) == 0)
		{
			status |= ERR;
			setIbsta(status);
		}
	}

	general_exit_library( ud, error, 0, 1, 0, 0, 1);

	return status;
}

void WaitSRQ( int boardID, short *result )
{
	ibConf_t *conf;
	int retval;
	int wait_mask;
	int status;

	conf = general_enter_library( boardID, 1, 0 );
	if( conf == NULL )
	{
		general_exit_library( boardID, 1, 0, 0, 0, 0, 1 );
		return;
	}

	if( conf->is_interface == 0 )
	{
		setIberr( EDVR );
		general_exit_library( boardID, 1, 0, 0, 0, 0, 1 );
		return;
	}

	wait_mask = SRQI | TIMO;
	retval = my_wait( conf, wait_mask, 0, 0, &status );
	if( retval < 0 )
	{
		general_exit_library( boardID, 1, 0, 0, 0, 0, 1 );
		return;
	}
	// XXX need better query of service request state, new ioctl?
	// should play nice with autopolling
	if( ThreadIbsta() & SRQI ) *result = 1;
	else *result = 0;

	general_exit_library( boardID, 0, 0, 0, 0, 0, 1 );
}
