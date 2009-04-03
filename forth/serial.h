// interface to serial port

#if !defined(_SERIAL_H_)
#define _SERIAL_H_ 1

#include <stdbool.h>

void Serial_PutString(const char *string);
void Serial_PutSpace(void);
void Serial_PutCRLF(void);
void Serial_PutChar(unsigned char c);
void Serial_PutHex(unsigned long int value);

unsigned char Serial_GetChar(void);
bool Serial_InputAvailable(void);

#endif
