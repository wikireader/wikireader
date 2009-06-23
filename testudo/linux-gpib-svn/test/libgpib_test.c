/***************************************************************************
                             libgpib_test.c
                             -------------------

Test program for libgpib.  Requires two gpib boards installed in the
computer, on the same GPIB bus, and one of which is the system controller.

    copyright            : (C) 2003 by Frank Mori Hess
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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "gpib/ib.h"

struct program_options
{
	int num_loops;
	int master;
	int pad;
	int sad;
	int verbosity;
	int minor;
};

#define PRINT_FAILED() \
	fprintf( stderr, "FAILED: %s line %i, ibsta 0x%x, iberr %i, ibcntl %li\n", \
		__FILE__, __LINE__, ThreadIbsta(), ThreadIberr(), ThreadIbcntl() ); \

static int sync_message(const struct program_options *options, const char *message)
{
	char buffer[1024] = {0};
	if(options->verbosity)
	{
		fprintf(stderr, "syncing: master=%i, message=%s\n", options->master, message);
	}
	if(options->master)
	{
		fprintf(stdout, "%s\n", message);
		fflush(stdout);
	}
	fgets(buffer, sizeof(buffer), stdin);
	if(strncmp(message, buffer, strlen(message)))
	{
		fprintf(stderr, "received unexpected sync message: %s\n", buffer);
		fprintf(stderr, "expected: %s\n", message);
		PRINT_FAILED();
		return -1;
	}
	if(options->master == 0)
	{
		// echo back
		fprintf(stdout, "%s\n", message);
		fflush(stdout);
	}
	return 0;
}

static int init_board(int board_desc, const struct program_options *options)
{
	int status;

	if(options->master == 0)
	{
		if(ibpad(board_desc, options->pad) & ERR)
		{
			PRINT_FAILED();
			return -1;
		}
		if(options->sad >= 0)
		{
			if(ibsad(board_desc, MSA(options->sad)) & ERR)
			{
				PRINT_FAILED();
				return -1;
			}
		}
	}
	ibtmo( board_desc, T30s );
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}

	ibeot( board_desc, 1 );
	if( ibsta & ERR )
	{
		PRINT_FAILED();
		return -1;
	}

	status = ibeos( board_desc, 0 );
	if( status & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	if( status != ThreadIbsta() || ThreadIbsta() != ibsta )
	{
		PRINT_FAILED();
		return -1;
	}
	// for parallel poll test
	if(options->master == 0)
	{
		ibist(board_desc, 1);
		if( ThreadIbsta() & ERR )
		{
			PRINT_FAILED();
			return -1;
		}
	}
	return 0;
}

static int find_board(int *board, const struct program_options *options)
{
	int i;
	int status, result;

	fprintf( stderr, "Finding board..." );
	for( i = 0; i < GPIB_MAX_NUM_BOARDS; i++ )
	{
		if(options->minor >= 0 && i != options->minor)
			continue;
		if(options->verbosity)
			fprintf(stderr, "\tchecking board %i\n", i);
		status = ibask( i, IbaSC, &result );
		if( status & ERR )
			continue;
		if(options->master && result != 0 )
		{
			*board = i;
			break;
		}else if(options->master == 0 && result == 0)
		{
			*board = i;
			break;
		}
	}
	if(i == GPIB_MAX_NUM_BOARDS)
	{
		PRINT_FAILED();
		return -1;
	}
	if(options->verbosity)
		fprintf(stderr, "\tfound board %i\n", i);
	if(init_board(*board, options))
	{
		PRINT_FAILED();
		return -1;
	}
	fprintf( stderr, "OK\n" );
	return 0;
}

Addr4882_t slaveAddress(const struct program_options *options)
{
	Addr4882_t address;
	int sad;
	if(options->sad >= 0)
		sad = MSA(options->sad);
	else
		sad = 0;
	address = MakeAddr(options->pad, sad);
	return address;
}

static int open_slave_device_descriptor(int board, const struct program_options *options,
	int timeout, int eot, int eos )
{
	int sad;
	int ud;

	if(options->sad >= 0)
		sad = MSA(options->sad);
	else
		sad = 0;
	ud = ibdev( board, options->pad, sad, timeout, eot, eos );
	if( ud < 0 )
	{
		PRINT_FAILED();
	}
	return ud;
}

static const char read_write_string1[] = "dig8esdfas sdf\n";
static const char read_write_string2[] = "DFLIJFES8F3	";

static int master_read_write_test(int board, const struct program_options *options)
{
	int ud;
	int status;
	char buffer[1000];
	int i;

	fprintf( stderr, "%s...", __FUNCTION__ );
	ud = open_slave_device_descriptor( board, options, T30s, 1, 0 );
	if( ud < 0 )
		return -1;
	for( i = 0; i < 2; i++ )
	{
		if(options->verbosity)
			fprintf(stderr, "\tloop %i\n", i);
		status = ibwrt( ud, read_write_string1, strlen( read_write_string1 ) + 1 );
		if( ( status & ERR ) || !( status & CMPL ) )
		{
			PRINT_FAILED();
			fprintf(stderr, "\tloop %i\n", i);
			ibonl( ud, 0 );
			return -1;
		}
		memset( buffer, 0, sizeof( buffer ) );
		status = ibrd(ud, buffer, sizeof(buffer) - 1);
		if( ( status & ERR ) || !( status & CMPL ) || !( status & END ) )
		{
			PRINT_FAILED();
			fprintf( stderr, "received bytes:\"%s\"\n", buffer );
			fprintf(stderr, "loop %i\n", i);
			ibonl( ud, 0 );
			return -1;
		}
		if(strcmp(buffer, read_write_string2))
		{
			PRINT_FAILED();
			fprintf( stderr, "received bytes:\"%s\"\n", buffer );
			fprintf(stderr, "\tloop %i\n", i);
			ibonl( ud, 0 );
			return -1;
		}
	}
	ibonl( ud, 0 );
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	fprintf( stderr, "OK\n" );
	return 0;
}

static int slave_read_write_test(int board, const struct program_options *options)
{
	char buffer[ 1000 ];
	int status;
	int i;

	fprintf( stderr, "%s...", __FUNCTION__ );
	for( i = 0; i < 2; i++ )
	{
		memset( buffer, 0, sizeof( buffer ) );
		status = ibrd(board, buffer, sizeof( buffer ) );
		if(status & ERR)
		{
			PRINT_FAILED();
			fprintf(stderr, "loop %i\n", i);
			return -1;
		}
		if(strcmp(buffer, read_write_string1))
		{
			PRINT_FAILED();
			fprintf( stderr, "got bad data from ibrd\n" );
			fprintf( stderr, "received %i bytes:%s\n", ThreadIbcnt(), buffer );
			fprintf(stderr, "loop %i\n", i);
			return -1;
		}
		status = ibwrt(board, read_write_string2, strlen(read_write_string2) + 1);
		if( status & ERR )
		{
			PRINT_FAILED();
			fprintf(stderr, "loop %i\n", i);
			return -1;
		}
	}
	fprintf( stderr, "OK\n" );
	return 0;
}

static int master_async_read_write_test(int board, const struct program_options *options)
{
	int ud;
	int i;
	int status;

	fprintf( stderr, "%s...", __FUNCTION__ );
	ud = open_slave_device_descriptor(board, options, T3s, 1, 0);
	if( ud < 0 )
		return -1;
	for( i = 0; i < 2; i++ )
	{
		status = ibwrta( ud, read_write_string1, strlen( read_write_string1 ) + 1 );
		if( status & ERR )
		{
			PRINT_FAILED();
			ibonl( ud, 0 );
			return -1;
		}
		status = ibwait(ud, CMPL | TIMO);
		if((status & (ERR | TIMO)) || !(status & CMPL) )
		{
			PRINT_FAILED();
			fprintf( stderr, "loop %i, write status 0x%x, error %i\n", i, ThreadIbsta(),
				ThreadIberr() );
			ibonl( ud, 0 );
			return -1;
		}
	}
	ibonl( ud, 0 );
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	fprintf( stderr, "OK\n" );
	return 0;
}

static int slave_async_read_write_test(int board, const struct program_options *options)
{
	char buffer[ 1000 ];
	int i;
	int status;

	fprintf( stderr, "%s...", __FUNCTION__ );
	for( i = 0; i < 2; i++ )
	{
		memset( buffer, 0, sizeof( buffer ) );
		status = ibrda( board, buffer, sizeof( buffer ) );
		if( status & ERR )
		{
			PRINT_FAILED();
			fprintf( stderr, "read error %i\n", ThreadIberr() );
			return -1;
		}
		status = ibwait( board, CMPL | TIMO );
		if((status & (ERR | TIMO)) || !(status & CMPL))
		{
			PRINT_FAILED();
			fprintf( stderr, "loop %i, read status 0x%x, error %i\n", i, ThreadIbsta(),
				ThreadIberr() );
			return -1;
		}
		if(strcmp(buffer, read_write_string1))
		{
			PRINT_FAILED();
			fprintf( stderr, "loop %i got bad data from ibrd\n", i);
			fprintf( stderr, "received %i bytes:%s\n", ThreadIbcnt(), buffer );
			return -1;
		}
	}
	fprintf( stderr, "OK\n" );
	return 0;
}

const int status_byte = 0x43;
static int master_serial_poll_test(int board, const struct program_options *options)
{
	int ud;
	char result;

	fprintf( stderr, "%s...", __FUNCTION__ );
	ud = open_slave_device_descriptor(board, options, T3s, 1, 0 );
	if( ud < 0 )
		return -1;
	/* make sure status queue is empty */
	while( ibwait( ud, 0 ) & RQS )
	{
		ibrsp( ud, &result );
		if( ThreadIbsta() & ERR )
		{
			PRINT_FAILED();
			ibonl( ud, 0 );
			return -1;
		}
	}
	if(ibconfig(board, IbcAUTOPOLL, 1 ) & ERR)
	{
		PRINT_FAILED();
		ibonl( ud, 0 );
		return -1;
	}
	if(sync_message(options, "request service"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	ibwait(ud, RQS | TIMO);
	if((ThreadIbsta() & (ERR | TIMO)) || !(ThreadIbsta() & RQS))
	{
		PRINT_FAILED();
		ibonl( ud, 0 );
		return -1;
	}
	result = 0;
	ibrsp(ud, &result);
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		ibonl( ud, 0 );
		return -1;
	}
	if((result & 0xff) != status_byte)
	{
		PRINT_FAILED();
		ibonl( ud, 0 );
		return -1;
	}
	ibonl( ud, 0 );
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	fprintf( stderr, "OK\n" );

	return 0;
}

