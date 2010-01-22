/***************************************************************************
                          ni_usb/ni_usb_gpib.c
                             -------------------
 driver for National Instruments usb to gpib adapters

    begin                : 2004-05-29
    copyright            : (C) 2004 by Frank Mori Hess
    email                : fmhess@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include "ni_usb_gpib.h"
#include "gpibP.h"
#include "nec7210.h"
#include "tnt4882_registers.h"

MODULE_LICENSE("GPL");

#define MAX_NUM_NI_USB_INTERFACES 128
static struct usb_interface *ni_usb_driver_interfaces[MAX_NUM_NI_USB_INTERFACES];

static int ni_usb_parse_status_block(const uint8_t *buffer, struct ni_usb_status_block *status);
static int ni_usb_set_interrupt_monitor(gpib_board_t *board, unsigned int monitored_bits);

static DECLARE_MUTEX(ni_usb_hotplug_lock);

//calculates a reasonable timeout in that can be passed to usb functions
static inline unsigned long ni_usb_timeout_msecs(unsigned int usec)
{
	if(usec == 0) return 0;
	return 2000 + usec / 500;
};
// returns timeout code byte for use in ni-usb-b instructions
static unsigned short ni_usb_timeout_code(unsigned int usec)
{
	if( usec == 0 ) return 0xf0;
	else if( usec <= 10 ) return 0xf1;
	else if( usec <= 30 ) return 0xf2;
	else if( usec <= 100 ) return 0xf3;
	else if( usec <= 300 ) return 0xf4;
	else if( usec <= 1000 ) return 0xf5;
	else if( usec <= 3000 ) return 0xf6;
	else if( usec <= 10000 ) return 0xf7;
	else if( usec <= 30000 ) return 0xf8;
	else if( usec <= 100000 ) return 0xf9;
	else if( usec <= 300000 ) return 0xfa;
	else if( usec <= 1000000 ) return 0xfb;
	else if( usec <= 3000000 ) return 0xfc;
	else if( usec <= 10000000 ) return 0xfd;
	else if( usec <= 30000000 ) return 0xfe;
	else if( usec <= 100000000 ) return 0xff;
	else if( usec <= 300000000 ) return 0x01;
	/* NI driver actually uses 0xff for timeout T1000s, which is a bug in their code.
	 * I've verified on a usb-b that a code of 0x2 is correct for a 1000 sec timeout */
	else if( usec <= 1000000000 ) return 0x02;
	else
	{
		printk("%s: bug? usec is greater than 1e9\n", __FILE__);
		return 0xf0;
	}
};

static void ni_usb_bulk_complete(struct urb *urb PT_REGS_ARG)
{
	ni_usb_urb_context_t *context = urb->context;

//	printk("debug: %s: %s: status=0x%x, error_count=%i, actual_length=%i\n", __FILE__, __FUNCTION__,
//		urb->status, urb->error_count, urb->actual_length);
	up(&context->complete);
}

static void ni_usb_timeout_handler(unsigned long arg)
{
	ni_usb_urb_context_t *context = (ni_usb_urb_context_t *) arg;
	context->timed_out = 1;
	up(&context->complete);
};

// I'm using nonblocking loosely here, it only means -EAGAIN can be returned in certain cases
int ni_usb_nonblocking_send_bulk_msg(ni_usb_private_t *ni_priv, void *data, int data_length, int *actual_data_length, int timeout_msecs)
{
	struct usb_device *usb_dev;
	int retval;
	unsigned int out_pipe;
	ni_usb_urb_context_t context;
	struct timer_list timer;

	*actual_data_length = 0;
	retval = down_interruptible(&ni_priv->bulk_transfer_lock);
	if(retval) return retval;
	if(ni_priv->bus_interface == NULL)
	{
		up(&ni_priv->bulk_transfer_lock);
		return -ENODEV;
	}
	if(ni_priv->bulk_urb)
	{
		up(&ni_priv->bulk_transfer_lock);
		return -EAGAIN;
	}
	ni_priv->bulk_urb = usb_alloc_urb(0, GFP_KERNEL);
	if(ni_priv->bulk_urb == NULL)
	{
		up(&ni_priv->bulk_transfer_lock);
		return -ENOMEM;
	}
	usb_dev = interface_to_usbdev(ni_priv->bus_interface);
	out_pipe = usb_sndbulkpipe(usb_dev, ni_priv->bulk_out_endpoint);
	init_MUTEX_LOCKED(&context.complete);
	context.timed_out = 0;
	usb_fill_bulk_urb(ni_priv->bulk_urb, usb_dev, out_pipe, data, data_length,
		&ni_usb_bulk_complete, &context);
	init_timer(&timer);
	if(timeout_msecs)
	{
		timer.expires = jiffies + msecs_to_jiffies(timeout_msecs);
		timer.function = ni_usb_timeout_handler;
		timer.data = (unsigned long) &context;
		add_timer(&timer);
	}
	//printk("%s: submitting urb\n", __FUNCTION__);
	retval = usb_submit_urb(ni_priv->bulk_urb, GFP_KERNEL);
	if(retval)
	{
		if(timer_pending(&timer))
			del_timer_sync(&timer);
		usb_free_urb(ni_priv->bulk_urb);
		ni_priv->bulk_urb = NULL;
		printk("%s: failed to submit bulk out urb, retval=%i\n", __FILE__, retval);
		up(&ni_priv->bulk_transfer_lock);
		return retval;
	}
	up(&ni_priv->bulk_transfer_lock);
	if(down_interruptible(&context.complete))
	{
		printk("%s: %s: interrupted\n", __FILE__, __FUNCTION__);
		if(timer_pending(&timer))
			del_timer_sync(&timer);
		usb_kill_urb(ni_priv->bulk_urb);
		down(&ni_priv->bulk_transfer_lock);
		usb_free_urb(ni_priv->bulk_urb);
		ni_priv->bulk_urb = NULL;
		up(&ni_priv->bulk_transfer_lock);
		return -ERESTARTSYS;
	}
	if(context.timed_out)
	{
		usb_kill_urb(ni_priv->bulk_urb);
		printk("%s: killed urb due to timeout\n", __FUNCTION__);
		retval = -ETIMEDOUT;
	}else
		retval = ni_priv->bulk_urb->status;
	if(timer_pending(&timer))
		del_timer_sync(&timer);
	*actual_data_length = ni_priv->bulk_urb->actual_length;
	down(&ni_priv->bulk_transfer_lock);
	usb_free_urb(ni_priv->bulk_urb);
	ni_priv->bulk_urb = NULL;
	up(&ni_priv->bulk_transfer_lock);
	return retval;
}

static int ni_usb_send_bulk_msg(ni_usb_private_t *ni_priv, void *data, int data_length, int *actual_data_length, int timeout_msecs)
{
	int retval;
	int timeout_msecs_remaining = timeout_msecs;
	retval = ni_usb_nonblocking_send_bulk_msg(ni_priv, data, data_length, actual_data_length, timeout_msecs_remaining);
	while(retval == -EAGAIN && (timeout_msecs == 0 || timeout_msecs_remaining > 0))
	{
		if(msleep_interruptible(1))
			return -ERESTARTSYS;
		retval = ni_usb_nonblocking_send_bulk_msg(ni_priv, data, data_length, actual_data_length, timeout_msecs_remaining);
		if(timeout_msecs != 0) --timeout_msecs_remaining;
	}
	if(timeout_msecs != 0 && timeout_msecs_remaining <= 0) return -ETIMEDOUT;
	return retval;
}

