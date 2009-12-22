/*
 * interrupt - interrupt enable/disable
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

#if  !defined(_INTERRUPT_H_)
#define _INTERRUPT_H_ 1

typedef enum {
	Interrupt_disabled = 0,
	Interrupt_enabled = 1,
} InterruptType;


inline InterruptType Interrupt_disable(void)
{
	register InterruptType state;
	asm volatile (
		"\tld.w\t%0, %%psr\n"
		"\txand\t%0, 0x010\n"
		"\tsra\t%0, 4\n"
		"\tpsrclr 4"
		: "=r" (state)
		:
		);
	return state;
}

inline void Interrupt_enable(InterruptType state)
{
	if (0 != state) {
		asm volatile ("psrset 4");
	}
}


#endif
