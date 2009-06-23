/***************************************************************************
                               sys/autopoll.h
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

#ifndef GPIB_AUTOPOLL_H
#define GPIB_AUTOPOLL_H

#include "gpib_types.h"

unsigned int num_status_bytes( const gpib_status_queue_t *dev );
int push_status_byte( gpib_status_queue_t *device, uint8_t poll_byte );
int pop_status_byte( gpib_status_queue_t *device, uint8_t *poll_byte );
gpib_status_queue_t * get_gpib_status_queue( gpib_board_t *board, unsigned int pad, int sad );
int get_serial_poll_byte( gpib_board_t *board, unsigned int pad, int sad,
	unsigned int usec_timeout, uint8_t *poll_byte );
int autopoll_all_devices( gpib_board_t *board );

#endif // GPIB_AUTOPOLL_H