// I'm using nonblocking loosely here, it only means -EAGAIN can be returned in certain cases
int ni_usb_nonblocking_receive_bulk_msg(ni_usb_private_t *ni_priv, void *data, int data_length, int *actual_data_length, int timeout_msecs)
{
	struct usb_device *usb_dev;
	int retval;
	unsigned int in_pipe;
	ni_usb_urb_context_t context;
	struct timer_list timer;

	*actual_data_length = 0;
	retval = down_interruptible(&ni_priv->bulk_transfer_lock);
	if(retval) return retval;
	if(ni_priv->bus_interface == NULL)
	{
		up(&ni_priv->bulk_transfer_lock);
		return -ENODEV;
	}
	if(ni_priv->bulk_urb)
	{
		up(&ni_priv->bulk_transfer_lock);
		return -EAGAIN;
	}
	ni_priv->bulk_urb = usb_alloc_urb(0, GFP_KERNEL);
	if(ni_priv->bulk_urb == NULL)
	{
		up(&ni_priv->bulk_transfer_lock);
		return -ENOMEM;
	}
	usb_dev = interface_to_usbdev(ni_priv->bus_interface);
	in_pipe = usb_rcvbulkpipe(usb_dev, ni_priv->bulk_in_endpoint);
	init_MUTEX_LOCKED(&context.complete);
	context.timed_out = 0;
	usb_fill_bulk_urb(ni_priv->bulk_urb, usb_dev, in_pipe, data, data_length,
		&ni_usb_bulk_complete, &context);
	init_timer(&timer);
	if(timeout_msecs)
	{
		timer.expires = jiffies + msecs_to_jiffies(timeout_msecs);
		timer.function = ni_usb_timeout_handler;
		timer.data = (unsigned long) &context;
		add_timer(&timer);
	}
	//printk("%s: submitting urb\n", __FUNCTION__);
	retval = usb_submit_urb(ni_priv->bulk_urb, GFP_KERNEL);
	if(retval)
	{
		if(timer_pending(&timer))
			del_timer_sync(&timer);
		usb_free_urb(ni_priv->bulk_urb);
		ni_priv->bulk_urb = NULL;
		printk("%s: failed to submit bulk out urb, retval=%i\n", __FILE__, retval);
		up(&ni_priv->bulk_transfer_lock);
		return retval;
	}
	up(&ni_priv->bulk_transfer_lock);
	if(down_interruptible(&context.complete))
	{
		printk("%s: %s: interrupted\n", __FILE__, __FUNCTION__);
		if(timer_pending(&timer))
			del_timer_sync(&timer);
		usb_kill_urb(ni_priv->bulk_urb);
		down(&ni_priv->bulk_transfer_lock);
		usb_free_urb(ni_priv->bulk_urb);
		ni_priv->bulk_urb = NULL;
		up(&ni_priv->bulk_transfer_lock);
		return -ERESTARTSYS;
	}
	if(context.timed_out)
	{
		usb_kill_urb(ni_priv->bulk_urb);
		printk("%s: killed urb due to timeout\n", __FUNCTION__);
		retval = -ETIMEDOUT;
	}else
		retval = ni_priv->bulk_urb->status;
	if(timer_pending(&timer))
		del_timer_sync(&timer);
	*actual_data_length = ni_priv->bulk_urb->actual_length;
	down(&ni_priv->bulk_transfer_lock);
	usb_free_urb(ni_priv->bulk_urb);
	ni_priv->bulk_urb = NULL;
	up(&ni_priv->bulk_transfer_lock);
	return retval;
}

static int ni_usb_receive_bulk_msg(ni_usb_private_t *ni_priv, void *data, int data_length, int *actual_data_length, int timeout_msecs)
{
	int retval;
	int timeout_msecs_remaining = timeout_msecs;
	retval = ni_usb_nonblocking_receive_bulk_msg(ni_priv, data, data_length, actual_data_length, timeout_msecs_remaining);
	while(retval == -EAGAIN && (timeout_msecs == 0 || timeout_msecs_remaining > 0))
	{
		if(msleep_interruptible(1))
			return -ERESTARTSYS;
		retval = ni_usb_nonblocking_receive_bulk_msg(ni_priv, data, data_length, actual_data_length, timeout_msecs_remaining);
		if(timeout_msecs != 0) --timeout_msecs_remaining;
	}
	if(timeout_msecs && timeout_msecs_remaining <= 0) return -ETIMEDOUT;
	return retval;
}

int ni_usb_receive_control_msg(ni_usb_private_t *ni_priv, __u8 request, __u8 requesttype, __u16 value, __u16 index,
	void *data, __u16 size, int timeout_msecs)
{
	struct usb_device *usb_dev;
	int retval;
	unsigned int in_pipe;

	retval = down_interruptible(&ni_priv->control_transfer_lock);
	if(retval) return retval;
	if(ni_priv->bus_interface == NULL)
	{
		up(&ni_priv->control_transfer_lock);
		return -ENODEV;
	}
	usb_dev = interface_to_usbdev(ni_priv->bus_interface);
	in_pipe = usb_rcvctrlpipe(usb_dev, 0);
	retval = USB_CONTROL_MSG(usb_dev, in_pipe, request, requesttype, value, index, data, size, timeout_msecs);
	up(&ni_priv->control_transfer_lock);
	return retval;
}

void ni_usb_soft_update_status(gpib_board_t *board, unsigned int ni_usb_ibsta, unsigned int clear_mask)
{
	ni_usb_private_t *ni_priv = board->private_data;
	static const unsigned int ni_usb_ibsta_mask = SRQI | ATN | CIC | REM | LACS | TACS | LOK;
	unsigned int need_monitoring_bits;
	unsigned long flags;

	board->status &= ~clear_mask;
	board->status &= ~ni_usb_ibsta_mask;
	board->status |= ni_usb_ibsta & ni_usb_ibsta_mask;
//	if(ni_usb_ibsta & ~ni_usb_ibsta_mask)
//	{
//		printk("%s: debug: ibsta from ni gpib usb adapter is 0x%x\n", __FILE__, ni_usb_ibsta);
//	}
	//FIXME should generate events on DTAS and DCAS
	need_monitoring_bits = ni_usb_ibsta_monitor_mask;
	spin_lock_irqsave(&board->spinlock, flags);
	ni_priv->monitored_ibsta_bits &= ~ni_usb_ibsta;
	need_monitoring_bits &= ~ni_priv->monitored_ibsta_bits;
	spin_unlock_irqrestore(&board->spinlock, flags);
	if(need_monitoring_bits & ~ni_usb_ibsta)
	{
		ni_usb_set_interrupt_monitor(board, ni_usb_ibsta_monitor_mask);
	}else if(need_monitoring_bits & ni_usb_ibsta)
	{
		wake_up_interruptible( &board->wait );
	}
// 	printk("%s: ni_usb_ibsta=0x%x\n", __FUNCTION__, ni_usb_ibsta);
	return;
}

static int ni_usb_parse_status_block(const uint8_t *buffer, struct ni_usb_status_block *status)
{
	uint16_t count;

	status->id = buffer[0];
	status->ibsta = (buffer[1] << 8) | buffer[2];
	status->error_code = buffer[3];
	count = buffer[4] | (buffer[5] << 8);
	count = ~count;
	count++;
	status->count = count;
	return 8;
};

static void ni_usb_dump_raw_block(const uint8_t *raw_data, int length)
{
	int i;

	printk("%s:", __FUNCTION__);
	for(i = 0; i < length; i++)
	{
		if(i % 8 == 0)
			printk("\n");
		printk(" %2x", raw_data[i]);
	}
	printk("\n");
}

int ni_usb_parse_register_read_block(const uint8_t *raw_data, unsigned int *results, int num_results)
{
	int i = 0;
	int j;
	int unexpected = 0;
	static const int results_per_chunk = 3;
	for(j = 0; j < num_results;)
	{
		int k;
		if(raw_data[i++] != NIUSB_REGISTER_READ_DATA_START_ID)
		{
			printk("%s: %s: parse error: wrong start id\n", __FILE__, __FUNCTION__);
			unexpected = 1;
		}
		for(k = 0; k < results_per_chunk && j < num_results; ++k)
		{
			results[j++] = raw_data[i++];
		}
	}
	while(i % 4)
	{
		i++;
	}
	if(raw_data[i++] != NIUSB_REGISTER_READ_DATA_END_ID)
	{
		printk("%s: %s: parse error: wrong end id\n", __FILE__, __FUNCTION__);
		unexpected = 1;
	}
	if(raw_data[i++] % results_per_chunk != num_results % results_per_chunk)
	{
		printk("%s: %s: parse error: wrong count=%i for NIUSB_REGISTER_READ_DATA_END\n",
			__FILE__, __FUNCTION__, (int)raw_data[i - 1]);
		unexpected = 1;
	}
	while(i % 4)
	{
		if(raw_data[i++] != 0)
		{
			printk("%s: %s: unexpected data: raw_data[%i]=0x%x, expected 0\n",
				__FILE__, __FUNCTION__, i - 1, (int)raw_data[i - 1]);
			unexpected = 1;
		}
	}
	if(unexpected)
		ni_usb_dump_raw_block(raw_data, i);
	return i;
}

int ni_usb_parse_termination_block(const uint8_t *buffer)
{
	int i = 0;

	if(buffer[i++] != NIUSB_TERM_ID ||
		buffer[i++] != 0x0 ||
		buffer[i++] != 0x0 ||
		buffer[i++] != 0x0)
	{
		printk("%s: received unexpected termination block\n" , __FILE__);
		printk(" expected: 0x%x 0x%x 0x%x 0x%x\n",
			NIUSB_TERM_ID, 0x0, 0x0, 0x0);
		printk(" received: 0x%x 0x%x 0x%x 0x%x\n",
			buffer[i - 4], buffer[i - 3], buffer[i - 2], buffer[i - 1]);
	}
	return i;
};

