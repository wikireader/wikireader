/***************************************************************************
                              gpib_config.c
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
#define _GNU_SOURCE

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include "ib_internal.h"


typedef struct
{
	char *config_file;
	char *device_file;
	unsigned int minor;
	char *board_type;
	int irq;
	uint64_t iobase;
	int dma;
	int pci_bus;
	int pci_slot;
	int pad;
	int sad;
	int assert_ifc;
	int assert_remote_enable;
	int offline;
	int is_system_controller;
	void *init_data;
	int init_data_length;
} parsed_options_t;

static void help( void )
{
	printf("gpib_config [options] - configures a GPIB interface board\n");
	printf("\t-t, --board-type BOARD_TYPE\n"
		"\t\tSet board type to BOARD_TYPE.\n");
	printf("\t-c, --device-file FILEPATH\n"
		"\t\tSpecify character device file path for the board.\n"
		"\t\tThis can be used as an alternative to the --minor option.\n");
	printf("\t-d, --dma NUM\n"
		"\t\tSpecify isa dma channel NUM for boards without plug-and-play cabability.\n");
	printf("\t-b, --iobase NUM\n"
		"\t\tSet io base address to NUM for boards without plug-and-play cabability.\n");
	printf("\t-I, --init-data FILE_PATH\n"
		"\t\tSpecify file containing binary initialization data (firmware) for board.\n");
	printf("\t-i, --irq NUM\n"
		"\t\tSpecify irq line NUM for boards without plug-and-play cabability.\n");
	printf("\t-f, --file FILEPATH\n"
		"\t\tSpecify file path for configuration file.  The values in the configuration\n"
		"\t\tfile will be used as defaults for unspecified options.  The default configuration\n"
		"\t\tfile is /etc/gpib.conf\n");
	printf("\t-h, --help\n"
		"\t\tPrint this help and exit.\n");
	printf("\t-m, --minor NUM\n"
		"\t\tConfigure gpib device file with minor number NUM (default 0).\n"
		"\t\tAlternatively, the device file may be specified with the --device-file option.\n");
	printf("\t--[no-]ifc\n"
		"\t\tPerform (or not) interface clear after bringing board online.  Default is --ifc.\n");
	printf("\t--[no-]sre\n"
		"\t\tAssert (or not) remote enable line after bringing board online.  Default is --sre.\n");
	printf("\t--[no-]system-controller\n"
		"\t\tConfigure board as system controller (or not).\n");
	printf("\t-o, --offline\n"
		"\t\tDon't bring board online.\n");
	printf("\t-p, --pad NUM\n"
		"\t\tSpecify primary gpib address.  NUM should be in the range 0 through 30.\n");
	printf("\t-u, --pci-bus NUM\n"
		"\t\tSpecify pci bus NUM to select a specific pci board.\n"
		"\t\tIf used, you must also specify the pci slot with --pci-slot.\n");
	printf("\t-l, --pci-slot NUM\n"
		"\t\tSpecify pci slot NUM to select a specific pci board.\n"
		"\t\tIf used, you must also specify the pci bus with --pci-bus.\n");
	printf("\t-s, --sad NUM\n"
		"\t\tSpecify secondary gpib address.  NUM should be 0 (disabled) or in the range\n"
		"\t\t96 through 126 (0x60 through 0x7e hexadecimal).\n");
}

static int load_init_data(parsed_options_t *settings, const char *file_path)
{
	int retval;
	FILE *init_file = fopen(file_path, "r");
	if(init_file)
	{
		struct stat file_status;
		if(fstat(fileno(init_file), &file_status) == 0)
		{
			settings->init_data = malloc(file_status.st_size);
			if(settings->init_data)
			{
				settings->init_data_length = fread(settings->init_data, 1, file_status.st_size, init_file);
				if(settings->init_data_length == file_status.st_size)
					retval = 0;
				else
				{
					settings->init_data_length = 0;
					free(settings->init_data);
					settings->init_data = NULL;
					fprintf(stderr, "fread() returned short read\n");
					retval = -EIO;
				}
			}else
			{
				fprintf(stderr, "malloc() failed.\n");
				perror(__FUNCTION__);
				retval = -errno;
			}
		}else
		{
			fprintf(stderr, "fstat() failed on file \'%s\'.\n", file_path);
			perror(__FUNCTION__);
			retval = -errno;
		}
	}else
	{
		fprintf(stderr, "Failed to open file \'%s\' for reading.\n", file_path);
		perror(__FUNCTION__);
		retval = -errno;
	}
	fclose(init_file);
	return retval;
}
static int parse_options( int argc, char *argv[], parsed_options_t *settings )
{
	int c, index;
	int retval;

	struct option options[] =
	{
		{ "iobase", required_argument, NULL, 'b' },
		{ "device-file", required_argument, NULL, 'c' },
		{ "dma", required_argument, NULL, 'd' },
		{ "file", required_argument, NULL, 'f' },
		{ "help", no_argument, NULL, 'h' },
		{ "init-data", required_argument, NULL, 'I' },
		{ "irq", required_argument, NULL, 'i' },
		{ "pci-slot", required_argument, NULL, 'l' },
		{ "minor", required_argument, NULL, 'm' },
		{ "offline", no_argument, NULL, 'o' },
		{ "pad", required_argument, NULL, 'p' },
		{ "sad", required_argument, NULL, 's' },
		{ "board-type", required_argument, NULL, 't' },
		{ "pci-bus", required_argument, NULL, 'u' },
		{ "no-ifc", no_argument, &settings->assert_ifc, 0 },
		{ "ifc", no_argument, &settings->assert_ifc, 1 },
		{ "no-sre", no_argument, &settings->assert_remote_enable, 0 },
		{ "sre", no_argument, &settings->assert_remote_enable, 1 },
		{ "no-system-controller", no_argument, &settings->is_system_controller, 0 },
		{ "system-controller", no_argument, &settings->is_system_controller, 1 },
		{ 0 },
	};

	memset(settings, 0, sizeof(parsed_options_t));
	settings->irq = -1;
	settings->iobase = -1;
	settings->dma = -1;
	settings->pci_bus = -1;
	settings->pci_slot = -1;
	settings->pad = -1;
	settings->sad = -1;
	settings->assert_ifc = 1;
	settings->assert_remote_enable = 1;
	settings->is_system_controller = -1;

	while( 1 )
	{
		c = getopt_long(argc, argv, "b:d:f:hi:I:l:m:op:s:t:u:", options, &index);
		if( c == -1 ) break;
		switch( c )
		{
		case 0:
			break;
		case 'b':
			settings->iobase = strtol( optarg, NULL, 0 );
			break;
		case 'c' :
			free(settings->config_file);
			settings->device_file = strdup( optarg );
			break;
		case 'd':
			settings->dma = strtol( optarg, NULL, 0 );
			break;
		case 'f':
			free(settings->config_file);
			settings->config_file = strdup( optarg );
			break;
		case 'h':
			help();
			exit( 0 );
			break;
		case 'I':
			retval = load_init_data(settings, optarg);
			if(retval < 0)
				return retval;
			break;
		case 'i':
			settings->irq = strtol( optarg, NULL, 0 );
			break;
		case 'l':
			settings->pci_slot = strtol( optarg, NULL, 0 );
			break;
		case 'm':
			settings->minor = strtol( optarg, NULL, 0 );
			break;
		case 'o':
			settings->offline = 1;
			break;
		case 'p':
			settings->pad = strtol( optarg, NULL, 0 );
			break;
		case 's':
			settings->sad = strtol( optarg, NULL, 0 );
			settings->sad -= sad_offset;
			break;
		case 't':
			free(settings->board_type);
			settings->board_type = strdup( optarg );
			break;
		case 'u':
			settings->pci_bus = strtol( optarg, NULL, 0 );
			break;
		default:
			help();
			exit(1);
		}
	}
	if(settings->device_file)
	{
		struct stat file_stats;
		if( stat( settings->device_file, &file_stats ) < 0 )
		{
			fprintf(stderr, "Failed to get file information on file \"%s\".\n", settings->device_file);
			perror(__FUNCTION__);
			return -errno;
		}
		if(S_ISCHR(file_stats.st_mode) == 0)
		{
			fprintf(stderr, "The device file \"%s\" is not a character device.\n", settings->device_file);
			return -EINVAL;
		}
		settings->minor = minor( file_stats.st_rdev );
	}else
	{
		if(asprintf(&settings->device_file , "/dev/gpib%i", settings->minor) < 0)
		{
			return -ENOMEM;
		}
	}
	return 0;
}

static int configure_board( int fileno, const parsed_options_t *options )
{
	board_type_ioctl_t boardtype;
	select_pci_ioctl_t pci_selection;
	pad_ioctl_t pad_cmd;
	sad_ioctl_t sad_cmd;
	online_ioctl_t online_cmd;
	int retval;

	online_cmd.online = 0;
	online_cmd.init_data_ptr = 0;
	online_cmd.init_data_length = 0;
	retval = ioctl( fileno, IBONL, &online_cmd );
	if( retval < 0 )
	{
		fprintf( stderr, "failed to bring board offline\n" );
		return retval;
	}
	if( options->offline != 0 )
		return 0;
	strncpy( boardtype.name, options->board_type, sizeof( boardtype.name ) );
	retval = ioctl( fileno, CFCBOARDTYPE, &boardtype );
	if( retval < 0 )
	{
		fprintf(stderr, "failed to configure boardtype: %s\n", boardtype.name);
		return retval;
	}
	retval = ioctl( fileno, CFCBASE, &options->iobase );
	if( retval < 0 )
	{
		fprintf(stderr, "failed to configure base address\n");
		return retval;
	}
	retval = ioctl( fileno, CFCIRQ, &options->irq );
	if( retval < 0 )
	{
		fprintf(stderr, "failed to configure irq\n");
		return retval;
	}
	retval = ioctl( fileno, CFCDMA, &options->dma );
	if( retval < 0 )
	{
		fprintf(stderr, "failed to configure dma channel\n");
		return retval;
	}
	pad_cmd.handle = 0;
	pad_cmd.pad = options->pad;
	retval = ioctl( fileno, IBPAD, &pad_cmd );
	if( retval < 0 )
	{
		fprintf(stderr, "failed to configure pad\n");
		return retval;
	}
	sad_cmd.handle = 0;
	sad_cmd.sad = options->sad;
	retval = ioctl( fileno, IBSAD, &sad_cmd );
	if( retval < 0 )
	{
		fprintf(stderr, "failed to configure sad\n");
		return retval;
	}
	pci_selection.pci_bus = options->pci_bus;
	pci_selection.pci_slot = options->pci_slot;
	retval = ioctl( fileno, IBSELECT_PCI, &pci_selection );
	if( retval < 0 )
	{
		fprintf(stderr, "failed to configure pci bus\n");
		return retval;
	}
	online_cmd.online = 1;
	assert(sizeof(options->init_data) <= sizeof(online_cmd.init_data_ptr));
	online_cmd.init_data_ptr = (uintptr_t)options->init_data;
	online_cmd.init_data_length = options->init_data_length;
	retval = ioctl( fileno, IBONL, &online_cmd );
	if( retval < 0 )
	{
		fprintf( stderr, "failed to bring board online\n" );
		return retval;
	}

	retval = ibrsc( options->minor, options->is_system_controller );
	if( retval & ERR )
	{
		fprintf( stderr, "failed to request/release system control\n" );
		return -1;
	}
	if( options->is_system_controller )
	{
		if( options->assert_ifc )
		{
			retval = ibsic( options->minor );
			if( retval & ERR )
			{
				fprintf( stderr, "failed to assert interface clear\n" );
				return -1;
			}
		}
		if( options->assert_remote_enable )
		{
			retval = ibsre( options->minor, 1 );
			if( retval & ERR )
			{
				fprintf( stderr, "failed to assert remote enable\n" );
				return -1;
			}
		}
	}

	return 0;
}

int main( int argc, char *argv[] )
{
	ibConf_t configs[ FIND_CONFIGS_LENGTH ];
	ibBoard_t boards[ GPIB_MAX_NUM_BOARDS ];
	char *filename, *envptr;
	int retval;
	parsed_options_t options;
	ibBoard_t *board;
	ibConf_t *conf = NULL;
	int i;

	retval = parse_options( argc, argv, &options );
	if(retval < 0)
	{
		fprintf( stderr, "failed to parse command line options." );
		return retval;
	};

	envptr = getenv( "IB_CONFIG" );
	if( options.config_file ) filename = options.config_file;
	else if( envptr ) filename = envptr;
	else filename = DEFAULT_CONFIG_FILE;

	retval = parse_gpib_conf( filename, configs, FIND_CONFIGS_LENGTH,
		boards, GPIB_MAX_NUM_BOARDS );
	if( retval < 0 )
	{
		fprintf( stderr, "failed to parse config file %s\n", filename );
		return retval;
	}

	if( options.minor >= GPIB_MAX_NUM_BOARDS )
	{
		fprintf( stderr, "minor number %i out of range\n", options.minor );
		return -1;
	}

	for( i = 0; i < FIND_CONFIGS_LENGTH; i++ )
	{
		if( configs[ i ].is_interface == 0 ) continue;
		if( configs[ i ].settings.board != options.minor ) continue;
		conf = &configs[ i ];
		break;
	}

	board = &boards[ options.minor ];

	if( options.board_type == NULL )
	{
		options.board_type = strdup( board->board_type );
		if( options.board_type == NULL )
			abort();
	}
	if( options.irq < 0 )
		options.irq = board->irq;
	if( options.iobase < 0 )
		options.iobase = board->base;
	if( options.dma < 0 )
		options.dma = board->dma;
	if( options.pci_bus < 0 )
		options.pci_bus = board->pci_bus;
	if( options.pci_slot < 0 )
		options.pci_slot = board->pci_slot;
	if( options.pad < 0 )
	{
		if( conf != NULL )
			options.pad = conf->settings.pad;
		else
			options.pad = 0;
	}
	if( options.sad < 0 )
	{
		if( conf != NULL )
			options.sad = conf->settings.sad;
		else
			options.sad = -1;
	}
	if( options.is_system_controller < 0 )
		options.is_system_controller = board->is_system_controller;
	board->fileno = open( options.device_file, O_RDWR );
	if( board->fileno < 0 )
	{
		fprintf( stderr, "failed to open device file '%s'\n", options.device_file );
		perror( __FUNCTION__ );
		return board->fileno;
	}
	retval = configure_board( board->fileno, &options );
	if( retval < 0 )
	{
		fprintf( stderr, "failed to configure board\n" );
		perror( __FUNCTION__ );
		return retval;
	}
	close( board->fileno );
	board->fileno = -1;
	free( options.init_data );
	free( options.device_file );
	free( options.config_file );
	free( options.board_type );

	return 0;
}
