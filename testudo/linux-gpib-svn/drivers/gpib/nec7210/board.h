/***************************************************************************
                              nec7210/board.h
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


#ifndef _GPIB_PCIIA_BOARD_H
#define _GPIB_PCIIA_BOARD_H

#include "gpibP.h"
#include <asm/io.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include "nec7210.h"

unsigned int update_status_nolock( gpib_board_t *board, nec7210_private_t *priv );

#endif	//_GPIB_PCIIA_BOARD_H

