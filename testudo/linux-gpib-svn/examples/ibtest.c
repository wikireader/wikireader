/***************************************************************************
                                 ibtest.c
                             -------------------

Example program which uses gpib c library.  More complex than an example
program needs to be really, but useful for testing library functions.

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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include "gpib/ib.h"

void fprint_status( FILE* filep, char *msg  );

enum Action
{
	GPIB_COMMAND,
	GPIB_DEVICE_CLEAR,
	GPIB_EOT,
	GPIB_GO_TO_LOCAL,
	GPIB_GO_TO_STANDBY,
	GPIB_IFC,
	GPIB_LINE_STATUS,
	GPIB_QUIT,
	GPIB_READ,
	GPIB_REMOTE_ENABLE,
	GPIB_REQUEST_SERVICE,
	GPIB_SERIAL_POLL,
	GPIB_TAKE_CONTROL,
	GPIB_TIMEOUT,
	GPIB_WAIT,
	GPIB_WRITE
};

typedef struct
{
	int minor;
} parsed_options_t;

static void parse_options( int argc, char *argv[], parsed_options_t *parsed_opts )
{
	int c, index;

	struct option options[] =
	{
		{ "minor", required_argument, NULL, 'm' },
		{ 0 },
	};

	parsed_opts->minor = 0;

	while(1)
	{
		c = getopt_long(argc, argv, "m:", options, &index);
		if( c == -1 ) break;
		switch( c )
		{
		case 0:
			break;
		case 'm':
			parsed_opts->minor = strtol( optarg, NULL, 0 );
			break;
		default:
			fprintf(stderr, "ibtest: invalid option\n");
			exit(1);
		}
	}
}

void descriptor_type( int ud, int *is_board, int *is_master )
{
	int master;
	int status;

	status = ibask( ud, IbaSC, &master );

	if( status & ERR )
	{
		if( ThreadIberr() != EARG )
		{
			fprint_status( stderr, "ibask error" );
			abort();
		}
		*is_board = 0;
		*is_master = 0;
	}else
	{
		*is_board = 1;
		*is_master = master;
	}
}

int descriptor_is_board( int ud )
{
	int is_board;
	int is_master;
	descriptor_type( ud, &is_board, &is_master );
	return is_board;
}

int descriptor_is_master( int ud )
{
	int is_board;
	int is_master;
	descriptor_type( ud, &is_board, &is_master );
	return is_board && is_master;
}

/* returns a device descriptor after prompting user for primary address */
int prompt_for_device(int minor)
{
	int ud, pad;
	const int sad = 0;
	const int send_eoi = 1;
	const int eos_mode = 0;
	const int timeout = T1s;
	char input[100];

	while(1)
	{
		printf("enter primary gpib address for device you wish to open [0-30]: ");
		fgets(input, sizeof(input), stdin);
		pad = strtol(input, NULL, 0);
		if(pad < 0 || pad > 30)
			printf("primary address must be between 0 and 30\n");
		else break;
	}

	printf("trying to open pad = %i on /dev/gpib%i ...\n", pad, minor);
	ud = ibdev(minor, pad, sad, timeout, send_eoi, eos_mode);
	if(ud < 0)
	{
		fprint_status( stderr, "ibdev error\n");
		abort();
	}

	return ud;
}

/* returns a device descriptor after prompting user for primary address */
int prompt_for_board(void)
{
	int ud;
	char board_name[100];

	do
	{
		int length;
		printf("enter name of interface board (or device) you wish to open: ");
		fgets( board_name, sizeof( board_name ), stdin);
		length = strlen( board_name );
		if( board_name[ length - 1 ] == '\n' )
			board_name[ length - 1 ] = 0;
		printf( "trying to open board named '%s'\n", board_name );
		ud = ibfind( board_name );
		if(ud < 0)
		{
			fprintf( stderr, "failed to open board\n" );
		}
	}while( ud < 0 );

	return ud;
}

