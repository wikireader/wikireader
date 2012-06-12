/*
 * Copyright (c) 2009 Openmoko Inc.
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

#ifndef WL_ENDIAN_H
#define WL_ENDIAN_H

#if BYTE_ORDER == LITTLE_ENDIAN
#define WL_LTONL(val)	(val)
#define WL_LTONS(val)	(val)
#else
#define WL_LTOBL(val)	(((((val) >> 24) & 0xff) << 0)  |	\
			 ((((val) >> 16) & 0xff) << 8)  |	\
			 ((((val) >> 8)  & 0xff) << 16) |	\
			 ((((val) >> 0)  & 0xff) << 24))
#define WL_LTOBL(val)	(((((val) >> 8)  & 0xff) << 0)  | 	\
			 ((((val) >> 0)  & 0xff) << 8))
#endif

#endif /* WL_ENDIAN_H */
