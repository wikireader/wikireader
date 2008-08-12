#ifndef EEPROM_H
#define EEPROM_H

int write_eeprom(int fd, char *buf, ssize_t len, ssize_t offset);
int verify_eeprom(int fd, char *buf, ssize_t len, ssize_t offset);

#endif /* EEPROM_H */

