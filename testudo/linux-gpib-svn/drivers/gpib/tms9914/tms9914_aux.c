/***************************************************************************
                                 tms9914/aux.c
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
#include <asm/bitops.h>

int tms9914_take_control(gpib_board_t *board, tms9914_private_t *priv, int synchronous)
{
	int i;
	const int timeout = 1000;
	
	if(synchronous)
	{
		write_byte(priv, AUX_TCS, AUXCR);
	}else
		write_byte(priv, AUX_TCA, AUXCR);
	// busy wait until ATN is asserted
	for(i = 0; i < timeout; i++)
	{
		if((read_byte(priv, ADSR) & HR_ATN))
			break;
		udelay(1);
	}
	if( i == timeout )
	{
		printk(" tms9914: error waiting for ATN\n");
		return -ETIMEDOUT;
	};
	clear_bit(WRITE_READY_BN, &priv->state);
	return 0;
}

int tms9914_go_to_standby(gpib_board_t *board, tms9914_private_t *priv)
{
	int i;
	const int timeout = 1000;

	write_byte(priv, AUX_GTS, AUXCR);
	// busy wait until ATN is released
	for(i = 0; i < timeout; i++)
	{
		if((read_byte(priv, ADSR) & HR_ATN) == 0)
			break;
		udelay(1);
	}
	if(i == timeout)
	{
		printk("error waiting for NATN\n");
		return -ETIMEDOUT;
	}
	clear_bit(COMMAND_READY_BN, &priv->state);
	return 0;
}

void tms9914_interface_clear(gpib_board_t *board, tms9914_private_t *priv, int assert)
{
	if(assert)
	{
		write_byte(priv, AUX_SIC | AUX_CS, AUXCR);
		set_bit(CIC_NUM, &board->status);
	}else
		write_byte(priv, AUX_SIC, AUXCR);
}

void tms9914_remote_enable(gpib_board_t *board, tms9914_private_t *priv, int enable)
{
	if(enable)
	{
		write_byte(priv, AUX_SRE | AUX_CS, AUXCR);
	}else
		write_byte(priv, AUX_SRE, AUXCR);
}

void tms9914_request_system_control( gpib_board_t *board, tms9914_private_t *priv,
	int request_control )
{
	if( request_control )
		write_byte(priv, AUX_RQC, AUXCR);
	else
	{
		clear_bit(CIC_NUM, &board->status);
		write_byte(priv, AUX_RLC, AUXCR);
	}
}

unsigned int tms9914_t1_delay( gpib_board_t *board, tms9914_private_t *priv,
	unsigned int nano_sec )
{
	static const int clock_period = 200;	// assuming 5Mhz input clock
	int num_cycles;

	num_cycles = 12;

	if( nano_sec <= 8 * clock_period )
	{
		write_byte( priv, AUX_STDL | AUX_CS, AUXCR );
		num_cycles = 8;
	}else
		write_byte( priv, AUX_STDL, AUXCR );

	if( nano_sec <= 4 * clock_period )
	{
		write_byte( priv, AUX_VSTDL | AUX_CS, AUXCR );
		num_cycles = 4;
	}else
		write_byte( priv, AUX_VSTDL, AUXCR );

	return num_cycles * clock_period;
}

void tms9914_return_to_local( const gpib_board_t *board, tms9914_private_t *priv )
{
	write_byte( priv, AUX_RTL, AUXCR );
}

void tms9914_set_holdoff_mode(tms9914_private_t *priv, enum tms9914_holdoff_mode mode)
{
	switch(mode)
	{
	case TMS9914_HOLDOFF_NONE:
		write_byte(priv, AUX_HLDE, AUXCR);
		write_byte(priv, AUX_HLDA, AUXCR);
		break;
	case TMS9914_HOLDOFF_EOI:
		write_byte(priv, AUX_HLDE | AUX_CS, AUXCR);
		write_byte(priv, AUX_HLDA, AUXCR);
		break;
	case TMS9914_HOLDOFF_ALL:
		write_byte(priv, AUX_HLDE, AUXCR);
		write_byte(priv, AUX_HLDA | AUX_CS, AUXCR);
		break;
	default:
		printk("%s: bug! bad holdoff mode %i\n", __FUNCTION__, mode);
		break;
	}
	priv->holdoff_mode = mode;
}

void tms9914_release_holdoff(tms9914_private_t *priv)
{
	if(priv->holdoff_active)
	{
		write_byte( priv, AUX_RHDF, AUXCR );
		priv->holdoff_active = 0;
	}
}

EXPORT_SYMBOL_GPL(tms9914_t1_delay);
EXPORT_SYMBOL_GPL(tms9914_request_system_control);
EXPORT_SYMBOL_GPL(tms9914_take_control);
EXPORT_SYMBOL_GPL(tms9914_go_to_standby);
EXPORT_SYMBOL_GPL(tms9914_interface_clear);
EXPORT_SYMBOL_GPL(tms9914_remote_enable);
EXPORT_SYMBOL_GPL(tms9914_return_to_local);
EXPORT_SYMBOL_GPL(tms9914_set_holdoff_mode);
EXPORT_SYMBOL_GPL(tms9914_release_holdoff);


