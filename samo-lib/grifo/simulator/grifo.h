/*
 * Simulation of the GRIFO header
 *
 * Copyright (c) 2010 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
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

#if !defined(_SIMULATED_GRIFO_H_)
#define _SIMULATED_GRIFO_H_ 1

// include definitions that are for 64 bit OS first
#include <inttypes.h>

#define GRIFO_SIMULATOR 1

#ifdef __cplusplus
extern "C" {
#endif

// include the real grifo.h file
// this generated grifo.h has a 32 bit interface
#include "../../../include/grifo.h"

// the main entry point for grifo programs
int grifo_main(int argc, char *argv[]);


#ifdef __cplusplus
}
#endif

#endif
