/***************************************************************************
                          driver/eastwood_gpib.c  -  description
                             -------------------
GPIB Driver for Fluke "Eastwood" device.

    Author: Frank Mori Hess <fmhess@users.sourceforge.net>
    copyright: (C) 2006 Fluke Corporation
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "eastwood_gpib.h"

#include <asm/dma.h>
#include "gpibP.h"
#include <linux/dma-mapping.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <nios2_system.h>

MODULE_LICENSE("GPL");

int eastwood_attach(gpib_board_t *board, gpib_board_config_t config);
void eastwood_detach(gpib_board_t *board);
void eastwood_config_dma(gpib_board_t *board, int output);
irqreturn_t eastwood_gpib_internal_interrupt(gpib_board_t *board);

uint8_t eastwood_locking_read_byte(nec7210_private_t *nec_priv, unsigned int register_number)
{
	uint8_t retval;
	unsigned long flags;

	spin_lock_irqsave(&nec_priv->register_page_lock, flags);
	retval = eastwood_read_byte_nolock(nec_priv, register_number);
	spin_unlock_irqrestore(&nec_priv->register_page_lock, flags);
	return retval;
}

void eastwood_locking_write_byte(nec7210_private_t *nec_priv, uint8_t byte, unsigned int register_number)
{
	unsigned long flags;

	spin_lock_irqsave(&nec_priv->register_page_lock, flags);
	eastwood_write_byte_nolock(nec_priv, byte, register_number);
	spin_unlock_irqrestore(&nec_priv->register_page_lock, flags);
}

// wrappers for interface functions
int eastwood_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_read(board, &priv->nec7210_priv, buffer, length, end, bytes_read);
}
int eastwood_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_write(board, &priv->nec7210_priv, buffer, length, send_eoi, bytes_written);
}
ssize_t eastwood_command(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_command(board, &priv->nec7210_priv, buffer, length);
}
int eastwood_take_control(gpib_board_t *board, int synchronous)
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_take_control(board, &priv->nec7210_priv, synchronous);
}
int eastwood_go_to_standby(gpib_board_t *board)
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_go_to_standby(board, &priv->nec7210_priv);
}
void eastwood_request_system_control( gpib_board_t *board, int request_control )
{
	eastwood_private_t *priv = board->private_data;
	nec7210_private_t *nec_priv = &priv->nec7210_priv;
	nec7210_request_system_control( board, nec_priv, request_control );
}
void eastwood_interface_clear(gpib_board_t *board, int assert)
{
	eastwood_private_t *priv = board->private_data;
	nec7210_interface_clear(board, &priv->nec7210_priv, assert);
}
void eastwood_remote_enable(gpib_board_t *board, int enable)
{
	eastwood_private_t *priv = board->private_data;
	nec7210_remote_enable(board, &priv->nec7210_priv, enable);
}
int eastwood_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits)
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_enable_eos(board, &priv->nec7210_priv, eos_byte, compare_8_bits);
}
void eastwood_disable_eos(gpib_board_t *board)
{
	eastwood_private_t *priv = board->private_data;
	nec7210_disable_eos(board, &priv->nec7210_priv);
}
unsigned int eastwood_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_update_status( board, &priv->nec7210_priv, clear_mask );
}
void eastwood_primary_address(gpib_board_t *board, unsigned int address)
{
	eastwood_private_t *priv = board->private_data;
	nec7210_primary_address(board, &priv->nec7210_priv, address);
}
void eastwood_secondary_address(gpib_board_t *board, unsigned int address, int enable)
{
	eastwood_private_t *priv = board->private_data;
	nec7210_secondary_address(board, &priv->nec7210_priv, address, enable);
}
int eastwood_parallel_poll(gpib_board_t *board, uint8_t *result)
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_parallel_poll(board, &priv->nec7210_priv, result);
}
void eastwood_parallel_poll_configure( gpib_board_t *board, uint8_t configuration )
{
	eastwood_private_t *priv = board->private_data;
	nec7210_parallel_poll_configure(board, &priv->nec7210_priv, configuration );
}
void eastwood_parallel_poll_response( gpib_board_t *board, int ist )
{
	eastwood_private_t *priv = board->private_data;
	nec7210_parallel_poll_response(board, &priv->nec7210_priv, ist );
}
void eastwood_serial_poll_response(gpib_board_t *board, uint8_t status)
{
	eastwood_private_t *priv = board->private_data;
	nec7210_serial_poll_response(board, &priv->nec7210_priv, status);
}
uint8_t eastwood_serial_poll_status( gpib_board_t *board )
{
	eastwood_private_t *priv = board->private_data;
	return nec7210_serial_poll_status( board, &priv->nec7210_priv );
}
void eastwood_return_to_local( gpib_board_t *board )
{
	eastwood_private_t *priv = board->private_data;
	nec7210_private_t *nec_priv = &priv->nec7210_priv;
	write_byte(nec_priv, AUX_RTL2, AUXMR);
	udelay(1);
	write_byte(nec_priv, AUX_RTL, AUXMR);
}
int eastwood_line_status( const gpib_board_t *board )
{
	int status = ValidALL;
	int bsr_bits;
	eastwood_private_t *e_priv;
	nec7210_private_t *nec_priv;

	e_priv = board->private_data;
	nec_priv = &e_priv->nec7210_priv;

	bsr_bits = eastwood_paged_read_byte(e_priv, BUS_STATUS, BUS_STATUS_PAGE);

	if( ( bsr_bits & BSR_REN_BIT ) == 0 )
		status |= BusREN;
	if( ( bsr_bits & BSR_IFC_BIT ) == 0 )
		status |= BusIFC;
	if( ( bsr_bits & BSR_SRQ_BIT ) == 0 )
		status |= BusSRQ;
	if( ( bsr_bits & BSR_EOI_BIT ) == 0 )
		status |= BusEOI;
	if( ( bsr_bits & BSR_NRFD_BIT ) == 0 )
		status |= BusNRFD;
	if( ( bsr_bits & BSR_NDAC_BIT ) == 0 )
		status |= BusNDAC;
	if( ( bsr_bits & BSR_DAV_BIT ) == 0 )
		status |= BusDAV;
	if( ( bsr_bits & BSR_ATN_BIT ) == 0 )
		status |= BusATN;

	return status;
}

unsigned int eastwood_t1_delay( gpib_board_t *board, unsigned int nano_sec )
{
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	unsigned int retval;

	retval = nec7210_t1_delay( board, nec_priv, nano_sec );

	if( nano_sec <= 350 )
	{
		write_byte( nec_priv, AUX_HI_SPEED, AUXMR );
		retval = 350;
	}else
		write_byte( nec_priv, AUX_LO_SPEED, AUXMR );

	return retval;
}

unsigned get_dma_status(void)
{
	return readl(na_gpib_dma_0 + AVALON_STATUS_REG);
}

/* clear fifo by doing an otherwise useless dma transfer 
 between two locations in memory */
