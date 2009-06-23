/***************************************************************************
                               sys/osfuncs.c
                             -------------------

    copyright            : (C) 2001, 2002 by Frank Mori Hess
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

#include "ibsys.h"
#include "autopoll.h"

#include <linux/fcntl.h>
#include <linux/kmod.h>
#include <linux/vmalloc.h>

static int board_type_ioctl(gpib_file_private_t *file_priv, gpib_board_t *board, unsigned long arg);
static int read_ioctl( gpib_file_private_t *file_priv, gpib_board_t *board,
	unsigned long arg);
static int write_ioctl( gpib_file_private_t *file_priv, gpib_board_t *board,
	unsigned long arg);
static int command_ioctl( gpib_file_private_t *file_priv, gpib_board_t *board,
	unsigned long arg);
static int open_dev_ioctl( struct file *filep, gpib_board_t *board, unsigned long arg );
static int close_dev_ioctl( struct file *filep, gpib_board_t *board, unsigned long arg );
static int serial_poll_ioctl( gpib_board_t *board, unsigned long arg );
static int wait_ioctl( gpib_file_private_t *file_priv, gpib_board_t *board, unsigned long arg );
static int parallel_poll_ioctl( gpib_board_t *board, unsigned long arg );
static int online_ioctl( gpib_board_t *board, unsigned long arg );
static int remote_enable_ioctl( gpib_board_t *board, unsigned long arg );
static int take_control_ioctl( gpib_board_t *board, unsigned long arg );
static int line_status_ioctl( gpib_board_t *board, unsigned long arg );
static int pad_ioctl( gpib_board_t *board, gpib_file_private_t *file_priv,
	unsigned long arg );
static int sad_ioctl( gpib_board_t *board, gpib_file_private_t *file_priv,
	unsigned long arg );
static int eos_ioctl( gpib_board_t *board, unsigned long arg );
static int request_service_ioctl( gpib_board_t *board, unsigned long arg );
static int iobase_ioctl( gpib_board_t *board, unsigned long arg );
static int irq_ioctl( gpib_board_t *board, unsigned long arg );
static int dma_ioctl( gpib_board_t *board, unsigned long arg );
static int autospoll_ioctl(gpib_board_t *board, unsigned long arg);
static int mutex_ioctl( gpib_board_t *board, gpib_file_private_t *file_priv,
	unsigned long arg );
static int timeout_ioctl( gpib_board_t *board, unsigned long arg );
static int status_bytes_ioctl( gpib_board_t *board, unsigned long arg );
static int board_info_ioctl( const gpib_board_t *board, unsigned long arg );
static int ppc_ioctl( gpib_board_t *board, unsigned long arg );
static int query_board_rsv_ioctl( gpib_board_t *board, unsigned long arg );
static int interface_clear_ioctl( gpib_board_t *board, unsigned long arg );
static int select_pci_ioctl( gpib_board_t *board, unsigned long arg );
static int event_ioctl( gpib_board_t *board, unsigned long arg );
static int request_system_control_ioctl( gpib_board_t *board, unsigned long arg );
static int t1_delay_ioctl( gpib_board_t *board, unsigned long arg );

static int cleanup_open_devices( gpib_file_private_t *file_priv, gpib_board_t *board );

static gpib_descriptor_t* handle_to_descriptor( const gpib_file_private_t *file_priv,
	int handle )
{
	if( handle < 0 || handle >= GPIB_MAX_NUM_DESCRIPTORS )
	{
		printk( "gpib: invalid handle %i\n", handle );
		return NULL;
	}

	return file_priv->descriptors[ handle ];
}

static int init_gpib_file_private( gpib_file_private_t *priv )
{
	memset(priv, 0, sizeof(*priv));
	priv->descriptors[ 0 ] = kmalloc( sizeof( gpib_descriptor_t ), GFP_KERNEL );
	if( priv->descriptors[ 0 ] == NULL )
	{
		printk( "gpib: failed to allocate default board descriptor\n" );
		return -ENOMEM;
	}
	init_gpib_descriptor( priv->descriptors[ 0 ] );
	priv->descriptors[ 0 ]->is_board = 1;
	mutex_init(&priv->descriptors_mutex);
	return 0;
}

int ibopen(struct inode *inode, struct file *filep)
{
	unsigned int minor = iminor(inode);
	gpib_board_t *board;
	gpib_file_private_t *priv;

	if(minor >= GPIB_MAX_NUM_BOARDS)
	{
		printk("gpib: invalid minor number of device file\n");
		return -ENXIO;
	}

	board = &board_array[minor];

	filep->private_data = kmalloc( sizeof( gpib_file_private_t ), GFP_KERNEL );
	if( filep->private_data == NULL )
	{
		return -ENOMEM;
	}
	priv = filep->private_data;
	init_gpib_file_private( ( gpib_file_private_t * ) filep->private_data );

	GPIB_DPRINTK( "gpib: opening minor %d\n", minor );

	if(board->use_count == 0)
	{
		char module_string[ 32 ];
		int retval;

		snprintf(module_string, sizeof(module_string), "gpib%i", minor);
		retval = request_module( module_string );
		if( retval )
		{
			printk( "gpib: (debug) request module returned %i\n", retval );
		}
	}
	if(board->interface)
	{
		if(!try_module_get(board->provider_module))
		{
			printk("gpib: try_module_get() failed\n");
			return -ENOSYS;
		}
		board->use_count++;
		priv->got_module = 1;
	}
	return 0;
}


int ibclose(struct inode *inode, struct file *filep)
{
	unsigned int minor = iminor(inode);
	gpib_board_t *board;
	gpib_file_private_t *priv = filep->private_data;

	if(minor >= GPIB_MAX_NUM_BOARDS)
	{
		printk("gpib: invalid minor number of device file\n");
		return -ENODEV;
	}

	GPIB_DPRINTK( "gpib: closing minor %d\n", minor );

	board = &board_array[ minor ];

	if( priv )
	{
		cleanup_open_devices( priv, board );
		if( priv->holding_mutex )
			mutex_unlock( &board->mutex );

		if(priv->got_module && board->use_count)
		{
			module_put(board->provider_module);
			--board->use_count;
		}
		kfree( filep->private_data );
		filep->private_data = NULL;
	}

	return 0;
}



long ibioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	unsigned int minor = iminor(filep->f_dentry->d_inode);
	gpib_board_t *board;
	gpib_file_private_t *file_priv = filep->private_data;
	long retval = -ENOTTY;

	if( minor >= GPIB_MAX_NUM_BOARDS )
	{
		printk("gpib: invalid minor number of device file\n");
		return -ENODEV;
	}
	board = &board_array[ minor ];

	lock_kernel();

	GPIB_DPRINTK( "pid %i, minor %i, ioctl %d, interface=%s, use=%d, onl=%d\n",
		current->pid, minor, cmd & 0xff,
		board->interface ? board->interface->name : "",
		board->use_count,
		board->online );

	switch( cmd )
	{
		case CFCBOARDTYPE:
			retval = board_type_ioctl(file_priv, board, arg);
			goto done;
			break;
		case IBONL:
			retval = online_ioctl( board, arg );
			goto done;
			break;
		default:
			break;
	}
	if( board->interface == NULL )
	{
		printk("gpib: no gpib board configured on /dev/gpib%i\n", minor);
		retval = -ENODEV;
		goto done;
	}
	if(file_priv->got_module == 0)
	{
		if(!try_module_get(board->provider_module))
		{
			printk("gpib: try_module_get() failed\n");
			retval = -ENOSYS;
			goto done;
		}
		file_priv->got_module = 1;
		board->use_count++;
	}
	switch( cmd )
	{
		case CFCBASE:
			retval = iobase_ioctl( board, arg );
			goto done;
			break;
		case CFCIRQ:
			retval = irq_ioctl( board, arg );
			goto done;
			break;
		case CFCDMA:
			retval = dma_ioctl( board, arg );
			goto done;
			break;
		case IBAUTOSPOLL:
			retval = autospoll_ioctl(board, arg);
			goto done;
			break;
		case IBBOARD_INFO:
			retval = board_info_ioctl( board, arg );
			goto done;
			break;
		case IBMUTEX:
			retval = mutex_ioctl( board, file_priv, arg );
			goto done;
			break;
		case IBPAD:
			retval = pad_ioctl( board, file_priv, arg );
			goto done;
			break;
		case IBSAD:
			retval = sad_ioctl( board, file_priv, arg );
			goto done;
			break;
		case IBSELECT_PCI:
			retval = select_pci_ioctl( board, arg );
			goto done;
			break;
		default:
			break;
	}

	if( !board->online )
	{
		printk( "gpib: ioctl %i invalid for offline board\n",
			cmd & 0xff );
		retval = -EINVAL;
		goto done;
	}

	switch( cmd )
	{
		case IBEVENT:
			retval = event_ioctl( board, arg );
			goto done;
			break;
		case IBCLOSEDEV:
			retval = close_dev_ioctl( filep, board, arg );
			goto done;
			break;
		case IBOPENDEV:
			retval = open_dev_ioctl( filep, board, arg );
			goto done;
			break;
		case IBSPOLL_BYTES:
			retval = status_bytes_ioctl( board, arg );
			goto done;
			break;
		case IBWAIT:
			retval = wait_ioctl( file_priv, board, arg );
			goto done;
			break;
		case IBLINES:
			retval = line_status_ioctl( board, arg );
			goto done;
			break;
		case IBLOC:
			board->interface->return_to_local( board );
			retval = 0;
			goto done;
			break;
		default:
			break;
	}

	if( current->pid != board->locking_pid )
	{
		printk( "gpib: need to hold board lock to perform ioctl %i\n",
			cmd & 0xff );
		retval = -EPERM;
		goto done;
	}

	switch( cmd )
	{
		case IB_T1_DELAY:
			retval = t1_delay_ioctl( board, arg );
			goto done;
			break;
		case IBCAC:
			retval = take_control_ioctl( board, arg );
			goto done;
			break;
		case IBCMD:
			retval = command_ioctl( file_priv, board, arg );
			goto done;
			break;
		case IBEOS:
			retval = eos_ioctl( board, arg );
			goto done;
			break;
		case IBGTS:
			retval = ibgts( board );
			goto done;
			break;
		case IBPPC:
			retval = ppc_ioctl( board, arg );
			goto done;
			break;
		case IBQUERY_BOARD_RSV:
			retval = query_board_rsv_ioctl( board, arg );
			goto done;
			break;
		case IBRD:
			retval = read_ioctl( file_priv, board, arg );
			goto done;
			break;
		case IBRPP:
			retval = parallel_poll_ioctl( board, arg );
			goto done;
			break;
		case IBRSC:
			retval = request_system_control_ioctl( board, arg );
			goto done;
			break;
		case IBRSP:
			retval = serial_poll_ioctl( board, arg );
			goto done;
			break;
		case IBRSV:
			retval = request_service_ioctl( board, arg );
			goto done;
			break;
		case IBSIC:
			retval = interface_clear_ioctl( board, arg );
			goto done;
			break;
		case IBSRE:
			retval = remote_enable_ioctl( board, arg );
			goto done;
			break;
		case IBTMO:
			retval = timeout_ioctl( board, arg );
			goto done;
			break;
		case IBWRT:
			retval = write_ioctl( file_priv, board, arg );
			goto done;
			break;
		default:
			retval = -ENOTTY;
			goto done;
			break;
	}

done:
	unlock_kernel();
	return retval;
}

static int board_type_ioctl(gpib_file_private_t *file_priv, gpib_board_t *board, unsigned long arg)
{
	struct list_head *list_ptr;
	board_type_ioctl_t cmd;
	int retval;

	if(!capable(CAP_SYS_ADMIN))
		return -EPERM;
	if(board->online)
	{
		printk("gpib: can't change board type while board is online.\n");
		return -EBUSY;
	}

	retval = copy_from_user(&cmd, (void*)arg, sizeof(board_type_ioctl_t));
	if(retval)
	{
		return retval;
	}

	for(list_ptr = registered_drivers.next; list_ptr != &registered_drivers; list_ptr = list_ptr->next)
	{
		gpib_interface_list_t *entry;

		entry = list_entry(list_ptr, gpib_interface_list_t, list);
		if(strcmp(entry->interface->name, cmd.name) == 0)
		{
			int i;
			int had_module = file_priv->got_module;
			if(board->use_count)
			{
				for(i = 0; i < board->use_count; ++i)
				{
					module_put(board->provider_module);
				}
				board->interface = NULL;
				file_priv->got_module = 0;
			}
			board->interface = entry->interface;
			board->provider_module = entry->module;
			for(i = 0; i < board->use_count; ++i)
			{
				if(!try_module_get(entry->module))
				{
					board->use_count = i;
					return -ENOSYS;
				}
			}
			if(had_module == 0)
			{
				if(!try_module_get(entry->module))
				{
					return -ENOSYS;
				}
				++board->use_count;
			}
			file_priv->got_module = 1;
			return 0;
		}
	}

	return -EINVAL;
}

static int read_ioctl( gpib_file_private_t *file_priv, gpib_board_t *board,
	unsigned long arg)
{
	read_write_ioctl_t read_cmd;
	uint8_t *userbuf;
	unsigned long remain;
	int end_flag = 0;
	int retval;
	ssize_t read_ret = 0;
	gpib_descriptor_t *desc;
	size_t nbytes;

	retval = copy_from_user(&read_cmd, (void*) arg, sizeof(read_cmd));
	if (retval)
		return -EFAULT;

	desc = handle_to_descriptor( file_priv, read_cmd.handle );
	if( desc == NULL ) return -EINVAL;

	BUG_ON(sizeof(userbuf) > sizeof(read_cmd.buffer_ptr));
	userbuf = (uint8_t*)(unsigned long)read_cmd.buffer_ptr;

	/* Check write access to buffer */
	if(!access_ok(VERIFY_WRITE, userbuf, read_cmd.count))
		return -EFAULT;

	desc->io_in_progress = 1;

	/* Read buffer loads till we fill the user supplied buffer */
	remain = read_cmd.count;
	while(remain > 0 && end_flag == 0)
	{
		nbytes = 0;
		read_ret = ibrd(board, board->buffer, (board->buffer_length < remain) ? board->buffer_length :
			remain, &end_flag, &nbytes);
		if(nbytes == 0) break;
		retval = copy_to_user(userbuf, board->buffer, nbytes);
		if(retval)
		{
			retval = -EFAULT;
			break;
		}
		remain -= nbytes;
		userbuf += nbytes;
		if(read_ret < 0) break;
	}
	read_cmd.count -= remain;
	read_cmd.end = end_flag;
	/* suppress errors (for example due to timeout or interruption by device clear)
	if all bytes got sent.  This prevents races that can occur in the various drivers
	if a device receives a device clear immediately after a transfer completes and
	the driver code wasn't careful enough to handle that case.
	*/
	if(remain == 0 || end_flag)
	{
		read_ret = 0;
	}
	if(retval == 0)
		retval = copy_to_user((void*) arg, &read_cmd, sizeof(read_cmd));
	desc->io_in_progress = 0;
	wake_up_interruptible( &board->wait );
	if(retval) return -EFAULT;

	return read_ret;
}

