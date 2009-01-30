#include "traps.h"

static void undef_irq_handler(void)
{
}

#define N_TRAPS 107
typedef void (*irq_callback)(void);
irq_callback trap_table[N_TRAPS] = {
	undef_irq_handler,	/* offset 0	*/
	undef_irq_handler,	/* offset 1	*/
	undef_irq_handler,	/* offset 2	*/
	undef_irq_handler,	/* offset 3	*/
	undef_irq_handler,	/* offset 4	*/
	undef_irq_handler,	/* offset 5	*/
	undef_irq_handler,	/* offset 6	*/
	undef_irq_handler,	/* offset 7	*/
	undef_irq_handler,	/* offset 8	*/
	undef_irq_handler,	/* offset 9	*/
	undef_irq_handler,	/* offset 10	*/
	undef_irq_handler,	/* offset 11	*/
	undef_irq_handler,	/* offset 12	*/
	undef_irq_handler,	/* offset 13	*/
	undef_irq_handler,	/* offset 14	*/
	undef_irq_handler,	/* offset 15	*/
	undef_irq_handler,	/* offset 16	*/
	undef_irq_handler,	/* offset 17	*/
	undef_irq_handler,	/* offset 18	*/
	undef_irq_handler,	/* offset 19	*/
	undef_irq_handler,	/* offset 20	*/
	undef_irq_handler,	/* offset 21	*/
	undef_irq_handler,	/* offset 22	*/
	undef_irq_handler,	/* offset 23	*/
	undef_irq_handler,	/* offset 24	*/
	undef_irq_handler,	/* offset 25	*/
	undef_irq_handler,	/* offset 26	*/
	undef_irq_handler,	/* offset 27	*/
	undef_irq_handler,	/* offset 28	*/
	undef_irq_handler,	/* offset 29	*/
	undef_irq_handler,	/* offset 30	*/
	undef_irq_handler,	/* offset 31	*/
	undef_irq_handler,	/* offset 32	*/
	undef_irq_handler,	/* offset 33	*/
	undef_irq_handler,	/* offset 34	*/
	undef_irq_handler,	/* offset 35	*/
	undef_irq_handler,	/* offset 36	*/
	undef_irq_handler,	/* offset 37	*/
	undef_irq_handler,	/* offset 38	*/
	undef_irq_handler,	/* offset 39	*/
	undef_irq_handler,	/* offset 40	*/
	undef_irq_handler,	/* offset 41	*/
	undef_irq_handler,	/* offset 42	*/
	undef_irq_handler,	/* offset 43	*/
	undef_irq_handler,	/* offset 44	*/
	undef_irq_handler,	/* offset 45	*/
	undef_irq_handler,	/* offset 46	*/
	undef_irq_handler,	/* offset 47	*/
	undef_irq_handler,	/* offset 48	*/
	undef_irq_handler,	/* offset 49	*/
	undef_irq_handler,	/* offset 50	*/
	undef_irq_handler,	/* offset 51	*/
	undef_irq_handler,	/* offset 52	*/
	undef_irq_handler,	/* offset 53	*/
	undef_irq_handler,	/* offset 54	*/
	undef_irq_handler,	/* offset 55	*/
	undef_irq_handler,	/* offset 56	*/
	undef_irq_handler,	/* offset 57	*/
	undef_irq_handler,	/* offset 58	*/
	undef_irq_handler,	/* offset 59	*/
	undef_irq_handler,	/* offset 60	*/
	undef_irq_handler,	/* offset 61	*/
	undef_irq_handler,	/* offset 62	*/
	undef_irq_handler,	/* offset 63	*/
	undef_irq_handler,	/* offset 64	*/
	undef_irq_handler,	/* offset 65	*/
	undef_irq_handler,	/* offset 66	*/
	undef_irq_handler,	/* offset 67	*/
	undef_irq_handler,	/* offset 68	*/
	undef_irq_handler,	/* offset 69	*/
	undef_irq_handler,	/* offset 70	*/
	undef_irq_handler,	/* offset 71	*/
	undef_irq_handler,	/* offset 72	*/
	undef_irq_handler,	/* offset 73	*/
	undef_irq_handler,	/* offset 74	*/
	undef_irq_handler,	/* offset 75	*/
	undef_irq_handler,	/* offset 76	*/
	undef_irq_handler,	/* offset 77	*/
	undef_irq_handler,	/* offset 78	*/
	undef_irq_handler,	/* offset 79	*/
	undef_irq_handler,	/* offset 80	*/
	undef_irq_handler,	/* offset 81	*/
	undef_irq_handler,	/* offset 82	*/
	undef_irq_handler,	/* offset 83	*/
	undef_irq_handler,	/* offset 84	*/
	undef_irq_handler,	/* offset 85	*/
	undef_irq_handler,	/* offset 86	*/
	undef_irq_handler,	/* offset 87	*/
	undef_irq_handler,	/* offset 88	*/
	undef_irq_handler,	/* offset 89	*/
	undef_irq_handler,	/* offset 90	*/
	undef_irq_handler,	/* offset 91	*/
	undef_irq_handler,	/* offset 92	*/
	undef_irq_handler,	/* offset 93	*/
	undef_irq_handler,	/* offset 94	*/
	undef_irq_handler,	/* offset 95	*/
	undef_irq_handler,	/* offset 96	*/
	undef_irq_handler,	/* offset 97	*/
	undef_irq_handler,	/* offset 98	*/
	undef_irq_handler,	/* offset 99	*/
	undef_irq_handler,	/* offset 100	*/
	undef_irq_handler,	/* offset 101	*/
	undef_irq_handler,	/* offset 102	*/
	undef_irq_handler,	/* offset 103	*/
	undef_irq_handler,	/* offset 104	*/
	undef_irq_handler,	/* offset 105	*/
	undef_irq_handler,	/* offset 106	*/
};

void traps_init(void)
{
	asm("ld.w %%ttbr, %0" :: "r"(trap_table));
}

