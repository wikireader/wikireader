/***************************************************************************
                              cec/interrupt.c
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

#include "cec.h"
#include <asm/bitops.h>
#include <asm/dma.h>

/*
 * GPIB interrupt service routines
 */

irqreturn_t cec_interrupt(int irq, void *arg PT_REGS_ARG)
{
	gpib_board_t *board = arg;
	cec_private_t *priv = board->private_data;
	unsigned long flags;
	irqreturn_t retval;
	
	spin_lock_irqsave( &board->spinlock, flags );
	retval = nec7210_interrupt(board, &priv->nec7210_priv);
	spin_unlock_irqrestore( &board->spinlock, flags );
	return retval;
}