int parse_board_ibrd_readback(const uint8_t *raw_data, struct ni_usb_status_block *status,
	uint8_t *parsed_data, int parsed_data_length, int *actual_bytes_read)
{
	static const int ibrd_data_block_length = 0xf;
	static const int ibrd_extended_data_block_length = 0x1e;
	int data_block_length = 0;
	int i = 0;
	int j = 0;
	int k;
	unsigned int adr1_bits;
	int num_data_blocks = 0;
	struct ni_usb_status_block register_write_status;
	int unexpected = 0;

	while(raw_data[i] == NIUSB_IBRD_DATA_ID || raw_data[i] == NIUSB_IBRD_EXTENDED_DATA_ID)
	{
		if(raw_data[i] == NIUSB_IBRD_DATA_ID)
		{
			data_block_length = ibrd_data_block_length;
		}else if(raw_data[i] == NIUSB_IBRD_EXTENDED_DATA_ID)
		{
			data_block_length = ibrd_extended_data_block_length;
			if(raw_data[++i] != 0)
			{
				printk("%s: %s: unexpected data: raw_data[%i]=0x%x, expected 0\n",
					__FILE__, __FUNCTION__, i, (int)raw_data[i]);
				unexpected = 1;
			}
		}else
		{
			printk("%s: logic bug!\n", __FILE__);
			return -EINVAL;
		}
		++i;
		for(k = 0; k < data_block_length; k++)
		{
			if(j < parsed_data_length)
				parsed_data[j++] = raw_data[i++];
			else
				++i;
		}
		++num_data_blocks;
	}
	i += ni_usb_parse_status_block(&raw_data[i], status);
	if(status->id != NIUSB_IBRD_STATUS_ID)
	{
		printk("%s: bug: status->id=%i, != ibrd_status_id\n", __FILE__, status->id);
		return -EIO;
	}
	adr1_bits = raw_data[i++];
	if(num_data_blocks)
		*actual_bytes_read = (num_data_blocks - 1) * data_block_length + raw_data[i++];
	else
	{
		++i;
		*actual_bytes_read = 0;
	}
	if(*actual_bytes_read > j)
	{
		printk("%s: bug: discarded data. actual_bytes_read=%i, j=%i\n", __FILE__, *actual_bytes_read, j);
	}
	for(k = 0; k < 2; k++)
		if(raw_data[i++] != 0)
		{
			printk("%s: %s: unexpected data: raw_data[%i]=0x%x, expected 0\n",
				__FILE__, __FUNCTION__, i - 1, (int)raw_data[i - 1]);
			unexpected = 1;
		}
	i += ni_usb_parse_status_block(&raw_data[i], &register_write_status);
	if(register_write_status.id != NIUSB_REG_WRITE_ID)
	{
		printk("%s: %s: unexpected data: register write status id=0x%x, expected 0x%x\n",
			__FILE__, __FUNCTION__, register_write_status.id, NIUSB_REG_WRITE_ID);
		unexpected = 1;
	}
	if(raw_data[i++] != 2)
	{
		printk("%s: %s: unexpected data: register write count=%i, expected 2\n",
			__FILE__, __FUNCTION__, (int)raw_data[i - 1]);
		unexpected = 1;
	}
	for(k = 0; k < 3; k++)
		if(raw_data[i++] != 0)
		{
			printk("%s: %s: unexpected data: raw_data[%i]=0x%x, expected 0\n",
				__FILE__, __FUNCTION__, i - 1, (int)raw_data[i - 1]);
			unexpected = 1;
		}
	i += ni_usb_parse_termination_block(&raw_data[i]);
	if(unexpected)
		ni_usb_dump_raw_block(raw_data, i);
	return i;
}

int ni_usb_parse_reg_write_status_block(const uint8_t *raw_data, struct ni_usb_status_block *status, int *writes_completed)
{
	int i = 0;

	i += ni_usb_parse_status_block(raw_data, status);
	*writes_completed = raw_data[i++];
	while(i % 4) i++;
	return i;
}
int ni_usb_write_registers(ni_usb_private_t *ni_priv, const struct ni_usb_register *writes, int num_writes,
	unsigned int *ibsta)
{
	int retval;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int bytes_written = 0, bytes_read = 0;
	int i = 0;
	int j;
	struct ni_usb_status_block status;
	static const int bytes_per_write = 3;
	int reg_writes_completed;

	out_data_length = num_writes * bytes_per_write + 0x10;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL)
	{
		printk("%s: %s: kmalloc failed\n", __FILE__, __FUNCTION__);
		return -ENOMEM;
	}
	i += ni_usb_bulk_register_write_header(&out_data[i], num_writes);
	for(j = 0; j < num_writes; j++)
	{
		i += ni_usb_bulk_register_write(&out_data[i], writes[j]);
	}
	while(i % 4)
		out_data[i++] = 0x00;
	i += ni_usb_bulk_termination(&out_data[i]);
	if(i > out_data_length)
	{
		printk("%s: bug! buffer overrun\n", __FUNCTION__);
	}
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, i, &bytes_written, 1000);
	kfree(out_data);
	if(retval)
	{
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__,
			retval, bytes_written, i);
		return retval;
	}
	in_data_length = 0x20;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, in_data_length, &bytes_read, 1000);
	if(retval || bytes_read != 16)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, bytes_read=%i\n", __FILE__, __FUNCTION__, retval, bytes_read);
		ni_usb_dump_raw_block(in_data, bytes_read);
		kfree(in_data);
		return retval;
	}
	ni_usb_parse_reg_write_status_block(in_data, &status, &reg_writes_completed);
	//FIXME parse extra 09 status bits and termination
	kfree(in_data);
	if(status.id != NIUSB_REG_WRITE_ID)
	{
		printk("%s: %s: parse error, id=0x%x != NIUSB_REG_WRITE_ID\n", __FILE__, __FUNCTION__, status.id);
		return -EIO;
	}
	if(status.error_code)
	{
		printk("%s: %s: nonzero error code 0x%x\n", __FILE__, __FUNCTION__, status.error_code);
		return -EIO;
	}
	if(reg_writes_completed != num_writes)
	{
		printk("%s: %s: reg_writes_completed=%i, num_writes=%i\n", __FILE__, __FUNCTION__,
			reg_writes_completed, num_writes);
		return -EIO;
	}
	if(ibsta) *ibsta = status.ibsta;
	return 0;
}

// interface functions
int ni_usb_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int usb_bytes_written = 0, usb_bytes_read = 0;
	int i = 0;
	int complement_count;
	int actual_length;
	struct ni_usb_status_block status;
	static const int max_read_length = 0xffff;
	struct ni_usb_register reg;

	*bytes_read = 0;
	if(length > max_read_length)
	{
		length = max_read_length;
		printk("%s: read length too long\n", __FILE__);
	}
	out_data_length = 0x20;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL) return -ENOMEM;
	out_data[i++] = 0x0a;
	out_data[i++] = ni_priv->eos_mode >> 8;
	out_data[i++] = ni_priv->eos_char;
	out_data[i++] = ni_usb_timeout_code(board->usec_timeout);
	complement_count = length - 1;
	complement_count = ~complement_count;
	out_data[i++] = complement_count & 0xff;
	out_data[i++] = (complement_count >> 8) & 0xff;
	out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	i += ni_usb_bulk_register_write_header(&out_data[i], 2);
	reg.device = NIUSB_SUBDEV_TNT4882;
	reg.address = nec7210_to_tnt4882_offset(AUXMR);
	reg.value = AUX_HLDI;
	i += ni_usb_bulk_register_write(&out_data[i], reg);
	reg.value = AUX_CLEAR_END;
	i += ni_usb_bulk_register_write(&out_data[i], reg);
	while(i % 4)	// pad with zeros to 4-byte boundary
		out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, i, &usb_bytes_written, 1000);
	kfree(out_data);
	if(retval || usb_bytes_written != i)
	{
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, usb_bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__, retval, usb_bytes_written, i);
		return retval;
	}
	in_data_length = (length / 30 + 1) * 0x20 + 0x20;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL) return -ENOMEM;
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, in_data_length, &usb_bytes_read,
		ni_usb_timeout_msecs(board->usec_timeout));
	if(retval)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, usb_bytes_read=%i\n", __FILE__, __FUNCTION__, retval, usb_bytes_read);
		kfree(in_data);
		return retval;
	}
	retval = parse_board_ibrd_readback(in_data, &status, buffer, length, &actual_length);
	if(retval != usb_bytes_read)
	{
		printk("%s: %s: retval=%i usb_bytes_read=%i\n", __FILE__, __FUNCTION__, retval, usb_bytes_read);
	}
	kfree(in_data);
	if(actual_length != length - status.count)
	{
		printk("%s: %s: actual_length=%i expected=%li\n", __FILE__, __FUNCTION__, actual_length, (long)(length - status.count));
		ni_usb_dump_raw_block(in_data, usb_bytes_read);
	}
	switch(status.error_code)
	{
	case NIUSB_NO_ERROR:
		retval = 0;
		break;
	case NIUSB_ADDRESSING_ERROR:
		retval = -EIO;
		break;
	case NIUSB_TIMEOUT_ERROR:
		retval = -ETIMEDOUT;
		break;
	case NIUSB_EOSMODE_ERROR:
		printk("%s: %s: driver bug, we should have been able to avoid NIUSB_EOSMODE_ERROR.\n", __FILE__, __FUNCTION__);
		retval = -EINVAL;
		break;
	default:
		printk("%s: %s: unknown error code=%i\n", __FILE__, __FUNCTION__, status.error_code);
		retval = -EIO;
		break;
	}
	ni_usb_soft_update_status(board, status.ibsta, 0);
	if(status.ibsta & END) *end = 1;
	else *end = 0;
	*bytes_read = actual_length;
	return retval;
}

