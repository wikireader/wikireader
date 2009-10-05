/*
 * (C) Copyright 2009 Openmoko, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "hash.h"


static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "verbose", 0, 0, 'v' },
	{ "fnd", 1, 0, 'f' },
	{ "pfx", 1, 0, 'p' },
	{ "hsh", 1, 0, 'o' },
	{ NULL, 0, NULL, 0 }
};

bool verbose = false;

static void usage(const char *message)
{
	if (NULL != message)
	{
		fprintf(stderr, "error: %s\n", message);
	}
	fprintf(stderr, "usage: %s <options>\n"
		"      --help       this message\n"
		"      --verbose    message output\n"
		"      --fnd=file   input fnd file\n"
		"      --pfx=file   input pfx file\n"
		"      --hsh=file   output hash file\n",
		"hash-gen");
	exit(1);
}


int main(int argc, char **argv)
{
	char fnd_name[1024];
	char pfx_name[1024];
	char hsh_name[1024];

	printf("hash generator - (C) 2009 by Openmoko Inc.\n"
	       "This program is Free Software and has ABSOLUTELY NO WARRANTY\n\n");

	memset(fnd_name, '\0', sizeof(fnd_name));
	memset(pfx_name, '\0', sizeof(pfx_name));
	memset(hsh_name, '\0', sizeof(hsh_name));

	for (;;)
	{
		int c, option_index = 0;
		c = getopt_long(argc, argv, "hvf:p:o:", opts, &option_index);
		if (c == -1)
		{
			break;
		}

		switch (c)
		{
		case 'h':
			usage(NULL);
			break;
		case 'f':
			strncpy(fnd_name, optarg, sizeof(fnd_name) - 1);
			fnd_name[sizeof(fnd_name) - 1] = '\0';
			break;
		case 'p':
			strncpy(pfx_name, optarg, sizeof(pfx_name) - 1);
			pfx_name[sizeof(pfx_name) - 1] = '\0';
			break;
		case 'o':
			strncpy(hsh_name, optarg, sizeof(hsh_name) - 1);
			hsh_name[sizeof(hsh_name) - 1] = '\0';
			break;
		case 'v':
			verbose = true;
			break;
		default:
			usage("invalid arguments");
		}
	}


	if ('\0' == fnd_name[0])
	{
		usage("missing --fnd=file");
	}

	if ('\0' == pfx_name[0])
	{
		usage("missing --pfx=file");
	}

	if ('\0' == hsh_name[0])
	{
		usage("missing --hsh=file");
	}

	generate_pedia_hsh(fnd_name, pfx_name, hsh_name);
	return 0;
}
