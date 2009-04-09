#ifndef RAMTEST_H
#define RAMTEST_H

#define START 0x10000000
#define SIZE  ((1024 * 1024 * 16))

static void ram_write(void)
{
	unsigned int i;

	for (i = 0; i < SIZE; i += 4) {
		*(volatile unsigned int *) (START + i) = ~i;
		if (i % 100000 == 0)
			print(".");
	}

	print("\n");
}

static void ram_read(void)
{
	unsigned int i;

	for (i = 0; i < SIZE; i += 4) {
		unsigned int v = *(volatile unsigned int *) (START + i);
		if (~v != i) {
			print("FAILED @");
			print_u32(i);
			print(" - ");
			print_u32(v);
			print("\n");
			return;
		}

		if (i % 100000 == 0)
			print(":");
	}

	print("\nPASSED\n");
}

#endif /* RAMTEST_H */
