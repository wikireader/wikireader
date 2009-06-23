/***************************************************************************
                          cec/init.c  -  description
                             -------------------
  Initialization stuff for Capital Equipment Corp. GPIB boards

    copyright            : (C) 2002 by Frank Mori Hess
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

#include "cec.h"

#include <linux/pci.h>
#include <asm/io.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

#define CEC_VENDOR_ID 0x12fc
#define CEC_DEV_ID    0x5cec
#define CEC_SUBID 0x9050

int cec_pci_attach(gpib_board_t *board, gpib_board_config_t config);

void cec_pci_detach(gpib_board_t *board);

// wrappers for interface functions
int cec_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	cec_private_t *priv = board->private_data;
	return nec7210_read(board, &priv->nec7210_priv, buffer, length, end, bytes_read);
}
int cec_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	cec_private_t *priv = board->private_data;
	return nec7210_write(board, &priv->nec7210_priv, buffer, length, send_eoi, bytes_written);
}
ssize_t cec_command(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	cec_private_t *priv = board->private_data;
	return nec7210_command(board, &priv->nec7210_priv, buffer, length);
}
int cec_take_control(gpib_board_t *board, int synchronous)
{
	cec_private_t *priv = board->private_data;
	return nec7210_take_control(board, &priv->nec7210_priv, synchronous);
}
int cec_go_to_standby(gpib_board_t *board)
{
	cec_private_t *priv = board->private_data;
	return nec7210_go_to_standby(board, &priv->nec7210_priv);
}
void cec_request_system_control( gpib_board_t *board, int request_control )
{
	cec_private_t *priv = board->private_data;
	nec7210_request_system_control( board, &priv->nec7210_priv, request_control );
}
void cec_interface_clear(gpib_board_t *board, int assert)
{
	cec_private_t *priv = board->private_data;
	nec7210_interface_clear(board, &priv->nec7210_priv, assert);
}
void cec_remote_enable(gpib_board_t *board, int enable)
{
	cec_private_t *priv = board->private_data;
	nec7210_remote_enable(board, &priv->nec7210_priv, enable);
}
int cec_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits)
{
	cec_private_t *priv = board->private_data;
	return nec7210_enable_eos(board, &priv->nec7210_priv, eos_byte, compare_8_bits);
}
void cec_disable_eos(gpib_board_t *board)
{
	cec_private_t *priv = board->private_data;
	nec7210_disable_eos(board, &priv->nec7210_priv);
}
unsigned int cec_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	cec_private_t *priv = board->private_data;
	return nec7210_update_status( board, &priv->nec7210_priv, clear_mask );
}
void cec_primary_address(gpib_board_t *board, unsigned int address)
{
	cec_private_t *priv = board->private_data;
	nec7210_primary_address(board, &priv->nec7210_priv, address);
}
void cec_secondary_address(gpib_board_t *board, unsigned int address, int enable)
{
	cec_private_t *priv = board->private_data;
	nec7210_secondary_address(board, &priv->nec7210_priv, address, enable);
}
int cec_parallel_poll(gpib_board_t *board, uint8_t *result)
{
	cec_private_t *priv = board->private_data;
	return nec7210_parallel_poll(board, &priv->nec7210_priv, result);
}
void cec_parallel_poll_configure( gpib_board_t *board, uint8_t config )
{
	cec_private_t *priv = board->private_data;
	nec7210_parallel_poll_configure( board, &priv->nec7210_priv, config );
}
void cec_parallel_poll_response( gpib_board_t *board, int ist )
{
	cec_private_t *priv = board->private_data;
	nec7210_parallel_poll_response( board, &priv->nec7210_priv, ist );
}
void cec_serial_poll_response(gpib_board_t *board, uint8_t status )
{
	cec_private_t *priv = board->private_data;
	nec7210_serial_poll_response(board, &priv->nec7210_priv, status);
}
uint8_t cec_serial_poll_status( gpib_board_t *board )
{
	cec_private_t *priv = board->private_data;
	return nec7210_serial_poll_status( board, &priv->nec7210_priv );
}
unsigned int cec_t1_delay( gpib_board_t *board, unsigned int nano_sec )
{
	cec_private_t *priv = board->private_data;
	return nec7210_t1_delay( board, &priv->nec7210_priv, nano_sec );
}
void cec_return_to_local( gpib_board_t *board )
{
	cec_private_t *priv = board->private_data;
	nec7210_return_to_local( board, &priv->nec7210_priv );
}

gpib_interface_t cec_pci_interface =
{
	name: "cec_pci",
	attach: cec_pci_attach,
	detach: cec_pci_detach,
	read: cec_read,
	write: cec_write,
	command: cec_command,
	take_control: cec_take_control,
	go_to_standby: cec_go_to_standby,
	request_system_control: cec_request_system_control,
	interface_clear: cec_interface_clear,
	remote_enable: cec_remote_enable,
	enable_eos: cec_enable_eos,
	disable_eos: cec_disable_eos,
	parallel_poll: cec_parallel_poll,
	parallel_poll_configure: cec_parallel_poll_configure,
	parallel_poll_response: cec_parallel_poll_response,
	line_status: NULL,	//XXX
	update_status: cec_update_status,
	primary_address: cec_primary_address,
	secondary_address: cec_secondary_address,
	serial_poll_response: cec_serial_poll_response,
	serial_poll_status: cec_serial_poll_status,
	t1_delay: cec_t1_delay,
	return_to_local: cec_return_to_local,
};

int cec_allocate_private(gpib_board_t *board)
{
	cec_private_t *priv;

	board->private_data = kmalloc(sizeof(cec_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -1;
	priv = board->private_data;
	memset( priv, 0, sizeof(cec_private_t));
	init_nec7210_private(&priv->nec7210_priv);
	return 0;
}

void cec_free_private(gpib_board_t *board)
{
	if(board->private_data)
	{
		kfree(board->private_data);
		board->private_data = NULL;
	}
}

int cec_generic_attach(gpib_board_t *board)
{
	cec_private_t *cec_priv;
	nec7210_private_t *nec_priv;

	board->status = 0;

	if(cec_allocate_private(board))
		return -ENOMEM;
	cec_priv = board->private_data;
	nec_priv = &cec_priv->nec7210_priv;
	nec_priv->read_byte = nec7210_ioport_read_byte;
	nec_priv->write_byte = nec7210_ioport_write_byte;
	nec_priv->offset = cec_reg_offset;
	nec_priv->type = NEC7210;	// guess
	return 0;
}

void cec_init( cec_private_t *cec_priv, const gpib_board_t *board )
{
	nec7210_private_t *nec_priv = &cec_priv->nec7210_priv;

	nec7210_board_reset( nec_priv, board );

	/* set internal counter register for 8 MHz input clock */
	write_byte( nec_priv, ICR | 8, AUXMR );

	nec7210_board_online( nec_priv, board );
}