static int command_ioctl( gpib_file_private_t *file_priv,
	gpib_board_t *board, unsigned long arg)
{
	read_write_ioctl_t cmd;
	uint8_t *userbuf;
	unsigned long remain;
	int retval;
	gpib_descriptor_t *desc;

	retval = copy_from_user(&cmd, (void*) arg, sizeof(cmd));
	if( retval )
		return -EFAULT;

	desc = handle_to_descriptor( file_priv, cmd.handle );
	if( desc == NULL ) return -EINVAL;

	userbuf = (uint8_t*)(unsigned long)cmd.buffer_ptr;

	/* Check read access to buffer */
	if(!access_ok(VERIFY_READ, userbuf, cmd.count))
		return -EFAULT;

	/* Write buffer loads till we empty the user supplied buffer */
	remain = cmd.count;

	desc->io_in_progress = 1;
	while( remain > 0 )
	{
		retval = copy_from_user(board->buffer, userbuf, (board->buffer_length < remain) ?
			board->buffer_length : remain );
		if(retval) retval = -EFAULT;
		else
			retval = ibcmd(board, board->buffer, (board->buffer_length < remain) ?
				board->buffer_length : remain );
		if(retval < 0)
		{
			desc->io_in_progress = 0;
			wake_up_interruptible( &board->wait );
			return retval;
			break;
		}
		if( retval == 0 ) break;
		remain -= retval;
		userbuf += retval;
	}

	cmd.count -= remain;

	retval = copy_to_user((void*) arg, &cmd, sizeof(cmd));
	desc->io_in_progress = 0;
	wake_up_interruptible( &board->wait );
	if( retval ) return -EFAULT;

	return 0;
}

