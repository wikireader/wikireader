/***************************************************************************
                          osinit.c  -  description
                             -------------------

    begin                : Dec 2001
    copyright            : (C) 2001, 2004 by Frank Mori Hess
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
#include <linux/module.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_ALIAS_CHARDEV_MAJOR(IBMAJOR);

struct file_operations ib_fops =
{
	owner: THIS_MODULE,
	llseek: NULL,
	read: NULL,
	write: NULL,
	readdir: NULL,
	poll: NULL,
	unlocked_ioctl: &ibioctl,
	compat_ioctl: &ibioctl,
	mmap: NULL,
	open: &ibopen,
	flush: NULL,
	release: &ibclose,
	fsync: NULL,
	fasync: NULL,
	lock: NULL,
//	readv: NULL,
//	writev: NULL,
	sendpage: NULL,
	get_unmapped_area: NULL,
};

gpib_board_t board_array[GPIB_MAX_NUM_BOARDS];

LIST_HEAD(registered_drivers);

void init_gpib_descriptor( gpib_descriptor_t *desc )
{
	desc->pad = 0;
	desc->sad = -1;
	desc->is_board = 0;
	desc->io_in_progress = 0;
}

void gpib_register_driver(gpib_interface_t *interface, struct module *provider_module)
{
	struct gpib_interface_list_struct *entry;

	entry = kmalloc(sizeof(struct gpib_interface_list_struct), GFP_KERNEL);
	if(entry == NULL)
	{
		printk("gpib: failed register %s interface, out of memory\n", interface->name);
		return;
	}
	entry->interface = interface;
	entry->module = provider_module;
	list_add(&entry->list, &registered_drivers);
	printk("gpib: registered %s interface\n", interface->name);
}

void gpib_unregister_driver(gpib_interface_t *interface)
{
	int i;
	struct list_head *list_ptr;

	for(i = 0; i < GPIB_MAX_NUM_BOARDS; i++)
	{
		gpib_board_t *board = &board_array[i];
		if (board->interface == interface)
		{
			if( board->use_count > 0 )
				printk("gpib: Warning:"
					" deregistered interface %s in use\n",
					interface->name);
			iboffline( board );
			board->interface = NULL;
		}
	}
	for(list_ptr = registered_drivers.next; list_ptr != &registered_drivers;)
	{
		gpib_interface_list_t *entry;

		entry = list_entry(list_ptr, gpib_interface_list_t, list);
		list_ptr = list_ptr->next;
		if(entry->interface == interface)
		{
			list_del(&entry->list);
			kfree(entry);
		}
	}
	printk("gpib: unregistered %s interface\n", interface->name);
}

void init_gpib_board( gpib_board_t *board )
{
	board->interface = NULL;
	board->provider_module = NULL;
	board->buffer = NULL;
	board->buffer_length = 0;
	board->status = 0;
	init_waitqueue_head(&board->wait);
	mutex_init(&board->mutex);
	board->locking_pid = 0;
	spin_lock_init(&board->spinlock);
	init_timer(&board->timer);
	board->ibbase = 0;
	board->ibirq = 0;
	board->ibdma = 0;
	board->pci_bus = -1;
	board->pci_slot = -1;
	board->private_data = NULL;
	board->use_count = 0;
	INIT_LIST_HEAD( &board->device_list );
	board->pad = 0;
	board->sad = -1;
	board->usec_timeout = 3000000;
	board->parallel_poll_configuration = 0;
	board->online = 0;
	board->autospollers = 0;
	board->autospoll_task = NULL;
	init_event_queue(&board->event_queue);
	board->minor = -1;
	init_gpib_pseudo_irq(&board->pseudo_irq);
	board->master = 1;
	board->stuck_srq = 0;
}

int gpib_allocate_board( gpib_board_t *board )
{
	if( board->buffer == NULL )
	{
		board->buffer_length = 0x4000;
		board->buffer = vmalloc( board->buffer_length );
		if(board->buffer == NULL)
		{
			board->buffer_length = 0;
			return -ENOMEM;
		}
	}
	return 0;
}

void gpib_deallocate_board( gpib_board_t *board )
{
	short dummy;

	if( board->buffer )
	{
		vfree( board->buffer );
		board->buffer = NULL;
		board->buffer_length = 0;
	}

	while( num_gpib_events( &board->event_queue ) )
		pop_gpib_event( &board->event_queue, &dummy );

}

void init_board_array( gpib_board_t *board_array, unsigned int length )
{
	int i;
	for( i = 0; i < length; i++)
	{
		init_gpib_board( &board_array[i] );
		board_array[i].minor = i;
	}
}

void init_gpib_status_queue( gpib_status_queue_t *device )
{
	INIT_LIST_HEAD( &device->list );
	INIT_LIST_HEAD( &device->status_bytes );
	device->num_status_bytes = 0;
	device->reference_count = 0;
	device->dropped_byte = 0;
}

static struct class *gpib_class;

static int __init gpib_common_init_module( void )
{
	int i;
	printk("Linux-GPIB %s Driver\n", VERSION);
	init_board_array(board_array, GPIB_MAX_NUM_BOARDS);
	if(register_chrdev(IBMAJOR, "gpib", &ib_fops))
	{
		printk( "gpib: can't get major %d\n", IBMAJOR );
		return -EIO;
	}
	gpib_class = class_create(THIS_MODULE, "gpib_common");
	if(IS_ERR(gpib_class))
	{
		printk("gpib: failed to create gpib class\n");
		unregister_chrdev(IBMAJOR, "gpib");
		return PTR_ERR(gpib_class);
	}
	for(i = 0; i < GPIB_MAX_NUM_BOARDS; ++i)
	{
		CLASS_DEVICE_CREATE(gpib_class, 0, MKDEV(IBMAJOR, i), NULL, "gpib%i", i);
	}
	return 0;
}

static void __exit gpib_common_exit_module( void )
{
	int i;
	for(i = 0; i < GPIB_MAX_NUM_BOARDS; ++i)
	{
		device_destroy(gpib_class, MKDEV(IBMAJOR, i));
	}
	class_destroy(gpib_class);
	unregister_chrdev(IBMAJOR, "gpib");
}

module_init( gpib_common_init_module );
module_exit( gpib_common_exit_module );

struct pci_dev* gpib_pci_get_device( const gpib_board_t *board, unsigned int vendor_id,
	unsigned int device_id, struct pci_dev *from)
{
	struct pci_dev *pci_device = from;

	while( ( pci_device = pci_get_device( vendor_id, device_id, pci_device ) ) )
	{
		if( board->pci_bus >=0 && board->pci_bus != pci_device->bus->number )
			continue;
		if( board->pci_slot >= 0 && board->pci_slot !=
			PCI_SLOT( pci_device->devfn ) )
			continue;
		return pci_device;
	}
	return NULL;
}

struct pci_dev* gpib_pci_get_subsys( const gpib_board_t *board, unsigned int vendor_id,
	unsigned int device_id, unsigned ss_vendor, unsigned ss_device,
	struct pci_dev *from)
{
	struct pci_dev *pci_device = from;

	while((pci_device = pci_get_subsys( vendor_id, device_id, ss_vendor, ss_device, pci_device)))
	{
		if(board->pci_bus >=0 && board->pci_bus != pci_device->bus->number)
			continue;
		if(board->pci_slot >= 0 && board->pci_slot !=
			PCI_SLOT( pci_device->devfn))
			continue;
		return pci_device;
	}
	return NULL;
}

EXPORT_SYMBOL( gpib_register_driver );
EXPORT_SYMBOL( gpib_unregister_driver );
EXPORT_SYMBOL( gpib_pci_get_device );
EXPORT_SYMBOL(gpib_pci_get_subsys);
