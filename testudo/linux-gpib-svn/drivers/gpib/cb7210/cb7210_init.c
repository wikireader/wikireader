/***************************************************************************
                          nec7210/cb7210_init.c  -  description
                             -------------------
 board specific initialization stuff for Measurement Computing boards
 using cb7210.2 and cbi488.2 chips

    begin                : Dec 2001
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

#include "cb7210.h"
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/dma.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/string.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

int cb_pci_attach(gpib_board_t *board, gpib_board_config_t config);
int cb_isa_attach(gpib_board_t *board, gpib_board_config_t config);

void cb_pci_detach(gpib_board_t *board);
void cb_isa_detach(gpib_board_t *board);

// wrappers for interface functions
int cb7210_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_read(board, &priv->nec7210_priv, buffer, length, end, bytes_read);
}
int cb7210_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_write(board, &priv->nec7210_priv, buffer, length, send_eoi, bytes_written);
}
ssize_t cb7210_command(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_command(board, &priv->nec7210_priv, buffer, length);
}
int cb7210_take_control(gpib_board_t *board, int synchronous)
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_take_control(board, &priv->nec7210_priv, synchronous);
}
int cb7210_go_to_standby(gpib_board_t *board)
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_go_to_standby(board, &priv->nec7210_priv);
}
void cb7210_request_system_control( gpib_board_t *board, int request_control )
{
	cb7210_private_t *priv = board->private_data;
	nec7210_private_t *nec_priv = &priv->nec7210_priv;

	if( request_control )
	{
		priv->hs_mode_bits |= HS_SYS_CONTROL;
	}else
		priv->hs_mode_bits &= ~HS_SYS_CONTROL;

	cb7210_write_byte(priv, priv->hs_mode_bits, HS_MODE);
	nec7210_request_system_control( board, nec_priv, request_control );
}
void cb7210_interface_clear(gpib_board_t *board, int assert)
{
	cb7210_private_t *priv = board->private_data;
	nec7210_interface_clear(board, &priv->nec7210_priv, assert);
}
void cb7210_remote_enable(gpib_board_t *board, int enable)
{
	cb7210_private_t *priv = board->private_data;
	nec7210_remote_enable(board, &priv->nec7210_priv, enable);
}
int cb7210_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits)
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_enable_eos(board, &priv->nec7210_priv, eos_byte, compare_8_bits);
}
void cb7210_disable_eos(gpib_board_t *board)
{
	cb7210_private_t *priv = board->private_data;
	nec7210_disable_eos(board, &priv->nec7210_priv);
}
unsigned int cb7210_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_update_status( board, &priv->nec7210_priv, clear_mask );
}
void cb7210_primary_address(gpib_board_t *board, unsigned int address)
{
	cb7210_private_t *priv = board->private_data;
	nec7210_primary_address(board, &priv->nec7210_priv, address);
}
void cb7210_secondary_address(gpib_board_t *board, unsigned int address, int enable)
{
	cb7210_private_t *priv = board->private_data;
	nec7210_secondary_address(board, &priv->nec7210_priv, address, enable);
}
int cb7210_parallel_poll(gpib_board_t *board, uint8_t *result)
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_parallel_poll(board, &priv->nec7210_priv, result);
}
void cb7210_parallel_poll_configure( gpib_board_t *board, uint8_t configuration )
{
	cb7210_private_t *priv = board->private_data;
	nec7210_parallel_poll_configure(board, &priv->nec7210_priv, configuration );
}
void cb7210_parallel_poll_response( gpib_board_t *board, int ist )
{
	cb7210_private_t *priv = board->private_data;
	nec7210_parallel_poll_response(board, &priv->nec7210_priv, ist );
}
void cb7210_serial_poll_response(gpib_board_t *board, uint8_t status)
{
	cb7210_private_t *priv = board->private_data;
	nec7210_serial_poll_response(board, &priv->nec7210_priv, status);
}
uint8_t cb7210_serial_poll_status( gpib_board_t *board )
{
	cb7210_private_t *priv = board->private_data;
	return nec7210_serial_poll_status( board, &priv->nec7210_priv );
}
void cb7210_return_to_local( gpib_board_t *board )
{
	cb7210_private_t *priv = board->private_data;
	nec7210_private_t *nec_priv = &priv->nec7210_priv;
	write_byte(nec_priv, AUX_RTL2, AUXMR);
	udelay(1);
	write_byte(nec_priv, AUX_RTL, AUXMR);
}

gpib_interface_t cb_pci_unaccel_interface =
{
	name: "cbi_pci_unaccel",
	attach: cb_pci_attach,
	detach: cb_pci_detach,
	read: cb7210_read,
	write: cb7210_write,
	command: cb7210_command,
	take_control: cb7210_take_control,
	go_to_standby: cb7210_go_to_standby,
	request_system_control: cb7210_request_system_control,
	interface_clear: cb7210_interface_clear,
	remote_enable: cb7210_remote_enable,
	enable_eos: cb7210_enable_eos,
	disable_eos: cb7210_disable_eos,
	parallel_poll: cb7210_parallel_poll,
	parallel_poll_configure: cb7210_parallel_poll_configure,
	parallel_poll_response: cb7210_parallel_poll_response,
	line_status: cb7210_line_status,
	update_status: cb7210_update_status,
	primary_address: cb7210_primary_address,
	secondary_address: cb7210_secondary_address,
	serial_poll_response: cb7210_serial_poll_response,
	serial_poll_status: cb7210_serial_poll_status,
	t1_delay: cb7210_t1_delay,
	return_to_local: cb7210_return_to_local,
};

gpib_interface_t cb_pci_accel_interface =
{
	name: "cbi_pci_accel",
	attach: cb_pci_attach,
	detach: cb_pci_detach,
	read: cb7210_accel_read,
	write: cb7210_accel_write,
	command: cb7210_command,
	take_control: cb7210_take_control,
	go_to_standby: cb7210_go_to_standby,
	request_system_control: cb7210_request_system_control,
	interface_clear: cb7210_interface_clear,
	remote_enable: cb7210_remote_enable,
	enable_eos: cb7210_enable_eos,
	disable_eos: cb7210_disable_eos,
	parallel_poll: cb7210_parallel_poll,
	parallel_poll_configure: cb7210_parallel_poll_configure,
	parallel_poll_response: cb7210_parallel_poll_response,
	line_status: cb7210_line_status,
	update_status: cb7210_update_status,
	primary_address: cb7210_primary_address,
	secondary_address: cb7210_secondary_address,
	serial_poll_response: cb7210_serial_poll_response,
	serial_poll_status: cb7210_serial_poll_status,
	t1_delay: cb7210_t1_delay,
	return_to_local: cb7210_return_to_local,
};

gpib_interface_t cb_pci_interface =
{
	name: "cbi_pci",
	attach: cb_pci_attach,
	detach: cb_pci_detach,
	read: cb7210_accel_read,
	write: cb7210_accel_write,
	command: cb7210_command,
	take_control: cb7210_take_control,
	go_to_standby: cb7210_go_to_standby,
	request_system_control: cb7210_request_system_control,
	interface_clear: cb7210_interface_clear,
	remote_enable: cb7210_remote_enable,
	enable_eos: cb7210_enable_eos,
	disable_eos: cb7210_disable_eos,
	parallel_poll: cb7210_parallel_poll,
	parallel_poll_configure: cb7210_parallel_poll_configure,
	parallel_poll_response: cb7210_parallel_poll_response,
	line_status: cb7210_line_status,
	update_status: cb7210_update_status,
	primary_address: cb7210_primary_address,
	secondary_address: cb7210_secondary_address,
	serial_poll_response: cb7210_serial_poll_response,
	serial_poll_status: cb7210_serial_poll_status,
	t1_delay: cb7210_t1_delay,
	return_to_local: cb7210_return_to_local,
};

gpib_interface_t cb_isa_unaccel_interface =
{
	name: "cbi_isa_unaccel",
	attach: cb_isa_attach,
	detach: cb_isa_detach,
	read: cb7210_read,
	write: cb7210_write,
	command: cb7210_command,
	take_control: cb7210_take_control,
	go_to_standby: cb7210_go_to_standby,
	request_system_control: cb7210_request_system_control,
	interface_clear: cb7210_interface_clear,
	remote_enable: cb7210_remote_enable,
	enable_eos: cb7210_enable_eos,
	disable_eos: cb7210_disable_eos,
	parallel_poll: cb7210_parallel_poll,
	parallel_poll_configure: cb7210_parallel_poll_configure,
	parallel_poll_response: cb7210_parallel_poll_response,
	line_status: cb7210_line_status,
	update_status: cb7210_update_status,
	primary_address: cb7210_primary_address,
	secondary_address: cb7210_secondary_address,
	serial_poll_response: cb7210_serial_poll_response,
	serial_poll_status: cb7210_serial_poll_status,
	t1_delay: cb7210_t1_delay,
	return_to_local: cb7210_return_to_local,
};

gpib_interface_t cb_isa_interface =
{
	name: "cbi_isa",
	attach: cb_isa_attach,
	detach: cb_isa_detach,
	read: cb7210_accel_read,
	write: cb7210_accel_write,
	command: cb7210_command,
	take_control: cb7210_take_control,
	go_to_standby: cb7210_go_to_standby,
	request_system_control: cb7210_request_system_control,
	interface_clear: cb7210_interface_clear,
	remote_enable: cb7210_remote_enable,
	enable_eos: cb7210_enable_eos,
	disable_eos: cb7210_disable_eos,
	parallel_poll: cb7210_parallel_poll,
	parallel_poll_configure: cb7210_parallel_poll_configure,
	parallel_poll_response: cb7210_parallel_poll_response,
	line_status: cb7210_line_status,
	update_status: cb7210_update_status,
	primary_address: cb7210_primary_address,
	secondary_address: cb7210_secondary_address,
	serial_poll_response: cb7210_serial_poll_response,
	serial_poll_status: cb7210_serial_poll_status,
	t1_delay: cb7210_t1_delay,
	return_to_local: cb7210_return_to_local,
};

gpib_interface_t cb_isa_accel_interface =
{
	name: "cbi_isa_accel",
	attach: cb_isa_attach,
	detach: cb_isa_detach,
	read: cb7210_accel_read,
	write: cb7210_accel_write,
	command: cb7210_command,
	take_control: cb7210_take_control,
	go_to_standby: cb7210_go_to_standby,
	request_system_control: cb7210_request_system_control,
	interface_clear: cb7210_interface_clear,
	remote_enable: cb7210_remote_enable,
	enable_eos: cb7210_enable_eos,
	disable_eos: cb7210_disable_eos,
	parallel_poll: cb7210_parallel_poll,
	parallel_poll_configure: cb7210_parallel_poll_configure,
	parallel_poll_response: cb7210_parallel_poll_response,
	line_status: cb7210_line_status,
	update_status: cb7210_update_status,
	primary_address: cb7210_primary_address,
	secondary_address: cb7210_secondary_address,
	serial_poll_response: cb7210_serial_poll_response,
	serial_poll_status: cb7210_serial_poll_status,
	t1_delay: cb7210_t1_delay,
	return_to_local: cb7210_return_to_local,
};

int cb7210_allocate_private(gpib_board_t *board)
{
	cb7210_private_t *priv;

	board->private_data = kmalloc(sizeof(cb7210_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -1;
	priv = board->private_data;
	memset( priv, 0, sizeof(cb7210_private_t));
	init_nec7210_private(&priv->nec7210_priv);
	return 0;
}

void cb7210_generic_detach(gpib_board_t *board)
{
	if(board->private_data)
	{
		kfree(board->private_data);
		board->private_data = NULL;
	}
}

// generic part of attach functions shared by all cb7210 boards
int cb7210_generic_attach(gpib_board_t *board)
{
	cb7210_private_t *cb_priv;
	nec7210_private_t *nec_priv;

	board->status = 0;

	if(cb7210_allocate_private(board))
		return -ENOMEM;
	cb_priv = board->private_data;
	nec_priv = &cb_priv->nec7210_priv;
	nec_priv->read_byte = nec7210_locking_ioport_read_byte;
	nec_priv->write_byte = nec7210_locking_ioport_write_byte;
	nec_priv->offset = cb7210_reg_offset;
	nec_priv->type = CB7210;
	return 0;
}

int cb7210_init( cb7210_private_t *cb_priv, gpib_board_t *board )
{
	nec7210_private_t *nec_priv = &cb_priv->nec7210_priv;

	cb7210_write_byte(cb_priv, HS_RESET7210, HS_INT_LEVEL);
	cb7210_write_byte(cb_priv, irq_bits(cb_priv->irq), HS_INT_LEVEL);

	nec7210_board_reset( nec_priv, board );
	cb7210_write_byte(cb_priv, HS_TX_ENABLE | HS_RX_ENABLE | HS_CLR_SRQ_INT |
		HS_CLR_EOI_EMPTY_INT | HS_CLR_HF_INT, HS_MODE);

	cb_priv->hs_mode_bits = HS_HF_INT_EN;
	cb7210_write_byte(cb_priv, cb_priv->hs_mode_bits, HS_MODE);

	write_byte( nec_priv, AUX_LO_SPEED, AUXMR );
	/* set clock register for maximum (20 MHz) driving frequency
	 * ICR should be set to clock in megahertz (1-15) and to zero
	 * for clocks faster than 15 MHz (max 20MHz) */
	write_byte(nec_priv, ICR | 0, AUXMR);

	if(cb_priv->pci_chip == PCI_CHIP_QUANCOM)
	{
		/* change interrupt polarity */
		nec_priv->auxb_bits |= HR_INV;
		write_byte(nec_priv, nec_priv->auxb_bits, AUXMR);
	}
	nec7210_board_online( nec_priv, board );

	/* poll so we can detect assertion of ATN */
	if(gpib_request_pseudo_irq(board, cb_pci_interrupt))
	{
		printk("pc2_gpib: failed to allocate pseudo_irq\n");
		return -1;
	}
	return 0;
}

