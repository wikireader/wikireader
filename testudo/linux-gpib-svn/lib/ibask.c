/***************************************************************************
                          lib/ibask.c
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

#include "ib_internal.h"

int query_ist( const ibBoard_t *board )
{
	int retval;
	board_info_ioctl_t info;

	retval = ioctl( board->fileno, IBBOARD_INFO, &info );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	return info.ist;
}

int query_ppc( const ibBoard_t *board )
{
	int retval;
	board_info_ioctl_t info;

	retval = ioctl( board->fileno, IBBOARD_INFO, &info );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	return info.parallel_poll_configuration;
}

int query_autopoll( const ibBoard_t *board )
{
	int retval;
	board_info_ioctl_t info;

	retval = ioctl( board->fileno, IBBOARD_INFO, &info );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	return info.autopolling;
}

int query_board_t1_delay( const ibBoard_t *board )
{
	int retval;
	board_info_ioctl_t info;

	retval = ioctl( board->fileno, IBBOARD_INFO, &info );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	if(info.t1_delay == 0)
	{
		fprintf(stderr, "%s: bug! we don't know what the T1 delay is because it has never been set.\n",
				__FUNCTION__);
		return -EIO;
	}else if( info.t1_delay < 500 ) return T1_DELAY_350ns;
	else if( info.t1_delay < 2000 ) return T1_DELAY_500ns;
	return T1_DELAY_2000ns;
}

int query_board_rsv( const ibBoard_t *board )
{
	int retval;
	int status;

	retval = ioctl( board->fileno, IBQUERY_BOARD_RSV, &status );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	return status;
}

int query_pad( const ibBoard_t *board, unsigned int *pad )
{
	int retval;
	board_info_ioctl_t info;

	retval = ioctl( board->fileno, IBBOARD_INFO, &info );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	*pad = info.pad;
	return 0;
}

int query_sad( const ibBoard_t *board, int *sad )
{
	int retval;
	board_info_ioctl_t info;

	retval = ioctl( board->fileno, IBBOARD_INFO, &info );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	*sad = info.sad;
	return 0;
}

int query_no_7_bit_eos(const ibBoard_t *board)
{
	int retval;
	board_info_ioctl_t info;

	retval = ioctl(board->fileno, IBBOARD_INFO, &info);
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}
	return info.no_7_bit_eos;
}

int ibask( int ud, int option, int *value )
{
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;

	conf = general_enter_library( ud, 1, 0 );
	if( conf == NULL )
		return exit_library( ud, 1);

	board = interfaceBoard( conf );

	switch( option )
	{
		case IbaPAD:
			if( conf->is_interface )
			{
				unsigned int pad;

				retval = query_pad( board, &pad );
				if( retval < 0 ) return exit_library( ud, 1 );
				conf->settings.pad = pad;
			}
			*value = conf->settings.pad;
			return exit_library( ud, 0 );
			break;
		case IbaSAD:
			if( conf->is_interface )
			{
				int sad;

				retval = query_sad( board, &sad );
				if( retval < 0 ) return exit_library( ud, 1 );
				conf->settings.sad = sad;
			}
			if( conf->settings.sad < 0 ) *value = 0;
			else *value = MSA( conf->settings.sad );
			return exit_library( ud, 0 );
			break;
		case IbaTMO:
			*value = usec_to_timeout( conf->settings.usec_timeout );
			return exit_library( ud, 0 );
			break;
		case IbaEOT:
			*value = conf->settings.send_eoi;
			return exit_library( ud, 0 );
			break;
		case IbaEOSrd:
			*value = conf->settings.eos_flags & REOS;
			return exit_library( ud, 0 );
			break;
		case IbaEOSwrt:
			*value = conf->settings.eos_flags & XEOS;
			return exit_library( ud, 0 );
			break;
		case IbaEOScmp:
			*value = conf->settings.eos_flags & BIN;
			return exit_library( ud, 0 );
			break;
		case IbaEOSchar:
			*value = conf->settings.eos;
			return exit_library( ud, 0 );
			break;
		case IbaReadAdjust:
			/* XXX I guess I could implement byte swapping stuff,
			 * it's pretty stupid though */
			*value = 0;
			return exit_library( ud, 0 );
			break;
		case IbaWriteAdjust:
			/* XXX I guess I could implement byte swapping stuff,
			 * it's pretty stupid though */
			*value = 0;
			return exit_library( ud, 0 );
			break;
		case IbaEndBitIsNormal:
			/* XXX no support for setting END status on EOI only yet */
			*value = 1;
			return exit_library( ud, 0 );
			break;
		default:
			break;
	}

	if( conf->is_interface )
	{
		switch( option )
		{
			case IbaPPC:
				retval = query_ppc( board );
				if( retval < 0 ) return exit_library( ud, 1 );
				*value = retval;
				return exit_library( ud, 0 );
				break;
			case IbaAUTOPOLL:
				retval = query_autopoll( board );
				if( retval < 0 ) return exit_library( ud, 1 );
				*value = retval;
				return exit_library( ud, 0 );
				break;
			case IbaCICPROT:
				// XXX we don't support pass control protocol yet
				*value = 0;
				return exit_library( ud, 0 );
				break;
			case IbaIRQ:
				// XXX we don't support interrupt-less operation yet
				*value = 0;
				return exit_library( ud, 0 );
				break;
			case IbaSC:
				retval = is_system_controller( board );
				if( retval < 0 ) return exit_library( ud, 1 );
				*value = retval;
				return exit_library( ud, 0 );
				break;
			case IbaSRE:
				/* XXX pretty worthless, until changing
				 * system controllers is supported */
				*value = 1;
				return exit_library( ud, 0 );
				break;
			case IbaPP2:
				*value = conf->settings.local_ppc;
				return exit_library( ud, 0 );
				break;
			case IbaTIMING:
				retval = query_board_t1_delay( board );
				if( retval < 0 ) return exit_library( ud, 1 );
				*value = retval;
				return exit_library( ud, 0 );
				break;
			case IbaDMA:
				// XXX bogus, but pretty unimportant
				*value = board->dma;
				return exit_library( ud, 0 );
				break;
			case IbaEventQueue:
				*value = board->use_event_queue;
				return exit_library( ud, 0 );
				break;
			case IbaSPollBit:
				*value = 1;
				return exit_library( ud, 0 );
				break;
			case IbaSendLLO:
				*value = conf->settings.local_lockout;
				return exit_library( ud, 0 );
				break;
			case IbaPPollTime:
				*value = usec_to_ppoll_timeout( conf->settings.ppoll_usec_timeout );
				return exit_library( ud, 0 );
				break;
			case IbaHSCableLength:
				/* HS transfer not supported and may never
				 * be as it is not part of GPIB standard */
				*value = 0;
				return exit_library( ud, 0 );
				break;
			case IbaIst:
				retval = query_ist( board );
				if( retval < 0 ) return exit_library( ud, 1 );
				*value = retval;
				return exit_library( ud, 0 );
				break;
			case IbaRsv:
				retval = query_board_rsv( board );
				if( retval < 0 ) return exit_library( ud, 1 );
				*value = retval;
				return exit_library( ud, 0 );
				break;
			case Iba7BitEOS:
				retval = query_no_7_bit_eos(board);
				if( retval < 0 ) return exit_library( ud, 1 );
				*value = !retval;
				return exit_library( ud, 0 );
				break;
			default:
				break;
		}
	}else
	{
		switch( option )
		{
			case IbaREADDR:
				*value = conf->settings.readdr;
				return exit_library( ud, 0 );
				break;
			case IbaSPollTime:
				*value = usec_to_timeout( conf->settings.spoll_usec_timeout );
				return exit_library( ud, 0 );
				break;
			case IbaUnAddr:
				/* XXX sending UNT and UNL after device level read/write
				 * not supported yet, I suppose it could be since it
				 * is harmless. */
				*value = 0;
				return exit_library( ud, 0 );
				break;
			case IbaBNA:
				*value = conf->settings.board;
				return exit_library( ud, 0 );
				break;
			default:
				break;
		}
	}

	setIberr( EARG );

	return exit_library( ud, 1 );
}


