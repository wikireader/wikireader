/***************************************************************************
                              nec7210/pc2.h
                             -------------------

    begin                : Jan 2002
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

#ifndef _PC2_H
#define _PC2_H

#include "nec7210.h"
#include "gpibP.h"

// struct which defines private_data for pc2 driver
typedef struct
{
	nec7210_private_t nec7210_priv;
	unsigned int irq;
	// io address that clears interrupt for pc2a (0x2f0 + irq)
	unsigned int clear_intr_addr;
} pc2_private_t;

// interfaces
extern gpib_interface_t pc2_interface;
extern gpib_interface_t pc2a_interface;

// pc2 uses 8 consecutive io addresses
static const int pc2_iosize = 8;
static const int pc2a_iosize = 8;
static const int pc2_2a_iosize = 16;

// offset between io addresses of successive nec7210 registers
static const int pc2a_reg_offset = 0x400;
static const int pc2_reg_offset = 1;

//interrupt service routine
irqreturn_t pc2_interrupt(int irq, void *arg PT_REGS_ARG);
irqreturn_t pc2a_interrupt(int irq, void *arg PT_REGS_ARG);

// pc2 specific registers and bits

// interrupt clear register address
static const int pc2a_clear_intr_iobase = 0x2f0;
inline static unsigned int CLEAR_INTR_REG( unsigned int irq )
{
	return pc2a_clear_intr_iobase + irq;
}
#endif	// _PC2_H