static int ni_usb_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int usb_bytes_written = 0, usb_bytes_read = 0;
	int i = 0, j;
	int complement_count;
	struct ni_usb_status_block status;
	static const int max_write_length = 0xffff;

	*bytes_written = 0;
	if(length > max_write_length)
	{
		length = max_write_length;
		send_eoi = 0;
		printk("%s: write length too long\n", __FILE__);
	}
	out_data_length = length + 0x10;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL) return -ENOMEM;
	out_data[i++] = 0x0d;
	complement_count = length;
	complement_count = length - 1;
	complement_count = ~complement_count;
	out_data[i++] = complement_count & 0xff;
	out_data[i++] = (complement_count >> 8) & 0xff;
	out_data[i++] = ni_usb_timeout_code(board->usec_timeout);
	out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	if(send_eoi)
		out_data[i++] = 0x8;
	else
		out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	for(j = 0; j < length; j++)
		out_data[i++] = buffer[j];
	while(i % 4)	// pad with zeros to 4-byte boundary
		out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, i, &usb_bytes_written,
		ni_usb_timeout_msecs(board->usec_timeout));
	kfree(out_data);
	if(retval || usb_bytes_written != i)
	{
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, usb_bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__, retval, usb_bytes_written, i);
		return retval;
	}
	in_data_length = 0x10;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL) return -ENOMEM;
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, in_data_length, &usb_bytes_read,
		ni_usb_timeout_msecs(board->usec_timeout));
	if(retval || usb_bytes_read != 12)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, usb_bytes_read=%i\n", __FILE__, __FUNCTION__, retval, usb_bytes_read);
		kfree(in_data);
		return retval;
	}
	ni_usb_parse_status_block(in_data, &status);
	kfree(in_data);
	switch(status.error_code)
	{
	case NIUSB_NO_ERROR:
		retval = 0;
		break;
	case NIUSB_ADDRESSING_ERROR:
		retval = -EIO;
		break;
	case NIUSB_NO_LISTENER_ERROR:
		retval = -EIO;
		break;
	case NIUSB_TIMEOUT_ERROR:
		retval = -ETIMEDOUT;
		break;
	default:
		printk("%s: %s: unknown error code=%i\n", __FILE__, __FUNCTION__, status.error_code);
		retval = -EIO;
		break;
	}
	ni_usb_soft_update_status(board, status.ibsta, 0);
	*bytes_written = length - status.count;
	return retval;
}

ssize_t ni_usb_command_chunk(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int bytes_written = 0, bytes_read = 0;
	int i = 0, j;
	unsigned complement_count;
	struct ni_usb_status_block status;
	// usb-b gives error 4 if you try to send more than 16 command bytes at once
	static const int max_command_length = 0x10;

	if(length > max_command_length) length = max_command_length;
	out_data_length = length + 0x10;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL) return -ENOMEM;
	out_data[i++] = 0x0c;
	complement_count = length - 1;
	complement_count = ~complement_count;
	out_data[i++] = complement_count;
	out_data[i++] = 0x0;
	out_data[i++] = ni_usb_timeout_code(board->usec_timeout);
	for(j = 0; j < length; j++)
		out_data[i++] = buffer[j];
	while(i % 4)	// pad with zeros to 4-byte boundary
		out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, i, &bytes_written,
		ni_usb_timeout_msecs(board->usec_timeout));
	kfree(out_data);
	if(retval || bytes_written != i)
	{
		int k;
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__, retval, bytes_written, i);
		printk("\t was attempting to write command bytes:\n");
		for(k = 0; k < length; ++k)
		{
			printk(" 0x%2x", buffer[k]);
		}
		printk("\n");
		return retval;
	}
	in_data_length = 0x10;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL) return -ENOMEM;
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, in_data_length, &bytes_read,
		ni_usb_timeout_msecs(board->usec_timeout));
	if(retval || bytes_read != 12)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, bytes_read=%i\n", __FILE__, __FUNCTION__, retval, bytes_read);
		kfree(in_data);
		return retval;
	}
	ni_usb_parse_status_block(in_data, &status);
	kfree(in_data);
	switch(status.error_code)
	{
	case NIUSB_NO_ERROR:
		break;
	case NIUSB_NO_BUS_ERROR:
		return -EIO;
		break;
	case NIUSB_EOSMODE_ERROR:
		printk("%s: %s: got eosmode error.  Driver bug?\n", __FILE__, __FUNCTION__);
		return -EIO;
		break;
	default:
		printk("%s: %s: unknown error code=%i\n", __FILE__, __FUNCTION__, status.error_code);
		return -EIO;
		break;
	}
	ni_usb_soft_update_status(board, status.ibsta, 0);
	return length - status.count;
}

ssize_t ni_usb_command(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	unsigned count = 0;
	int retval;

	while(count < length)
	{
		retval = ni_usb_command_chunk(board, buffer + count, length - count);
		if(retval < 0) return retval;
		count += retval;
	}
	return count;
}

int ni_usb_take_control(gpib_board_t *board, int synchronous)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int bytes_written = 0, bytes_read = 0;
	int i = 0;
	struct ni_usb_status_block status;

	out_data_length = 0x10;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	out_data[i++] = NIUSB_IBCAC_ID;
	if(synchronous)
		out_data[i++] = 0x1;
	else
		out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, i, &bytes_written, 1000);
	kfree(out_data);
	if(retval || bytes_written != i)
	{
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__, retval, bytes_written, i);
		return retval;
	}
	in_data_length = 0x10;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, in_data_length, &bytes_read, 1000);
	if(retval || bytes_read != 12)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, bytes_read=%i\n", __FILE__, __FUNCTION__, retval, bytes_read);
		kfree(in_data);
		return retval;
	}
	ni_usb_parse_status_block(in_data, &status);
	kfree(in_data);
	ni_usb_soft_update_status(board, status.ibsta, 0);
	return 0;
}
int ni_usb_go_to_standby(gpib_board_t *board)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int bytes_written = 0, bytes_read = 0;
	int i = 0;
	struct ni_usb_status_block status;

	out_data_length = 0x10;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	out_data[i++] = NIUSB_IBGTS_ID;
	out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, i, &bytes_written, 1000);
	kfree(out_data);
	if(retval || bytes_written != i)
	{
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__, retval, bytes_written, i);
		return retval;
	}
	in_data_length = 0x20;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, in_data_length, &bytes_read, 1000);
	if(retval || bytes_read != 12)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, bytes_read=%i\n", __FILE__, __FUNCTION__, retval, bytes_read);
		kfree(in_data);
		return retval;
	}
	ni_usb_parse_status_block(in_data, &status);
	kfree(in_data);
	if(status.id != NIUSB_IBGTS_ID)
	{
		printk("%s: %s: bug: status.id 0x%x != INUSB_IBGTS_ID\n", __FILE__, __FUNCTION__, status.id);
	}
	ni_usb_soft_update_status(board, status.ibsta, 0);
	return 0;
}
//FIXME should change prototype to return int
void ni_usb_request_system_control( gpib_board_t *board, int request_control )
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register writes[4];
	unsigned int ibsta;

	if(request_control)
	{
		writes[i].device = NIUSB_SUBDEV_TNT4882;
		writes[i].address = CMDR;
		writes[i].value = SETSC;
		i++;
		writes[i].device = NIUSB_SUBDEV_TNT4882;
		writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
		writes[i].value = AUX_CIFC;
		i++;
	}else
	{
		writes[i].device = NIUSB_SUBDEV_TNT4882;
		writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
		writes[i].value = AUX_CREN;
		i++;
		writes[i].device = NIUSB_SUBDEV_TNT4882;
		writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
		writes[i].value = AUX_CIFC;
		i++;
		writes[i].device = NIUSB_SUBDEV_TNT4882;
		writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
		writes[i].value = AUX_DSC;
		i++;
		writes[i].device = NIUSB_SUBDEV_TNT4882;
		writes[i].address = CMDR;
		writes[i].value = CLRSC;
		i++;
	}
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return;// retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return;// 0;
}
//FIXME maybe the interface should have a "pulse interface clear" function that can return an error?
void ni_usb_interface_clear(gpib_board_t *board, int assert)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int bytes_written = 0, bytes_read = 0;
	int i = 0;
	struct ni_usb_status_block status;

	// FIXME: we are going to pulse when assert is true, and ignore otherwise
	if(assert == 0) return;
	out_data_length = 0x10;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return;
	}
	out_data[i++] = NIUSB_IBSIC_ID;
	out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, i, &bytes_written, 1000);
	kfree(out_data);
	if(retval || bytes_written != i)
	{
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__, retval, bytes_written, i);
		return;
	}
	in_data_length = 0x10;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return;
	}
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, in_data_length, &bytes_read, 1000);
	if(retval || bytes_read != 12)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, bytes_read=%i\n", __FILE__, __FUNCTION__, retval, bytes_read);
		kfree(in_data);
		return;
	}
	ni_usb_parse_status_block(in_data, &status);
	kfree(in_data);
	ni_usb_soft_update_status(board, status.ibsta, 0);
	return;
}
void ni_usb_remote_enable(gpib_board_t *board, int enable)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	struct ni_usb_register reg;
	unsigned int ibsta;

	reg.device = NIUSB_SUBDEV_TNT4882;
	reg.address = nec7210_to_tnt4882_offset(AUXMR);
	if(enable)
	{
		reg.value = AUX_SREN;
	}else
	{
		reg.value = AUX_CREN;
	}
	retval = ni_usb_write_registers(ni_priv, &reg, 1, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return; //retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return;// 0;
}