static int slave_serial_poll_test(int board, const struct program_options *options)
{
	fprintf( stderr, "%s...", __FUNCTION__ );

	if(sync_message(options, "request service"))
	{
		PRINT_FAILED();
		return -1;
	}
	ibrsv(board, status_byte);
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	fprintf( stderr, "OK\n" );
	return 0;
}

static int master_parallel_poll_test(int board, const struct program_options *options)
{
	int ud;
	char result;
	int line, sense;

	fprintf( stderr, "%s...", __FUNCTION__ );
	ud = open_slave_device_descriptor(board, options, T3s, 1, 0 );
	if( ud < 0 )
		return -1;
	if(sync_message(options, "ist is 1"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	line = 4; sense = 1;
	ibppc(ud, PPE_byte(line, sense));
	if(ibsta & ERR)
	{
		PRINT_FAILED();
		return -1;
	}
	ibrpp( board, &result );
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	if((result & (1 << (line - 1))) == 0)
	{
		PRINT_FAILED();
		fprintf( stderr, "parallel poll result: 0x%x\n", (unsigned int)result );
		return -1;
	}
	if(sync_message(options, "set ist to 0"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	if(sync_message(options, "ist is 0"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	ibrpp( board, &result );
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	if((result & (1 << (line - 1))))
	{
		PRINT_FAILED();
		fprintf( stderr, "parallel poll result: 0x%x\n", (unsigned int)result );
		return -1;
	}
	ibonl( ud, 0 );
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	fprintf( stderr, "OK\n" );
	return 0;
}

static int slave_parallel_poll_test(int board, const struct program_options *options)
{
	fprintf( stderr, "%s...", __FUNCTION__ );

	ibist(board, 1);
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "ist is 1"))
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "set ist to 0"))
	{
		PRINT_FAILED();
		return -1;
	}
	ibist(board, 0);
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "ist is 0"))
	{
		PRINT_FAILED();
		return -1;
	}
	fprintf( stderr, "OK\n" );
	return 0;
}

