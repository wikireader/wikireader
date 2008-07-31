#ifndef WIKIREADER_H
#define WIKIREADER_H

#define INIT_PINS()			\
{					\
	/* P13 & P14: debug LEDs */	\
	REG_P1_IOC1 = 0x18;		\
					\
	/* P50 & P52: CS lines */	\
	REG_P5_IOC5 = 0x06;		\
	REG_PA_IOC = 0x08;		\
					\
	/* P65-67: SPI */		\
	REG_P6_47_CFP = 0x54;		\
					\
	/* Serial interface */		\
	REG_P0_03_CFP = 0x05;		\
}

#define DEBUGLED1_ON()	do { REG_P1_P1D &= ~(1 << 4); } while (0)
#define DEBUGLED1_OFF()	do { REG_P1_P1D |=  (1 << 4); } while (0)

#define DEBUGLED2_ON()	do { REG_P1_P1D &= ~(1 << 3); } while (0)
#define DEBUGLED2_OFF()	do { REG_P1_P1D |=  (1 << 3); } while (0)

//#define SDCARD_CS_LO()	do { REG_P5_P5D &= ~(1 << 0); } while (0)
//#define SDCARD_CS_HI()	do { REG_P5_P5D |=  (1 << 0); } while (0)
#define SDCARD_CS_LO()	do { REG_PA_DATA &= ~(1 << 3); } while (0)
#define SDCARD_CS_HI()	do { REG_PA_DATA |=  (1 << 3); } while (0)

#define EEPROM_CS_LO()	do { REG_P5_P5D &= ~(1 << 2); } while (0)
#define EEPROM_CS_HI()	do { REG_P5_P5D |=  (1 << 2); } while (0)

#endif /* WIKIREADER_H */