static int write_ioctl(gpib_file_private_t *file_priv, gpib_board_t *board,
	unsigned long arg)
{
	read_write_ioctl_t write_cmd;
	uint8_t *userbuf;
	unsigned long remain;
	int retval = 0;
	int fault;
	gpib_descriptor_t *desc;

	fault = copy_from_user(&write_cmd, (void*) arg, sizeof(write_cmd));
	if(fault)
		return -EFAULT;

	desc = handle_to_descriptor( file_priv, write_cmd.handle );
	if( desc == NULL ) return -EINVAL;

	userbuf = (uint8_t*)(unsigned long)write_cmd.buffer_ptr;

	/* Check read access to buffer */
	if(!access_ok(VERIFY_READ, userbuf, write_cmd.count))
		return -EFAULT;

	desc->io_in_progress = 1;

	/* Write buffer loads till we empty the user supplied buffer */
	remain = write_cmd.count;
	while(remain > 0)
	{
		int send_eoi;
		size_t bytes_written = 0;

		send_eoi = remain <= board->buffer_length && write_cmd.end;
		fault = copy_from_user(board->buffer, userbuf, (board->buffer_length < remain) ?
			board->buffer_length : remain );
		if(fault)
		{
			retval = -EFAULT;
			break;
		}
		retval = ibwrt(board, board->buffer, (board->buffer_length < remain) ?
			board->buffer_length : remain, send_eoi, &bytes_written);
		remain -= bytes_written;
		userbuf += bytes_written;
		if(retval < 0)
		{
			break;
		}
	}
	write_cmd.count -= remain;
	/* suppress errors (for example due to timeout or interruption by device clear)
	if all bytes got sent.  This prevents races that can occur in the various drivers
	if a device receives a device clear immediately after a transfer completes and
	the driver code wasn't careful enough to handle that case.
	*/
	if(remain == 0)
	{
		retval = 0;
	}
	fault = copy_to_user((void*) arg, &write_cmd, sizeof(write_cmd));
	desc->io_in_progress = 0;
	wake_up_interruptible( &board->wait );
	if(fault) return -EFAULT;

	return retval;
}

