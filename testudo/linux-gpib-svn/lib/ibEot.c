/***************************************************************************
                          lib/ibEot.c
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

void internal_ibeot( ibConf_t *conf, int send_eoi )
{
	if(send_eoi)
		conf->settings.send_eoi = 1;
	else
		conf->settings.send_eoi = 0;
}

int ibeot( int ud, int send_eoi )
{
	ibConf_t *conf;

	conf = general_enter_library( ud, 1, 0 );
	if( conf == NULL )
		return general_exit_library( ud, 1, 0, 0, 0, 0, 1 );

	internal_ibeot( conf, send_eoi );

	return general_exit_library( ud, 0, 0, 0, 0, 0, 1 );
}