int ni_usb_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits)
{
	ni_usb_private_t *ni_priv = board->private_data;

	ni_priv->eos_char = eos_byte;
	ni_priv->eos_mode |= REOS;
	if(compare_8_bits)
		ni_priv->eos_mode |= BIN;
	else
		ni_priv->eos_mode &= ~BIN;
	return 0;
}

void ni_usb_disable_eos(gpib_board_t *board)
{
	ni_usb_private_t *ni_priv = board->private_data;
	/* adapter gets unhappy if you don't zero all the bits
		for the eos mode and eos char (returns error 4 on reads). */
	ni_priv->eos_mode = 0;
	ni_priv->eos_char = 0;
}

unsigned int ni_usb_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	static const int bufferLength = 8;
	uint8_t *buffer;
	struct ni_usb_status_block status;

	//printk("%s: receive control pipe is %i\n", __FILE__, pipe);
	buffer = kmalloc(bufferLength, GFP_KERNEL);
	if(buffer == NULL)
	{
		printk("%s: kmalloc failed!\n", __FILE__);
		return board->status;
	}
	retval = ni_usb_receive_control_msg(ni_priv, ni_usb_control_request, USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
		0x200, 0x0, buffer, bufferLength, 1000);
	if(retval != bufferLength)
	{
		printk("%s: usb_control_msg returned %i\n", __FILE__, retval);
		kfree(buffer);
		return board->status;
	}
	ni_usb_parse_status_block(buffer, &status);
	kfree(buffer);
	ni_usb_soft_update_status(board, status.ibsta, clear_mask);
	return board->status;
}
//FIXME: prototype should return int
void ni_usb_primary_address(gpib_board_t *board, unsigned int address)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register writes[2];
	unsigned int ibsta;

	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(ADR);
	writes[i].value = address;
	i++;
	writes[i].device = NIUSB_SUBDEV_UNKNOWN2;
	writes[i].address = 0x0;
	writes[i].value = address;
	i++;
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return;// retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return;// 0;
}

int ni_usb_write_sad(struct ni_usb_register *writes, int address, int enable)
{
	unsigned int adr_bits, admr_bits;
	int i = 0;

	adr_bits = HR_ARS;
	admr_bits = HR_TRM0 | HR_TRM1;
	if(enable)
	{
		adr_bits |= address;
		admr_bits |= HR_ADM1;
	}else
	{
		adr_bits |= HR_DT | HR_DL;
		admr_bits |= HR_ADM0;
	}
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(ADR);
	writes[i].value = adr_bits;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(ADMR);
	writes[i].value = admr_bits;
	i++;
	writes[i].device = NIUSB_SUBDEV_UNKNOWN2;
	writes[i].address = 0x1;
	writes[i].value = enable ? MSA(address) : 0x0;
	i++;
	return i;
}

void ni_usb_secondary_address(gpib_board_t *board, unsigned int address, int enable)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register writes[3];
	unsigned int ibsta;

	i += ni_usb_write_sad(writes, address, enable);
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return; // retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return; // 0;
}
int ni_usb_parallel_poll(gpib_board_t *board, uint8_t *result)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int bytes_written = 0, bytes_read = 0;
	int i = 0;
	int j = 0;
	struct ni_usb_status_block status;

	out_data_length = 0x10;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	out_data[i++] = NIUSB_IBRPP_ID;
	out_data[i++] = 0xf0;	//FIXME: this should be the parallel poll timeout code
	out_data[i++] = 0x0;
	out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, i, &bytes_written, 1000 /*FIXME: should use parallel poll timeout (not supported yet)*/);
	kfree(out_data);
	if(retval || bytes_written != i)
	{
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__, retval, bytes_written, i);
		return retval;
	}
	in_data_length = 0x20;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, in_data_length, &bytes_read, 1000 /*FIXME: should use parallel poll timeout (not supported yet)*/);
	if(retval)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, bytes_read=%i\n", __FILE__, __FUNCTION__, retval, bytes_read);
		kfree(in_data);
		return retval;
	}
	j += ni_usb_parse_status_block(in_data, &status);
	*result = in_data[j++];
	kfree(in_data);
	ni_usb_soft_update_status(board, status.ibsta, 0);
	return 0;
}
void ni_usb_parallel_poll_configure(gpib_board_t *board, uint8_t config)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register writes[1];
	unsigned int ibsta;

	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = PPR | config;
	i++;
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return;// retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return;// 0;
}
void ni_usb_parallel_poll_response(gpib_board_t *board, int ist)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register writes[1];
	unsigned int ibsta;

	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	if(ist)
		writes[i].value = AUX_SPPF;
	else
		writes[i].value = AUX_CPPF;
	i++;
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return;// retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return;// 0;
}
void ni_usb_serial_poll_response(gpib_board_t *board, uint8_t status)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register writes[1];
	unsigned int ibsta;

	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(SPMR);
	writes[i].value = status;
	i++;
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return;// retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return;// 0;
}
uint8_t ni_usb_serial_poll_status( gpib_board_t *board )
{
	return 0;
}
void ni_usb_return_to_local( gpib_board_t *board )
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register writes[1];
	unsigned int ibsta;

	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUX_RTL;
	i++;
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return;// retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return;// 0;
}
int ni_usb_line_status( const gpib_board_t *board )
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	uint8_t *out_data, *in_data;
	int out_data_length, in_data_length;
	int bytes_written = 0, bytes_read = 0;
	int i = 0;
	unsigned int bsr_bits;
	int line_status = ValidALL;
	// NI windows driver reads 0xd(HSSEL), 0xc (ARD0), 0x1f (BSR)

	out_data_length = 0x20;
	out_data = kmalloc(out_data_length, GFP_KERNEL);
	if(out_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	i += ni_usb_bulk_register_read_header(&out_data[i], 1);
	i += ni_usb_bulk_register_read(&out_data[i], NIUSB_SUBDEV_TNT4882, BSR);
	while(i % 4)
		out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_nonblocking_send_bulk_msg(ni_priv, out_data, i, &bytes_written, 1000);
	kfree(out_data);
	if(retval || bytes_written != i)
	{
		if(retval != -EAGAIN)
			printk("%s: %s: ni_usb_send_bulk_msg returned %i, bytes_written=%i, i=%i\n", __FILE__, __FUNCTION__, retval, bytes_written, i);
		return retval;
	}
	in_data_length = 0x20;
	in_data = kmalloc(in_data_length, GFP_KERNEL);
	if(in_data == NULL)
	{
		printk("%s: kmalloc failed\n", __FILE__);
		return -ENOMEM;
	}
	retval = ni_usb_nonblocking_receive_bulk_msg(ni_priv, in_data, in_data_length, &bytes_read, 1000);
	if(retval)
	{
		if(retval != -EAGAIN)
			printk("%s: %s: ni_usb_receive_bulk_msg returned %i, bytes_read=%i\n", __FILE__, __FUNCTION__, retval, bytes_read);
		kfree(in_data);
		return retval;
	}
#if 0
{
int k;
printk("reg read response:\n");
for(k=0;k<bytes_read;k++)
	printk("%.2x ", in_data[k]);
printk("\n");
}
#endif
	ni_usb_parse_register_read_block(in_data, &bsr_bits, 1);
	kfree(in_data);
	if(bsr_bits & BCSR_REN_BIT)
		line_status |= BusREN;
	if(bsr_bits & BCSR_IFC_BIT)
		line_status |= BusIFC;
	if(bsr_bits & BCSR_SRQ_BIT)
		line_status |= BusSRQ;
	if(bsr_bits & BCSR_EOI_BIT)
		line_status |= BusEOI;
	if(bsr_bits & BCSR_NRFD_BIT)
		line_status |= BusNRFD;
	if(bsr_bits & BCSR_NDAC_BIT)
		line_status |= BusNDAC;
	if(bsr_bits & BCSR_DAV_BIT)
		line_status |= BusDAV;
	if(bsr_bits & BCSR_ATN_BIT)
		line_status |= BusATN;
	return line_status;
}
unsigned int ni_usb_t1_delay( gpib_board_t *board, unsigned int nano_sec )
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register writes[3];
	unsigned int ibsta;
	unsigned int actual_ns = 2000;

	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	if(nano_sec <= 1100)
	{
		writes[i].value = AUXRI | USTD | SISB;
		actual_ns = 1100;
	}else
		writes[i].value = AUXRI | SISB;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	if(nano_sec <= 500)
	{
		writes[i].value = AUXRB | HR_TRI;
		actual_ns = 500;
	}else
		writes[i].value = AUXRB;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = KEYREG;
	if(nano_sec <= 350)
	{
		writes[i].value = MSTD;
		actual_ns = 350;
	}else
		writes[i].value = 0x0;
	i++;
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval < 0)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return -1;	//FIXME should change return type to int for error reporting
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return actual_ns;
}

