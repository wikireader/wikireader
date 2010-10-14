/*
 * some miscellaneous definitions
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

#if !defined(_STANDARD_H_)
#define _STANDARD_H_ 1

#include <inttypes.h>  // (u)intXX_t
#include <stdbool.h>   // bool, true, false
#include <stdlib.h>    // size_t
#include <stdarg.h>    // va_list

#ifndef __ssize_t_defined
typedef int32_t ssize_t;
#endif

#if !defined(SizeOfArray)
#define SizeOfArray(a) (sizeof(a) / sizeof((a)[0]))
#endif


// clip min <= value <= max
static inline int Standard_ClipValue(int value, int min, int max)
{
	if (value < min) {
		return min;
	} else if (value > max) {
		return max;
	}
	return value;
}


// generic callback returning a flag
typedef bool Standard_BoolCallBackType(void *arg);


#endif