static int status_bytes_ioctl( gpib_board_t *board, unsigned long arg )
{
	gpib_status_queue_t *device;
	spoll_bytes_ioctl_t cmd;
	int retval;

	retval = copy_from_user( &cmd, (void *) arg, sizeof( cmd ) );
	if( retval )
		return -EFAULT;

	device = get_gpib_status_queue( board, cmd.pad, cmd.sad );
	if( device == NULL )
		cmd.num_bytes = 0;
	else
		cmd.num_bytes = num_status_bytes( device );

	retval = copy_to_user( (void *) arg, &cmd, sizeof( cmd ) );
	if( retval )
		return -EFAULT;

	return 0;
}

static int increment_open_device_count( struct list_head *head, unsigned int pad, int sad )
{
	struct list_head *list_ptr;
	gpib_status_queue_t *device;

	/* first see if address has already been opened, then increment
	 * open count */
	for( list_ptr = head->next; list_ptr != head; list_ptr = list_ptr->next )
	{
		device = list_entry( list_ptr, gpib_status_queue_t, list );
		if( gpib_address_equal( device->pad, device->sad, pad, sad ) )
		{
			GPIB_DPRINTK( "incrementing open count for pad %i, sad %i\n",
				device->pad, device->sad );
			device->reference_count++;
			return 0;
		}
	}

	/* otherwise we need to allocate a new gpib_status_queue_t */
	device = kmalloc( sizeof( gpib_status_queue_t ), GFP_ATOMIC );
	if( device == NULL )
		return -ENOMEM;
	init_gpib_status_queue( device );
	device->pad = pad;
	device->sad = sad;
	device->reference_count = 1;

	list_add( &device->list, head );

	GPIB_DPRINTK( "opened pad %i, sad %i\n",
		device->pad, device->sad );

	return 0;
}