int clear_dma_fifo(gpib_board_t *board)
{
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	int retval;
	char *fifo_read_buffer = NULL;
	char *fifo_write_buffer = NULL;
	unsigned long flags;
	dma_addr_t read_bus_address;
	dma_addr_t write_bus_address;
	static const int bogus_transfer_length = AVALON_DMA_FIFO_SIZE;
	if(e_priv->fifo_dirty == 0) return 0;
// 	printk("%s: enter\n", __FUNCTION__);
	if(e_priv->dma_buffer_size < 2 * bogus_transfer_length) BUG(); 
	fifo_read_buffer = e_priv->dma_buffer;
	fifo_write_buffer = e_priv->dma_buffer + bogus_transfer_length;
	read_bus_address = dma_map_single(NULL, fifo_read_buffer,
		bogus_transfer_length, DMA_TO_DEVICE);
	write_bus_address = dma_map_single(NULL, fifo_write_buffer,
		bogus_transfer_length, DMA_FROM_DEVICE);
	disable_dma(e_priv->dma_channel);
	set_dma_count(e_priv->dma_channel, bogus_transfer_length);
	nios2_set_dma_rcon(e_priv->dma_channel, 0);
	nios2_set_dma_wcon(e_priv->dma_channel, 0);
	nios2_set_dma_raddr(e_priv->dma_channel, read_bus_address);
	nios2_set_dma_waddr(e_priv->dma_channel, write_bus_address);
	spin_lock_irqsave(&board->spinlock, flags);
	enable_dma(e_priv->dma_channel);
	set_bit(DMA_WRITE_IN_PROGRESS_BN, &nec_priv->state);
	spin_unlock_irqrestore(&board->spinlock, flags);

	retval = wait_event_interruptible(board->wait, 
		get_dma_residue(e_priv->dma_channel) == 0 &&
		test_bit(DMA_WRITE_IN_PROGRESS_BN, &nec_priv->state) == 0);
	disable_dma(e_priv->dma_channel);
	if(retval)
	{
		printk("%s: interrupted by signal, residue=%i.\n", 
			__FUNCTION__, get_dma_residue(e_priv->dma_channel));
		retval = -EINTR;
	}else
		e_priv->fifo_dirty = 0;
	dma_unmap_single(NULL, read_bus_address, bogus_transfer_length, DMA_TO_DEVICE);
	dma_unmap_single(NULL, write_bus_address, bogus_transfer_length, DMA_FROM_DEVICE);
// 	printk("%s: exit, retval=%i\n", __FUNCTION__, retval);
	return retval;
}

