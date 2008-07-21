#ifndef WIKIREADER_H
#define WIKIREADER_H

#define INIT_PINS()			\
{					\
	/* P13 & P14: debug LEDs */	\
	REG_P1_IOC1 = 0x18;		\
}

#define DEBUGLED1_ON()	REG_P1_P1D &= ~(1 << 4)
#define DEBUGLED1_OFF()	REG_P1_P1D |=  (1 << 4)

#define DEBUGLED2_ON()	REG_P1_P1D &= ~(1 << 3)
#define DEBUGLED2_OFF()	REG_P1_P1D |=  (1 << 3)

#endif /* WIKIREADER_H */