static int subtract_open_device_count( struct list_head *head, unsigned int pad, int sad, unsigned int count )
{
	gpib_status_queue_t *device;
	struct list_head *list_ptr;

	for( list_ptr = head->next; list_ptr != head; list_ptr = list_ptr->next )
	{
		device = list_entry( list_ptr, gpib_status_queue_t, list );
		if( gpib_address_equal( device->pad, device->sad, pad, sad ) )
		{
			GPIB_DPRINTK( "decrementing open count for pad %i, sad %i\n",
				device->pad, device->sad );
			if( count > device->reference_count )
			{
				printk( "gpib: bug! in subtract_open_device_count()\n" );
				return -EINVAL;
			}
			device->reference_count -= count;
			if( device->reference_count == 0 )
			{
				GPIB_DPRINTK( "closing pad %i, sad %i\n",
					device->pad, device->sad );
				list_del( list_ptr );
				kfree( device );
			}
			return 0;
		}
	}
	printk( "gpib: bug! tried to close address that was never opened!\n" );
	return -EINVAL;
}

static inline int decrement_open_device_count( struct list_head *head, unsigned int pad, int sad )
{
	return subtract_open_device_count( head, pad, sad, 1 );
}

static int cleanup_open_devices( gpib_file_private_t *file_priv, gpib_board_t *board )
{
	int retval = 0;
	int i;

	for( i = 0; i < GPIB_MAX_NUM_DESCRIPTORS; i++ )
	{
		gpib_descriptor_t *desc;

		desc = file_priv->descriptors[ i ];
		if( desc == NULL ) continue;

		if( desc->is_board == 0 )
		{
			retval = decrement_open_device_count( &board->device_list, desc->pad,
				desc->sad );
			if( retval < 0 ) return retval;
		}
		kfree( desc );
		file_priv->descriptors[ i ] = NULL;
	}

	return 0;
}

static int open_dev_ioctl( struct file *filep, gpib_board_t *board, unsigned long arg )
{
	open_dev_ioctl_t open_dev_cmd;
	int retval;
	gpib_file_private_t *file_priv = filep->private_data;
	int i;

	retval = copy_from_user( &open_dev_cmd, ( void* ) arg, sizeof( open_dev_cmd ) );
	if (retval)
		return -EFAULT;

	if(mutex_lock_interruptible(&file_priv->descriptors_mutex))
	{
		return -ERESTARTSYS;
	}
	for( i = 0; i < GPIB_MAX_NUM_DESCRIPTORS; i++ )
		if( file_priv->descriptors[ i ] == NULL ) break;
	if( i == GPIB_MAX_NUM_DESCRIPTORS )
		return -ERANGE;
	file_priv->descriptors[ i ] = kmalloc( sizeof( gpib_descriptor_t ), GFP_KERNEL );
	if( file_priv->descriptors[ i ] == NULL )
	{
		mutex_unlock(&file_priv->descriptors_mutex);
		return -ENOMEM;
	}
	init_gpib_descriptor( file_priv->descriptors[ i ] );

	file_priv->descriptors[ i ]->pad = open_dev_cmd.pad;
	file_priv->descriptors[ i ]->sad = open_dev_cmd.sad;
	file_priv->descriptors[ i ]->is_board = open_dev_cmd.is_board;
	mutex_unlock(&file_priv->descriptors_mutex);

	retval = increment_open_device_count( &board->device_list, open_dev_cmd.pad, open_dev_cmd.sad );
	if( retval < 0 )
		return retval;

	/* clear stuck srq state, since we may be able to find service request on
	 * the new device */
	board->stuck_srq = 0;

	open_dev_cmd.handle = i;
	retval = copy_to_user( ( void* ) arg, &open_dev_cmd, sizeof( open_dev_cmd ) );
	if (retval)
		return -EFAULT;

	return 0;
}

