/*
 * system - miscellaneous system routines
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

#if  !defined(_SYSTEM_H_)
#define _SYSTEM_H_ 1

void System_initialise(void);

void System_panic(const char *format, ...)  __attribute__((format (printf, 1, 2), noreturn));
void System_PowerOff(void) __attribute__((noreturn));
void System_reboot(void) __attribute__((noreturn));

typedef enum {
//+MakeSystemCalls: exit
	EXIT_RESTART_INIT = 0,
	EXIT_POWER_OFF    = 1,
	EXIT_REBOOT       = 2,
//-MakeSystemCalls: exit
} System_ExitType;

void System_exit(System_ExitType result) __attribute__((noreturn));
void System_chain(const char *command) __attribute__((noreturn));

#endif
