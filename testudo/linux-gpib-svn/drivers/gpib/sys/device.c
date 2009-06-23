/***************************************************************************
                              sys/device.c
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
#include <linux/delay.h>

static int setup_serial_poll( gpib_board_t *board, unsigned int usec_timeout )
{
	uint8_t cmd_string[8];
	int i;

	GPIB_DPRINTK( "entering setup_serial_poll()\n" );

	ibcac( board, 0 );

	i = 0;
	cmd_string[ i++ ] = UNL;
	cmd_string[ i++ ] = MLA( board->pad );	/* controller's listen address */
	if( board->sad >= 0 )
		cmd_string[ i++ ] = MSA( board->sad );
	cmd_string[ i++ ] = SPE;	//serial poll enable

	osStartTimer( board, usec_timeout );
	if( board->interface->command( board, cmd_string, i ) < i )
	{
		printk("gpib: failed to setup serial poll\n");
		osRemoveTimer( board );
		return -EIO;
	}
	osRemoveTimer( board );

	return 0;
}

static int read_serial_poll_byte( gpib_board_t *board, unsigned int pad,
	int sad, unsigned int usec_timeout, uint8_t *result )
{
	uint8_t cmd_string[8];
	int end_flag;
	int ret;
	int i;
	size_t nbytes;

	GPIB_DPRINTK( "entering read_serial_poll_byte(), pad=%i sad=%i\n", pad, sad );

	ibcac( board, 0);

	i = 0;
	// send talk address
	cmd_string[i++] = MTA( pad );
	if( sad >= 0 )
		cmd_string[i++] = MSA( sad );

	osStartTimer( board, usec_timeout );
	if( board->interface->command( board, cmd_string, i ) < i )
	{
		printk("gpib: failed to setup serial poll\n");
		osRemoveTimer( board );
		return -EIO;
	}

	ibgts( board );

	// read poll result
	ret = board->interface->read( board, result, 1, &end_flag ,&nbytes);
	if( ret < 0 || nbytes < 1)
	{
		printk( "gpib: serial poll failed\n" );
		osRemoveTimer( board );
		return -EIO;
	}
	osRemoveTimer( board );

	return 0;
}

static int cleanup_serial_poll( gpib_board_t *board, unsigned int usec_timeout )
{
	uint8_t cmd_string[8];

	GPIB_DPRINTK( "entering cleanup_serial_poll()\n" );

	ibcac( board, 0 );

	cmd_string[ 0 ] = SPD;	/* disable serial poll bytes */
	cmd_string[ 1 ] = UNT;
	osStartTimer( board, usec_timeout );
	if( board->interface->command( board, cmd_string, 2 ) < 2 )
	{
		printk( "gpib: failed to disable serial poll\n" );
		osRemoveTimer( board );
		return -EIO;
	}
	osRemoveTimer( board );

	return 0;
}

static int serial_poll_single( gpib_board_t *board, unsigned int pad, int sad,
	unsigned int usec_timeout, uint8_t *result )
{
	int retval, cleanup_retval;

	retval = setup_serial_poll( board, usec_timeout );
	if( retval < 0 ) return retval;
	retval = read_serial_poll_byte( board, pad, sad, usec_timeout, result );
	cleanup_retval = cleanup_serial_poll( board, usec_timeout );
	if( retval < 0 ) return retval;
	if( cleanup_retval < 0 ) return retval;

	return 0;
}

int serial_poll_all( gpib_board_t *board, unsigned int usec_timeout )
{
	int retval;
	struct list_head *cur;
	const struct list_head *head = &board->device_list;
	gpib_status_queue_t *device;
	uint8_t result;
	unsigned int num_bytes;

	GPIB_DPRINTK( "entering serial_poll_all()\n" );

	if( head->next == head ) return 0;


	retval = setup_serial_poll( board, usec_timeout );
	if( retval < 0 ) return retval;

	num_bytes = 0;

	for( cur = head->next; cur != head; cur = cur->next )
	{
		device = list_entry( cur, gpib_status_queue_t, list );
		retval = read_serial_poll_byte( board,
			device->pad, device->sad, usec_timeout, &result );
		if( retval < 0 ) continue;
		if( result & request_service_bit )
		{
			retval = push_status_byte( device, result );
			if( retval < 0 ) continue;
			num_bytes++;
		}
	}

	retval = cleanup_serial_poll( board, usec_timeout );
	if( retval < 0 ) return retval;

	return num_bytes;
}

/*
 * DVRSP
 * This function performs a serial poll of the device with primary
 * address pad and secondary address sad. If the device has no
 * secondary adddress, pass a zero in for this argument.  At the
 * end of a successful serial poll the response is returned in result.
 * SPD and UNT are sent at the completion of the poll.
 */

int dvrsp( gpib_board_t *board, unsigned int pad, int sad,
	unsigned int usec_timeout, uint8_t *result )
{
	int status = ibstatus( board );
	int retval;

	if( ( status & CIC ) == 0 )
	{
		printk("gpib: not CIC during serial poll\n");
		return -1;
	}

	if( pad > gpib_addr_max || sad > gpib_addr_max )
	{
		printk("gpib: bad address for serial poll");
		return -1;
	}

	retval = serial_poll_single( board, pad, sad, usec_timeout, result );
	if( io_timed_out( board ) ) retval = -ETIMEDOUT;

	return retval;
}

