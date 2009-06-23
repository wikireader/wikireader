/***************************************************************************
                               sys/event.c
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
#include <linux/slab.h>
#include <linux/module.h>

#include "gpibP.h"

static int push_gpib_event_nolock( gpib_board_t *board, short event_type );
static int pop_gpib_event_nolock( gpib_event_queue_t *queue, short *event_type );

unsigned int num_gpib_events( const gpib_event_queue_t *queue )
{
	return queue->num_events;
}

// push event onto back of event queue
int push_gpib_event( gpib_board_t *board, short event_type )
{
	unsigned long flags;
	int retval;

	spin_lock_irqsave( &board->event_queue.lock, flags );
	retval = push_gpib_event_nolock( board, event_type );
	spin_unlock_irqrestore( &board->event_queue.lock, flags );

	if( event_type == EventDevTrg ) board->status |= DTAS;
	if( event_type == EventDevClr ) board->status |= DCAS;

	return retval;
}

static int push_gpib_event_nolock( gpib_board_t *board, short event_type )
{
	gpib_event_queue_t *queue = &board->event_queue;
	struct list_head *head = &queue->event_head;
	gpib_event_t *event;
	static const unsigned int max_num_events = 1024;
	int retval;

	if( num_gpib_events( queue ) >= max_num_events )
	{
		short lost_event;

		queue->dropped_event = 1;
		retval = pop_gpib_event_nolock( queue, &lost_event );
		if( retval < 0 )
			return retval;
	}

	event = kmalloc( sizeof( gpib_event_t ), GFP_ATOMIC );
	if( event == NULL )
	{
		queue->dropped_event = 1;
		printk( "gpib: failed to allocate memory for event\n");
		return -ENOMEM;
	}

	INIT_LIST_HEAD( &event->list );
	event->event_type = event_type;

	list_add_tail( &event->list, head );

	queue->num_events++;

	GPIB_DPRINTK( "pushed event %i, %i in queue\n",
		(int) event_type, num_gpib_events( queue ) );

	return 0;
}

// pop event from front of event queue
int pop_gpib_event( gpib_event_queue_t *queue, short *event_type )
{
	unsigned long flags;
	int retval;

	spin_lock_irqsave( &queue->lock, flags );
	retval = pop_gpib_event_nolock( queue, event_type );
	spin_unlock_irqrestore( &queue->lock, flags );
	return retval;
}

static int pop_gpib_event_nolock( gpib_event_queue_t *queue, short *event_type )
{
	struct list_head *head = &queue->event_head;
	struct list_head *front = head->next;
	gpib_event_t *event;

	if( num_gpib_events( queue ) == 0 )
	{
		*event_type = EventNone;
		return 0;
	}

	if( front == head ) return -EIO;

	if( queue->dropped_event )
	{
		queue->dropped_event = 0;
		return -EPIPE;
	}

	event = list_entry( front, gpib_event_t, list );
	*event_type = event->event_type;

	list_del( front );
	kfree( event );

	queue->num_events--;

	GPIB_DPRINTK( "popped event %i, %i in queue\n",
		(int) *event_type, num_gpib_events( queue ) );

	return 0;
}

EXPORT_SYMBOL( push_gpib_event );
