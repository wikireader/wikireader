/***************************************************************************
                          nec7210/tnt4882_aux.c
                             -------------------

    begin                : 2002
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

#include "tnt4882.h"
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>

int tnt4882_line_status( const gpib_board_t *board )
{
	int status = ValidALL;
	int bcsr_bits;
	tnt4882_private_t *tnt_priv;

	tnt_priv = board->private_data;

	bcsr_bits = tnt_readb( tnt_priv, BSR );

	if( bcsr_bits & BCSR_REN_BIT )
		status |= BusREN;
	if( bcsr_bits & BCSR_IFC_BIT )
		status |= BusIFC;
	if( bcsr_bits & BCSR_SRQ_BIT )
		status |= BusSRQ;
	if( bcsr_bits & BCSR_EOI_BIT )
		status |= BusEOI;
	if( bcsr_bits & BCSR_NRFD_BIT )
		status |= BusNRFD;
	if( bcsr_bits & BCSR_NDAC_BIT )
		status |= BusNDAC;
	if( bcsr_bits & BCSR_DAV_BIT )
		status |= BusDAV;
	if( bcsr_bits & BCSR_ATN_BIT )
		status |= BusATN;

	return status;
}

unsigned int tnt4882_t1_delay( gpib_board_t *board, unsigned int nano_sec )
{
	tnt4882_private_t *tnt_priv = board->private_data;
	nec7210_private_t *nec_priv = &tnt_priv->nec7210_priv;
	unsigned int retval;

	retval = nec7210_t1_delay( board, nec_priv, nano_sec );
	if( nec_priv->type == NEC7210 ) return retval;

	if( nano_sec <= 350 )
	{
		tnt_writeb( tnt_priv, MSTD, KEYREG );
		retval = 350;
	}else
		tnt_writeb( tnt_priv, 0, KEYREG );

	if( nano_sec > 500 && nano_sec <= 1100 )
	{
		write_byte( nec_priv, AUXRI | USTD, AUXMR );
		retval = 1100;
	}else
		write_byte( nec_priv, AUXRI, AUXMR );
	return retval;
}