static int wait_for_idle(gpib_board_t *board, short wake_on_listener_idle,
	short wake_on_talker_idle)
{
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	int retval = 0;
// 	printk("%s: enter\n", __FUNCTION__);
	if(wait_event_interruptible(board->wait,
		(wake_on_listener_idle && test_bit(LACS_NUM, &board->status) == 0) ||
		(wake_on_talker_idle && test_bit(TACS_NUM, &board->status) == 0) ||
		test_bit(DEV_CLEAR_BN, &nec_priv->state) ||
		test_bit(TIMO_NUM, &board->status)))
	{
		retval = -EINTR;
	}
	if(test_bit(TIMO_NUM, &board->status))
		retval = -ETIMEDOUT;
	if(test_and_clear_bit(DEV_CLEAR_BN, &nec_priv->state))
		retval = -EINTR;
// 	printk("%s: exit, retval=%i\n", __FUNCTION__, retval);
	return retval;
}

static int eastwood_dma_write(gpib_board_t *board, 
	uint8_t *buffer, size_t length, size_t *bytes_written)
{
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	unsigned long flags;
	int retval = 0;
	dma_addr_t address;
	*bytes_written = 0;
// 	printk("%s: enter\n", __FUNCTION__);
	if(length > e_priv->dma_buffer_size)
		BUG();
	clear_dma_fifo(board);
	// write-clear counter
	writel(0x0, e_priv->write_transfer_counter);
	retval = wait_for_idle(board, 1, 0);
	if(retval < 0) return retval;
	memcpy(e_priv->dma_buffer, buffer, length);
	address = dma_map_single(NULL, buffer,
		 length, DMA_TO_DEVICE);
	/* program dma controller */
	disable_dma(e_priv->dma_channel);
	eastwood_config_dma(board, 1);
	set_dma_count(e_priv->dma_channel, length);
	nios2_set_dma_raddr(e_priv->dma_channel, address);

	spin_lock_irqsave(&board->spinlock, flags);
	nec7210_set_reg_bits(nec_priv, IMR1, HR_DOIE, 0);
	nec7210_set_reg_bits(nec_priv, IMR2, HR_DMAO, HR_DMAO);
	e_priv->fifo_dirty = 1;
	enable_dma(e_priv->dma_channel);
	clear_bit(WRITE_READY_BN, &nec_priv->state);
	set_bit(DMA_WRITE_IN_PROGRESS_BN, &nec_priv->state);
// 	printk("%s: in spin lock\n", __FUNCTION__);
	spin_unlock_irqrestore(&board->spinlock, flags);

//	printk("%s: waiting for write.\n", __FUNCTION__);
	// suspend until message is sent
	if(wait_event_interruptible(board->wait, 
	   ((readl(e_priv->write_transfer_counter) & write_transfer_counter_mask) == length 
			   /*&& test_bit(WRITE_READY_BN, &nec_priv->state) */) ||
		test_bit(BUS_ERROR_BN, &nec_priv->state) || 
		test_bit(DEV_CLEAR_BN, &nec_priv->state) ||
		test_bit(TIMO_NUM, &board->status)))
	{
		GPIB_DPRINTK( "gpib write interrupted!\n" );
		retval = -EINTR;
	}
	if(test_bit(TIMO_NUM, &board->status))
		retval = -ETIMEDOUT;
	if(test_and_clear_bit(DEV_CLEAR_BN, &nec_priv->state))
		retval = -EINTR;
	if(test_and_clear_bit(BUS_ERROR_BN, &nec_priv->state))
		retval = -EIO;
	// disable board's dma
	nec7210_set_reg_bits(nec_priv, IMR2, HR_DMAO, 0);

	disable_dma(e_priv->dma_channel);
	if(retval)
		write_byte(nec_priv, AUX_NBAF, AUXMR);	
	*bytes_written = readl(e_priv->write_transfer_counter) & write_transfer_counter_mask;
	if(*bytes_written > length) BUG();
	/*	printk("length=%i, *bytes_written=%i, residue=%i, retval=%i\n",
		length, *bytes_written, get_dma_residue(e_priv->dma_channel), retval);*/
	dma_unmap_single(NULL, address, length, DMA_TO_DEVICE);
	return retval;
}

