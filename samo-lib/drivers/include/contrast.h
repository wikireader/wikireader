/*
 * contrast pwm driver
 * Copyright (c) 2009 Openmoko
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

#if !defined(_CONTRAST_H_)
#define _CONTRAST_H_ 1

#define CONTRAST_MIN 0
#define CONTRAST_DEFAULT 2048
#define CONTRAST_MAX 4095

void contrast_initialise(unsigned int max_value);
void contrast_set(int value);
int contrast_get(void);

#endif
