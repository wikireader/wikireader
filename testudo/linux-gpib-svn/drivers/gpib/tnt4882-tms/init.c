/***************************************************************************
                          tnt4882-tms/init.c  -  description
                             -------------------
 board specific initialization stuff for National Instruments boards
 using tnt4882, this driver puts the board into tms9914 compatibility mode
 so I can test support for tms9914 compatible chips.


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

#include "tnt4882.h"
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

int ni_pci_attach(gpib_board_t *board);

void ni_pci_detach(gpib_board_t *board);

// wrappers for interface functions
ssize_t tnt4882_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end)
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_read(board, &priv->tms9914_priv, buffer, length, end);
}
ssize_t tnt4882_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi)
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_write(board, &priv->tms9914_priv, buffer, length, send_eoi);
}
ssize_t tnt4882_command(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_command(board, &priv->tms9914_priv, buffer, length);
}
int tnt4882_take_control(gpib_board_t *board, int synchronous)
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_take_control(board, &priv->tms9914_priv, synchronous);
}
int tnt4882_go_to_standby(gpib_board_t *board)
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_go_to_standby(board, &priv->tms9914_priv);
}
void tnt4882_interface_clear(gpib_board_t *board, int assert)
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_interface_clear(board, &priv->tms9914_priv, assert);
}
void tnt4882_remote_enable(gpib_board_t *board, int enable)
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_remote_enable(board, &priv->tms9914_priv, enable);
}
void tnt4882_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits)
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_enable_eos(board, &priv->tms9914_priv, eos_byte, compare_8_bits);
}
void tnt4882_disable_eos(gpib_board_t *board)
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_disable_eos(board, &priv->tms9914_priv);
}
unsigned int tnt4882_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_update_status( board, &priv->tms9914_priv, clear_mask );
}
void tnt4882_primary_address(gpib_board_t *board, unsigned int address)
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_primary_address(board, &priv->tms9914_priv, address);
}
void tnt4882_secondary_address(gpib_board_t *board, unsigned int address, int enable)
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_secondary_address(board, &priv->tms9914_priv, address, enable);
}
int tnt4882_parallel_poll(gpib_board_t *board, uint8_t *result)
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_parallel_poll(board, &priv->tms9914_priv, result);
}
void tnt4882_parallel_poll_configure(gpib_board_t *board, uint8_t config )
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_parallel_poll_configure(board, &priv->tms9914_priv, config );
}
void tnt4882_serial_poll_response(gpib_board_t *board, uint8_t status)
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_serial_poll_response(board, &priv->tms9914_priv, status);
}
uint8_t tnt4882_serial_poll_status( gpib_board_t *board )
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_serial_poll_status( board, &priv->tms9914_priv );
}
unsigned int tnt4882_t1_delay( gpib_board_t *board, unsigned int nano_sec )
{
	tnt4882_private_t *priv = board->private_data;
	return tms9914_t1_delay( board, &priv->tms9914_priv, nano_sec );
}
void tnt4882_return_to_local( gpib_board_t *board )
{
	tnt4882_private_t *priv = board->private_data;
	tms9914_return_to_local( board, &priv->tms9914_priv );
}

gpib_interface_t ni_pci_interface =
{
	name: "ni_pci_tms",
	attach: ni_pci_attach,
	detach: ni_pci_detach,
	read: tnt4882_read,
	write: tnt4882_write,
	command: tnt4882_command,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	line_status: NULL,	//XXX
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
	provider_module: &__this_module,
};

int tnt4882_allocate_private(gpib_board_t *board)
{
	board->private_data = kmalloc(sizeof(tnt4882_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -1;
	memset(board->private_data, 0, sizeof(tnt4882_private_t));
	return 0;
}

void tnt4882_free_private(gpib_board_t *board)
{
	if(board->private_data)
	{
		kfree(board->private_data);
		board->private_data = NULL;
	}
}

int ni_pci_attach(gpib_board_t *board)
{
	tnt4882_private_t *tnt_priv;
	tms9914_private_t *tms_priv;

	board->status = 0;

	if(tnt4882_allocate_private(board))
		return -ENOMEM;
	tnt_priv = board->private_data;
	tms_priv = &tnt_priv->tms9914_priv;
	tms_priv->read_byte = tms9914_iomem_read_byte;
	tms_priv->write_byte = tms9914_iomem_write_byte;
	tms_priv->offset = atgpib_reg_offset;

	if(mite_devices == NULL)
	{
		printk("no National Instruments PCI boards found\n");
		return -1;
	}

	for(tnt_priv->mite = mite_devices; tnt_priv->mite; tnt_priv->mite = tnt_priv->mite->next)
	{
		if(mite_device_id(tnt_priv->mite) == PCI_DEVICE_ID_NI_GPIB) break;
	}
	if(tnt_priv->mite == NULL)
	{
		printk("no NI PCI-GPIB boards found\n");
		return -1;
	}

	if(mite_setup(tnt_priv->mite) < 0)
	{
		printk("error setting up mite");
		return -1;
	}

	tms_priv->iobase = mite_iobase(tnt_priv->mite);

	// get irq
	if(request_irq(mite_irq(tnt_priv->mite), tnt4882_interrupt, SA_SHIRQ, "ni-pci-gpib", board))
	{
		printk("gpib: can't request IRQ %d\n", mite_irq( tnt_priv->mite ) );
		return -1;
	}
	tnt_priv->irq = mite_irq(tnt_priv->mite);

	/* NAT 4882 reset */
	writeb(SFTRST, tms_priv->iobase + CMDR);	/* Turbo488 software reset */
	udelay(1);
	// enable system control
	writeb(SETSC, tms_priv->iobase + CMDR);
	udelay(1);

	// turn off one-chip mode
	writeb(NODMA, tms_priv->iobase + HSSEL);

	// make sure we are in 7210 mode
	write_byte(tms_priv, AUX_7210, AUXCR);
	// registers might be swapped, so write it to the swapped address too
	writeb(AUX_7210, tms_priv->iobase +  SWAPPED_AUXCR);
	udelay(1);

	// clear SWAP bit
	writeb(0x0, tms_priv->iobase + KEYREG);

	// put it in 9914 mode
	writeb(AUX_9914, tms_priv->iobase + AUXMR);
	udelay(1);
	
	// chip reset command
	write_byte(tms_priv, 0x1c, AUXCR);

	tms9914_board_reset(tms_priv);

	// enable passing of tms9914 interrupts
	writeb(0x2, tms_priv->iobase + IMR3);

	// enable interrupt
	writeb(0x1, tms_priv->iobase + INTRT);

	tms9914_online( board, tms_priv );

	return 0;
}

void ni_pci_detach(gpib_board_t *board)
{
	tnt4882_private_t *tnt_priv = board->private_data;
	tms9914_private_t *tms_priv;

	if(tnt_priv)
	{
		tms_priv = &tnt_priv->tms9914_priv;
		if(tnt_priv->irq)
		{
			free_irq(tnt_priv->irq, board);
		}
		if(tms_priv->iobase)
		{
			tms9914_board_reset(tms_priv);
		}
		if(tnt_priv->mite)
			mite_unsetup(tnt_priv->mite);
	}
	tnt4882_free_private(board);
}


static int __init tnt4882_tms_init_module(void)
{
	EXPORT_NO_SYMBOLS;

	gpib_register_driver(&ni_pci_interface);

	mite_init();
	mite_list_devices();

	return 0;
}

static void __exit tnt4882_tms_exit_module(void)
{
	gpib_unregister_driver(&ni_pci_interface);

	mite_cleanup();
}

module_init( tnt4882_tms_init_module );
module_exit( tnt4882_tms_exit_module );







