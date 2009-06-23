/***************************************************************************
                          lib/ibutil.c
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

#define _GNU_SOURCE

#include "ib_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "parse.h"

ibConf_t *ibConfigs[ GPIB_CONFIGS_LENGTH ] = {NULL};
ibConf_t ibFindConfigs[ FIND_CONFIGS_LENGTH ];

int insert_descriptor( ibConf_t p, int ud )
{
	int i;

	if( ud < 0 )
	{
		for( i = GPIB_MAX_NUM_BOARDS; i < GPIB_CONFIGS_LENGTH; i++ )
		{
			if( ibConfigs[ i ] == NULL ) break;
		}
		if( i == GPIB_CONFIGS_LENGTH )
		{
			fprintf( stderr, "libgpib: out of room in ibConfigs[]\n" );
			setIberr( ENEB ); // ETAB?
			return -1;
		}
		ud = i;
	}else
	{
		if( ud >= GPIB_CONFIGS_LENGTH )
		{
			fprintf( stderr, "libgpib: bug! tried to allocate past end if ibConfigs array\n" );
			setIberr( EDVR );
			setIbcnt( EINVAL );
			return -1;
		}
		if( ibConfigs[ ud ] )
		{
			fprintf( stderr, "libgpib: bug! tried to allocate board descriptor twice\n" );
			setIberr( EDVR );
			setIbcnt( EINVAL );
			return -1;
		}
	}
	ibConfigs[ ud ] = malloc( sizeof( ibConf_t ) );
	if( ibConfigs[ ud ] == NULL )
	{
		fprintf( stderr, "libgpib: out of memory\n" );
		setIberr( EDVR );
		setIbcnt( ENOMEM );
		return -1;
	}
	init_ibconf(ibConfigs[ ud ]);
	/* put entry to the table */
	*ibConfigs[ud] = p;

	return ud;
}

int setup_global_board_descriptors( void )
{
	int i;
	int retval = 0;

	for( i = 0; i < FIND_CONFIGS_LENGTH; i++ )
	{
		if(ibFindConfigs[ i ].is_interface && ibFindConfigs[ i ].settings.board >= 0 &&
			ibFindConfigs[ i ].settings.board < GPIB_MAX_NUM_BOARDS)
		{
			if(insert_descriptor( ibFindConfigs[ i ], ibFindConfigs[ i ].settings.board) < 0 )
			{
				retval = -1;
			}
		}
	}
	/* boards use handle 0 */
	for( i = 0; i < GPIB_MAX_NUM_BOARDS; i++ )
		if( ibConfigs[ i ] )
			ibConfigs[ i ]->handle = 0;
	return retval;
}

static pthread_mutex_t config_lock = PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP;

static void gpib_atfork_prepare(void)
{
	int i;

	for(i = 0; i < GPIB_CONFIGS_LENGTH; i++)
		if(ibConfigs[i])
		{
			pthread_mutex_lock(&ibConfigs[i]->async.lock);
			pthread_mutex_lock(&ibConfigs[i]->async.join_lock);
		}
	pthread_mutex_lock(&config_lock);
}

static void gpib_atfork_parent(void)
{
	int i;

	pthread_mutex_unlock(&config_lock);
	for(i = 0; i < GPIB_CONFIGS_LENGTH; i++)
		if(ibConfigs[i])
		{
			pthread_mutex_unlock(&ibConfigs[i]->async.join_lock);
			pthread_mutex_unlock(&ibConfigs[i]->async.lock);
		}
}

static void gpib_atfork_child(void)
{
	int i;

	pthread_mutex_init(&config_lock, NULL);
	for(i = 0; i < GPIB_CONFIGS_LENGTH; i++)
		if(ibConfigs[i])
		{
			pthread_mutex_init(&ibConfigs[i]->async.join_lock, NULL);
			pthread_mutex_init(&ibConfigs[i]->async.lock, NULL);
		}
}

