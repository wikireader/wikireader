/***************************************************************************
                          lib/ibDma.c
                             -------------------

    copyright            : (C) 2001,2002 by Frank Mori Hess
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

int ibdma( int ud, int v )
{
	ibConf_t *conf;
	ibBoard_t *board;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	board = interfaceBoard( conf );

	fprintf( stderr, "libgpib: ibdma() unimplemented!\n" );

	return exit_library( ud, 0 );
} /* ibdma */
