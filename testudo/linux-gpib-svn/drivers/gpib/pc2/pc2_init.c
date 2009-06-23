/***************************************************************************
                          nec7210/pc2_init.c  -  description
                             -------------------
 initialization for pc2 and pc2a compatible boards

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

#include "pc2.h"
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/dma.h>
#include <linux/pci.h>
#include <linux/string.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

int pc2_attach(gpib_board_t *board, gpib_board_config_t config);
int pc2a_attach(gpib_board_t *board, gpib_board_config_t config);
int pc2a_cb7210_attach(gpib_board_t *board, gpib_board_config_t config);
int pc2_2a_attach(gpib_board_t *board, gpib_board_config_t config);

void pc2_detach(gpib_board_t *board);
void pc2a_detach(gpib_board_t *board);
void pc2_2a_detach(gpib_board_t *board);

// wrappers for interface functions
int pc2_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	pc2_private_t *priv = board->private_data;
	return nec7210_read(board, &priv->nec7210_priv, buffer, length, end, bytes_read);
}
int pc2_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	pc2_private_t *priv = board->private_data;
	return nec7210_write(board, &priv->nec7210_priv, buffer, length, send_eoi, bytes_written);
}
ssize_t pc2_command(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	pc2_private_t *priv = board->private_data;
	return nec7210_command(board, &priv->nec7210_priv, buffer, length);
}
int pc2_take_control(gpib_board_t *board, int synchronous)
{
	pc2_private_t *priv = board->private_data;
	return nec7210_take_control(board, &priv->nec7210_priv, synchronous);
}
int pc2_go_to_standby(gpib_board_t *board)
{
	pc2_private_t *priv = board->private_data;
	return nec7210_go_to_standby(board, &priv->nec7210_priv);
}
void pc2_request_system_control( gpib_board_t *board, int request_control )
{
	pc2_private_t *priv = board->private_data;
	nec7210_request_system_control( board, &priv->nec7210_priv, request_control );
}
void pc2_interface_clear(gpib_board_t *board, int assert)
{
	pc2_private_t *priv = board->private_data;
	nec7210_interface_clear(board, &priv->nec7210_priv, assert);
}
void pc2_remote_enable(gpib_board_t *board, int enable)
{
	pc2_private_t *priv = board->private_data;
	nec7210_remote_enable(board, &priv->nec7210_priv, enable);
}
int pc2_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits)
{
	pc2_private_t *priv = board->private_data;
	return nec7210_enable_eos(board, &priv->nec7210_priv, eos_byte, compare_8_bits);
}
void pc2_disable_eos(gpib_board_t *board)
{
	pc2_private_t *priv = board->private_data;
	nec7210_disable_eos(board, &priv->nec7210_priv);
}
unsigned int pc2_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	pc2_private_t *priv = board->private_data;
	return nec7210_update_status( board, &priv->nec7210_priv, clear_mask );
}
void pc2_primary_address(gpib_board_t *board, unsigned int address)
{
	pc2_private_t *priv = board->private_data;
	nec7210_primary_address(board, &priv->nec7210_priv, address);
}
void pc2_secondary_address(gpib_board_t *board, unsigned int address, int enable)
{
	pc2_private_t *priv = board->private_data;
	nec7210_secondary_address(board, &priv->nec7210_priv, address, enable);
}
int pc2_parallel_poll(gpib_board_t *board, uint8_t *result)
{
	pc2_private_t *priv = board->private_data;
	return nec7210_parallel_poll(board, &priv->nec7210_priv, result);
}
void pc2_parallel_poll_configure(gpib_board_t *board, uint8_t config )
{
	pc2_private_t *priv = board->private_data;
	nec7210_parallel_poll_configure(board, &priv->nec7210_priv, config );
}
void pc2_parallel_poll_response(gpib_board_t *board, int ist )
{
	pc2_private_t *priv = board->private_data;
	nec7210_parallel_poll_response(board, &priv->nec7210_priv, ist );
}
void pc2_serial_poll_response(gpib_board_t *board, uint8_t status)
{
	pc2_private_t *priv = board->private_data;
	nec7210_serial_poll_response(board, &priv->nec7210_priv, status);
}
uint8_t pc2_serial_poll_status( gpib_board_t *board )
{
	pc2_private_t *priv = board->private_data;
	return nec7210_serial_poll_status( board, &priv->nec7210_priv );
}
unsigned int pc2_t1_delay( gpib_board_t *board, unsigned int nano_sec )
{
	pc2_private_t *priv = board->private_data;
	return nec7210_t1_delay( board, &priv->nec7210_priv, nano_sec );
}
void pc2_return_to_local( gpib_board_t *board )
{
	pc2_private_t *priv = board->private_data;
	nec7210_return_to_local( board, &priv->nec7210_priv );
}

gpib_interface_t pc2_interface =
{
	name:	"pcII",
	attach:	pc2_attach,
	detach:	pc2_detach,
	read:	pc2_read,
	write:	pc2_write,
	command:	pc2_command,
	take_control:	pc2_take_control,
	go_to_standby:	pc2_go_to_standby,
	request_system_control:	pc2_request_system_control,
	interface_clear:	pc2_interface_clear,
	remote_enable:	pc2_remote_enable,
	enable_eos:	pc2_enable_eos,
	disable_eos:	pc2_disable_eos,
	parallel_poll:	pc2_parallel_poll,
	parallel_poll_configure:	pc2_parallel_poll_configure,
	parallel_poll_response:	pc2_parallel_poll_response,
	line_status:	NULL,
	update_status:	pc2_update_status,
	primary_address:	pc2_primary_address,
	secondary_address:	pc2_secondary_address,
	serial_poll_response:	pc2_serial_poll_response,
	serial_poll_status:	pc2_serial_poll_status,
	t1_delay: pc2_t1_delay,
	return_to_local: pc2_return_to_local,
};

gpib_interface_t pc2a_interface =
{
	name:	"pcIIa",
	attach:	pc2a_attach,
	detach:	pc2a_detach,
	read:	pc2_read,
	write:	pc2_write,
	command:	pc2_command,
	take_control:	pc2_take_control,
	go_to_standby:	pc2_go_to_standby,
	request_system_control:	pc2_request_system_control,
	interface_clear:	pc2_interface_clear,
	remote_enable:	pc2_remote_enable,
	enable_eos:	pc2_enable_eos,
	disable_eos:	pc2_disable_eos,
	parallel_poll:	pc2_parallel_poll,
	parallel_poll_configure:	pc2_parallel_poll_configure,
	parallel_poll_response:	pc2_parallel_poll_response,
	line_status:	NULL,
	update_status:	pc2_update_status,
	primary_address:	pc2_primary_address,
	secondary_address:	pc2_secondary_address,
	serial_poll_response:	pc2_serial_poll_response,
	serial_poll_status:	pc2_serial_poll_status,
	t1_delay: pc2_t1_delay,
	return_to_local: pc2_return_to_local,
};

gpib_interface_t pc2a_cb7210_interface =
{
	name:	"pcIIa_cb7210",
	attach:	pc2a_cb7210_attach,
	detach:	pc2a_detach,
	read:	pc2_read,
	write:	pc2_write,
	command:	pc2_command,
	take_control:	pc2_take_control,
	go_to_standby:	pc2_go_to_standby,
	request_system_control:	pc2_request_system_control,
	interface_clear:	pc2_interface_clear,
	remote_enable:	pc2_remote_enable,
	enable_eos:	pc2_enable_eos,
	disable_eos:	pc2_disable_eos,
	parallel_poll:	pc2_parallel_poll,
	parallel_poll_configure:	pc2_parallel_poll_configure,
	parallel_poll_response:	pc2_parallel_poll_response,
	line_status:	NULL, //XXX
	update_status:	pc2_update_status,
	primary_address:	pc2_primary_address,
	secondary_address:	pc2_secondary_address,
	serial_poll_response:	pc2_serial_poll_response,
	serial_poll_status:	pc2_serial_poll_status,
	t1_delay: pc2_t1_delay,
	return_to_local: pc2_return_to_local,
};

gpib_interface_t pc2_2a_interface =
{
	name:	"pcII_IIa",
	attach:	pc2_2a_attach,
	detach:	pc2_2a_detach,
	read:	pc2_read,
	write:	pc2_write,
	command:	pc2_command,
	take_control:	pc2_take_control,
	go_to_standby:	pc2_go_to_standby,
	request_system_control:	pc2_request_system_control,
	interface_clear:	pc2_interface_clear,
	remote_enable:	pc2_remote_enable,
	enable_eos:	pc2_enable_eos,
	disable_eos:	pc2_disable_eos,
	parallel_poll:	pc2_parallel_poll,
	parallel_poll_configure:	pc2_parallel_poll_configure,
	parallel_poll_response:	pc2_parallel_poll_response,
	line_status:	NULL,
	update_status:	pc2_update_status,
	primary_address:	pc2_primary_address,
	secondary_address:	pc2_secondary_address,
	serial_poll_response:	pc2_serial_poll_response,
	serial_poll_status:	pc2_serial_poll_status,
	t1_delay: pc2_t1_delay,
	return_to_local: pc2_return_to_local,
};

static int allocate_private(gpib_board_t *board)
{
	pc2_private_t *priv;

	board->private_data = kmalloc(sizeof(pc2_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -1;
	priv = board->private_data;
	memset( priv, 0, sizeof(pc2_private_t));
	init_nec7210_private( &priv->nec7210_priv );
	return 0;
}

static void free_private(gpib_board_t *board)
{
	if(board->private_data)
	{
		kfree(board->private_data);
		board->private_data = NULL;
	}
}

int pc2_generic_attach(gpib_board_t *board, enum nec7210_chipset chipset)
{
	pc2_private_t *pc2_priv;
	nec7210_private_t *nec_priv;

	board->status = 0;
	if(allocate_private(board))
		return -ENOMEM;
	pc2_priv = board->private_data;
	nec_priv = &pc2_priv->nec7210_priv;
	nec_priv->read_byte = nec7210_ioport_read_byte;
	nec_priv->write_byte = nec7210_ioport_write_byte;
	nec_priv->type = chipset;
	if(board->ibdma)
	{
		nec_priv->dma_buffer_length = 0x1000;
		nec_priv->dma_buffer = pci_alloc_consistent(NULL, nec_priv->dma_buffer_length,
			&nec_priv->dma_buffer_addr);
		if(nec_priv->dma_buffer == NULL)
			return -ENOMEM;

		// request isa dma channel
		if( request_dma( board->ibdma, "pc2" ) )
		{
			printk("gpib: can't request DMA %d\n", board->ibdma);
			return -1;
		}
		nec_priv->dma_channel = board->ibdma;
	}
	return 0;
}

int pc2_attach(gpib_board_t *board, gpib_board_config_t config)
{
	int isr_flags = 0;
	pc2_private_t *pc2_priv;
	nec7210_private_t *nec_priv;
	int retval;

	retval = pc2_generic_attach(board, NEC7210);
	if(retval) return retval;

	pc2_priv = board->private_data;
	nec_priv = &pc2_priv->nec7210_priv;
	nec_priv->offset = pc2_reg_offset;

	if(request_region((unsigned long)(board->ibbase), pc2_iosize, "pc2") == 0)
	{
		printk("gpib: ioports are already in use\n");
		return -1;
	}
	nec_priv->iobase = board->ibbase;

	nec7210_board_reset( nec_priv, board );

	// install interrupt handler
	if(board->ibirq)
	{
		if(request_irq(board->ibirq, pc2_interrupt, isr_flags, "pc2", board))
		{
			printk("gpib: can't request IRQ %d\n", board->ibirq);
			return -1;
		}
	}
	pc2_priv->irq = board->ibirq;
	/* poll so we can detect assertion of ATN */
	if(gpib_request_pseudo_irq(board, pc2_interrupt))
	{
		printk("pc2_gpib: failed to allocate pseudo_irq\n");
		return -1;
	}
	/* set internal counter register for 8 MHz input clock */
	write_byte( nec_priv, ICR | 8, AUXMR );

	nec7210_board_online( nec_priv, board );

	return 0;
}