static int do_master_eos_pass(int board, const struct program_options *options,
	int eosmode, const char *test_message, const char *first_read_result,
	const char *second_read_result)
{
	int ud;
	char buffer[1024];

	ud = open_slave_device_descriptor(board, options, T3s, 0, eosmode);
	if( ud < 0 )
		return -1;
	ibrd(ud, buffer, sizeof(buffer) - 1);
	buffer[ThreadIbcntl()] = '\0';
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		fprintf( stderr, "ibrd status 0x%x, error %i, count %li\n", ThreadIbsta(),
			ThreadIberr(), ThreadIbcntl());
		fprintf(stderr, "first read got: '%s'\n"
			"expected: '%s'\n", buffer, first_read_result);
		return -1;
	}
	if(strcmp(buffer, first_read_result))
	{
		PRINT_FAILED();
		fprintf(stderr, "first read got: '%s'\n"
			"expected: '%s'\n", buffer, first_read_result);
		return -1;
	}
	if(second_read_result != NULL)
	{
		ibrd(ud, buffer, sizeof(buffer) - 1);
		if( ThreadIbsta() & ERR )
		{
			PRINT_FAILED();
			return -1;
		}
		buffer[ThreadIbcntl()] = '\0';
		if(strcmp(buffer, second_read_result))
		{
			PRINT_FAILED();
			fprintf(stderr, "second read got: '%s'\n"
				"expected: '%s'\n", buffer, second_read_result);
			return -1;
		}
	}
	ibonl( ud, 0 );
	if( ThreadIbsta() & ERR )
	{
		PRINT_FAILED();
		return -1;
	}
	return 0;
}

