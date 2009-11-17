/*
 * suspend - put the CPU into low power mode, retain LCD controller
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

#if  !defined(_SUSPEND_H_)
#define _SUSPEND_H_ 1

#include "standard.h"

void Suspend_initialise(void);

// callback returns:
//   true  => continue
//   false => close all files an power off
// This is a user code callback - do not use in the kernel
void Suspend(Standard_BoolCallBackType *callback, void *arg);

#endif
