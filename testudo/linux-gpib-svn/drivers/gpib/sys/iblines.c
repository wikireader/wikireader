
#include "gpibP.h"

/*
 * IBLINES
 * Poll the GPIB control lines and return their status in buf.
 *
 *      LSB (bits 0-7)  -  VALID lines mask (lines that can be monitored).
 * Next LSB (bits 8-15) - STATUS lines mask (lines that are currently set).
 *
 */
int iblines( const gpib_board_t *board, short *lines)
{
	int retval;
	
	*lines = 0;
	if( board->interface->line_status == NULL )
	{
		return 0;
	}
	retval = board->interface->line_status( board );
	if(retval < 0) return retval;
	*lines = retval;
	return 0;
}