int cb_pci_attach(gpib_board_t *board, gpib_board_config_t config)
{
	cb7210_private_t *cb_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = 0;
	int bits;
	int retval;

	retval = cb7210_generic_attach(board);
	if(retval) return retval;

	cb_priv = board->private_data;
	nec_priv = &cb_priv->nec7210_priv;

	cb_priv->pci_device = gpib_pci_get_device(board, PCI_VENDOR_ID_CBOARDS,
		PCI_DEVICE_ID_CBOARDS_PCI_GPIB, NULL);
	if(cb_priv->pci_device != NULL)
		cb_priv->pci_chip = PCI_CHIP_AMCC_S5933;
	if(cb_priv->pci_device == NULL)
	{
		cb_priv->pci_device = gpib_pci_get_device(board, PCI_VENDOR_ID_CBOARDS,
			PCI_DEVICE_ID_CBOARDS_CPCI_GPIB, NULL);
		if(cb_priv->pci_device != NULL)
			cb_priv->pci_chip = PCI_CHIP_AMCC_S5933;
	}
	if(cb_priv->pci_device == NULL)
	{
		cb_priv->pci_device = gpib_pci_get_device(board, PCI_VENDOR_ID_QUANCOM,
			PCI_DEVICE_ID_QUANCOM_GPIB, NULL);
		if(cb_priv->pci_device != NULL)
		{
			cb_priv->pci_chip = PCI_CHIP_QUANCOM;
			nec_priv->offset = 4;
		}
	}
	if(cb_priv->pci_device == NULL)
	{
		printk( "cb7210: no supported boards found.\n" );
		return -1;
	}

	if(pci_enable_device(cb_priv->pci_device))
	{
		printk( "cb7210: error enabling pci device\n" );
		return -1;
	}

	if(pci_request_regions(cb_priv->pci_device, "cb7210"))
		return -1;
	switch(cb_priv->pci_chip)
	{
	case PCI_CHIP_AMCC_S5933:
		cb_priv->amcc_iobase = pci_resource_start(cb_priv->pci_device, 0);
		nec_priv->iobase = (void*)(pci_resource_start(cb_priv->pci_device, 1));
		cb_priv->fifo_iobase = pci_resource_start(cb_priv->pci_device, 2);
		break;
	case PCI_CHIP_QUANCOM:
		nec_priv->iobase = (void*)(pci_resource_start(cb_priv->pci_device, 0));
		cb_priv->fifo_iobase = (unsigned long)nec_priv->iobase;
		break;
	default:
		printk("cb7210: bug! unhandled pci_chip=%i\n", cb_priv->pci_chip);
		return -EIO;
		break;
	}
	isr_flags |= IRQF_SHARED;
	if(request_irq(cb_priv->pci_device->irq, cb_pci_interrupt, isr_flags, "cb7210", board))
	{
		printk( "cb7210: can't request IRQ %d\n",cb_priv->pci_device->irq );
		return -1;
	}
	cb_priv->irq = cb_priv->pci_device->irq;

	switch(cb_priv->pci_chip)
	{
	case PCI_CHIP_AMCC_S5933:
		// make sure mailbox flags are clear
		inl(cb_priv->amcc_iobase + INCOMING_MAILBOX_REG(3));
		// enable interrupts on amccs5933 chip
		bits = INBOX_FULL_INTR_BIT | INBOX_BYTE_BITS(3) | INBOX_SELECT_BITS(3) |
			INBOX_INTR_CS_BIT;
		outl(bits, cb_priv->amcc_iobase + INTCSR_REG );
		break;
	default:
		break;
	}
	return cb7210_init( cb_priv, board );
}

