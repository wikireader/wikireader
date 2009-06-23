/***************************************************************************
                               sys/autopoll.c
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

#include "ibsys.h"
#include "autopoll.h"

static const unsigned int serial_timeout = 1000000;

unsigned int num_status_bytes( const gpib_status_queue_t *dev )
{
	return dev->num_status_bytes;
}

// push status byte onto back of status byte fifo
int push_status_byte( gpib_status_queue_t *device, uint8_t poll_byte )
{
	struct list_head *head = &device->status_bytes;
	status_byte_t *status;
	static const unsigned int max_num_status_bytes = 1024;
	int retval;

	if( num_status_bytes( device ) >= max_num_status_bytes )
	{
		uint8_t lost_byte;

		device->dropped_byte = 1;
		retval = pop_status_byte( device, &lost_byte );
		if( retval < 0 ) return retval;
	}

	status = kmalloc( sizeof( status_byte_t ), GFP_KERNEL );
	if( status == NULL ) return -ENOMEM;

	INIT_LIST_HEAD( &status->list );
	status->poll_byte = poll_byte;

	list_add_tail( &status->list, head );

	device->num_status_bytes++;

	GPIB_DPRINTK( "pushed status byte 0x%x, %i in queue\n",
		(int) poll_byte, num_status_bytes( device ) );

	return 0;
}

// pop status byte from front of status byte fifo
int pop_status_byte( gpib_status_queue_t *device, uint8_t *poll_byte )
{
	struct list_head *head = &device->status_bytes;
	struct list_head *front = head->next;
	status_byte_t *status;

	if( num_status_bytes( device ) == 0 ) return -EIO;

	if( front == head ) return -EIO;

	if( device->dropped_byte )
	{
		device->dropped_byte = 0;
		return -EPIPE;
	}

	status = list_entry( front, status_byte_t, list );
	*poll_byte = status->poll_byte;

	list_del( front );
	kfree( status );

	device->num_status_bytes--;

	GPIB_DPRINTK( "popped status byte 0x%x, %i in queue\n",
		(int) *poll_byte, num_status_bytes( device ) );

	return 0;
}

gpib_status_queue_t * get_gpib_status_queue( gpib_board_t *board, unsigned int pad, int sad )
{
	gpib_status_queue_t *device;
	struct list_head *list_ptr;
	const struct list_head *head = &board->device_list;

	for( list_ptr = head->next; list_ptr != head; list_ptr = list_ptr->next )
	{
		device = list_entry( list_ptr, gpib_status_queue_t, list );
		if( gpib_address_equal( device->pad, device->sad, pad, sad ) )
			return device;
	}

	return NULL;
}

int get_serial_poll_byte( gpib_board_t *board, unsigned int pad, int sad, unsigned int usec_timeout,
		uint8_t *poll_byte )
{
	gpib_status_queue_t *device;

	device = get_gpib_status_queue( board, pad, sad );
	if( device == NULL ) return -EINVAL;

	if( num_status_bytes( device ) )
	{
		return pop_status_byte( device, poll_byte );
	}else
	{
		return dvrsp( board, pad, sad, usec_timeout, poll_byte );
	}
}

int autopoll_all_devices( gpib_board_t *board )
{
	int retval;

	GPIB_DPRINTK( "entered autopoll_all_devices()\n" );
	if( mutex_lock_interruptible( &board->mutex ) )
	{
		return -ERESTARTSYS;
	}

	GPIB_DPRINTK( "autopoll has board lock\n" );

	retval = serial_poll_all( board, serial_timeout );
	if( retval < 0 )
	{
		mutex_unlock( &board->mutex );
		return retval;
	}

	GPIB_DPRINTK( "autopoll_all_devices() complete\n" );
	/* need to wake wait queue in case someone is
	* waiting on RQS */
	wake_up_interruptible( &board->wait );
	mutex_unlock( &board->mutex );

	return retval;
}