static int ni_usb_allocate_private(gpib_board_t *board)
{
	ni_usb_private_t *ni_priv;

	board->private_data = kmalloc(sizeof(ni_usb_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -ENOMEM;
	ni_priv = board->private_data;
	memset(ni_priv, 0, sizeof(ni_usb_private_t));
	init_MUTEX(&ni_priv->bulk_transfer_lock);
	init_MUTEX(&ni_priv->control_transfer_lock);
	init_MUTEX(&ni_priv->interrupt_transfer_lock);
	return 0;
}

static void ni_usb_free_private(ni_usb_private_t *ni_priv)
{
	if(ni_priv->interrupt_urb)
		usb_free_urb(ni_priv->interrupt_urb);
	kfree(ni_priv);
	return;
}

static int ni_usb_init(gpib_board_t *board)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	int i = 0;
	struct ni_usb_register *writes;
	unsigned int ibsta;
	static const int writes_length = 24;

	writes = kmalloc(sizeof(struct ni_usb_register) * writes_length, GFP_KERNEL);
	if(writes == NULL)
	{
		printk("%s: %s: kmalloc failed\n", __FILE__, __FUNCTION__);
		return -ENOMEM;
	}
	writes[i].device = NIUSB_SUBDEV_UNKNOWN3;
	writes[i].address = 0x10;
	writes[i].value = 0x0;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = CMDR;
	writes[i].value = SOFT_RESET;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUX_7210;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = SWAPPED_AUXCR;
	writes[i].value = AUX_7210;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = HSSEL;
	writes[i].value = TNT_ONE_CHIP_BIT;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUX_CR;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = IMR0;
	writes[i].value = TNT_IMR0_ALWAYS_BITS;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(IMR1);
	writes[i].value = 0x0;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(IMR2);
	writes[i].value = 0x0;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = IMR3;
	writes[i].value = 0x0;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUX_HLDI;
	i++;
	/* the following three writes should share code with set_t1_delay */
	board->t1_nano_sec = 500;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUXRI | SISB | USTD;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUXRB | HR_TRI;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = KEYREG;
	writes[i].value = 0x0;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUXRG | NTNL_BIT;
	i++;
#if 0	// request system control
	i += ni_usb_bulk_register_write(&out_data[i], NIUSB_SUBDEV_TNT4882, CMDR, SETSC);
	i += ni_usb_bulk_register_write(&out_data[i], NIUSB_SUBDEV_TNT4882, nec7210_to_tnt4882_offset(AUXMR), AUX_CIFC);
#endif
	// primary address
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(ADR);
	writes[i].value = board->pad;
	i++;
	writes[i].device = NIUSB_SUBDEV_UNKNOWN2;
	writes[i].address = 0x0;
	writes[i].value = board->pad;
	i++;
	// secondary address
	i += ni_usb_write_sad(&writes[i], board->sad, board->sad >= 0);
	// is this a timeout?
	writes[i].device = NIUSB_SUBDEV_UNKNOWN2;
	writes[i].address = 0x2;
	writes[i].value = 0xfd;
	i++;
	// what is this?  There is no documented tnt4882 register at offset 0xf
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = 0xf;
	writes[i].value = 0x11;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUX_PON;
	i++;
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUX_CPPF;
	i++;
	if(i > writes_length)
	{
		printk("%s: %s: bug!, buffer overrun, i=%i\n", __FILE__, __FUNCTION__, i);
		return -EINVAL;
	}
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	kfree(writes);
	if(retval)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return retval;
	}
	ni_usb_soft_update_status(board, ibsta, 0);
	return 0;
}

void ni_usb_interrupt_complete(struct urb *urb PT_REGS_ARG)
{
	gpib_board_t *board = urb->context;
	ni_usb_private_t *ni_priv = board->private_data;
	int retval;
	struct ni_usb_status_block status;
	unsigned long flags;

// 	printk("debug: %s: %s: status=0x%x, error_count=%i, actual_length=%i\n", __FILE__, __FUNCTION__,
// 		urb->status, urb->error_count, urb->actual_length);

	// don't resubmit if urb was unlinked
	if(urb->status) return;
	ni_usb_parse_status_block(urb->transfer_buffer, &status);
// 	printk("debug: ibsta=0x%x\n", status.ibsta);

	spin_lock_irqsave(&board->spinlock, flags);
//	ni_priv->monitored_ibsta_bits &= ~status.ibsta;
	ni_priv->monitored_ibsta_bits = 0;
// 	printk("debug: monitored_ibsta_bits=0x%x\n", ni_priv->monitored_ibsta_bits);
	spin_unlock_irqrestore(&board->spinlock, flags);

	retval = usb_submit_urb(ni_priv->interrupt_urb, GFP_ATOMIC);
	if(retval)
	{
		printk("%s: failed to resubmit interrupt urb\n", __FUNCTION__);
	}
	wake_up_interruptible( &board->wait );
}

static int ni_usb_set_interrupt_monitor(gpib_board_t *board, unsigned int monitored_bits)
{
	int retval;
	ni_usb_private_t *ni_priv = board->private_data;
	static const int bufferLength = 8;
	uint8_t *buffer;
	struct ni_usb_status_block status;
	unsigned long flags;
	//printk("%s: receive control pipe is %i\n", __FILE__, pipe);
	buffer = kmalloc(bufferLength, GFP_KERNEL);
	if(buffer == NULL)
	{
		printk("%s: kmalloc failed!\n", __FILE__);
		return -ENOMEM;
	}
	spin_lock_irqsave(&board->spinlock, flags);
	ni_priv->monitored_ibsta_bits = ni_usb_ibsta_monitor_mask & monitored_bits;
// 	printk("debug: %s: monitored_ibsta_bits=0x%x\n", __FUNCTION__, ni_priv->monitored_ibsta_bits);
	spin_unlock_irqrestore(&board->spinlock, flags);
	retval = ni_usb_receive_control_msg(ni_priv, ni_usb_control_request, USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
		0x300, ni_usb_ibsta_monitor_mask & monitored_bits, buffer, bufferLength, 1000);
	if(retval != bufferLength)
	{
		printk("%s: usb_control_msg returned %i\n", __FILE__, retval);
		kfree(buffer);
		return -1;
	}
	ni_usb_parse_status_block(buffer, &status);
	kfree(buffer);
	return 0;
}

static int ni_usb_setup_urbs(gpib_board_t *board)
{
	ni_usb_private_t *ni_priv = board->private_data;
	struct usb_device *usb_dev;
	int int_pipe;
	int retval;
	retval = down_interruptible(&ni_priv->interrupt_transfer_lock);
	if(retval) return retval;
	if(ni_priv->bus_interface == NULL)
	{
		up(&ni_priv->interrupt_transfer_lock);
		return -ENODEV;
	}
	ni_priv->interrupt_urb = usb_alloc_urb(0, GFP_KERNEL);
	if(ni_priv->interrupt_urb == NULL)
	{
		up(&ni_priv->interrupt_transfer_lock);
		return -ENOMEM;
	}
	usb_dev = interface_to_usbdev(ni_priv->bus_interface);
	int_pipe = usb_rcvintpipe(usb_dev, ni_priv->interrupt_in_endpoint);
	usb_fill_int_urb(ni_priv->interrupt_urb, usb_dev, int_pipe, ni_priv->interrupt_buffer,
		sizeof(ni_priv->interrupt_buffer), &ni_usb_interrupt_complete, board, 1);
	retval = usb_submit_urb(ni_priv->interrupt_urb, GFP_KERNEL);
	up(&ni_priv->interrupt_transfer_lock);
	if(retval)
	{
		printk("%s: failed to submit first interrupt urb, retval=%i\n", __FILE__, retval);
		return retval;
	}
	return 0;
}


static void ni_usb_cleanup_urbs(ni_usb_private_t *ni_priv)
{
	if(ni_priv && ni_priv->bus_interface)
	{
		if(ni_priv->interrupt_urb)
			usb_kill_urb(ni_priv->interrupt_urb);
		if(ni_priv->bulk_urb)
			usb_kill_urb(ni_priv->bulk_urb);
	}
};