int ibParseConfigFile( void )
{
	int retval = 0;
	static volatile int config_parsed = 0;
	char *filename, *envptr;

	pthread_mutex_lock( &config_lock );
	if( config_parsed )
	{
		pthread_mutex_unlock( &config_lock );
		return 0;
	}

	envptr = getenv("IB_CONFIG");
	if( envptr ) filename = envptr;
	else filename = DEFAULT_CONFIG_FILE;

	retval = parse_gpib_conf( filename, ibFindConfigs, FIND_CONFIGS_LENGTH,
		ibBoard, GPIB_MAX_NUM_BOARDS );
	if( retval < 0 )
	{
		pthread_mutex_unlock( &config_lock );
		return retval;
	}
	retval = setup_global_board_descriptors();

	config_parsed = 1;
	/* be extra safe about dealing with forks */
	pthread_atfork(gpib_atfork_prepare, gpib_atfork_parent,
		gpib_atfork_child);
	pthread_mutex_unlock( &config_lock );

	return retval;
}

/**********************************************************************/

int ibGetDescriptor( ibConf_t p )
{
	int retval;

	/* XXX should go somewhere else XXX check validity of values */
	if(p.settings.pad > gpib_addr_max || p.settings.sad > gpib_addr_max)
	{
		setIberr( ETAB );
		return -1;
	}

	retval = insert_descriptor( p, -1 );
	if( retval < 0 )
		return retval;

	return retval;
}

int ibFindDevIndex( const char *name )
{
	int i;

	if( strcmp( "", name ) == 0 ) return -1;

	for(i = 0; i < FIND_CONFIGS_LENGTH; i++)
	{
		if(!strcmp(ibFindConfigs[i].name, name)) return i;
	}

	return -1;
}

static int ibCheckDescriptor( int ud )
{
	if( ud < 0 || ud >= GPIB_CONFIGS_LENGTH || ibConfigs[ud] == NULL )
	{
		fprintf( stderr, "libgpib: invalid descriptor\n" );
		setIberr( EDVR );
		setIbcnt( EINVAL );
		return -1;
	}

	return 0;
}

void init_descriptor_settings( descriptor_settings_t *settings )
{
	settings->pad = -1;
	settings->sad = -1;
	settings->board = -1;
	settings->usec_timeout = 3000000;
	settings->spoll_usec_timeout = 1000000;
	settings->ppoll_usec_timeout = 2;
	settings->eos = 0;
	settings->eos_flags = 0;
	settings->ppoll_config = 0;
	settings->send_eoi = 1;
	settings->local_lockout = 0;
	settings->local_ppc = 0;
	settings->readdr = 0;
}

void init_ibconf( ibConf_t *conf )
{
	conf->handle = -1;
	memset(conf->name, 0, sizeof(conf->name));
	init_descriptor_settings( &conf->defaults );
	init_descriptor_settings( &conf->settings );
	memset(conf->init_string, 0, sizeof(conf->init_string));
	conf->flags = 0;
	init_async_op( &conf->async );
	conf->end = 0;
	conf->is_interface = 0;
	conf->board_is_open = 0;
	conf->has_lock = 0;
	conf->timed_out = 0;
}

int open_gpib_handle( ibConf_t *conf )
{
	open_dev_ioctl_t open_cmd;
	int retval;
	ibBoard_t *board;

	if( conf->handle >= 0 ) return 0;

	board = interfaceBoard( conf );

	open_cmd.handle = -1;
	open_cmd.pad = conf->settings.pad;
	open_cmd.sad = conf->settings.sad;
	open_cmd.is_board = conf->is_interface;
	retval = ioctl( board->fileno, IBOPENDEV, &open_cmd );
	if( retval < 0 )
	{
		fprintf( stderr, "libgpib: IBOPENDEV ioctl failed\n" );
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	conf->handle = open_cmd.handle;

	return 0;
}

int close_gpib_handle( ibConf_t *conf )
{
	close_dev_ioctl_t close_cmd;
	int retval;
	ibBoard_t *board;

	if( conf->handle < 0 ) return 0;

	board = interfaceBoard( conf );

	close_cmd.handle = conf->handle;
	retval = ioctl( board->fileno, IBCLOSEDEV, &close_cmd );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	conf->handle = -1;

	return 0;
}

int gpibi_change_address( ibConf_t *conf, unsigned int pad, int sad )
{
	int retval;
	ibBoard_t *board;
	pad_ioctl_t pad_cmd;
	sad_ioctl_t sad_cmd;

	board = interfaceBoard( conf );

	pad_cmd.handle = conf->handle;
	pad_cmd.pad = pad;
	retval = ioctl( board->fileno, IBPAD, &pad_cmd );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	sad_cmd.handle = conf->handle;
	sad_cmd.sad = sad;
	retval = ioctl( board->fileno, IBSAD, &sad_cmd );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		return retval;
	}

	conf->settings.pad = pad;
	conf->settings.sad = sad;

	return 0;
}

