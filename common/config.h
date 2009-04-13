#ifndef CONFIG_H
#define CONFIG_H

/* select one (and only one) board */

//#define BOARD_S1C33E07 1
//#define BOARD_PRT33L17LCD 1
//#define BOARD_PROTO1 1
#define BOARD_PROTO2	1
//#define BOARD_SAMO_A1 1

#define LCD_MONOCHROME 1

#if BOARD_SAMO_A1
	#define EEPROM_PM25LV512	1
#elif BOARD_PROTO1
	#define LCD_INVERTED		1
	#define EEPROM_SST25VF040	1
#elif BOARD_PROTO2
	#define EEPROM_SST25VF040	1
#elif BOARD_S1C33E07 || BOARD_PRT33L17LCD
	#define EEPROM_MP45PE80		1
#else
#error "Unsupported board"
#endif

#endif /* CONFIG_H */