static int eastwood_accel_write(gpib_board_t *board, 
	uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	size_t remainder = length;
	size_t transfer_size;
	ssize_t retval = 0;
	size_t dma_remainder = remainder;
	*bytes_written = 0;
	if(length < 1) return 0;
	clear_bit(DEV_CLEAR_BN, &nec_priv->state); // XXX FIXME
	if(send_eoi) --dma_remainder;
// 	printk("%s: entering while loop\n", __FUNCTION__);
	
	while(dma_remainder > 0)
	{
		size_t num_bytes;
		transfer_size = (e_priv->dma_buffer_size < dma_remainder) ? 
			e_priv->dma_buffer_size : dma_remainder;
		retval = eastwood_dma_write(board, buffer, transfer_size, &num_bytes);
		*bytes_written += num_bytes;
		if(retval < 0) break;
		dma_remainder -= num_bytes;
		remainder -= num_bytes;
		buffer += num_bytes;
		if(need_resched()) schedule();
	}
	if(retval < 0) return retval;
	//handle sending of last byte with eoi
	if(send_eoi)
	{
		size_t num_bytes;
		// 		printk("%s: handling last byte\n", __FUNCTION__);
		if(remainder != 1) BUG();
		write_byte(nec_priv, AUX_SEOI, AUXMR);
		retval = eastwood_dma_write(board, buffer, remainder, &num_bytes);
		*bytes_written += num_bytes;
		if(retval < 0) return retval;
		remainder -= num_bytes;
	}
// 	printk("%s: bytes send=%i\n", __FUNCTION__, (int)(length - remainder));
	return 0;
}

static int eastwood_dma_read(gpib_board_t *board, uint8_t *buffer,
	size_t length, int *end, size_t *bytes_read)
{
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	int retval = 0;
	unsigned long flags;
	int residue;
	int wait_retval;
	dma_addr_t bus_address;
	// 	printk("%s: enter, bus_address=0x%x, length=%i\n", __FUNCTION__, (unsigned)bus_address,
// 		   (int)length);

	*bytes_read = 0;
	*end = 0;
	if(length == 0)
		return 0;

	clear_dma_fifo(board);
	retval = wait_for_idle(board, 0, 1);
	if(retval < 0) return retval;
	bus_address = dma_map_single(NULL, e_priv->dma_buffer,
		length, DMA_FROM_DEVICE);
	spin_lock_irqsave(&board->spinlock, flags);
	/* program dma controller */
	disable_dma(e_priv->dma_channel);
	eastwood_config_dma(board, 0);
	set_dma_count(e_priv->dma_channel, length);
// 	printk("set dma%i count to %i\n", e_priv->dma_channel, (int)length);
	nios2_set_dma_waddr(e_priv->dma_channel, bus_address);
// 	printk("set dma%i waddr to 0x%x\n", e_priv->dma_channel, (int)bus_address);
	// enable nec7210 dma
	nec7210_set_reg_bits(nec_priv, IMR1, HR_DIIE, 0);
	nec7210_set_reg_bits(nec_priv, IMR2, HR_DMAI, HR_DMAI);
	e_priv->fifo_dirty = 1;
	enable_dma(e_priv->dma_channel);

	set_bit(DMA_READ_IN_PROGRESS_BN, &nec_priv->state);
	clear_bit(READ_READY_BN, &nec_priv->state);
	
	spin_unlock_irqrestore(&board->spinlock, flags);
// 	printk("waiting for data transfer.\n");
	// wait for data to transfer
	if((wait_retval = wait_event_interruptible(board->wait,
		((residue = get_dma_residue(e_priv->dma_channel)) == 0 && 
			test_bit(DMA_READ_IN_PROGRESS_BN, &nec_priv->state) == 0) ||
		test_bit(RECEIVED_END_BN, &nec_priv->state) ||
		test_bit(DEV_CLEAR_BN, &nec_priv->state) ||
		test_bit(TIMO_NUM, &board->status))))
	{
		printk("eastwood: dma read wait interrupted\n");
		retval = -EINTR;
	}
	if(test_bit(TIMO_NUM, &board->status))
		retval = -ETIMEDOUT;
	if(test_bit(DEV_CLEAR_BN, &nec_priv->state))
		retval = -EINTR;
	if(test_and_clear_bit(RECEIVED_END_BN, &nec_priv->state)) 
		*end = 1;
	// stop the dma transfer
	nec7210_set_reg_bits(nec_priv, IMR2, HR_DMAI, 0);
	/* delay a little just to make sure any bytes in dma controller's fifo get
	 written to memory before we disable it */
	udelay(1);
	disable_dma(e_priv->dma_channel);
	/* deal with race if and END byte arrives just as we disable
	 * dma */
	if(*end == 0 && get_dma_residue(e_priv->dma_channel))
	{
		/* run the interrupt handler to make sure the RECEIVED_END bit
		* is updated */
		spin_lock_irqsave(&board->spinlock, flags);
		eastwood_gpib_internal_interrupt(board);
		spin_unlock_irqrestore(&board->spinlock, flags);
		/* if end is true now, re-enable dma to be sure we got the
		 * last byte that caused END to be true */
		if(test_and_clear_bit(RECEIVED_END_BN, &nec_priv->state)) 
		{
			*end = 1;
			nec7210_set_reg_bits(nec_priv, IMR2, HR_DMAI, 1);
			enable_dma(e_priv->dma_channel);
			/* 10 usec should be long enough to transfer a byte if it is there */
			udelay(10);
			nec7210_set_reg_bits(nec_priv, IMR2, HR_DMAI, 0);
			disable_dma(e_priv->dma_channel);
		}
	}
	// record how many bytes we transferred
	residue = get_dma_residue(e_priv->dma_channel);
	if(residue < 0) BUG();
	*bytes_read += length - residue;
	dma_unmap_single(NULL, bus_address, length, DMA_FROM_DEVICE);
	memcpy(buffer, e_priv->dma_buffer, *bytes_read);
// 	printk("\tbytes_read=%i, residue=%i, end=%i, retval=%i, wait_retval=%i\n", 
// 		   *bytes_read, residue, *end, retval, wait_retval);

	return retval;
}