void cb_pci_detach(gpib_board_t *board)
{
	cb7210_private_t *cb_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(cb_priv)
	{
		gpib_free_pseudo_irq(board);
		nec_priv = &cb_priv->nec7210_priv;
		if(cb_priv->irq)
		{
			// disable amcc interrupts
			outl(0, cb_priv->amcc_iobase + INTCSR_REG );
			free_irq(cb_priv->irq, board);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset( nec_priv, board );
			pci_release_regions(cb_priv->pci_device);
		}
		if(cb_priv->pci_device)
			pci_dev_put(cb_priv->pci_device);
	}
	cb7210_generic_detach(board);
}

int cb_isa_attach(gpib_board_t *board, gpib_board_config_t config)
{
	int isr_flags = 0;
	cb7210_private_t *cb_priv;
	nec7210_private_t *nec_priv;
	unsigned int bits;
	int retval;

	retval = cb7210_generic_attach(board);
	if(retval) return retval;
	cb_priv = board->private_data;
	nec_priv = &cb_priv->nec7210_priv;
	if(request_region((unsigned long)(board->ibbase), cb7210_iosize, "cb7210") == 0)
	{
		printk("gpib: ioports starting at 0x%p are already in use\n", board->ibbase);
		return -EIO;
	}
	nec_priv->iobase = board->ibbase;
	cb_priv->fifo_iobase = nec7210_iobase(cb_priv);
	
	bits = irq_bits( board->ibirq );
	if( bits == 0 )
	{
		printk("board incapable of using irq %i, try 2-5, 7, 10, or 11\n", board->ibirq);
	}

	// install interrupt handler
	if(request_irq(board->ibirq, cb7210_interrupt, isr_flags, "cb7210", board))
	{
		printk("gpib: can't request IRQ %d\n", board->ibirq);
		return -EBUSY;
	}
	cb_priv->irq = board->ibirq;

	return cb7210_init( cb_priv, board );
}

