#include <regs.h>

#include "types.h"
#include "msg.h"

#define WAIT_FOR_EFSIF0_RDY()		\
	timeout = 100;			\
	do {				\
		asm("nop");		\
	} while ((REG_EFSIF0_STATUS & (1 << 5)) && timeout--);

void print(const u8 *txt)
{
	char timeout;

	while (txt && *txt) {
		int delay = 0xfff;

		REG_EFSIF0_TXD = *txt;
		WAIT_FOR_EFSIF0_RDY();

		if (*txt == '\n') {
			REG_EFSIF0_TXD = '\r';
			WAIT_FOR_EFSIF0_RDY();
		}

		while (delay--)
			asm("nop");
		
		txt++;
	}
}

static void printnibble(u8 nib)
{
	char a[2] = { '\0' };

	nib &= 0xf;

	if (nib >= 10)
		a[0] = nib - 10 + 'a';
	else
		a[0] = nib + '0';

	print(a);
}

static void printbyte(u8 byte)
{
	printnibble(byte >> 4);
	printnibble(byte);
}

void hex_dump(const u8 *buf, u32 size)
{
	int i, l;
	char a[2] = { '\0' };

	for (l = 0; l < size; l += 16) {
		printbyte(l >> 24);
		printbyte(l >> 16);
		printbyte(l >> 8);
		printbyte(l);
		print("  ");

		for (i = 0; i < 16; i++) {
			if (l + i < size) {
				printbyte(buf[l + i]);
				print(" ");
			} else
				print("   ");
		}

		print("  |");
		for (i = 0; i < 16; i++) {
			if (l + i < size) {
				if (buf[l + i] >= ' ' && buf[l + i] <= '~')
					a[0] = buf[l + i];
				else
					a[0] = '.';
			} else
				a[0] = ' ';

			print(a);
		}

		print("|\n");
	}
}

void print_u32(u32 val)
{
	print("0x");
	printbyte(val >> 24);
	printbyte(val >> 16);
	printbyte(val >> 8);
	printbyte(val);
}

void msg(const char *fmt, ...)
{
}

