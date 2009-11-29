/*
 * console - console input functions
 *
 * Copyright (c) 2009 Openmoko Inc.
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

#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#include <regs.h>
#include <samo.h>

#include "console.h"


int console_input_available(void) {
	return (0 != (REG_EFSIF0_STATUS	& RDBFx));
}


int console_input_char(void)
{
	while (!console_input_available()) {
	}

	return(REG_EFSIF0_RXD);
}