int prompt_for_descriptor(int minor)
{
	char input[100];

	do
	{
		printf( "Do you wish to open a (d)evice or an interface (b)oard?\n"
			"\t(you probably want to open a device): ");
		fgets( input, sizeof( input ), stdin );
		switch( input[0] )
		{
			case 'd':
			case 'D':
				return prompt_for_device(minor);
				break;
			case 'b':
			case 'B':
				return prompt_for_board();
				break;
			default:
				break;
		}
	} while( 1 );

	return -1;
}

/* asks user what they want to do next */
int prompt_for_action(void)
{
	char input[100];
	while(1)
	{
		printf("You can:\n"
			"\tw(a)it for an event\n"
			"\twrite (c)ommand bytes to bus (system controller only)\n"
			"\tsend (d)evice clear (device only)\n"
			"\tchange remote (e)nable line (system controller only)\n"
			"\t(g)o to standby (release ATN line, system controller only)\n"
			"\tsend (i)nterface clear (system controller only)\n"
			"\tta(k)e control (assert ATN line, system controller only)\n"
			"\tget bus (l)ine status (board only)\n"
			"\tgo to local (m)ode\n"
			"\tchange end (o)f transmission configuration\n"
 			"\t(q)uit\n"
			"\t(r)ead string\n"
			"\tperform (s)erial poll (device only)\n"
			"\tchange (t)imeout on io operations\n"
			"\trequest ser(v)ice (board only)\n"
			"\t(w)rite data string\n"
			": " );
		do fgets( input, sizeof( input ), stdin );
		while( input[0] == '\n' );

		switch( input[0] )
		{
			case 'A':
			case 'a':
				return GPIB_WAIT;
				break;
			case 'C':
			case 'c':
				return GPIB_COMMAND;
				break;
			case 'd':
			case 'D':
				return GPIB_DEVICE_CLEAR;
				break;
			case 'E':
			case 'e':
				return GPIB_REMOTE_ENABLE;
				break;
			case 'G':
			case 'g':
				return GPIB_GO_TO_STANDBY;
				break;
			case 'L':
			case 'l':
				return GPIB_LINE_STATUS;
				break;
			case 'I':
			case 'i':
				return GPIB_IFC;
				break;
			case 'K':
			case 'k':
				return GPIB_TAKE_CONTROL;
				break;
			case 'm':
			case 'M':
				return GPIB_GO_TO_LOCAL;
				break;
			case 'o':
			case 'O':
				return GPIB_EOT;
				break;
			case 'q':
			case 'Q':
				return GPIB_QUIT;
				break;
			case 'r':
			case 'R':
				return GPIB_READ;
				break;
			case 's':
			case 'S':
				return GPIB_SERIAL_POLL;
				break;
			case 't':
			case 'T':
				return GPIB_TIMEOUT;
				break;
			case 'v':
			case 'V':
				return GPIB_REQUEST_SERVICE;
				break;
			case 'w':
			case 'W':
				return GPIB_WRITE;
				break;
			default:
				fprintf( stderr, "invalid selection\n");
				break;
		}
	}

	return -1;
}

int perform_read(int ud, int max_num_bytes)
{
	char *buffer;
	int buffer_size = max_num_bytes + 1;
	buffer = malloc(buffer_size);
	int is_string;
	int i;
	if(buffer == NULL) 
	{
		fprintf(stderr, "%s: failed to allocate buffer.\n", __FUNCTION__);
		return -1;
	}
	memset(buffer, 0, buffer_size);
	printf("trying to read %i bytes from device...\n", max_num_bytes);

	ibrd(ud, buffer, buffer_size - 1);
	is_string = 1;
	for(i = 0; i < ThreadIbcntl(); ++i)
	{
		if(isascii(buffer[i]) == 0) 
		{
			is_string = 0;
			break;
		}
	}
	if(is_string)
	{
		printf("received string: '%s'\n", buffer);
	}else
	{
		printf("received binary data (hex): ");
		for(i = 0; i < ThreadIbcntl(); ++i)
		{
			printf("%2x ", (unsigned)buffer[i]);
		}
		printf("\n");
	}
	free(buffer);
	printf("Number of bytes read: %li\n", ThreadIbcntl());
	if(ThreadIbsta() & ERR)
		return -1;
	return 0;
}

