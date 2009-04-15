/*
    e07 bootloader suite - ROM base application header
    Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>

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

#if !defined(_APPLICATION_H_)
#define _APPLICATION_H_

#include "regs.h"
#include "types.h"
#include "wikireader.h"
#include "misc.h"


#if !defined(APPLICATION_TITLE)
#error "APPLICATION_TITLE must be defined before including application.h"
#endif

// setup and configure run time environment
// the very first line of main() after the '}'
#define APPLICATION_INITIALISE()			\
	do {						\
		asm volatile ("xld.w   %r15, __dp");	\
		init_pins();				\
		init_rs232_ch0();				\
		disable_card_power();			\
		EEPROM_CS_HI();				\
		SDCARD_CS_HI();				\
		init_ram();				\
	} while (0)


// the last line of main() before the final '}'
// at present just returns the next_program number to
#define APPLICATION_FINALISE(next_program)  \
	do {				    \
		disable_card_power();	    \
		EEPROM_CS_HI();		    \
		SDCARD_CS_HI();		    \
		return (next_program);	    \
	} while (0)

#endif
