
#include "gpibP.h"


/*
 * IBRSV
 * Request service from the CIC and/or set the serial poll
 * status byte.
 */
int ibrsv( gpib_board_t *board, uint8_t poll_status )
{
	int status = ibstatus( board );

	if( ( status & CIC ) )
	{
		printk("gpib: interface requested service while CIC\n");
		return -EINVAL;
	}

	board->interface->serial_poll_response( board, poll_status );

	return 0;
}
