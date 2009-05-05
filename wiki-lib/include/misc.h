#ifndef MISC_H
#define MISC_H

#include <inttypes.h>

// character input
int serial_input_available(void);
int serial_input_char(void);

// character output
int print_char(int c);
void print(const char *txt);

// decimal output
void print_dec32(uint32_t value);

// hexadecimal output
void hex_dump(const void *buffer, uint32_t size);
void print_byte(uint8_t val);
void print_u32(uint32_t val);

// simple busy wait delay loops
void delay(unsigned int nops);  // deprecated
void delay_us(unsigned int microsec);

#endif /* MISC_H */