static int close_dev_ioctl( struct file *filep, gpib_board_t *board, unsigned long arg )
{
	close_dev_ioctl_t cmd;
	gpib_file_private_t *file_priv = filep->private_data;
	int retval;

	retval = copy_from_user( &cmd, ( void* ) arg, sizeof( cmd ) );
	if (retval)
		return -EFAULT;

	if( cmd.handle >= GPIB_MAX_NUM_DESCRIPTORS ) return -EINVAL;
	if( file_priv->descriptors[ cmd.handle ] == NULL ) return -EINVAL;

	retval = decrement_open_device_count( &board->device_list, file_priv->descriptors[ cmd.handle ]->pad,
		file_priv->descriptors[ cmd.handle ]->sad );
	if( retval < 0 ) return retval;

	kfree( file_priv->descriptors[ cmd.handle ] );
	file_priv->descriptors[ cmd.handle ] = NULL;

	return 0;
}

static int serial_poll_ioctl( gpib_board_t *board, unsigned long arg )
{
	serial_poll_ioctl_t serial_cmd;
	int retval;

	retval = copy_from_user( &serial_cmd, ( void* ) arg, sizeof( serial_cmd ) );
	if( retval )
		return -EFAULT;

	retval = get_serial_poll_byte( board, serial_cmd.pad, serial_cmd.sad, board->usec_timeout,
		&serial_cmd.status_byte );
	if( retval < 0 )
		return retval;

	retval = copy_to_user( ( void * ) arg, &serial_cmd, sizeof( serial_cmd ) );
	if( retval )
		return -EFAULT;

	return 0;
}

static int wait_ioctl( gpib_file_private_t *file_priv, gpib_board_t *board,
	unsigned long arg )
{
	wait_ioctl_t wait_cmd;
	int retval;
	gpib_descriptor_t *desc;

	retval = copy_from_user( &wait_cmd, ( void * ) arg, sizeof( wait_cmd ) );
	if( retval )
		return -EFAULT;

	desc = handle_to_descriptor( file_priv, wait_cmd.handle );
	if( desc == NULL ) return -EINVAL;

	retval = ibwait( board, wait_cmd.wait_mask, wait_cmd.clear_mask,
		wait_cmd.set_mask, &wait_cmd.ibsta, wait_cmd.usec_timeout, desc );
	if( retval < 0 ) return retval;

	retval = copy_to_user( ( void * ) arg, &wait_cmd, sizeof( wait_cmd ) );
	if( retval )
		return -EFAULT;

	return 0;
}

static int parallel_poll_ioctl( gpib_board_t *board, unsigned long arg )
{
	uint8_t poll_byte;
	int retval;

	retval = ibrpp( board, &poll_byte );
	if( retval < 0 )
		return retval;

	retval = copy_to_user( ( void * ) arg, &poll_byte, sizeof( poll_byte ) );
	if( retval )
		return -EFAULT;

	return 0;
}

static int online_ioctl( gpib_board_t *board, unsigned long arg )
{
	online_ioctl_t online_cmd;
	gpib_board_config_t config;
	int retval;
	void *init_data = NULL;

	config.init_data = NULL;

	if(!capable(CAP_SYS_ADMIN))
		return -EPERM;

	retval = copy_from_user( &online_cmd, ( void * ) arg, sizeof( online_cmd ) );
	if(retval)
		return -EFAULT;
	if(online_cmd.init_data_length > 0)
	{
		config.init_data = vmalloc(online_cmd.init_data_length);
		if(config.init_data == NULL)
			return -ENOMEM;
		BUG_ON(sizeof(init_data) > sizeof(online_cmd.init_data_ptr));
		init_data = (void*)(unsigned long)(online_cmd.init_data_ptr);
		retval = copy_from_user(config.init_data, init_data, online_cmd.init_data_length);
		if(retval)
		{
			vfree(config.init_data);
			return -EFAULT;
		}
		config.init_data_length = online_cmd.init_data_length;
	}else
	{
		config.init_data = NULL;
		config.init_data_length = 0;
	}
	if(online_cmd.online)
		retval = ibonline(board, config);
	else
		retval = iboffline(board);
	if(config.init_data)
		vfree(config.init_data);
	return retval;
}

static int remote_enable_ioctl( gpib_board_t *board, unsigned long arg )
{
	int enable;
	int retval;

	retval = copy_from_user( &enable, ( void * ) arg, sizeof( enable ) );
	if( retval )
		return -EFAULT;

	return ibsre( board, enable );
}

static int take_control_ioctl( gpib_board_t *board, unsigned long arg )
{
	int synchronous;
	int retval;

	retval = copy_from_user( &synchronous, ( void * ) arg, sizeof( synchronous ) );
	if( retval )
		return -EFAULT;

	return ibcac( board, synchronous );
}

static int line_status_ioctl( gpib_board_t *board, unsigned long arg )
{
	short lines;
	int retval;

	retval = iblines( board, &lines );
	if( retval < 0 )
		return retval;

	retval = copy_to_user( ( void * ) arg, &lines, sizeof( lines ) );
	if( retval )
		return -EFAULT;

	return 0;
}

