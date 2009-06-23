#include "gpibP.h"

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/timer.h>

#include <asm/io.h>
#include <asm/segment.h>
#include <asm/irq.h>
#include <asm/dma.h>
#include <asm/uaccess.h>
#include <asm/system.h>

extern int gpib_allocate_board( gpib_board_t *board );
extern void gpib_deallocate_board( gpib_board_t *board );
