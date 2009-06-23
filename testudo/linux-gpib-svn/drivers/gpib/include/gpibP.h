/***************************************************************************
                          gpibP.h
                             -------------------

    copyright            : (C) 2002,2003 by Frank Mori Hess
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

#ifndef _GPIB_P_H
#define _GPIB_P_H

#include <linux/types.h>

#include "gpib/gpib_user.h"
#include "gpib_types.h"
#include "gpib_proto.h"
#include "gpib_ioctl.h"
#include "config.h"

#include <linux/fs.h>
#include <linux/interrupt.h>

void gpib_register_driver(gpib_interface_t *interface, struct module *mod);
void gpib_unregister_driver(gpib_interface_t *interface);
struct pci_dev* gpib_pci_get_device( const gpib_board_t *board, unsigned int vendor_id,
	unsigned int device_id, struct pci_dev *from);
struct pci_dev* gpib_pci_get_subsys( const gpib_board_t *board, unsigned int vendor_id,
	unsigned int device_id, unsigned ss_vendor, unsigned ss_device,
	struct pci_dev *from);
unsigned int num_gpib_events( const gpib_event_queue_t *queue );
int push_gpib_event( gpib_board_t *board, short event_type );
int pop_gpib_event( gpib_event_queue_t *queue, short *event_type );
int gpib_request_pseudo_irq(gpib_board_t *board, irqreturn_t (*handler)(int, void * PT_REGS_ARG));
void gpib_free_pseudo_irq(gpib_board_t *board);

extern gpib_board_t board_array[GPIB_MAX_NUM_BOARDS];

extern struct list_head registered_drivers;

#if defined( GPIB_CONFIG_KERNEL_DEBUG )
#define GPIB_DPRINTK( format, args... ) printk( "gpib debug: " format, ## args )
#else
#define GPIB_DPRINTK( arg... )
#endif

#include <asm/io.h>

void writeb_wrapper(unsigned int value, void *address);
unsigned int readb_wrapper(void *address);
void outb_wrapper(unsigned int value, void *address);
unsigned int inb_wrapper(void *address);
void writew_wrapper(unsigned int value, void *address);
unsigned int readw_wrapper(void *address);
void outw_wrapper(unsigned int value, void *address);
unsigned int inw_wrapper(void *address);

#endif	// _GPIB_P_H