static int ni_usb_b_read_serial_number(ni_usb_private_t *ni_priv)
{
	int retval;
	uint8_t out_data[0x20];
	uint8_t in_data[0x20];
	int bytes_written = 0, bytes_read = 0;
	int i = 0;
	static const int num_reads = 4;
	unsigned results[4];
	int j;
	unsigned serial_number;
// 	printk("%s: %s\n", __FILE__, __FUNCTION__);
	i += ni_usb_bulk_register_read_header(&out_data[i], num_reads);
	i += ni_usb_bulk_register_read(&out_data[i], NIUSB_SUBDEV_UNKNOWN3, SERIAL_NUMBER_1_REG);
	i += ni_usb_bulk_register_read(&out_data[i], NIUSB_SUBDEV_UNKNOWN3, SERIAL_NUMBER_2_REG);
	i += ni_usb_bulk_register_read(&out_data[i], NIUSB_SUBDEV_UNKNOWN3, SERIAL_NUMBER_3_REG);
	i += ni_usb_bulk_register_read(&out_data[i], NIUSB_SUBDEV_UNKNOWN3, SERIAL_NUMBER_4_REG);
	while(i % 4)
		out_data[i++] = 0x0;
	i += ni_usb_bulk_termination(&out_data[i]);
	retval = ni_usb_send_bulk_msg(ni_priv, out_data, sizeof(out_data), &bytes_written, 1000);
	if(retval)
	{
		printk("%s: %s: ni_usb_send_bulk_msg returned %i, bytes_written=%i, i=%li\n", __FILE__, __FUNCTION__,
			retval, bytes_written, (long)(sizeof(out_data)));
		return retval;
	}
	retval = ni_usb_receive_bulk_msg(ni_priv, in_data, sizeof(in_data), &bytes_read, 1000);
	if(retval)
	{
		printk("%s: %s: ni_usb_receive_bulk_msg returned %i, bytes_read=%i\n", __FILE__, __FUNCTION__, retval, bytes_read);
		ni_usb_dump_raw_block(in_data, bytes_read);
		return retval;
	}
	if(sizeof(results) / sizeof(results[0]) < num_reads) BUG();
	ni_usb_parse_register_read_block(in_data, results, num_reads);
	serial_number = 0;
	for(j = 0; j < num_reads; ++j)
	{
		serial_number |= (results[j] & 0xff) << (8 * j);
	}
	printk("%s: board serial number is 0x%x\n", __FUNCTION__, serial_number);
	return 0;
}

static int ni_usb_hs_wait_for_ready(ni_usb_private_t *ni_priv)
{
	uint8_t buffer[0x10] = {0};
	int retval;
	static const int timeout = 50;
	static const int msec_sleep_duration = 100;
	int i;
	static const int poll_ready_request = 0x40;
	static const int serial_number_request = 0x41;
	int j;
	int unexpected = 0;
	unsigned serial_number;
	retval = ni_usb_receive_control_msg(ni_priv, serial_number_request, USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
		0x0, 0x0, buffer, sizeof(buffer), 1000);
	if(retval < 0)
	{
		printk("%s: usb_control_msg request 0x%x returned %i\n", __FILE__, serial_number_request, retval);
		return retval;
	}
	j = 0;
	if(buffer[j] != serial_number_request)
	{
		printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x%x\n",
			__FILE__, __FUNCTION__, j, (int)buffer[j], serial_number_request);
		unexpected = 1;
	}
	if(unexpected)
		ni_usb_dump_raw_block(buffer, retval);
	if(retval != 5)
	{
		printk("%s: %s: received unexpected number of bytes = %i, expected 5\n",
			__FILE__, __FUNCTION__, retval);
		ni_usb_dump_raw_block(buffer, retval);
	}
	serial_number = 0;
	serial_number |= buffer[++j];
	serial_number |= (buffer[++j] << 8);
	serial_number |= (buffer[++j] << 16);
	serial_number |= (buffer[++j] << 24);
	printk("%s: board serial number is 0x%x\n", __FUNCTION__, serial_number);
	for(i = 0; i < timeout; ++i)
	{
		int ready = 0;
		retval = ni_usb_receive_control_msg(ni_priv, poll_ready_request, USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0x0, 0x0, buffer, sizeof(buffer), 100);
		if(retval < 0)
		{
			printk("%s: usb_control_msg request 0x%x returned %i\n", __FILE__, poll_ready_request, retval);
			return retval;
		}
		j = 0;
		unexpected = 0;
		if(buffer[j] != poll_ready_request)
		{
			printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x%x\n",
				__FILE__, __FUNCTION__, j, (int)buffer[j], poll_ready_request);
			unexpected = 1;
		}
		if(buffer[++j] != 0x1)
		{
			printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x%x\n",
				__FILE__, __FUNCTION__, j, (int)buffer[j], 0x1);
			unexpected = 1;
		}
		if(buffer[++j] != 0x0)
		{
			printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x%x\n",
				__FILE__, __FUNCTION__, j, (int)buffer[j], 0x0);
			unexpected = 1;
		}
		++j;
		if(buffer[j] != 0x1 && buffer[j] != 0x8) // MC usb-488 sends 0x8 here
		{
			printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x1 or 0x8\n",
				__FILE__, __FUNCTION__, j, (int)buffer[j]);
			unexpected = 1;
		}
		if(buffer[++j] != 0x30)
		{
			printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x%x\n",
				__FILE__, __FUNCTION__, j, (int)buffer[j], 0x30);
			unexpected = 1;
		}
		++j;
		if(buffer[j] != 0x1 && buffer[j] != 0x0) // MC usb-488 sends 0x0 here
		{
			printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x1 or 0x0\n",
				__FILE__, __FUNCTION__, j, (int)buffer[j]);
			unexpected = 1;
		}
		if(buffer[++j] != 0x0)
		{
			ready = 1;
			if(buffer[j] != 0x2)
			{
				printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x%x\n",
					__FILE__, __FUNCTION__, j, (int)buffer[j], 0x2);
				unexpected = 1;
			}
		}
		if(buffer[++j] != 0x0)
		{
			ready = 1;
			if(buffer[j] != 0x3 && buffer[j] != 0x5) // MC usb-488 sends 0x5 here
			{
				printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x3 or 0x5\n",
					__FILE__, __FUNCTION__, j, (int)buffer[j]);
				unexpected = 1;
			}
		}
		++j;
		if(buffer[j] != 0x0 && buffer[j] != 0x2) // MC usb-488 sends 0x2 here
		{
			printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x0 ox 0x2\n",
				__FILE__, __FUNCTION__, j, (int)buffer[j]);
			unexpected = 1;
		}
		if(buffer[++j] != 0x0)
		{
			printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x%x\n",
				__FILE__, __FUNCTION__, j, (int)buffer[j], 0x0);
			unexpected = 1;
		}
		if(buffer[++j] != 0x0)
		{
			ready = 1;
			if(buffer[j] != 0x96 && buffer[j] != 0x7) // MC usb-488 sends 0x7 here
			{
				printk("%s: %s: unexpected data: buffer[%i]=0x%x, expected 0x96 or 0x07\n",
					__FILE__, __FUNCTION__, j, (int)buffer[j]);
				unexpected = 1;
			}
		}
		if(unexpected)
			ni_usb_dump_raw_block(buffer, retval);
		if(ready) break;
		retval = msleep_interruptible(msec_sleep_duration);
		if(retval)
		{
			printk("ni_usb_gpib: msleep interrupted\n");
			return -ERESTARTSYS;
		}
	}
	return 0;
}

int ni_usb_attach(gpib_board_t *board, gpib_board_config_t config)
{
	int retval;
	int i;
	ni_usb_private_t *ni_priv;
	int product_id;

	printk("ni_usb_gpib: attach\n");
	if(down_interruptible(&ni_usb_hotplug_lock))
		return -ERESTARTSYS;
	retval = ni_usb_allocate_private(board);
	if(retval < 0)
	{
		up(&ni_usb_hotplug_lock);
		return retval;
	}
	ni_priv = board->private_data;
	/*FIXME: should allow user to specifiy which device he wants to attach.
	 Use usb_make_path() */
	for(i = 0; i < MAX_NUM_NI_USB_INTERFACES; i++)
	{
		if(ni_usb_driver_interfaces[i] && usb_get_intfdata(ni_usb_driver_interfaces[i]) == NULL)
		{
			ni_priv->bus_interface = ni_usb_driver_interfaces[i];
			usb_set_intfdata(ni_usb_driver_interfaces[i], board);
			printk("\tattached to bus interface %i, address 0x%p\n", i, ni_priv->bus_interface);
			break;
		}
	}
	if(i == MAX_NUM_NI_USB_INTERFACES)
	{
		up(&ni_usb_hotplug_lock);
		printk("No supported NI usb gpib adapters found, have you loaded its firmware?\n");
		return -ENODEV;
	}
	if(usb_reset_configuration(interface_to_usbdev(ni_priv->bus_interface)))
	{
		printk("ni_usb_gpib: usb_reset_configuration() failed.\n");
	}
	product_id = USBID_TO_CPU(interface_to_usbdev(ni_priv->bus_interface)->descriptor.idProduct);
	printk("\tproduct id=0x%x\n", product_id);
	if(product_id == USB_DEVICE_ID_NI_USB_B)
	{
		ni_priv->bulk_out_endpoint = NIUSB_B_BULK_OUT_ENDPOINT;
		ni_priv->bulk_in_endpoint = NIUSB_B_BULK_IN_ENDPOINT;
		ni_priv->interrupt_in_endpoint = NIUSB_B_INTERRUPT_IN_ENDPOINT;
		ni_usb_b_read_serial_number(ni_priv);
	}else if(product_id == USB_DEVICE_ID_NI_USB_HS || product_id == USB_DEVICE_ID_MC_USB_488)
	{
		ni_priv->bulk_out_endpoint = NIUSB_HS_BULK_OUT_ENDPOINT;
		ni_priv->bulk_in_endpoint = NIUSB_HS_BULK_IN_ENDPOINT;
		ni_priv->interrupt_in_endpoint = NIUSB_HS_INTERRUPT_IN_ENDPOINT;
		retval = ni_usb_hs_wait_for_ready(ni_priv);
		if(retval < 0)
		{
			up(&ni_usb_hotplug_lock);
			return retval;
		}
	}else
	{
		up(&ni_usb_hotplug_lock);
		printk("\tDriver bug: unknown endpoints for usb device id\n");
		return -EINVAL;
	}
	retval = ni_usb_setup_urbs(board);
	if(retval < 0)
	{
		up(&ni_usb_hotplug_lock);
		return retval;
	}
	retval = ni_usb_set_interrupt_monitor(board, 0);
	if(retval < 0)
	{
		up(&ni_usb_hotplug_lock);
		return retval;
	}
	retval = ni_usb_init(board);
	if(retval < 0)
	{
		up(&ni_usb_hotplug_lock);
		return retval;
	}
	retval = ni_usb_set_interrupt_monitor(board, ni_usb_ibsta_monitor_mask);
	if(retval < 0)
	{
		up(&ni_usb_hotplug_lock);
		return retval;
	}
	up(&ni_usb_hotplug_lock);
	return retval;
}