int lock_board_mutex( ibBoard_t *board )
{
	static const int lock = 1;
	int retval;

	retval = ioctl( board->fileno, IBMUTEX, &lock );
	if( retval < 0 )
	{
		fprintf( stderr, "libgpib: error locking board mutex!\n");
		setIberr( EDVR );
		setIbcnt( errno );
	}

	return retval;
}

int unlock_board_mutex( ibBoard_t *board )
{
	static const int unlock = 0;
	int retval;

	retval = ioctl( board->fileno, IBMUTEX, &unlock );
	if( retval < 0 )
	{
		fprintf( stderr, "libgpib: error unlocking board mutex!\n");
		setIberr( EDVR );
		setIbcnt( errno );
	}
	return retval;
}

int conf_lock_board( ibConf_t *conf )
{
	ibBoard_t *board;
	int retval;

	board = interfaceBoard( conf );

	assert( conf->has_lock == 0 );

	retval = lock_board_mutex( board );
	if( retval < 0 ) return retval;

	conf->has_lock = 1;

	return retval;
}

void conf_unlock_board( ibConf_t *conf )
{
	ibBoard_t *board;
	int retval;

	board = interfaceBoard( conf );

	assert( conf->has_lock );

	conf->has_lock = 0;

	retval = unlock_board_mutex( board );
	assert( retval == 0 );
}

ibConf_t * enter_library( int ud )
{
	return general_enter_library( ud, 0, 0 );
}

ibConf_t * general_enter_library( int ud, int no_lock_board, int ignore_eoip )
{
	ibConf_t *conf;
	ibBoard_t *board;
	int retval;

	retval = ibParseConfigFile();
	if(retval < 0)
	{
		return NULL;
	}

	setIberr( 0 );
	setIbcnt( 0 );

	if( ibCheckDescriptor( ud ) < 0 )
	{
		return NULL;
	}
	conf = ibConfigs[ ud ];

	retval = conf_online( conf, 1 );
	if( retval < 0 ) return NULL;

	conf->timed_out = 0;

	board = interfaceBoard( conf );

	if( no_lock_board == 0 )
	{
		if( ignore_eoip == 0 )
		{
			pthread_mutex_lock( &conf->async.lock );
			if( conf->async.in_progress )
			{
				pthread_mutex_unlock( &conf->async.lock );
				setIberr( EOIP );
				return NULL;
			}
			pthread_mutex_unlock( &conf->async.lock );
		}

		retval = conf_lock_board( conf );
		if( retval < 0 )
		{
			return NULL;
		}
	}

	return conf;
}

int ibstatus( ibConf_t *conf, int error, int clear_mask, int set_mask )
{
	int status = 0;
	int retval;

	retval = my_wait( conf, 0, clear_mask, set_mask, &status);
	if( retval < 0 ) error = 1;

	if( error ) status |= ERR;
	if( conf->timed_out )
		status |= TIMO;
	if( conf->end )
		status |= END;

	setIbsta( status );

	return status;
}

int exit_library( int ud, int error )
{
	return general_exit_library( ud, error, 0, 0, 0, 0, 0 );
}

int general_exit_library( int ud, int error, int no_sync_globals, int no_update_ibsta,
	int status_clear_mask, int status_set_mask, int no_unlock_board )
{
	ibConf_t *conf = ibConfigs[ ud ];
	ibBoard_t *board;
	int status;

	if( ibCheckDescriptor( ud ) < 0 )
	{
		setIbsta( ERR );
		if( no_sync_globals == 0 )
			sync_globals();
		return ERR;
	}

	board = interfaceBoard( conf );

	if( no_update_ibsta )
		status = ThreadIbsta();
	else
		status = ibstatus( conf, error, status_clear_mask, status_set_mask );

	if( no_unlock_board == 0 && conf->has_lock )
		conf_unlock_board( conf );

	if( no_sync_globals == 0 )
		sync_globals();

	return status;
}

