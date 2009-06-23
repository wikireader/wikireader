/***************************************************************************
                          lib/ibTmo.c
                             -------------------

    copyright            : (C) 2001,2002,2003 by Frank Mori Hess
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
#include <sys/ioctl.h>

static const int default_ppoll_usec_timeout = 2;

unsigned int timeout_to_usec( enum gpib_timeout timeout )
{
	switch ( timeout )
	{
		default:
		case TNONE:
			return 0;
			break;
		case T10us:
			return 10;
			break;
		case T30us:
			return 30;
			break;
		case T100us:
			return 100;
			break;
		case T300us:
			return 300;
			break;
		case T1ms:
			return 1000;
			break;
		case T3ms:
			return 3000;
			break;
		case T10ms:
			return 10000;
			break;
		case T30ms:
			return 30000;
			break;
		case T100ms:
			return 100000;
			break;
		case T300ms:
			return 300000;
			break;
		case T1s:
			return 1000000;
			break;
		case T3s:
			return 3000000;
			break;
		case T10s:
			return 10000000;
			break;
		case T30s:
			return 30000000;
			break;
		case T100s:
			return 100000000;
			break;
		case T300s:
			return 300000000;
			break;
		case T1000s:
			return 1000000000;
			break;
	}
	return 0;
}

unsigned int ppoll_timeout_to_usec( unsigned int timeout )
{
	if( timeout == 0 )
		return default_ppoll_usec_timeout;
	else
		return timeout_to_usec(timeout);
}

unsigned int usec_to_timeout( unsigned int usec )
{
	if( usec == 0 ) return TNONE;
	else if( usec <= 10 ) return T10us;
	else if( usec <= 30 ) return T30us;
	else if( usec <= 100 ) return T100us;
	else if( usec <= 300 ) return T300us;
	else if( usec <= 1000 ) return T1ms;
	else if( usec <= 3000 ) return T3ms;
	else if( usec <= 10000 ) return T10ms;
	else if( usec <= 30000 ) return T30ms;
	else if( usec <= 100000 ) return T100ms;
	else if( usec <= 300000 ) return T300ms;
	else if( usec <= 1000000 ) return T1s;
	else if( usec <= 3000000 ) return T3s;
	else if( usec <= 10000000 ) return T10s;
	else if( usec <= 30000000 ) return T30s;
	else if( usec <= 100000000 ) return T100s;
	else if( usec <= 300000000 ) return T300s;
	else if( usec <= 1000000000 ) return T1000s;

	return TNONE;
}

unsigned int usec_to_ppoll_timeout( unsigned int usec )
{
	if( usec <= default_ppoll_usec_timeout ) return 0;
	else return usec_to_timeout( usec );
}

int internal_ibtmo( ibConf_t *conf, int timeout )
{
	if( timeout < TNONE || timeout > T1000s )
	{
		setIberr( EARG );
		return -1;
	}

	conf->settings.usec_timeout = timeout_to_usec( timeout );

	return 0;
}

int ibtmo(int ud, int v )
{
	ibConf_t *conf;
	int retval;

	conf = general_enter_library( ud, 1, 0 );
	if( conf == NULL )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	retval = internal_ibtmo( conf, v );
	if( retval < 0 )
	{
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );
	}

	return general_exit_library( ud, 0, 0, 0, 0, 0, 1 );
}

int set_timeout( const ibBoard_t *board, unsigned int usec_timeout)
{
       return ioctl( board->fileno, IBTMO, &usec_timeout);
}