static int ni_usb_shutdown_hardware(ni_usb_private_t *ni_priv)
{
	int retval;
	int i = 0;
	struct ni_usb_register writes[2];
	static const int writes_length = sizeof(writes) / sizeof(writes[0]);
	unsigned int ibsta;

// 	printk("%s: %s\n", __FILE__, __FUNCTION__);
	writes[i].device = NIUSB_SUBDEV_TNT4882;
	writes[i].address = nec7210_to_tnt4882_offset(AUXMR);
	writes[i].value = AUX_CR;
	i++;
	writes[i].device = NIUSB_SUBDEV_UNKNOWN3;
	writes[i].address = 0x10;
	writes[i].value = 0x0;
	i++;
	if(i > writes_length)
	{
		printk("%s: %s: bug!, buffer overrun, i=%i\n", __FILE__, __FUNCTION__, i);
		return -EINVAL;
	}
	retval = ni_usb_write_registers(ni_priv, writes, i, &ibsta);
	if(retval)
	{
		printk("%s: %s: register write failed, retval=%i\n", __FILE__, __FUNCTION__, retval);
		return retval;
	}
	return 0;
}

void ni_usb_detach(gpib_board_t *board)
{
	ni_usb_private_t *ni_priv;

	down(&ni_usb_hotplug_lock);
//	printk("%s: enter\n", __FUNCTION__);
// under windows, software unplug does chip_reset nec7210 aux command, then writes 0x0 to address 0x10 of device 3
	ni_priv = board->private_data;
	if(ni_priv)
	{
		if(ni_priv->bus_interface)
		{
			ni_usb_set_interrupt_monitor(board, 0);
			ni_usb_shutdown_hardware(ni_priv);
			usb_set_intfdata(ni_priv->bus_interface, NULL);
		}
		down(&ni_priv->bulk_transfer_lock);
		down(&ni_priv->control_transfer_lock);
		down(&ni_priv->interrupt_transfer_lock);
		ni_usb_cleanup_urbs(ni_priv);
		ni_usb_free_private(ni_priv);
	}
//	printk("%s: exit\n", __FUNCTION__);
	up(&ni_usb_hotplug_lock);
}

gpib_interface_t ni_usb_gpib_interface =
{
	name: "ni_usb_b",
	attach: ni_usb_attach,
	detach: ni_usb_detach,
	read: ni_usb_read,
	write: ni_usb_write,
	command: ni_usb_command,
	take_control: ni_usb_take_control,
	go_to_standby: ni_usb_go_to_standby,
	request_system_control: ni_usb_request_system_control,
	interface_clear: ni_usb_interface_clear,
	remote_enable: ni_usb_remote_enable,
	enable_eos: ni_usb_enable_eos,
	disable_eos: ni_usb_disable_eos,
	parallel_poll: ni_usb_parallel_poll,
	parallel_poll_configure: ni_usb_parallel_poll_configure,
	parallel_poll_response: ni_usb_parallel_poll_response,
	line_status: ni_usb_line_status,
	update_status: ni_usb_update_status,
	primary_address: ni_usb_primary_address,
	secondary_address: ni_usb_secondary_address,
	serial_poll_response: ni_usb_serial_poll_response,
	serial_poll_status: ni_usb_serial_poll_status,
	t1_delay: ni_usb_t1_delay,
	return_to_local: ni_usb_return_to_local,
};

// Table with the USB-devices: just now only testing IDs
static struct usb_device_id ni_usb_driver_device_table [] =
{
	{USB_DEVICE(USB_VENDOR_ID_NI, USB_DEVICE_ID_NI_USB_B)},
	{USB_DEVICE(USB_VENDOR_ID_NI, USB_DEVICE_ID_NI_USB_HS)},
	{USB_DEVICE(USB_VENDOR_ID_NI, USB_DEVICE_ID_MC_USB_488)},
	{} /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, ni_usb_driver_device_table);

static int ni_usb_driver_probe(struct usb_interface *interface,
	const struct usb_device_id *id)
{
	int i;
	char *path;
	static const int pathLength = 1024;

//	printk("ni_usb_driver_probe\n");
	if(down_interruptible(&ni_usb_hotplug_lock))
		return -ERESTARTSYS;
	usb_get_dev(interface_to_usbdev(interface));
	for(i = 0; i < MAX_NUM_NI_USB_INTERFACES; i++)
	{
		if(ni_usb_driver_interfaces[i] == NULL)
		{
			ni_usb_driver_interfaces[i] = interface;
			usb_set_intfdata(interface, NULL);
//			printk("set bus interface %i to address 0x%p\n", i, interface);
			break;
		}
	}
	if(i == MAX_NUM_NI_USB_INTERFACES)
	{
		usb_put_dev(interface_to_usbdev(interface));
		up(&ni_usb_hotplug_lock);
		printk("ni_usb_gpib: out of space in ni_usb_driver_interfaces[]\n");
		return -1;
	}
	path = kmalloc(pathLength, GFP_KERNEL);
	if(path == NULL)
	{
		usb_put_dev(interface_to_usbdev(interface));
		up(&ni_usb_hotplug_lock);
		return -ENOMEM;
	}
	usb_make_path(interface_to_usbdev(interface), path, pathLength);
	printk("ni_usb_gpib: probe succeeded for path: %s\n", path);
	kfree(path);
	up(&ni_usb_hotplug_lock);
	return 0;
}

static void ni_usb_driver_disconnect(struct usb_interface *interface)
{
	int i;

	down(&ni_usb_hotplug_lock);
//	printk("%s: enter\n", __FUNCTION__);
	for(i = 0; i < MAX_NUM_NI_USB_INTERFACES; i++)
	{
		if(ni_usb_driver_interfaces[i] == interface)
		{
			gpib_board_t *board = usb_get_intfdata(interface);

			if(board)
			{
				ni_usb_private_t *ni_priv = board->private_data;
				if(ni_priv)
				{
					down(&ni_priv->bulk_transfer_lock);
					down(&ni_priv->control_transfer_lock);
					down(&ni_priv->interrupt_transfer_lock);
					ni_usb_cleanup_urbs(ni_priv);
					ni_priv->bus_interface = NULL;
					up(&ni_priv->interrupt_transfer_lock);
					up(&ni_priv->control_transfer_lock);
					up(&ni_priv->bulk_transfer_lock);
				}
			}
//			printk("nulled ni_usb_driver_interfaces[%i]\n", i);
			ni_usb_driver_interfaces[i] = NULL;
			break;
		}
	}
	if(i == MAX_NUM_NI_USB_INTERFACES)
	{
		printk("unable to find interface in ni_usb_driver_interfaces[]? bug?\n");
	}
	usb_put_dev(interface_to_usbdev(interface));
//	printk("%s: exit\n", __FUNCTION__);
	up(&ni_usb_hotplug_lock);
}

static struct usb_driver ni_usb_bus_driver =
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
	.owner = THIS_MODULE,
#endif
	.name = "ni_usb_gpib",
	.probe = ni_usb_driver_probe,
	.disconnect = ni_usb_driver_disconnect,
	.id_table = ni_usb_driver_device_table,
};

static int __init ni_usb_init_module(void)
{
	int i;

	pr_info("ni_usb_gpib driver loading");
	for(i = 0; i < MAX_NUM_NI_USB_INTERFACES; i++)
		ni_usb_driver_interfaces[i] = NULL;
	usb_register(&ni_usb_bus_driver);
	gpib_register_driver(&ni_usb_gpib_interface, THIS_MODULE);

	return 0;
}

static void __exit ni_usb_exit_module(void)
{
	pr_info("ni_usb_gpib driver unloading");
//	printk("%s: enter\n", __FUNCTION__);
	gpib_unregister_driver(&ni_usb_gpib_interface);
	usb_deregister(&ni_usb_bus_driver);
//	printk("%s: exit\n", __FUNCTION__);
}

module_init(ni_usb_init_module);
module_exit(ni_usb_exit_module);