static int pad_ioctl( gpib_board_t *board, gpib_file_private_t *file_priv,
	unsigned long arg )
{
	pad_ioctl_t cmd;
	int retval;
	gpib_descriptor_t *desc;

	retval = copy_from_user( &cmd, ( void * ) arg, sizeof( cmd ) );
	if( retval )
		return -EFAULT;

	desc = handle_to_descriptor( file_priv, cmd.handle );
	if( desc == NULL )
		return -EINVAL;

	if( desc->is_board )
	{
		retval = ibpad( board, cmd.pad );
		if( retval < 0 ) return retval;
	}else
	{
		retval = decrement_open_device_count( &board->device_list, desc->pad, desc->sad );
		if( retval < 0 )
			return retval;

		desc->pad = cmd.pad;

		retval = increment_open_device_count( &board->device_list, desc->pad, desc->sad );
		if( retval < 0 )
			return retval;
	}

	return 0;
}

static int sad_ioctl( gpib_board_t *board, gpib_file_private_t *file_priv,
	unsigned long arg )
{
	sad_ioctl_t cmd;
	int retval;
	gpib_descriptor_t *desc;

	retval = copy_from_user( &cmd, ( void * ) arg, sizeof( cmd ) );
	if( retval )
		return -EFAULT;

	desc = handle_to_descriptor( file_priv, cmd.handle );
	if( desc == NULL )
		return -EINVAL;

	if( desc->is_board )
	{
		retval = ibsad( board, cmd.sad );
		if( retval < 0 ) return retval;
	}else
	{
		retval = decrement_open_device_count( &board->device_list, desc->pad, desc->sad );
		if( retval < 0 )
			return retval;

		desc->sad = cmd.sad;

		retval = increment_open_device_count( &board->device_list, desc->pad, desc->sad );
		if( retval < 0 )
			return retval;
	}
	return 0;
}

static int eos_ioctl( gpib_board_t *board, unsigned long arg )
{
	eos_ioctl_t eos_cmd;
	int retval;

	retval = copy_from_user( &eos_cmd, ( void * ) arg, sizeof( eos_cmd ) );
	if( retval )
		return -EFAULT;

	return ibeos( board, eos_cmd.eos, eos_cmd.eos_flags );
}

static int request_service_ioctl( gpib_board_t *board, unsigned long arg )
{
	uint8_t status_byte;
	int retval;

	retval = copy_from_user( &status_byte, ( void * ) arg, sizeof( status_byte ) );
	if( retval )
		return -EFAULT;

	return ibrsv( board, status_byte );
}

static int iobase_ioctl( gpib_board_t *board, unsigned long arg )
{
	uint64_t base_addr;
	int retval;

	if(!capable(CAP_SYS_ADMIN))
		return -EPERM;

	retval = copy_from_user( &base_addr, ( void * ) arg, sizeof( base_addr ) );
	if( retval )
		return -EFAULT;

	BUG_ON(sizeof(void*) > sizeof(base_addr));
	board->ibbase = (void*)(unsigned long)(base_addr);

	return 0;
}

static int irq_ioctl( gpib_board_t *board, unsigned long arg )
{
	unsigned int irq;
	int retval;

	if(!capable(CAP_SYS_ADMIN))
		return -EPERM;

	retval = copy_from_user( &irq, ( void * ) arg, sizeof( irq ) );
	if( retval )
		return -EFAULT;

	board->ibirq = irq;

	return 0;
}

static int dma_ioctl( gpib_board_t *board, unsigned long arg )
{
	unsigned int dma_channel;
	int retval;

	if(!capable(CAP_SYS_ADMIN))
		return -EPERM;

	retval = copy_from_user( &dma_channel, ( void * ) arg, sizeof( dma_channel ) );
	if( retval )
		return -EFAULT;

	board->ibdma = dma_channel;

	return 0;
}

static int autospoll_ioctl(gpib_board_t *board, unsigned long arg)
{
	autospoll_ioctl_t enable;
	int retval;

	retval = copy_from_user( &enable, ( void * ) arg, sizeof( enable ) );
	if(retval)
		return -EFAULT;

/*FIXME: should keep track of whether autospolling is on or off
 * by descriptor.  That would also allow automatic decrement
 * of autospollers when descriptors are closed. */
	if(enable)
		board->autospollers++;
	else
	{
		if(board->autospollers <= 0)
		{
			printk("gpib: tried to set number of autospollers negative\n");
			retval = -EINVAL;
		}else
		{
			board->autospollers--;
			retval = 0;
		}
	}
	return retval;
}