static int do_slave_eos_pass(int board, const struct program_options *options,
	int eosmode, const char *test_message, const char *first_read_result,
	const char *second_read_result)
{
	ibwrt(board, test_message, strlen(test_message));
	if(ThreadIbsta() & ERR)
	{
		PRINT_FAILED();
		fprintf(stderr, "%s: test_message=%s\n", __FUNCTION__, test_message);
		return -1;
	}
	return 0;
}

static int master_eos_test(int board, const struct program_options *options)
{
	int retval;
	int seven_bit_eos;
	fprintf( stderr, "%s...", __FUNCTION__ );

	//check if board supports 7 bit eos compares
	if(ibask(board, Iba7BitEOS, &seven_bit_eos) & ERR)
	{
		PRINT_FAILED();
		return -1;
	}
	if(seven_bit_eos == 0)
	{
		fprintf(stderr, "board does not support 7 bit eos comparisons, skipping 7 bit eos test.\n");
		retval = do_master_eos_pass(board, options, 0, "adfis\xf8gibblex",
			"adfis\xf8gibblex", NULL);
		if(retval < 0) return retval;
	}else
	{
		retval = do_master_eos_pass(board, options, 'x' | REOS, "adfis\xf8gibblex",
			"adfis\xf8", "gibblex");
		if(retval < 0) return retval;
	}
	retval = do_master_eos_pass(board, options, 'x' | REOS | BIN, "adfis\xf8gibblex",
		"adfis\xf8gibblex", NULL);
	if(retval < 0) return retval;

	fprintf( stderr, "OK\n" );
	return 0;
}

static int slave_eos_test(int board, const struct program_options *options)
{
	int retval;
	fprintf( stderr, "%s...", __FUNCTION__ );

	retval = do_slave_eos_pass(board, options, 'x' | REOS, "adfis\xf8gibblex",
		"adfis\xf8", "gibblex");
	if(retval < 0) return retval;

	retval = do_slave_eos_pass(board, options, 'x' | REOS | BIN, "adfis\xf8gibblex",
		"adfis\xf8gibblex", NULL);
	if(retval < 0) return retval;

	fprintf( stderr, "OK\n" );
	return 0;
}