int extractPAD( Addr4882_t address )
{
	int pad = address & 0xff;

	if( address == NOADDR ) return ADDR_INVALID;

	if( pad < 0 || pad > gpib_addr_max ) return ADDR_INVALID;

	return pad;
}

int extractSAD( Addr4882_t address )
{
	int sad = ( address >> 8 ) & 0xff;

	if( address == NOADDR ) return ADDR_INVALID;

	if( sad == NO_SAD ) return SAD_DISABLED;

	if( ( sad & 0x60 ) == 0 ) return ADDR_INVALID;

	sad &= ~0x60;

	if( sad < 0 || sad > gpib_addr_max ) return ADDR_INVALID;

	return sad;
}

Addr4882_t packAddress( unsigned int pad, int sad )
{
	Addr4882_t address;

	address = 0;
	address |= pad & 0xff;
	if( sad >= 0 )
		address |= ( ( sad | sad_offset ) << 8 ) & 0xff00;

	return address;
}

int addressIsValid( Addr4882_t address )
{
	if( address == NOADDR ) return 1;

	if( extractPAD( address ) == ADDR_INVALID ||
		extractSAD( address ) == ADDR_INVALID )
	{
		setIberr( EARG );
		return 0;
	}

	return 1;
}

int addressListIsValid( const Addr4882_t addressList[] )
{
	int i;

	if( addressList == NULL ) return 1;

	for( i = 0; addressList[ i ] != NOADDR; i++ )
	{
		if( addressIsValid( addressList[ i ] ) == 0 )
		{
			setIbcnt( i );
			return 0;
		}
	}

	return 1;
}

unsigned int numAddresses( const Addr4882_t addressList[] )
{
	unsigned int count;

	if( addressList == NULL )
		return 0;

	count = 0;
	while( addressList[ count ] != NOADDR )
	{
		count++;
	}

	return count;
}

int is_cic( const ibBoard_t *board )
{
	int retval;
	wait_ioctl_t cmd;

	cmd.usec_timeout = 0;
	cmd.wait_mask = 0;
	cmd.clear_mask = 0;
	cmd.set_mask = 0;
	cmd.pad = NOADDR;
	cmd.sad = NOADDR;
	cmd.handle = 0;
	cmd.ibsta = 0;
	retval = ioctl( board->fileno, IBWAIT, &cmd );
	if( retval < 0 )
	{
		setIberr( EDVR );
		setIbcnt( errno );
		fprintf( stderr, "libgpib: error in is_cic()!\n");
		return -1;
	}

	if( cmd.ibsta & CIC )
		return 1;

	return 0;
}

int is_system_controller( const ibBoard_t *board )
{
	int retval;
	board_info_ioctl_t info;

	retval = ioctl( board->fileno, IBBOARD_INFO, &info );
	if( retval < 0 )
	{
		fprintf( stderr, "libgpib: error in is_system_controller()!\n");
		return retval;
	}

	return info.is_system_controller;
}

const char* gpib_error_string( int error )
{
	static const char* error_descriptions[] =
	{
		"EDVR 0: OS error",
		"ECIC 1: Board not controller in charge",
		"ENOL 2: No listeners",
		"EADR 3: Improper addressing",
		"EARG 4: Bad argument",
		"ESAC 5: Board not system controller",
		"EABO 6: Operation aborted",
		"ENEB 7: Non-existant board",
		"EDMA 8: DMA error",
		"libgpib: Unknown error code 9",
		"EOIP 10: IO operation in progress",
		"ECAP 11: Capability does not exist",
		"EFSO 12: File system error",
		"libgpib: Unknown error code 13",
		"EBUS 14: Bus error",
		"ESTB 15: Lost status byte",
		"ESRQ 16: Stuck service request",
		"libgpib: Unknown error code 17",
		"libgpib: Unknown error code 18",
		"libgpib: Unknown error code 19",
		"ETAB 20: Table problem",
	};
	static const int max_error_code = ETAB;

	if( error < 0 || error > max_error_code )
		return "libgpib: Unknown error code";

	return error_descriptions[ error ];
}
