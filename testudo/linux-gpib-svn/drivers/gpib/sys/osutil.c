
#include "ibsys.h"
#include <asm/io.h>
#include <linux/sched.h>

void writeb_wrapper(unsigned int value, void *address)
{
	writeb(value, address);
};

void writew_wrapper(unsigned int value, void *address)
{
	writew(value, address);
};

unsigned int readb_wrapper(void *address)
{
	return readb(address);
};

unsigned int readw_wrapper(void *address)
{
	return readw(address);
};

void outb_wrapper(unsigned int value, void *address)
{
	outb(value, (unsigned long)(address));
};

void outw_wrapper(unsigned int value, void *address)
{
	outw(value, (unsigned long)(address));
};

unsigned int inb_wrapper(void *address)
{
	return inb((unsigned long)(address));
};

unsigned int inw_wrapper(void *address)
{
	return inw((unsigned long)(address));
};
/* this is a function instead of a constant because of Suse
 * defining HZ to be a function call to get_hz() */
static inline int pseudo_irq_period(void)
{
	return (HZ + 99) / 100;
}

void pseudo_irq_handler(unsigned long arg)
{
	gpib_board_t *board = (gpib_board_t*) arg;
	if(board->pseudo_irq.handler)
		board->pseudo_irq.handler(0, board
#ifdef HAVE_PT_REGS		
		, NULL
#endif
		);
	else
		printk("gpib: bug! pseudo_irq.handler is NULL\n");
	if(board->pseudo_irq.active)
		mod_timer(&board->pseudo_irq.timer, jiffies + pseudo_irq_period());
}

int gpib_request_pseudo_irq(gpib_board_t *board, irqreturn_t (*handler)(int, void * PT_REGS_ARG))
{
	if(timer_pending(&board->pseudo_irq.timer) || board->pseudo_irq.handler)
	{
		printk("gpib: only one psuedo interrupt per board allowed\n");
		return -1;
	}

	board->pseudo_irq.handler = handler;
	board->pseudo_irq.timer.expires = jiffies + pseudo_irq_period();
	board->pseudo_irq.timer.function = pseudo_irq_handler;
	board->pseudo_irq.timer.data = (unsigned long) board;
	board->pseudo_irq.active = 1;
	add_timer(&board->pseudo_irq.timer);

	return 0;
}

void gpib_free_pseudo_irq(gpib_board_t *board)
{
	board->pseudo_irq.active = 0;
	del_timer_sync(&board->pseudo_irq.timer);
	board->pseudo_irq.handler = NULL;
}

EXPORT_SYMBOL( writeb_wrapper );
EXPORT_SYMBOL( readb_wrapper );
EXPORT_SYMBOL( outb_wrapper );
EXPORT_SYMBOL( inb_wrapper );
EXPORT_SYMBOL( writew_wrapper );
EXPORT_SYMBOL( readw_wrapper );
EXPORT_SYMBOL( outw_wrapper );
EXPORT_SYMBOL( inw_wrapper );
EXPORT_SYMBOL(gpib_request_pseudo_irq);
EXPORT_SYMBOL(gpib_free_pseudo_irq);