static int master_remote_and_lockout_test(int board, const struct program_options *options)
{
	Addr4882_t addressList[] = {slaveAddress(options), NOADDR};
	int ud;
	
	fprintf( stderr, "%s...", __FUNCTION__ );

	ud = open_slave_device_descriptor(board, options, T3s, 1, 0);
	if( ud < 0 )
		return -1;

	EnableLocal(board, addressList);
	if(sync_message(options, "set local mode"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	if(ThreadIbsta() & ERR)
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "local detected"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	SetRWLS(board, addressList);
	if(ThreadIbsta() & ERR)
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "set remote with lockout"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	if(sync_message(options, "lockout detected"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	if(ibsre(board, 0) & ERR)
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "lockout cleared"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	if(sync_message(options, "lockout clear detected"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	EnableRemote(board, addressList);
	if(ThreadIbsta() & ERR)
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "remote enabled"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	if(sync_message(options, "remote detected"))
	{
		ibonl( ud, 0 );
		return -1;
	}
	ibonl( ud, 0 );
	fprintf( stderr, "OK\n" );
	return 0;
}

static int slave_remote_and_lockout_test(int board, const struct program_options *options)
{
	int failed = 0;
	
	fprintf( stderr, "%s...", __FUNCTION__ );

	if(sync_message(options, "set local mode"))
	{
		PRINT_FAILED();
		return -1;
	}
	if((ibwait(board, 0) & REM))
	{
		fprintf(stderr, "REM did not clear.  ibsta=0x%x\n", ThreadIbsta());
		PRINT_FAILED();
		++failed;
	}
	if(sync_message(options, "local detected"))
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "set remote with lockout"))
	{
		PRINT_FAILED();
		return -1;
	}
	if((ibwait(board, LOK | TIMO) & LOK) == 0)
	{
		fprintf(stderr, "failed to detect LOK status.  ibsta=0x%x\n", ThreadIbsta());
		PRINT_FAILED();
		++failed;
	}
	if(sync_message(options, "lockout detected"))
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "lockout cleared"))
	{
		PRINT_FAILED();
		return -1;
	}
	if(ibwait(board, 0) & LOK)
	{
		PRINT_FAILED();
		++failed;
	}
	if(sync_message(options, "lockout clear detected"))
	{
		PRINT_FAILED();
		return -1;
	}
	if(sync_message(options, "remote enabled"))
	{
		PRINT_FAILED();
		return -1;
	}
	if((ibwait(board, REM | TIMO) & REM) == 0)
	{
		PRINT_FAILED();
		++failed;
	}
	if(sync_message(options, "remote detected"))
	{
		PRINT_FAILED();
		return -1;
	}
	if(failed == 0)
	{
		fprintf( stderr, "OK\n" );
		return 0;
	}else
		return -1;
}

int parse_program_options(int argc, char *argv[], struct program_options *options)
{
	static struct option long_options[] =
	{
		{"num_loops", required_argument, NULL, 'n'},
		{"master", no_argument, NULL, 'm'},
		{"minor", required_argument, NULL, 'M'},
		{"slave", no_argument, NULL, 'S'},
		{"pad", required_argument, NULL, 'p'},
		{"sad", required_argument, NULL, 's'},
		{"verbose", no_argument, NULL, 'v'},
		{0, 0, 0, 0}
	};
	int c;
	int option_index = 0;

	memset(options, 0, sizeof(*options));
	options->num_loops = 1;
	options->pad = 1;
	options->sad = -1;
	options->minor = -1;
	while(1)
	{
		c = getopt_long(argc, argv, "n:mSp:s:v", long_options, &option_index);
		if(c < 0) break;
		switch(c)
		{
		case 'n':
			options->num_loops = strtol(optarg, NULL, 0);
			fprintf(stderr, "option: loop %i times\n", options->num_loops);
			break;
		case 'm':
			options->master = 1;
			fprintf(stderr, "option: run as bus master\n");
			break;
		case 'M':
			options->minor = strtol(optarg, NULL, 0);
			fprintf(stderr, "option: minor %i\n", options->minor);
			break;
		case 'S':
			options->master = 0;
			fprintf(stderr, "option: run as slave\n");
			break;
		case 'p':
			options->pad = strtol(optarg, NULL, 0);
			fprintf(stderr, "option: pad %i\n", options->pad);
			break;
		case 's':
			options->sad = strtol(optarg, NULL, 0);
			fprintf(stderr, "option: sad %i\n", options->sad);
			break;
		case 'v':
			options->verbosity = 1;
			fprintf(stderr, "option: verbose\n");
			break;
		default:
			fprintf(stderr, "bad option?\n");
			return -1;
			break;
		}
	}
	return 0;
}

int main( int argc, char *argv[] )
{
	int board;
	int retval;
	struct program_options options;
	int i;

	if(parse_program_options(argc, argv, &options) < 0)
		return -1;

	for(i = 0; i < options.num_loops; i++)
	{
		if(options.master)
		{
			if(sync_message(&options, "found slave board"))
				return -1;
			retval = find_board(&board, &options);
			if( retval < 0 ) return retval;
			retval = master_read_write_test(board, &options);
			if( retval < 0 ) return retval;
			retval = master_async_read_write_test(board, &options);
			if( retval < 0 ) return retval;
			retval = master_serial_poll_test(board, &options);
			if( retval < 0 ) return retval;
			retval = master_parallel_poll_test(board, &options);
			if( retval < 0 ) return retval;
			retval = master_eos_test(board, &options);
			if( retval < 0 ) return retval;
			retval = master_remote_and_lockout_test(board, &options);
			if( retval < 0 ) return retval;
		}else
		{
			retval = find_board(&board, &options);
			if( retval < 0 ) return retval;
			if(sync_message(&options, "found slave board"))
				return -1;
			retval = slave_read_write_test(board, &options);
			if( retval < 0 ) return retval;
			retval = slave_async_read_write_test(board, &options);
			if( retval < 0 ) return retval;
			retval = slave_serial_poll_test(board, &options);
			if( retval < 0 ) return retval;
			retval = slave_parallel_poll_test(board, &options);
			if( retval < 0 ) return retval;
			retval = slave_eos_test(board, &options);
			if( retval < 0 ) return retval;
			retval = slave_remote_and_lockout_test(board, &options);
			if( retval < 0 ) return retval;
		}
	}
	return 0;
}

