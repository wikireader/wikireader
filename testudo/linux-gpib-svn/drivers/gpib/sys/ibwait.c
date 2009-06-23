/***************************************************************************
                              ibwait.c
                             -------------------

    begin                : Dec 2001
    copyright            : (C) 2001, 2002 by Frank Mori Hess
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

#include "gpibP.h"
#include "autopoll.h"
#include <linux/sched.h>

struct wait_info
{
	gpib_board_t *board;
	struct timer_list timer;
	volatile int timed_out;
	unsigned long usec_timeout;
};

static void init_wait_info( struct wait_info *winfo )
{
	winfo->board = NULL;
	init_timer( &winfo->timer );
	winfo->timed_out = 0;
}

static int wait_satisfied( struct wait_info *winfo, gpib_status_queue_t *status_queue,
	int wait_mask, int *status, gpib_descriptor_t *desc )
{
	gpib_board_t *board = winfo->board;
	int temp_status;

	temp_status = general_ibstatus( board, status_queue, 0, 0, desc );
	if( winfo->timed_out )
		temp_status |= TIMO;
	else
		temp_status &= ~TIMO;
	if( wait_mask & temp_status )
	{
		*status = temp_status;
		return 1;
	}
//XXX does wait for END work?
	return 0;
}

static void wait_timeout( unsigned long arg )
/* Watchdog timeout routine */
{
	struct wait_info *winfo = ( struct wait_info * ) arg;

	winfo->timed_out = 1;
	wake_up_interruptible( &winfo->board->wait );
}

/* install timer interrupt handler */
static void startWaitTimer( struct wait_info *winfo )
/* Starts the timeout task  */
{
	winfo->timed_out = 0;

	if( winfo->usec_timeout > 0 )
	{
		winfo->timer.expires = jiffies + usec_to_jiffies( winfo->usec_timeout );
		winfo->timer.function = wait_timeout;
		winfo->timer.data = (unsigned long) winfo;
		add_timer( &winfo->timer );              /* add timer           */
	}
}

static void removeWaitTimer( struct wait_info *winfo )
{
	if( timer_pending( &winfo->timer ) )
		del_timer_sync( &winfo->timer );
}

/*
 * IBWAIT
 * Check or wait for a GPIB event to occur.  The mask argument
 * is a bit vector corresponding to the status bit vector.  It
 * has a bit set for each condition which can terminate the wait
 * If the mask is 0 then
 * no condition is waited for.
 */
int ibwait( gpib_board_t *board, int wait_mask, int clear_mask, int set_mask,
	int *status, unsigned long usec_timeout, gpib_descriptor_t *desc )
{
	int retval = 0;
	gpib_status_queue_t *status_queue;
	struct wait_info winfo;

	if( desc->is_board ) status_queue = NULL;
	else status_queue = get_gpib_status_queue( board, desc->pad, desc->sad );

	if( wait_mask == 0 )
	{
		*status = general_ibstatus( board, status_queue, clear_mask, set_mask, desc );
		return 0;
	}

	init_wait_info( &winfo );
	winfo.board = board;
	winfo.usec_timeout = usec_timeout;
	startWaitTimer( &winfo );

	if( wait_event_interruptible( board->wait,
		wait_satisfied( &winfo, status_queue, wait_mask, status, desc ) ) )
	{
		printk( "wait interrupted\n" );
		retval = -ERESTARTSYS;
	}
	removeWaitTimer( &winfo );

	/* make sure we only clear status bits that we are reporting */
	if( *status & clear_mask || set_mask )
		general_ibstatus( board, status_queue, *status & clear_mask, set_mask, 0 );

	return retval;
}

