/***************************************************************************
                          lib/ibFind.c
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
#include <string.h>
#include <stdlib.h>

int ibfind( const char *dev )
{
	int index;
	int retval;
	int ud;
	ibConf_t *conf;
	int status;

	retval = ibParseConfigFile();
	if(retval < 0)
	{
		setIberr( EDVR );
		setIbsta( ERR );
		return -1;
	}

	if( ( index = ibFindDevIndex( dev ) ) < 0 )
	{ /* find desired entry */
		setIberr( EDVR );
		setIbsta( ERR );
		return -1;
	}

	conf = &ibFindConfigs[ index ];

	ud = my_ibdev( *conf );
	if(ud < 0)
	{
		fprintf(stderr, "libgpib: ibfind failed to get descriptor\n");
		return -1;
	}
	conf = general_enter_library( ud, 1, 0 );

	if(conf->flags & CN_SDCL)
	{
		status = ibclr(ud);
		if( status & ERR ) return -1;
	}

	if(strcmp(conf->init_string, ""))
	{
		status = ibwrt(ud, conf->init_string, strlen(conf->init_string));
		if( status & ERR )
			return -1;
	}

	return ud;
}











