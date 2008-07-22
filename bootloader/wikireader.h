#ifndef WIKIREADER_H
#define WIKIREADER_H

#define INIT_PINS()			\
{					\
	/* P13 & P14: debug LEDs */	\
	REG_P1_IOC1 = 0x18;		\
					\
	/* P50 & P52: CS lines */	\
	REG_P5_IOC5 = 0x05;		\
}

#define DEBUGLED1_ON()	REG_P1_P1D &= ~(1 << 4)
#define DEBUGLED1_OFF()	REG_P1_P1D |=  (1 << 4)

#define DEBUGLED2_ON()	REG_P1_P1D &= ~(1 << 3)
#define DEBUGLED2_OFF()	REG_P1_P1D |=  (1 << 3)

#define SDCARD_CS_LO()	REG_P5_P5D &= ~(1 << 0);
#define SDCARD_CS_HI()	REG_P5_P5D |=  (1 << 0);

#define EEPROM_CS_LO()	REG_P5_P5D &= ~(1 << 2);
#define EEPROM_CS_HI()	REG_P5_P5D |=  (1 << 2);

#endif /* WIKIREADER_H */