static ssize_t eastwood_accel_read(gpib_board_t *board, uint8_t *buffer, size_t length,
	int *end, size_t *bytes_read)
{
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	size_t remain = length;
	size_t transfer_size;
	int retval = 0;
	int dma_nbytes;
/*	printk("%s: enter, buffer=0x%p, length=%i\n", __FUNCTION__,
		   buffer, (int)length);
	printk("\t dma_buffer=0x%p\n", e_priv->dma_buffer);*/
	clear_bit(DEV_CLEAR_BN, &nec_priv->state); // XXX FIXME
	*end = 0;
	*bytes_read = 0;
	nec7210_set_handshake_mode(board, nec_priv, HR_HLDE);
	write_byte(nec_priv, AUX_FH, AUXMR );
//	nec7210_release_rfd_holdoff(board, nec_priv);
// 	printk("%s: entering while loop\n", __FUNCTION__);
	while(remain > 1)
	{
		transfer_size = (e_priv->dma_buffer_size < remain - 1) ? e_priv->dma_buffer_size : remain - 1;
		retval = eastwood_dma_read(board, buffer, transfer_size, end, &dma_nbytes);
		remain -= dma_nbytes;
		buffer += dma_nbytes;
		*bytes_read += dma_nbytes;
		if(*end) 
		{
			break;
		}
		if(retval < 0) 
		{
// 			printk("%s: early exit, retval=%i\n", __FUNCTION__, (int)retval);
			return retval;
		}
		if(need_resched()) schedule();
	}
	if(*end == 0)
	{
// 		printk("%s: reading last byte\n", __FUNCTION__);
		nec7210_set_handshake_mode(board, nec_priv, HR_HLDA);
		retval = eastwood_dma_read(board, buffer, 1, end, &dma_nbytes);
		*bytes_read += dma_nbytes;
	}
// 	printk("%s: exit, retval=%i\n", __FUNCTION__, (int)retval);
	return retval;
}