int prompt_for_commands(int ud)
{
	char buffer[ 1024 ];
	char *next = buffer;
	char *end;
	int i;

	printf("enter command bytes to send to the bus: ");
	fgets( buffer, sizeof(buffer), stdin );
	for(i = 0; i < sizeof(buffer); ++i)
	{
		buffer[i] = strtol(next, &end, 0);
		if(end == next) break;
		next = end;
	}
	printf("writing %i command bytes to the bus\n", i);
	if(ibcmd(ud, buffer, i) & ERR)
	{
		return -1;
	}
	return 0;
}

int prompt_for_read(int ud)
{
	char *buffer;
	static const int buffer_size = 1024;
	int max_num_bytes;
	static const int default_num_bytes = 1024;
	char *endptr;
	
	buffer = malloc(buffer_size);
	if(buffer == NULL)
		return -ENOMEM;
	printf("enter maximum number of bytes to read [1024]: ");
	fgets(buffer, buffer_size, stdin);
	max_num_bytes = strtol(buffer, &endptr, 0);
	if(endptr == buffer)
		max_num_bytes = default_num_bytes;
	free(buffer);
	return perform_read(ud, max_num_bytes);
}

int prompt_for_write(int ud)
{
	char buffer[ 1024 ];

	printf("enter a string to send to your device: ");
	fgets( buffer, sizeof(buffer), stdin );

	printf("sending string: %s\n", buffer);
	if( ibwrt(ud, buffer, strlen(buffer)) & ERR )
	{
		return -1;
	}
	return 0;
}

int do_serial_poll( int ud )
{
	char result;
	int status;

	if( descriptor_is_board( ud ) != 0 )
	{
		fprintf( stderr, "You have a board open (as opposed to a device).\n"
			"\tYou cannot perform a serial poll.\n" );
		return -1;
	}

	status = ibrsp( ud, &result );
	if( status & ERR )
	{
		return -1;
	}

	printf( "serial poll result: 0x%x\n", ( (unsigned int) result ) & 0xff );
	return 0;
}

int request_service( int ud )
{
	int status_byte;
	int status;

	if( descriptor_is_board( ud ) == 0 )
	{
		fprintf( stderr, "You have a device open (as opposed to a board).\n"
			"\tYou cannot request service.\n" );
		return -1;
	}

	printf( "enter new status byte (bit 0x40 requests service): " );
	scanf( "%i", &status_byte );

	status = ibrsv( ud, status_byte );

	if( status & ERR )
	{
		return -1;
	}

	return 0;
}

int prompt_for_timeout( int ud )
{
	int timeout;

	printf( "enter the desired timeout:\n"
		"\t(0) none\n"
		"\t(1) 10 microsec\n"
		"\t(2) 30 microsec\n"
		"\t(3) 100 microsec\n"
		"\t(4) 300 microsec\n"
		"\t(5) 1 millisec\n"
		"\t(6) 3 millisec\n"
		"\t(7) 10 millisec\n"
		"\t(8) 30 millisec\n"
		"\t(9) 100 millisec\n"
		"\t(10) 300 millisec\n"
		"\t(11) 1 sec\n"
		"\t(12) 3 sec\n"
		"\t(13) 10 sec\n"
		"\t(14) 30 sec\n"
		"\t(15) 100 sec\n"
		"\t(16) 300 sec\n"
		"\t(17) 1000 sec\n"
		);
	scanf( "%i", &timeout );

	if( ibtmo( ud, timeout ) & ERR )
	{
		fprintf( stderr, "failed to set timeout to %i\n", timeout );
		return -1;
	}
	return 0;
}

