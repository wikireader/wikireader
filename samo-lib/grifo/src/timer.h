/*
 * simple 32bit counter incrementing at MCU MCLK
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

#if !defined(_TIMER_H_)
#define _TIMER_H_ 1

void Timer_initialise();

//+MakeSystemCalls: enum
enum {
	TIMER_CountsPerMicroSecond = 48,
};
//-MakeSystemCalls: enum

unsigned long Timer_get(void);

#endif