gpib_interface_t eastwood_unaccel_interface =
{
	name: "eastwood_unaccel",
	attach: eastwood_attach,
	detach: eastwood_detach,
	read: eastwood_read,
	write: eastwood_write,
	command: eastwood_command,
	take_control: eastwood_take_control,
	go_to_standby: eastwood_go_to_standby,
	request_system_control: eastwood_request_system_control,
	interface_clear: eastwood_interface_clear,
	remote_enable: eastwood_remote_enable,
	enable_eos: eastwood_enable_eos,
	disable_eos: eastwood_disable_eos,
	parallel_poll: eastwood_parallel_poll,
	parallel_poll_configure: eastwood_parallel_poll_configure,
	parallel_poll_response: eastwood_parallel_poll_response,
	line_status: eastwood_line_status,
	update_status: eastwood_update_status,
	primary_address: eastwood_primary_address,
	secondary_address: eastwood_secondary_address,
	serial_poll_response: eastwood_serial_poll_response,
	serial_poll_status: eastwood_serial_poll_status,
	t1_delay: eastwood_t1_delay,
	return_to_local: eastwood_return_to_local,
};

/* eastwood_hybrid uses dma for writes but not for reads.  Added
 to deal with occasional corruption of bytes seen when doing dma
 reads.  From looking at the cb7210 vhdl, I believe the corruption
 is due to a hardware bug triggered by the cpu reading a cb7210
 register just as the dma controller is also doing a read. */
gpib_interface_t eastwood_hybrid_interface =
{
	name: "eastwood_hybrid",
	attach: eastwood_attach,
	detach: eastwood_detach,
	read: eastwood_read,
	write: eastwood_accel_write,
	command: eastwood_command,
	take_control: eastwood_take_control,
	go_to_standby: eastwood_go_to_standby,
	request_system_control: eastwood_request_system_control,
	interface_clear: eastwood_interface_clear,
	remote_enable: eastwood_remote_enable,
	enable_eos: eastwood_enable_eos,
	disable_eos: eastwood_disable_eos,
	parallel_poll: eastwood_parallel_poll,
	parallel_poll_configure: eastwood_parallel_poll_configure,
	parallel_poll_response: eastwood_parallel_poll_response,
	line_status: eastwood_line_status,
	update_status: eastwood_update_status,
	primary_address: eastwood_primary_address,
	secondary_address: eastwood_secondary_address,
	serial_poll_response: eastwood_serial_poll_response,
	serial_poll_status: eastwood_serial_poll_status,
	t1_delay: eastwood_t1_delay,
	return_to_local: eastwood_return_to_local,
};

gpib_interface_t eastwood_interface =
{
	name: "eastwood",
	attach: eastwood_attach,
	detach: eastwood_detach,
	read: eastwood_accel_read,
	write: eastwood_accel_write,
	command: eastwood_command,
	take_control: eastwood_take_control,
	go_to_standby: eastwood_go_to_standby,
	request_system_control: eastwood_request_system_control,
	interface_clear: eastwood_interface_clear,
	remote_enable: eastwood_remote_enable,
	enable_eos: eastwood_enable_eos,
	disable_eos: eastwood_disable_eos,
	parallel_poll: eastwood_parallel_poll,
	parallel_poll_configure: eastwood_parallel_poll_configure,
	parallel_poll_response: eastwood_parallel_poll_response,
	line_status: eastwood_line_status,
	update_status: eastwood_update_status,
	primary_address: eastwood_primary_address,
	secondary_address: eastwood_secondary_address,
	serial_poll_response: eastwood_serial_poll_response,
	serial_poll_status: eastwood_serial_poll_status,
	t1_delay: eastwood_t1_delay,
	return_to_local: eastwood_return_to_local,
};

irqreturn_t eastwood_gpib_internal_interrupt(gpib_board_t *board)
{
	int status1, status2;
	eastwood_private_t *priv = board->private_data;
	nec7210_private_t *nec_priv = &priv->nec7210_priv;
	int retval;
	status1 = read_byte( nec_priv, ISR1 );
	status2 = read_byte( nec_priv, ISR2 );
	retval = nec7210_interrupt_have_status(board, nec_priv, status1, status2);
	
/*	if((status1 & nec_priv->reg_bits[IMR1]) ||
		(status2 & (nec_priv->reg_bits[IMR2] & IMR2_ENABLE_INTR_MASK)))
	{
		printk("eastwood: status1 0x%x, status2 0x%x\n", status1, status2);
	}
*/
	return retval;
}

