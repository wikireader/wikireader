/***************************************************************************
                          nec7210/ines_util.c  -  description
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

#include "ines.h"

#include <linux/pci.h>
#include <asm/io.h>
#include <linux/module.h>

int ines_line_status( const gpib_board_t *board )
{
	int status = ValidALL;
	int bcm_bits;
	ines_private_t *ines_priv;
	nec7210_private_t *nec_priv;

	ines_priv = board->private_data;
	nec_priv = &ines_priv->nec7210_priv;

	bcm_bits = ines_inb( ines_priv, BUS_CONTROL_MONITOR );

	if( bcm_bits & BCM_REN_BIT )
		status |= BusREN;
	if( bcm_bits & BCM_IFC_BIT )
		status |= BusIFC;
	if( bcm_bits & BCM_SRQ_BIT )
		status |= BusSRQ;
	if( bcm_bits & BCM_EOI_BIT )
		status |= BusEOI;
	if( bcm_bits & BCM_NRFD_BIT )
		status |= BusNRFD;
	if( bcm_bits & BCM_NDAC_BIT )
		status |= BusNDAC;
	if( bcm_bits & BCM_DAV_BIT )
		status |= BusDAV;
	if( bcm_bits & BCM_ATN_BIT )
		status |= BusATN;

	return status;
}

void ines_set_xfer_counter( ines_private_t *priv, unsigned int count )
{
	if( count > 0xffff )
	{
		printk("ines: bug! tried to set xfer counter > 0xffff\n" );
		return;
	}
	ines_outb( priv, ( count >> 8 ) & 0xff, XFER_COUNT_UPPER );
	ines_outb( priv, count & 0xff, XFER_COUNT_LOWER );
}

unsigned int ines_t1_delay( gpib_board_t *board, unsigned int nano_sec )
{
	ines_private_t *ines_priv = board->private_data;
	nec7210_private_t *nec_priv = &ines_priv->nec7210_priv;
	unsigned int retval;

	retval = nec7210_t1_delay( board, nec_priv, nano_sec );

	if( nano_sec <= 250 )
	{
		write_byte( nec_priv, INES_AUXD | INES_FOLLOWING_T1_250ns |
			INES_INITIAL_T1_2000ns, AUXMR );
		retval = 250;
	}else if( nano_sec <=350 )
	{
		write_byte( nec_priv, INES_AUXD | INES_FOLLOWING_T1_350ns |
			INES_INITIAL_T1_2000ns, AUXMR );
		retval = 350;
	}else 
	{
		write_byte( nec_priv, INES_AUXD | INES_FOLLOWING_T1_500ns |
			INES_INITIAL_T1_2000ns, AUXMR );
		retval = 500;
	}
	
	return retval;
}
