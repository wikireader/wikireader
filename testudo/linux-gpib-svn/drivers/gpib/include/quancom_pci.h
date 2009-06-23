/***************************************************************************
 Quancom pci stuff
 copyright (C) 2005 by Frank Mori Hess <fmhess@users.sourceforge.net>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

 
#ifndef _QUANCOM_PCI_H
#define _QUANCOM_PCI_H

/* quancom registers */
enum quancom_regs
{
	QUANCOM_IRQ_CONTROL_STATUS_REG = 0xfc,
};

enum quancom_irq_control_status_bits
{
	QUANCOM_IRQ_ASSERTED_BIT = 0x1, /* readable (any write to the register clears the interrupt)*/
	QUANCOM_IRQ_ENABLE_BIT = 0x4, /* writeable */
};

#endif	// _QUANCOM_PCI_H