int prompt_for_wait( int ud )
{
	int wait_mask;
	int status;

	printf( "Possible wait bits:\n"
		"\t0x%x timeout\n"
		"\t0x%x device requesting service\n"
		"\t0x%x board serial polled\n",
			TIMO, RQS, SPOLL);
	printf("Enter wait mask: ");
	scanf( "%i", &wait_mask );

	status = ibwait( ud, wait_mask );

	return 0;
}

void printf_line_status( const char *name, int line_status,
	int valid_bit, int bus_bit )
{
	printf( "%s ", name );
	if( line_status & valid_bit )
	{
		if( line_status & bus_bit )
			printf( "on\n" );
		else
			printf( "off\n" );
	}else
		printf( "unknown\n" );
}

int get_lines( int ud )
{
	short line_status;

	if( iblines( ud, &line_status ) & ERR )
		return -1;

	printf_line_status( "DAV", line_status, ValidDAV, BusDAV );
	printf_line_status( "NDAC", line_status, ValidNDAC, BusNDAC );
	printf_line_status( "NRFD", line_status, ValidNRFD, BusNRFD );
	printf_line_status( "IFC", line_status, ValidIFC, BusIFC );
	printf_line_status( "REN", line_status, ValidREN, BusREN );
	printf_line_status( "SRQ", line_status, ValidSRQ, BusSRQ );
	printf_line_status( "ATN", line_status, ValidATN, BusATN );
	printf_line_status( "EOI", line_status, ValidEOI, BusEOI );

	return 0;
}

int device_clear(int ud)
{
	if(ibclr(ud) & ERR)
	{
		return -1;
	}
	printf("Device clear sent.\n" );
	return 0;
}

int interface_clear( int ud )
{

	if( ibsic( ud ) & ERR )
		return -1;

	printf( "Inferface clear sent\n" );

	return 0;
}

int go_to_local(int ud)
{
	if(ibloc(ud) & ERR)
	{
		return -1;
	}
	printf("Go to local sent.\n" );
	return 0;
}

int go_to_standby(int ud)
{
	if(ibgts(ud, 0) & ERR)
	{
		return -1;
	}
	printf("ATN released.\n" );
	return 0;
}

int prompt_for_remote_enable( int ud )
{
	int status;
	int assert;

	printf("Enter '1' to assert remote enable, or '0' to unassert: ");
	scanf( "%i", &assert );

	status = ibsre( ud, assert );
	if( status & ERR )
		return -1;

	return 0;
}

int prompt_for_take_control(int ud)
{
	int status;
	int synchronous;
	char *buffer;
	static const int buffer_size = 1024;
	char *endptr;

	buffer = malloc(buffer_size);
	if(buffer == NULL)
		return -ENOMEM;
	printf("Enter '1' to assert ATN synchronously, or '0' for asynchronously [0]: ");
	fgets(buffer, buffer_size, stdin);
	synchronous = strtol(buffer, &endptr, 0);
	if(endptr == buffer)
		synchronous = 0;
	free(buffer);
	printf("Taking control ");
	if(synchronous)
		printf("synchronously...\n");
	else
		printf("asynchronously...\n");
	status = ibcac(ud, synchronous);
	if(status & ERR)
		return -1;
	printf("ATN asserted.\n");
	return 0;
}

int prompt_for_eot(int ud)
{
	int status;
	int assert_eoi;
	char *buffer;
	static const int buffer_size = 1024;
	char *endptr;

	buffer = malloc(buffer_size);
	if(buffer == NULL)
		return -ENOMEM;
	printf("Enter '1' to assert EOI with the last byte of writes, or '0' otherwise [1]: ");
	fgets(buffer, buffer_size, stdin);
	assert_eoi = strtol(buffer, &endptr, 0);
	if(endptr == buffer)
		assert_eoi = 1;
	free(buffer);
	status = ibeot(ud, assert_eoi);
	if(status & ERR)
		return -1;
	printf("EOI will ");
	if(assert_eoi == 0)
		printf("not ");
	printf("be asserted with the last byte of writes.\n");
	return 0;
}

