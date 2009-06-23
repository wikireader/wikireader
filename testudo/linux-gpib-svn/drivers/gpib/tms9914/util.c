/***************************************************************************
                              tms9914/util.c
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

#include "board.h"
#include <linux/delay.h>

int tms9914_enable_eos(gpib_board_t *board, tms9914_private_t *priv, uint8_t eos_byte, int compare_8_bits)
{
	priv->eos = eos_byte;
	priv->eos_flags = REOS;
	if( compare_8_bits )
		priv->eos_flags |= BIN;
	return 0;
}

void tms9914_disable_eos(gpib_board_t *board, tms9914_private_t *priv)
{
	priv->eos_flags &= ~REOS;
}

int tms9914_parallel_poll(gpib_board_t *board, tms9914_private_t *priv, uint8_t *result)
{
	// execute parallel poll
	write_byte(priv, AUX_CS | AUX_RPP, AUXCR);
	udelay(2);
	*result = read_byte(priv, CPTR);
	// clear parallel poll state
	write_byte(priv, AUX_RPP, AUXCR);
	return 0;
}

void set_ppoll_reg( tms9914_private_t *priv, int enable,
	unsigned int dio_line, int sense, int ist )
{
	uint8_t dio_byte;

	if( enable && ( ( sense && ist ) || ( !sense && !ist ) ) )
	{
		dio_byte = 1 << (dio_line - 1);
		write_byte( priv, dio_byte, PPR );
	}else
	{
		write_byte( priv, 0, PPR );
	}
}

void tms9914_parallel_poll_configure( gpib_board_t *board,
	tms9914_private_t *priv, uint8_t config )
{
	priv->ppoll_enable = ( config & PPC_DISABLE ) == 0;
	priv->ppoll_line = ( config & PPC_DIO_MASK ) + 1;
	priv->ppoll_sense = ( config & PPC_SENSE ) != 0;
	set_ppoll_reg( priv, priv->ppoll_enable, priv->ppoll_line, priv->ppoll_sense, board->ist );
}

void tms9914_parallel_poll_response( gpib_board_t *board,
	tms9914_private_t *priv, int ist )
{
	set_ppoll_reg( priv, priv->ppoll_enable, priv->ppoll_line, priv->ppoll_sense, ist );
}

void tms9914_serial_poll_response(gpib_board_t *board, tms9914_private_t *priv, uint8_t status)
{
	unsigned long flags;
	
	spin_lock_irqsave( &board->spinlock, flags );
	write_byte(priv, status, SPMR);
	priv->spoll_status = status;
	if(status & request_service_bit)
		write_byte(priv, AUX_RSV2 | AUX_CS, AUXCR);
	else
		write_byte(priv, AUX_RSV2, AUXCR);
	spin_unlock_irqrestore( &board->spinlock, flags );
}

uint8_t tms9914_serial_poll_status( gpib_board_t *board, tms9914_private_t *priv )
{
	uint8_t status;
	unsigned long flags;

	spin_lock_irqsave( &board->spinlock, flags );
	status = priv->spoll_status;
	spin_unlock_irqrestore( &board->spinlock, flags );

	return status;
}

void tms9914_primary_address(gpib_board_t *board, tms9914_private_t *priv, unsigned int address)
{
	// put primary address in address0
	write_byte(priv, address & ADDRESS_MASK, ADR);
}

void tms9914_secondary_address(gpib_board_t *board, tms9914_private_t *priv, unsigned int address, int enable)
{
	if( enable )
		priv->imr1_bits |= HR_APTIE;
	else
		priv->imr1_bits &= ~HR_APTIE;

	write_byte( priv, priv->imr1_bits, IMR1 );
}

unsigned int tms9914_update_status( gpib_board_t *board, tms9914_private_t *priv,
	unsigned int clear_mask )
{
	unsigned long flags;
	unsigned int retval;

	spin_lock_irqsave( &board->spinlock, flags );
	retval = update_status_nolock( board, priv );
	board->status &= ~clear_mask;
	spin_unlock_irqrestore( &board->spinlock, flags );

	return retval;
}

static void update_talker_state(tms9914_private_t *priv, unsigned address_status_bits)
{
	if(address_status_bits & HR_TA)
	{
		if(address_status_bits & HR_ATN)
		{
			priv->talker_state = talker_addressed;
		}else
		{
			/* this could also be serial_poll_active, but the tms9914 provides no
			 * way to distinguish, so we'll assume talker_active */
			priv->talker_state = talker_active;
		}
	}else
	{
		priv->talker_state = talker_idle;
	}
}

