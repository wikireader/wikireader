/***************************************************************************
                          lib/ibBoard.c
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

#include <stdio.h>
#include <stdlib.h>

#include "ib_internal.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>

ibBoard_t ibBoard[ GPIB_MAX_NUM_BOARDS ];

void init_ibboard(ibBoard_t *board)
{
	strcpy(board->board_type, "");
	board->base = 0;
	board->irq = 0;
	board->dma = 0;
	board->pci_bus = -1;
	board->pci_slot = -1;
	board->fileno = -1;
	strcpy(board->device, "");
	board->open_count = 0;
	board->is_system_controller = 0;
	board->use_event_queue = 0;
	board->autospoll = 0;
}

int configure_autospoll(ibConf_t *conf, int enable)
{
	autospoll_ioctl_t spoll_enable = enable != 0;
	int retval = 0;
	ibBoard_t *board = interfaceBoard(conf);

	if((spoll_enable && board->autospoll == 0) ||
		(spoll_enable == 0 && board->autospoll))
	{
		retval = ioctl(interfaceBoard(conf)->fileno, IBAUTOSPOLL, &spoll_enable);
		if(retval)
		{
			fprintf(stderr, "libgpib: autospoll ioctl returned error %i\n", retval);
		}else
		{
			board->autospoll = enable != 0;
		}
	}
	return retval;
}

int ibBoardOpen( ibBoard_t *board )
{
	int fd;
	int flags = 0;

	if( board->fileno >= 0 ) return 0;

	if( ( fd = open( board->device, O_RDWR | flags ) ) < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		fprintf( stderr, "libgpib: ibBoardOpen failed to open device file %s\n", board->device);
		perror( "libgpib" );
		return -1;
	}
	board->fileno = fd;
	board->open_count++;

	return 0;
}

int ibBoardClose( ibBoard_t *board )
{

	if( board->open_count == 0 )
	{
		fprintf( stderr, "libgpib: bug! board->open_count is zero on close\n");
		return -1;
	}

	board->open_count--;
	if( board->open_count > 0 )
		return 0;

	if( board->fileno >= 0 )
	{
		close( board->fileno );
		board->fileno = -1;
	}

	return 0;
}

int InternalResetSys( ibConf_t *conf, const Addr4882_t addressList[] )
{
	ibBoard_t *board;
	int retval;

	board = interfaceBoard( conf );

	if( addressListIsValid( addressList ) == 0 )
	{
		setIberr( EARG );
		return -1;
	}

	if( conf->is_interface == 0 )
	{
		setIberr( EDVR );
		return -1;
	}

	if( is_system_controller( board ) == 0 )
	{
		setIberr( ESAC );
		return -1;
	}

	if( is_cic( board ) == 0 )
	{
		setIberr( ECIC );
		return -1;
	}

	retval = remote_enable( board, 1 );
	if( retval < 0 ) return retval;

	retval = internal_ibsic( conf );
	if( retval < 0 ) return retval;

	retval = InternalDevClearList( conf, NULL );
	if( retval < 0 ) return retval;

	retval = InternalSendList( conf, addressList, "*RST", 4, NLend );
	if( retval < 0 ) return retval;

	return 0;
}

void ResetSys( int boardID, const Addr4882_t addressList[] )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	retval = InternalResetSys( conf, addressList );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	exit_library( boardID, 0 );

}







