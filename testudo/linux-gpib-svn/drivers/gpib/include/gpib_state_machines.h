/***************************************************************************
                                   gpib_state_machines.h
                             -------------------
    begin                : June 2006
    copyright            : (C) 2006 by Frank Mori Hess
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

#ifndef _GPIB_STATE_MACHINES_H
#define _GPIB_STATE_MACHINES_H

enum talker_function_state
{
	talker_idle,
	talker_addressed,
	talker_active,
	serial_poll_active
};

enum listener_function_state
{
	listener_idle,
	listener_addressed,
	listener_active
};

#endif	// _GPIB_STATE_MACHINES_H