int main(int argc, char **argv)
{
	int dev;
	enum Action act;
	parsed_options_t parsed_opts;

	parse_options( argc, argv, &parsed_opts );

	dev = prompt_for_descriptor(parsed_opts.minor);

	do
	{
		act = prompt_for_action();

		if( act == GPIB_QUIT ) break;

		switch( act )
		{
			case GPIB_COMMAND:
				prompt_for_commands(dev);
				break;
			case GPIB_DEVICE_CLEAR:
				device_clear(dev);
				break;
			case GPIB_EOT:
				prompt_for_eot(dev);
				break;
			case GPIB_GO_TO_LOCAL:
				go_to_local(dev);
				break;
			case GPIB_GO_TO_STANDBY:
				go_to_standby(dev);
				break;
			case GPIB_IFC:
				interface_clear(dev);
				break;
			case GPIB_LINE_STATUS:
				get_lines( dev );
				break;
			case GPIB_READ:
				prompt_for_read(dev);
				break;
			case GPIB_REMOTE_ENABLE:
				prompt_for_remote_enable( dev );
				break;
			case GPIB_REQUEST_SERVICE:
				request_service( dev );
				break;
			case GPIB_SERIAL_POLL:
				do_serial_poll( dev );
				break;
			case GPIB_TAKE_CONTROL:
				prompt_for_take_control(dev);
				break;
			case GPIB_TIMEOUT:
				prompt_for_timeout( dev );
				break;
			case GPIB_WAIT:
				prompt_for_wait( dev );
				break;
			case GPIB_WRITE:
				prompt_for_write( dev );
				break;
			default:
				fprintf( stderr, "bug, unknown selection\n");
				break;
		}
		fprint_status( stdout, "gpib status is: " );

	}while( act != GPIB_QUIT );

	ibonl(dev, 0);
	return 0;
}

/*
* This is a simple error handling function
*
*/

void fprint_status( FILE* filep, char *msg )
{
	fprintf( filep, "%s\n", msg);

	fprintf( filep, "ibsta = 0x%x  < ", ThreadIbsta() );
	if(ThreadIbsta() & ERR)  fprintf(filep, "ERR ");
	if(ThreadIbsta() & TIMO) fprintf(filep, "TIMO ");
	if(ThreadIbsta() & END)  fprintf(filep, "END ");
	if(ThreadIbsta() & SRQI) fprintf(filep, "SRQI ");
	if(ThreadIbsta() & RQS) fprintf(filep, "RQS ");
	if(ThreadIbsta() & SPOLL) fprintf(filep, "SPOLL ");
	if(ThreadIbsta() & EVENT) fprintf(filep, "EVENT ");
	if(ThreadIbsta() & CMPL) fprintf(filep, "CMPL ");
	if(ThreadIbsta() & LOK) fprintf(filep, "LOK ");
	if(ThreadIbsta() & REM)  fprintf(filep, "REM ");
	if(ThreadIbsta() & CIC)  fprintf(filep, "CIC ");
	if(ThreadIbsta() & ATN)  fprintf(filep, "ATN ");
	if(ThreadIbsta() & TACS) fprintf(filep, "TACS ");
	if(ThreadIbsta() & LACS) fprintf(filep, "LACS ");
	if(ThreadIbsta() & DCAS) fprintf(filep, "DCAS ");
	if(ThreadIbsta() & DTAS) fprintf(filep, "DTAS ");
	fprintf( filep, ">\n" );

	fprintf( filep,"iberr= %d\n", iberr);
	if( ( ThreadIbsta() & ERR ) )
	{
		fprintf( filep, "%s\n", gpib_error_string( ThreadIberr() ) );
	}

	fprintf( filep, "\n" );

	fprintf( filep, "ibcnt = %d\n", ibcnt );
}

