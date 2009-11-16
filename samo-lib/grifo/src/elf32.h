/*
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

#ifndef _ELF32_H_
#define _ELF32_H_

#include "standard.h"

typedef enum {
	ELF32_OK,
	ELF32_NULL_ADDRESS,
	ELF32_OPEN_FAIL,
	ELF32_INVALID_HEADER,
	ELF32_INVALID_MAGIC_NUMBER,
	ELF32_FILE_TYPE,
	ELF32_MACHINE_TYPE,
	ELF32_DATA_READ_FAIL,
} ELF32_ErrorType;


// returns:
//   ELF32_OK   => file is loaded and execution_address is set
//   ELF32_xxx  => error code

ELF32_ErrorType ELF32_load(uint32_t *execution_address,
			   uint32_t *highest_free_address,
			   const char *filename);

#endif