void pc2_detach(gpib_board_t *board)
{
	pc2_private_t *pc2_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(pc2_priv)
	{
		nec_priv = &pc2_priv->nec7210_priv;
		if(nec_priv->dma_channel)
		{
			free_dma(nec_priv->dma_channel);
		}
		gpib_free_pseudo_irq(board);
		if(pc2_priv->irq)
		{
			free_irq(pc2_priv->irq, board);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset( nec_priv, board );
			release_region((unsigned long)(nec_priv->iobase), pc2_iosize);
		}
		if(nec_priv->dma_buffer)
		{
			pci_free_consistent(NULL, nec_priv->dma_buffer_length, nec_priv->dma_buffer,
				nec_priv->dma_buffer_addr);
			nec_priv->dma_buffer = NULL;
		}
	}
	free_private(board);
}

int pc2a_common_attach(gpib_board_t *board, unsigned int num_registers, enum nec7210_chipset chipset)
{
	unsigned int i, err;
	pc2_private_t *pc2_priv;
	nec7210_private_t *nec_priv;
	int retval;

	retval = pc2_generic_attach(board, chipset);
	if(retval) return retval;

	pc2_priv = board->private_data;
	nec_priv = &pc2_priv->nec7210_priv;
	nec_priv->offset = pc2a_reg_offset;

	switch((unsigned long)(board->ibbase))
	{
		case 0x02e1:
		case 0x22e1:
		case 0x42e1:
		case 0x62e1:
			break;
		default:
			printk("PCIIa base range invalid, must be one of 0x[0246]2e1, but is 0x%p \n", board->ibbase);
			return -1;
			break;
	}

	if(board->ibirq)
	{
		if(board->ibirq < 2 || board->ibirq > 7)
		{
			printk("pc2_gpib: illegal interrupt level %i\n", board->ibirq);
			return -1;
		}
	}else
	{
		printk("pc2_gpib: interrupt disabled, using polling mode (slow)\n");
	}
	err = 0;
	for(i = 0; i < num_registers; i++)
	{
		if(check_region((unsigned long)(board->ibbase) + i * pc2a_reg_offset, 1))
			err++;
	}
	if(board->ibirq && check_region(pc2a_clear_intr_iobase + board->ibirq, 1))
	{
		err++;
	}
	if(err)
	{
		printk("gpib: ioports are already in use");
		return -1;
	}
	for(i = 0; i < num_registers; i++)
	{
		request_region((unsigned long)(board->ibbase) + i * pc2a_reg_offset, 1, "pc2a");
	}
	nec_priv->iobase = board->ibbase;
	if(board->ibirq)
	{
		request_region(pc2a_clear_intr_iobase + board->ibirq, 1, "pc2a");
		pc2_priv->clear_intr_addr = pc2a_clear_intr_iobase + board->ibirq;
		if(request_irq(board->ibirq, pc2a_interrupt, 0, "pc2a", board))
		{
			printk("gpib: can't request IRQ %d\n", board->ibirq);
			return -1;
		}
	}
	pc2_priv->irq = board->ibirq;
	/* poll so we can detect assertion of ATN */
	if(gpib_request_pseudo_irq(board, pc2_interrupt))
	{
		printk("pc2_gpib: failed to allocate pseudo_irq\n");
		return -1;
	}

	// make sure interrupt is clear
	if(pc2_priv->irq)
		outb(0xff , CLEAR_INTR_REG(pc2_priv->irq));

	nec7210_board_reset( nec_priv, board );

	/* set internal counter register for 8 MHz input clock */
	write_byte( nec_priv, ICR | 8, AUXMR );

	nec7210_board_online( nec_priv, board );

	return 0;
}

