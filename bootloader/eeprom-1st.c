#include "regs.h"
#include "wikireader.h"

int main(void) {
	INIT_PINS();

	REG_P6_IOC6 = (1 << 4);
	
	bla:
		REG_P6_P6D |=  (1 << 4);
		REG_P6_P6D &= ~(1 << 4);
	goto bla;

	return 0;
}

