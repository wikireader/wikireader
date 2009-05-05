#ifndef MISC_H
#define MISC_H

// character input
int serial_input_available(void);
int serial_input_char(void);

// character output
void print_char(char c);
void print(const char *txt);

// decimal output
void print_dec32(u32 value);

// hexadecimal output
void hex_dump(const u8 *buf, u32 size);
void print_byte(u8 val);
void print_u32(u32 val);

// simple busy wait delay loops
void delay(u32 nops);  // deprecated
void delay_us(unsigned int microsec);


#endif /* MISC_H */