static void update_listener_state(tms9914_private_t *priv, unsigned address_status_bits)
{
	if(address_status_bits & HR_LA)
	{
		if(address_status_bits & HR_ATN)
		{
			priv->listener_state = listener_addressed;
		}else
		{
			priv->listener_state = listener_active;
		}
	}else
	{
		priv->listener_state = listener_idle;
	}
}

unsigned int update_status_nolock( gpib_board_t *board, tms9914_private_t *priv )
{
	int address_status;

	address_status = read_byte( priv, ADSR );

	// check for remote/local
	if(address_status & HR_REM)
		set_bit( REM_NUM, &board->status );
	else
		clear_bit( REM_NUM, &board->status );
	// check for lockout
	if(address_status & HR_LLO)
		set_bit( LOK_NUM, &board->status );
	else
		clear_bit( LOK_NUM, &board->status );
	// check for ATN
	if(address_status & HR_ATN)
	{
		set_bit( ATN_NUM, &board->status );
	}else
	{
		clear_bit( ATN_NUM, &board->status );
	}
	// check for talker/listener addressed
	update_talker_state(priv, address_status);
	if(priv->talker_state == talker_active)
	{
		set_bit( TACS_NUM, &board->status );
	}else
		clear_bit( TACS_NUM, &board->status );
	update_listener_state(priv, address_status);
	if(priv->listener_state == listener_active)
	{
		set_bit(LACS_NUM, &board->status);
	}else
		clear_bit( LACS_NUM, &board->status );

//	GPIB_DPRINTK( "status 0x%x, state 0x%x\n", board->status, priv->state );
	
	return board->status;
}

int tms9914_line_status( const gpib_board_t *board, tms9914_private_t *priv)
{
	int bsr_bits;
	int status = ValidALL;

	bsr_bits = read_byte(priv, BSR);
	
	if( bsr_bits & BSR_REN_BIT )
		status |= BusREN;
	if( bsr_bits & BSR_IFC_BIT )
		status |= BusIFC;
	if( bsr_bits & BSR_SRQ_BIT )
		status |= BusSRQ;
	if( bsr_bits & BSR_EOI_BIT )
		status |= BusEOI;
	if( bsr_bits & BSR_NRFD_BIT )
		status |= BusNRFD;
	if( bsr_bits & BSR_NDAC_BIT )
		status |= BusNDAC;
	if( bsr_bits & BSR_DAV_BIT )
		status |= BusDAV;
	if( bsr_bits & BSR_ATN_BIT )
		status |= BusATN;

	return status;
}

EXPORT_SYMBOL(tms9914_enable_eos);
EXPORT_SYMBOL(tms9914_disable_eos);
EXPORT_SYMBOL(tms9914_serial_poll_response);
EXPORT_SYMBOL(tms9914_serial_poll_status);
EXPORT_SYMBOL(tms9914_parallel_poll);
EXPORT_SYMBOL(tms9914_parallel_poll_configure);
EXPORT_SYMBOL(tms9914_parallel_poll_response);
EXPORT_SYMBOL(tms9914_primary_address);
EXPORT_SYMBOL(tms9914_secondary_address);
EXPORT_SYMBOL(tms9914_update_status);
EXPORT_SYMBOL(tms9914_line_status);