int pc2a_attach( gpib_board_t *board, gpib_board_config_t config )
{
	return pc2a_common_attach(board, pc2a_iosize, NEC7210);
}

int pc2a_cb7210_attach( gpib_board_t *board, gpib_board_config_t config )
{
	return pc2a_common_attach(board, pc2a_iosize, CB7210);
}

int pc2_2a_attach( gpib_board_t *board, gpib_board_config_t config )
{
	return pc2a_common_attach( board, pc2_2a_iosize, NAT4882);
}

void pc2a_common_detach( gpib_board_t *board, unsigned int num_registers )
{
	int i;
	pc2_private_t *pc2_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(pc2_priv)
	{
		nec_priv = &pc2_priv->nec7210_priv;
		if(nec_priv->dma_channel)
		{
			free_dma(nec_priv->dma_channel);
		}
		gpib_free_pseudo_irq(board);
		if(pc2_priv->irq)
		{
			free_irq(pc2_priv->irq, board);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset( nec_priv, board );
			for(i = 0; i < num_registers; i++)
				release_region((unsigned long)(nec_priv->iobase) + i * pc2a_reg_offset, 1);
		}
		if(pc2_priv->clear_intr_addr)
			release_region(pc2_priv->clear_intr_addr, 1);
		if(nec_priv->dma_buffer)
		{
			pci_free_consistent(NULL, nec_priv->dma_buffer_length, nec_priv->dma_buffer,
				nec_priv->dma_buffer_addr);
			nec_priv->dma_buffer = NULL;
		}
	}
	free_private(board);

}

void pc2a_detach( gpib_board_t *board )
{
	pc2a_common_detach( board, pc2a_iosize );
}

void pc2_2a_detach( gpib_board_t *board )
{
	pc2a_common_detach( board, pc2_2a_iosize );
}

static int __init pc2_init_module( void )
{
	gpib_register_driver(&pc2_interface, THIS_MODULE);
	gpib_register_driver(&pc2a_interface, THIS_MODULE);
	gpib_register_driver(&pc2a_cb7210_interface, THIS_MODULE);
	gpib_register_driver(&pc2_2a_interface, THIS_MODULE);

	return 0;
}

static void __exit pc2_exit_module( void )
{
	gpib_unregister_driver(&pc2_interface);
	gpib_unregister_driver(&pc2a_interface);
	gpib_unregister_driver(&pc2a_cb7210_interface);
	gpib_unregister_driver(&pc2_2a_interface);
}

module_init( pc2_init_module );
module_exit( pc2_exit_module );







