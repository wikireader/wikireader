/*
 * watchdog - driver for the watchdog timer
 *
 * Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
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

#if  !defined(_WATCHDOG_H_)
#define _WATCHDOG_H_ 1

void Watchdog_initialise(void);

typedef enum {
//+MakeSystemCalls: key
	WATCHDOG_KEY = 0xcafe1a7e,
//-MakeSystemCalls: key
} Watchdog_type;

void Watchdog_KeepAlive(Watchdog_type key);
void Watchdog_SetTimeout(Watchdog_type key, uint32_t WatchdogTimeout);

#endif