static int mutex_ioctl( gpib_board_t *board, gpib_file_private_t *file_priv,
	unsigned long arg )
{
	int retval, lock_mutex;

	retval = copy_from_user( &lock_mutex, ( void * ) arg, sizeof( lock_mutex ) );
	if( retval )
		return -EFAULT;

	if( lock_mutex )
	{
		retval = mutex_lock_interruptible(&board->mutex);
		if(retval)
		{
			printk("gpib: ioctl interrupted while waiting on lock\n");
			return -ERESTARTSYS;
		}
		board->locking_pid = current->pid;
		file_priv->holding_mutex = 1;
		GPIB_DPRINTK("locked board %d mutex\n", board->minor);
	}else
	{
		if( current->pid != board->locking_pid )
		{
			printk( "gpib: bug! pid %i tried to release mutex held by pid %i\n",
				current->pid, board->locking_pid );
			return -EPERM;
		}
		file_priv->holding_mutex = 0;
		board->locking_pid = 0;
		mutex_unlock( &board->mutex );
		GPIB_DPRINTK("unlocked board %i mutex\n", board->minor);
	}


	return 0;
}

static int timeout_ioctl( gpib_board_t *board, unsigned long arg )
{
	unsigned int timeout;
	int retval;

	retval = copy_from_user( &timeout, ( void * ) arg, sizeof( timeout ) );
	if( retval )
		return -EFAULT;

	board->usec_timeout = timeout;
	GPIB_DPRINTK( "timeout set to %i usec\n", timeout );

	return 0;
}

static int ppc_ioctl( gpib_board_t *board, unsigned long arg)
{
	ppoll_config_ioctl_t cmd;
	int retval;

	retval = copy_from_user( &cmd, ( void * ) arg, sizeof( cmd ) );
	if( retval )
		return -EFAULT;

	if( cmd.set_ist )
	{
		board->ist = 1;
		board->interface->parallel_poll_response( board, board->ist );
	}else if( cmd.clear_ist )
	{
		board->ist = 0;
		board->interface->parallel_poll_response( board, board->ist );
	}

	if( cmd.config )
	{
		retval = ibppc( board, cmd.config );
		if( retval < 0 ) return retval;
	}

	return 0;
}

static int query_board_rsv_ioctl( gpib_board_t *board, unsigned long arg )
{
	int status;
	int retval;

	status = board->interface->serial_poll_status( board );

	retval = copy_to_user( ( void * ) arg, &status, sizeof( status ) );
	if( retval )
		return -EFAULT;

	return 0;
}

static int board_info_ioctl( const gpib_board_t *board, unsigned long arg)
{
	board_info_ioctl_t info;
	int retval;

	info.pad = board->pad;
	info.sad = board->sad;
	info.parallel_poll_configuration = board->parallel_poll_configuration;
	info.is_system_controller = board->master;
	if( board->autospollers )
		info.autopolling = 1;
	else
		info.autopolling = 0;
	info.t1_delay = board->t1_nano_sec;
	info.ist = board->ist;
	info.no_7_bit_eos = board->interface->no_7_bit_eos;
	retval = copy_to_user( ( void * ) arg, &info, sizeof( info ) );
	if( retval )
		return -EFAULT;

	return 0;
}

static int interface_clear_ioctl( gpib_board_t *board, unsigned long arg )
{
	unsigned int usec_duration;
	int retval;

	retval = copy_from_user( &usec_duration, ( void * ) arg, sizeof( usec_duration ) );
	if( retval )
		return -EFAULT;

	return ibsic( board, usec_duration );
}

static int select_pci_ioctl( gpib_board_t *board, unsigned long arg )
{
	select_pci_ioctl_t selection;
	int retval;

	if(!capable(CAP_SYS_ADMIN))
		return -EPERM;

	retval = copy_from_user( &selection, ( void * ) arg, sizeof( selection ) );
	if( retval ) return -EFAULT;

	board->pci_bus = selection.pci_bus;
	board->pci_slot = selection.pci_slot;

	return 0;
}

static int event_ioctl( gpib_board_t *board, unsigned long arg )
{
	event_ioctl_t user_event;
	int retval;
	short event;

	retval = pop_gpib_event( &board->event_queue, &event );
	if( retval < 0 ) return retval;

	user_event = event;

	retval = copy_to_user( (void*) arg, &user_event, sizeof( user_event ) );
	if( retval ) return -EFAULT;

	return 0;
}

static int request_system_control_ioctl( gpib_board_t *board, unsigned long arg )
{
	rsc_ioctl_t request_control;
	int retval;

	retval = copy_from_user( &request_control, ( void * ) arg, sizeof( request_control ) );
	if( retval ) return -EFAULT;

	ibrsc( board, request_control );

	return 0;
}

static int t1_delay_ioctl( gpib_board_t *board, unsigned long arg )
{
	t1_delay_ioctl_t cmd;
	unsigned int delay;
	int retval;

	if( board->interface->t1_delay == NULL )
	{
		printk("gpib: t1 delay not implemented in driver!\n" );
		return -EIO;
	}

	retval = copy_from_user( &cmd, ( void * ) arg, sizeof( cmd ) );
	if( retval ) return -EFAULT;

	delay = cmd;

	board->t1_nano_sec = board->interface->t1_delay( board, delay );

	return 0;
}

