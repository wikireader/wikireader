/***************************************************************************
                             nec7210/cb7210_aux.c
                             -------------------

    begin                : Nov 2002
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

#include "cb7210.h"
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>

int cb7210_line_status( const gpib_board_t *board )
{
	int status = ValidALL;
	int bsr_bits;
	cb7210_private_t *cb_priv;
	nec7210_private_t *nec_priv;

	cb_priv = board->private_data;
	nec_priv = &cb_priv->nec7210_priv;

	bsr_bits = cb7210_paged_read_byte( cb_priv, BUS_STATUS, BUS_STATUS_PAGE );

	if( ( bsr_bits & BSR_REN_BIT ) == 0 )
		status |= BusREN;
	if( ( bsr_bits & BSR_IFC_BIT ) == 0 )
		status |= BusIFC;
	if( ( bsr_bits & BSR_SRQ_BIT ) == 0 )
		status |= BusSRQ;
	if( ( bsr_bits & BSR_EOI_BIT ) == 0 )
		status |= BusEOI;
	if( ( bsr_bits & BSR_NRFD_BIT ) == 0 )
		status |= BusNRFD;
	if( ( bsr_bits & BSR_NDAC_BIT ) == 0 )
		status |= BusNDAC;
	if( ( bsr_bits & BSR_DAV_BIT ) == 0 )
		status |= BusDAV;
	if( ( bsr_bits & BSR_ATN_BIT ) == 0 )
		status |= BusATN;

	return status;
}

unsigned int cb7210_t1_delay( gpib_board_t *board, unsigned int nano_sec )
{
	cb7210_private_t *cb_priv = board->private_data;
	nec7210_private_t *nec_priv = &cb_priv->nec7210_priv;
	unsigned int retval;

	retval = nec7210_t1_delay( board, nec_priv, nano_sec );

	if( nano_sec <= 350 )
	{
		write_byte( nec_priv, AUX_HI_SPEED, AUXMR );
		retval = 350;
	}else
		write_byte( nec_priv, AUX_LO_SPEED, AUXMR );

	return retval;
}
