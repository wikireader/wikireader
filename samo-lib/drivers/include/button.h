/*
 * button - driver for keypad buttons
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

#if  !defined(_BUTTON_H_)
#define _BUTTON_H_ 1

#include <stdbool.h>

typedef enum {
	Button_KeyRight,
	Button_KeyLeft,
	Button_KeyCentre,
	Button_PowerKey,
} ButtonType;

void Button_initialise(void);

bool Button_available(void);
bool Button_get(ButtonType *button, bool *pressed);

void Button_KeyInterrupt(void) __attribute__((interrupt_handler));
void Button_PowerInterrupt(void) __attribute__((interrupt_handler));

#endif
