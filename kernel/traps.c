#include "traps.h"

#define N_TRAPS 107
typedef void (*irq_callback)(void);
irq_callback trap_table[N_TRAPS];

void traps_init(void)
{
	asm("ld.w %%ttbr, %0" :: "r"(trap_table));
}

