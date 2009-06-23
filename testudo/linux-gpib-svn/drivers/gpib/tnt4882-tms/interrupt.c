/***************************************************************************
                              nec7210/interrupt.c
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

#include "tnt4882.h"
#include <asm/bitops.h>
#include <asm/dma.h>

/*
 * GPIB interrupt service routines
 */

void tnt4882_interrupt(int irq, void *arg, struct pt_regs *registerp)
{
	gpib_board_t *board = arg;
	tnt4882_private_t *priv = board->private_data;
	unsigned long flags;

	spin_lock_irqsave( &board->spinlock, flags );
	tms9914_interrupt(board, &priv->tms9914_priv);
	spin_unlock_irqrestore( &board->spinlock, flags );
}