int cec_pci_attach(gpib_board_t *board, gpib_board_config_t config)
{
	cec_private_t *cec_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = 0;
	int retval;

	retval = cec_generic_attach(board);
	if(retval) return retval;

	cec_priv = board->private_data;
	nec_priv = &cec_priv->nec7210_priv;

	// find board
	cec_priv->pci_device = NULL;
	while( ( cec_priv->pci_device = gpib_pci_get_device( board, CEC_VENDOR_ID,
		CEC_DEV_ID, cec_priv->pci_device ) ) )
	{
		// check for board with plx9050 controller
		if(cec_priv->pci_device->subsystem_device == CEC_SUBID)
		{
			break;
		}
	}
	if(cec_priv->pci_device == NULL)
	{
		printk("gpib: no cec PCI board found\n");
		return -1;
	}

	if(pci_enable_device(cec_priv->pci_device))
	{
		printk("error enabling pci device\n");
		return -1;
	}

	if(pci_request_regions(cec_priv->pci_device, "cec-gpib"))
		return -1;

	cec_priv->plx_iobase = pci_resource_start(cec_priv->pci_device, 1);
	printk(" plx9050 base address 0x%lx\n", cec_priv->plx_iobase);
	nec_priv->iobase = (void*)(pci_resource_start(cec_priv->pci_device, 3));
	printk(" nec7210 base address 0x%p\n", nec_priv->iobase);

	isr_flags |= IRQF_SHARED;
	if(request_irq(cec_priv->pci_device->irq, cec_interrupt, isr_flags, "pci-gpib", board))
	{
		printk("gpib: can't request IRQ %d\n",cec_priv->pci_device->irq);
		return -1;
	}
	cec_priv->irq = cec_priv->pci_device->irq;
	if(gpib_request_pseudo_irq(board, cec_interrupt))
	{
		printk("cec: failed to allocate pseudo irq\n");
		return -1;
	}
	cec_init( cec_priv, board );

	// enable interrupts on plx chip
	outl(PLX9050_LINTR1_EN_BIT | PLX9050_LINTR1_POLARITY_BIT | PLX9050_PCI_INTR_EN_BIT,
		cec_priv->plx_iobase + PLX9050_INTCSR_REG);

	return 0;
}

void cec_pci_detach(gpib_board_t *board)
{
	cec_private_t *cec_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(cec_priv)
	{
		nec_priv = &cec_priv->nec7210_priv;
		gpib_free_pseudo_irq(board);
		if(cec_priv->irq)
		{
			// disable plx9050 interrupts
			outl(0, cec_priv->plx_iobase + PLX9050_INTCSR_REG );
			free_irq(cec_priv->irq, board);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset( nec_priv, board );
			pci_release_regions(cec_priv->pci_device);
		}
		if(cec_priv->pci_device)
			pci_dev_put(cec_priv->pci_device);
	}
	cec_free_private(board);
}

static const struct pci_device_id cec_pci_table[] =
{
	{CEC_VENDOR_ID, CEC_DEV_ID, PCI_ANY_ID, CEC_SUBID, 0, 0, 0 },
	{0}
};
MODULE_DEVICE_TABLE(pci, cec_pci_table);

int __init cec_init_module(void)
{
	gpib_register_driver(&cec_pci_interface, THIS_MODULE);

	return 0;
}

void cec_exit_module(void)
{
	gpib_unregister_driver(&cec_pci_interface);
}

module_init( cec_init_module );
module_exit( cec_exit_module );






