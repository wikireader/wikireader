/*
 * Copyright (c) 2010 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Holger Hans Peter Freyther <zecke@openmoko.org>
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

#include <stdio.h>
#include <stdlib.h>

#include <grifo.h>

#include "wikilib.h"

// Note: this must be the first object in the linker command line
//       since Grifo always starts an app from the first .text location.

int grifo_main(int argc, char **argv)
{
	(void)argc; // *** unused argument
	(void)argv; // *** unused argument

	debug_printf("starting wiki app\n");
	wikilib_run();
	return 1;
}