irqreturn_t eastwood_gpib_interrupt(int irq, void *arg, struct pt_regs *registerp)
{
	gpib_board_t *board = arg;
	unsigned long flags;
	irqreturn_t retval;
	
	spin_lock_irqsave(&board->spinlock, flags);
	retval = eastwood_gpib_internal_interrupt(board);
	spin_unlock_irqrestore(&board->spinlock, flags);
	return retval;
}

int eastwood_allocate_private(gpib_board_t *board)
{
	eastwood_private_t *priv;

	board->private_data = kmalloc(sizeof(eastwood_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -ENOMEM;
	priv = board->private_data;
	memset( priv, 0, sizeof(eastwood_private_t));
	init_nec7210_private(&priv->nec7210_priv);
	priv->dma_channel = -1;
	priv->dma_buffer_size = 0x7ff;
#if 0	
	priv->dma_buffer = (void*)na_ocmem_s1;
// printk("eastwood: using ocmem dma buffer at 0x%p\n", priv->dma_buffer);
#else
	priv->dma_buffer = kmalloc(priv->dma_buffer_size, GFP_KERNEL);
// printk("eastwood: using sdram dma buffer at 0x%p\n", priv->dma_buffer);
#endif
	if(priv->dma_buffer == NULL)
		return -ENOMEM;
	priv->fifo_dirty = 1;
	return 0;
}

void eastwood_generic_detach(gpib_board_t *board)
{
	if(board->private_data)
	{
		eastwood_private_t *e_priv = board->private_data;
		if(e_priv->dma_buffer && (unsigned long)(e_priv->dma_buffer) != na_ocmem_s1)
			kfree(e_priv->dma_buffer);
		kfree(board->private_data);
		board->private_data = NULL;
	}
}

// generic part of attach functions shared by all cb7210 boards
int eastwood_generic_attach(gpib_board_t *board)
{
	eastwood_private_t *e_priv;
	nec7210_private_t *nec_priv;
	int retval;

	board->status = 0;

	retval = eastwood_allocate_private(board);
	if(retval < 0)
		return retval;
	e_priv = board->private_data;
	nec_priv = &e_priv->nec7210_priv;
	nec_priv->read_byte = eastwood_locking_read_byte;
	nec_priv->write_byte = eastwood_locking_write_byte;
	nec_priv->offset = eastwood_reg_offset;
	nec_priv->type = CB7210;
	return 0;
}

int eastwood_dma_handler(void *arg, int status)
{
	gpib_board_t *board = arg;
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;
	unsigned long flags;
	spin_lock_irqsave(&board->spinlock, flags);
// 	printk("%s: dma status=0x%x\n", __FUNCTION__, status);
	if((get_dma_status() & AVALON_BUSY_BIT) == 0)
	{
		clear_bit(DMA_WRITE_IN_PROGRESS_BN, &nec_priv->state);
		clear_bit(DMA_READ_IN_PROGRESS_BN, &nec_priv->state);
		nec7210_set_reg_bits(nec_priv, IMR1, HR_DOIE | HR_DIIE, HR_DOIE | HR_DIIE);
		e_priv->fifo_dirty = 0;
		wake_up_interruptible(&board->wait);
	}
	eastwood_gpib_internal_interrupt(board);
// 	printk("%s: exit\n", __FUNCTION__);
	spin_unlock_irqrestore(&board->spinlock, flags);
	return IRQ_HANDLED;
}

void eastwood_config_dma(gpib_board_t *board, int output)
{
	eastwood_private_t *e_priv = board->private_data;
	if(output)
	{
		nios2_set_dma_waddr(e_priv->dma_channel, (dma_addr_t)e_priv->dma_port);
		nios2_set_dma_wcon(e_priv->dma_channel, 1);
		nios2_set_dma_rcon(e_priv->dma_channel, 0);
	}else
	{
		nios2_set_dma_raddr(e_priv->dma_channel, (dma_addr_t)e_priv->dma_port);
// 		printk("set dma%i raddr to 0x%x\n", e_priv->dma_channel, (int)e_priv->dma_port);
		nios2_set_dma_rcon(e_priv->dma_channel, 1);
// 		printk("set dma%i rcon to 1\n", e_priv->dma_channel);
		nios2_set_dma_wcon(e_priv->dma_channel, 0);
// 		printk("set dma%i wcon to 0\n", e_priv->dma_channel);
	}
}

int eastwood_init(eastwood_private_t *e_priv, gpib_board_t *board )
{
	nec7210_private_t *nec_priv = &e_priv->nec7210_priv;

	nec7210_board_reset(nec_priv, board);
	write_byte(nec_priv, AUX_LO_SPEED, AUXMR);
	/* set clock register for maximum (20 MHz) driving frequency
	 * ICR should be set to clock in megahertz (1-15) and to zero
	 * for clocks faster than 15 MHz (max 20MHz) */
	write_byte(nec_priv, ICR | 10, AUXMR);

	nec7210_board_online( nec_priv, board );

	/* poll so we can detect ATN changes */
#if 1	
	if(gpib_request_pseudo_irq(board, eastwood_gpib_interrupt))
	{
		printk("eastwood_gpib: failed to allocate pseudo_irq\n");
		return -EINVAL;
	}
#endif
	return 0;
}

int eastwood_attach(gpib_board_t *board, gpib_board_config_t config)
{
	eastwood_private_t *e_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = 0;
	int retval;

	retval = eastwood_generic_attach(board);
	if(retval) return retval;

	e_priv = board->private_data;
	nec_priv = &e_priv->nec7210_priv;
	nec_priv->offset = eastwood_reg_offset;	
	if(request_mem_region(na_gpib_0_s0, eastwood_num_regs * eastwood_reg_offset, "eastwood_gpib") == NULL)
	{
		printk("eastwood_gpib: failed to allocate io memory region starting at 0x%x.\n",
			   na_gpib_0_s0);
		return -EBUSY;
	}
	nec_priv->iobase = (void*)(na_gpib_0_s0);
	if(request_mem_region(na_gpib_0_s1, 1, "eastwood_gpib") == NULL)
	{
		printk("eastwood_gpib: failed to allocate io memory port at 0x%x for dma.\n",
			   na_gpib_0_s1);
		return -EBUSY;
	}
	e_priv->dma_port = (void*)na_gpib_0_s1;
	if(request_mem_region(na_gpib_0_s2, 4, "eastwood_gpib") == NULL)
	{
		printk("eastwood_gpib: failed to allocate io memory at 0x%x for write transfer counter.\n",
			   na_gpib_0_s2);
		return -EBUSY;
	}
	e_priv->write_transfer_counter = (void*)na_gpib_0_s2;
	if(request_irq(na_gpib_0_s0_irq, eastwood_gpib_interrupt, isr_flags, "eastwood_gpib", board))
	{
		printk( "eastwood_gpib: request for IRQ %d failed\n", na_gpib_0_s0_irq);
		return -EBUSY;
	}
	e_priv->irq = na_gpib_0_s0_irq;
	retval = nios2_request_dma("eastwood_gpib");
	if(retval < 0) 
	{
		printk( "eastwood_gpib: failed to allocate a dma channel.\n");
		return retval;
	}
	e_priv->dma_channel = retval;
	nios2_set_dma_data_width(e_priv->dma_channel, 1);
	nios2_set_dma_handler(e_priv->dma_channel, &eastwood_dma_handler, board);
	
	return eastwood_init(e_priv, board);
}

void eastwood_detach(gpib_board_t *board)
{
	eastwood_private_t *e_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(e_priv)
	{
		if(e_priv->dma_channel >= 0)
			free_dma(e_priv->dma_channel);
		gpib_free_pseudo_irq(board);
		nec_priv = &e_priv->nec7210_priv;
		if(e_priv->irq)
		{
			free_irq(e_priv->irq, board);
		}
		if(e_priv->write_transfer_counter)
		{
			release_mem_region((unsigned long)e_priv->write_transfer_counter, 4);
		}
		if(e_priv->dma_port)
		{
			release_mem_region((unsigned long)e_priv->dma_port, 1);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset(nec_priv, board);
			release_mem_region((unsigned long)nec_priv->iobase, eastwood_num_regs * eastwood_reg_offset);
		}
	}
	eastwood_generic_detach(board);
}

static int __init eastwood_init_module( void )
{
	gpib_register_driver(&eastwood_unaccel_interface, THIS_MODULE);
	gpib_register_driver(&eastwood_hybrid_interface, THIS_MODULE);
	gpib_register_driver(&eastwood_interface, THIS_MODULE);
	printk("eastwood_gpib: driver version 2006-06-18-1845\n");
	return 0;
}

static void __exit eastwood_exit_module( void )
{
	gpib_unregister_driver(&eastwood_unaccel_interface);
	gpib_unregister_driver(&eastwood_hybrid_interface);
	gpib_unregister_driver(&eastwood_interface);
}

module_init( eastwood_init_module );
module_exit( eastwood_exit_module );