void cb_isa_detach(gpib_board_t *board)
{
	cb7210_private_t *cb_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(cb_priv)
	{
		gpib_free_pseudo_irq(board);
		nec_priv = &cb_priv->nec7210_priv;
		if(cb_priv->irq)
		{
			free_irq(cb_priv->irq, board);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset( nec_priv, board );
			release_region(nec7210_iobase(cb_priv), cb7210_iosize);
		}
	}
	cb7210_generic_detach(board);
}

static const struct pci_device_id cb7210_pci_table[] = 
{
	{PCI_VENDOR_ID_CBOARDS, PCI_DEVICE_ID_CBOARDS_PCI_GPIB, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{PCI_VENDOR_ID_CBOARDS, PCI_DEVICE_ID_CBOARDS_CPCI_GPIB, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{PCI_VENDOR_ID_QUANCOM, PCI_DEVICE_ID_QUANCOM_GPIB, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, cb7210_pci_table);

static int __init cb7210_init_module( void )
{
	int err = 0;

	gpib_register_driver(&cb_pci_interface, THIS_MODULE);
	gpib_register_driver(&cb_isa_interface, THIS_MODULE);
	gpib_register_driver(&cb_pci_accel_interface, THIS_MODULE);
	gpib_register_driver(&cb_pci_unaccel_interface, THIS_MODULE);
	gpib_register_driver(&cb_isa_accel_interface, THIS_MODULE);
	gpib_register_driver(&cb_isa_unaccel_interface, THIS_MODULE);

#if defined(GPIB_CONFIG_PCMCIA)
	gpib_register_driver(&cb_pcmcia_interface, THIS_MODULE);
	gpib_register_driver(&cb_pcmcia_accel_interface, THIS_MODULE);
	gpib_register_driver(&cb_pcmcia_unaccel_interface, THIS_MODULE);
	err += cb_pcmcia_init_module();
#endif
	if(err)
		return -1;

	return 0;
}

static void __exit cb7210_exit_module( void )
{
	gpib_unregister_driver(&cb_pci_interface);
	gpib_unregister_driver(&cb_isa_interface);
	gpib_unregister_driver(&cb_pci_accel_interface);
	gpib_unregister_driver(&cb_pci_unaccel_interface);
	gpib_unregister_driver(&cb_isa_accel_interface);
	gpib_unregister_driver(&cb_isa_unaccel_interface);
#if defined(GPIB_CONFIG_PCMCIA)
	gpib_unregister_driver(&cb_pcmcia_interface);
	gpib_unregister_driver(&cb_pcmcia_accel_interface);
	gpib_unregister_driver(&cb_pcmcia_unaccel_interface);
	cb_pcmcia_cleanup_module();
#endif
}

module_init( cb7210_init_module );
module_exit( cb7210_exit_module );







