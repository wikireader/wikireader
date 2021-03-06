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

#ifndef MISC_H
#define MISC_H

#include <inttypes.h>

static inline uint16_t __swab16(uint16_t x)
{
	return (((x & 0x00ffU) << 8) |
		((x & 0xff00U) >> 8));
}

static inline uint32_t __swab32(uint32_t x)
{
	return (((x & 0x000000ffUL) << 24) |
		((x & 0x0000ff00UL) << 8) |
		((x & 0x00ff0000UL) >> 8) |
		((x & 0xff000000UL) >> 24));
}

#define __be32_to_cpu(x) __swab32((uint32_t)(x))
#define __be16_to_cpu(x) __swab16((uint16_t)(x))
#define __cpu_to_be32(x) __swab32((uint32_t)(x))
#define __cpu_to_be16(x) __swab16((uint16_t)(x))

#define __le32_to_cpu(x) ((uint32_t)(x))
#define __le16_to_cpu(x) ((uint16_t)(x))
#define __cpu_to_le32(x) ((uint32_t)(x))
#define __cpu_to_le16(x) ((uint16_t)(x))

// See linux/include/unaligned/generic.h etc. for more elaborate versions...

static inline uint16_t __get_unaligned_2(const uint8_t *p)
{
	return p[0] | p[1] << 8;
}

static inline uint32_t __get_unaligned_4(const uint8_t *p)
{
	return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

static inline void __put_unaligned_2(uint16_t val, uint8_t* p)
{
	*p = val;
	p[1] = val >> 8;
}

static inline void __put_unaligned_4(uint32_t val, uint8_t* p)
{
	__put_unaligned_2(val >> 16, p + 2);
	__put_unaligned_2(val, p);
}

// character input
int serial_input_available(void);
int serial_input_char(void);

// character output
int print_char(int c);
void print(const char *txt);

// decimal output
void print_int32(int32_t value);
void print_dec32(uint32_t value);

// hexadecimal output
void hex_dump(const void *buffer, uint32_t size);
void print_byte(uint8_t val);
void print_u32(uint32_t val);

#endif /* MISC_H */
