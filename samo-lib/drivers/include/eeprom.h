#ifndef EEPROM_H
#define EEPROM_H

#include <stdlib.h>

void eeprom_load(uint32_t addr, uint8_t *dest, uint32_t size);

#endif /* EEPROM_H */
