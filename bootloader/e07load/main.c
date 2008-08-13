/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>

#include "eeprom.h"
#include "misc.h"
#include "e07.h"
#include "mapfile.h"
#include "tty.h"

#define DEFAULT_TTY		"/dev/ttyUSB0"
#define DEFAULT_BOOTSTRAP_FILE	"rs232"

int verbose_flag = 0;

static void usage(const char *argv0)
{
	msg("Usage: %s [<options>] mapfile\n", argv0);
	msg("Options:\n");
	msg("  --tty <node>	set tty to connect to (default: %s)\n", DEFAULT_TTY);
	msg("  --verbose	enable verbose message output\n");
	msg("  --bootstrap	file to use for initial boostrap (default: %s)\n", DEFAULT_BOOTSTRAP_FILE);
	msg("\n");
	msg("The mapfile is expected to have the following layout:\n");
	msg("\n");
	msg("<address>	<filename>\n");
	msg("\n");
	msg("Lines starting with '#' are ignored, addresses may be given in decimal,\n");
	msg("octal or hexadecimal format. Filenames may be relative to current pwd.\n");
	
	msg("\n");
	msg("This program comes with ABSOLUTELY NO WARRANTY.\n");
	msg("This is free software, and you are welcome to redistribute it\n");
	msg("under certain conditions; see file LICENSE.txt for details.\n");
	exit(1);
}

int main(int argc, char **argv)
{
	char *mapfile;
	char *tty = DEFAULT_TTY;
	char *bootstrap_file = DEFAULT_BOOTSTRAP_FILE;
	int ret, ttyfd;

	static int c;

	while (1) {
		static struct option long_options[] = {
			{ "help",	no_argument,		0,		'h'},
			{ "verbose",	no_argument,		&verbose_flag,	1},
			{ "tty",	required_argument,	0,		't'},
			{ "bootstrap",	required_argument,	0,		'b'},
			{ 0, 0, 0, 0 }
		};
           
		int option_index = 0;
     
		c = getopt_long (argc, argv, "hvt:", long_options, &option_index);
		if (c == -1)
			break;
     
		switch (c) {
			case 0:
			/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				
				printf ("\n");
				break;
     
			case 't':
				tty = strdup(optarg);
				break;
			
			case 'b':
				bootstrap_file = strdup(optarg);
				break;
     
			case 'h':
			case '?':
				usage(argv[0]);
				break;
     
			default:
				break;
		}
	}

	if (optind >= argc)
		usage(argv[0]);
	
	mapfile = argv[optind];
	if (mapfile_parse(mapfile) < 0)
		return -1;

	ttyfd = open_tty(tty);
	if (ttyfd < 0)
		return ttyfd;

	ret = sync_cpu(ttyfd);
	if (ret < 0)
		return ret;
	
	ret = bootstrap(ttyfd, bootstrap_file);
	if (ret < 0)
		return ret;

	if (mapfile_write_eeprom(ttyfd) < 0)
		return -1;
	
	msg("YEAH.\n");
	return 0;
}

