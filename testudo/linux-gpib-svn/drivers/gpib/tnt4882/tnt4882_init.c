/***************************************************************************
                          nec7210/tnt4882_init.c  -  description
                             -------------------
 board specific initialization stuff for National Instruments boards
 using tnt4882 or compatible chips (at-gpib, etc).

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
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/string.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

int ni_tnt_isa_attach( gpib_board_t *board, gpib_board_config_t config );
int ni_nat4882_isa_attach( gpib_board_t *board, gpib_board_config_t config );
int ni_nec_isa_attach( gpib_board_t *board, gpib_board_config_t config );
int ni_pci_attach(gpib_board_t *board, gpib_board_config_t config);

void ni_isa_detach(gpib_board_t *board);
void ni_pci_detach(gpib_board_t *board);

// wrappers for interface functions
int tnt4882_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_private_t *nec_priv = &priv->nec7210_priv;
	int retval;
	int dummy;

	retval = nec7210_read(board, &priv->nec7210_priv, buffer, length, end, bytes_read);

	if( retval < 0 )
	{	// force immediate holdoff
		write_byte( nec_priv, AUX_HLDI, AUXMR );
		set_bit( RFD_HOLDOFF_BN, &nec_priv->state );
		nec7210_read_data_in( board, nec_priv, &dummy );
	}
	return retval;
}
int tnt4882_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	tnt4882_private_t *priv = board->private_data;
	return nec7210_write(board, &priv->nec7210_priv, buffer, length, send_eoi, bytes_written);
}
ssize_t tnt4882_command_unaccel(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	tnt4882_private_t *priv = board->private_data;

	return nec7210_command(board, &priv->nec7210_priv, buffer, length);
}
int tnt4882_take_control(gpib_board_t *board, int synchronous)
{
	tnt4882_private_t *priv = board->private_data;
	return nec7210_take_control(board, &priv->nec7210_priv, synchronous);
}
int tnt4882_go_to_standby(gpib_board_t *board)
{
	tnt4882_private_t *priv = board->private_data;
	return nec7210_go_to_standby(board, &priv->nec7210_priv);
}
void tnt4882_request_system_control( gpib_board_t *board, int request_control )
{
	tnt4882_private_t *priv = board->private_data;

	if(request_control)
	{
		tnt_writeb( priv, SETSC, CMDR );
		udelay(1);
	}
	nec7210_request_system_control( board, &priv->nec7210_priv, request_control );
	if(!request_control)
	{
		tnt_writeb( priv, CLRSC, CMDR );
		udelay(1);
	}
}
void tnt4882_interface_clear(gpib_board_t *board, int assert)
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_interface_clear(board, &priv->nec7210_priv, assert);
}
void tnt4882_remote_enable(gpib_board_t *board, int enable)
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_remote_enable(board, &priv->nec7210_priv, enable);
}
int tnt4882_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits)
{
	tnt4882_private_t *priv = board->private_data;
	return nec7210_enable_eos(board, &priv->nec7210_priv, eos_byte, compare_8_bits);
}
void tnt4882_disable_eos(gpib_board_t *board)
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_disable_eos(board, &priv->nec7210_priv);
}
unsigned int tnt4882_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	tnt4882_private_t *priv = board->private_data;
	return nec7210_update_status( board, &priv->nec7210_priv, clear_mask );
}
void tnt4882_primary_address(gpib_board_t *board, unsigned int address)
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_primary_address(board, &priv->nec7210_priv, address);
}
void tnt4882_secondary_address(gpib_board_t *board, unsigned int address, int enable)
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_secondary_address(board, &priv->nec7210_priv, address, enable);
}
int tnt4882_parallel_poll(gpib_board_t *board, uint8_t *result)
{
	tnt4882_private_t *tnt_priv = board->private_data;
	if(tnt_priv->nec7210_priv.type != NEC7210)
	{
		tnt_priv->auxg_bits |= RPP2_BIT;
		write_byte(&tnt_priv->nec7210_priv, tnt_priv->auxg_bits, AUXMR);
		udelay(2);	//FIXME use parallel poll timeout
		*result = read_byte(&tnt_priv->nec7210_priv, CPTR);
		tnt_priv->auxg_bits &= ~RPP2_BIT;
		write_byte(&tnt_priv->nec7210_priv, tnt_priv->auxg_bits, AUXMR);
		return 0;
	}else
	{
		return nec7210_parallel_poll(board, &tnt_priv->nec7210_priv, result);
	}
}
void tnt4882_parallel_poll_configure(gpib_board_t *board, uint8_t config )
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_parallel_poll_configure( board, &priv->nec7210_priv, config );
}
void tnt4882_parallel_poll_response(gpib_board_t *board, int ist )
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_parallel_poll_response( board, &priv->nec7210_priv, ist );
}
// XXX tnt4882 has fancier serial poll capability, should send reqt AUX command
void tnt4882_serial_poll_response(gpib_board_t *board, uint8_t status)
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_serial_poll_response(board, &priv->nec7210_priv, status);
}
uint8_t tnt4882_serial_poll_status( gpib_board_t *board )
{
	tnt4882_private_t *priv = board->private_data;
	return nec7210_serial_poll_status( board, &priv->nec7210_priv );
}
void tnt4882_return_to_local( gpib_board_t *board )
{
	tnt4882_private_t *priv = board->private_data;
	nec7210_return_to_local( board, &priv->nec7210_priv );
}

gpib_interface_t ni_pci_interface =
{
	name: "ni_pci",
	attach: ni_pci_attach,
	detach: ni_pci_detach,
	read: tnt4882_accel_read,
	write: tnt4882_accel_write,
	command: tnt4882_command,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	request_system_control: tnt4882_request_system_control,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	parallel_poll_response: tnt4882_parallel_poll_response,
	line_status: tnt4882_line_status,
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	serial_poll_status: tnt4882_serial_poll_status,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
};

gpib_interface_t ni_pci_accel_interface =
{
	name: "ni_pci_accel",
	attach: ni_pci_attach,
	detach: ni_pci_detach,
	read: tnt4882_accel_read,
	write: tnt4882_accel_write,
	command: tnt4882_command,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	request_system_control: tnt4882_request_system_control,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	parallel_poll_response: tnt4882_parallel_poll_response,
	line_status: tnt4882_line_status,
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	serial_poll_status: tnt4882_serial_poll_status,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
};

gpib_interface_t ni_isa_interface =
{
	name: "ni_isa",
	attach: ni_tnt_isa_attach,
	detach: ni_isa_detach,
	read: tnt4882_accel_read,
	write: tnt4882_accel_write,
	command: tnt4882_command,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	request_system_control: tnt4882_request_system_control,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	parallel_poll_response: tnt4882_parallel_poll_response,
	line_status: tnt4882_line_status,
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	serial_poll_status: tnt4882_serial_poll_status,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
};

gpib_interface_t ni_nat4882_isa_interface =
{
	name: "ni_nat4882_isa",
	attach: ni_nat4882_isa_attach,
	detach: ni_isa_detach,
	read: tnt4882_read,
	write: tnt4882_write,
	command: tnt4882_command_unaccel,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	request_system_control: tnt4882_request_system_control,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	parallel_poll_response: tnt4882_parallel_poll_response,
	line_status: tnt4882_line_status,
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	serial_poll_status: tnt4882_serial_poll_status,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
};

gpib_interface_t ni_nec_isa_interface =
{
	name: "ni_nec_isa",
	attach: ni_nec_isa_attach,
	detach: ni_isa_detach,
	read: tnt4882_read,
	write: tnt4882_write,
	command: tnt4882_command_unaccel,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	request_system_control: tnt4882_request_system_control,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	parallel_poll_response: tnt4882_parallel_poll_response,
	line_status: NULL,
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	serial_poll_status: tnt4882_serial_poll_status,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
};

gpib_interface_t ni_isa_accel_interface =
{
	name: "ni_isa_accel",
	attach: ni_tnt_isa_attach,
	detach: ni_isa_detach,
	read: tnt4882_accel_read,
	write: tnt4882_accel_write,
	command: tnt4882_command,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	request_system_control: tnt4882_request_system_control,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	parallel_poll_response: tnt4882_parallel_poll_response,
	line_status: tnt4882_line_status,
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	serial_poll_status: tnt4882_serial_poll_status,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
};

gpib_interface_t ni_nat4882_isa_accel_interface =
{
	name: "ni_nat4882_isa_accel",
	attach: ni_nat4882_isa_attach,
	detach: ni_isa_detach,
	read: tnt4882_accel_read,
	write: tnt4882_accel_write,
	command: tnt4882_command_unaccel,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	request_system_control: tnt4882_request_system_control,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	parallel_poll_response: tnt4882_parallel_poll_response,
	line_status: tnt4882_line_status,
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	serial_poll_status: tnt4882_serial_poll_status,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
};

gpib_interface_t ni_nec_isa_accel_interface =
{
	name: "ni_nec_isa_accel",
	attach: ni_nec_isa_attach,
	detach: ni_isa_detach,
	read: tnt4882_accel_read,
	write: tnt4882_accel_write,
	command: tnt4882_command_unaccel,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	request_system_control: tnt4882_request_system_control,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	parallel_poll_configure: tnt4882_parallel_poll_configure,
	parallel_poll_response: tnt4882_parallel_poll_response,
	line_status: NULL,
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
	serial_poll_status: tnt4882_serial_poll_status,
	t1_delay: tnt4882_t1_delay,
	return_to_local: tnt4882_return_to_local,
};

void tnt4882_board_reset( tnt4882_private_t *tnt_priv, gpib_board_t *board )
{
	nec7210_private_t *nec_priv = &tnt_priv->nec7210_priv;

	tnt_priv->imr0_bits = 0;
	tnt_writeb( tnt_priv, tnt_priv->imr0_bits, IMR0 );
	tnt_priv->imr3_bits = 0;
	tnt_writeb( tnt_priv, tnt_priv->imr3_bits, IMR3 );
	tnt_readb( tnt_priv, IMR0 );
	tnt_readb( tnt_priv, IMR3 );
	nec7210_board_reset( nec_priv, board );
}

int tnt4882_allocate_private(gpib_board_t *board)
{
	tnt4882_private_t *tnt_priv;

	board->private_data = kmalloc(sizeof(tnt4882_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -1;
	tnt_priv = board->private_data;
	memset(tnt_priv, 0, sizeof(tnt4882_private_t));
	init_nec7210_private( &tnt_priv->nec7210_priv );
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

void tnt4882_init( tnt4882_private_t *tnt_priv, const gpib_board_t *board )
{
	nec7210_private_t *nec_priv = &tnt_priv->nec7210_priv;

	/* Turbo488 software reset */
	tnt_writeb( tnt_priv, SOFT_RESET, CMDR );
	udelay(1);

	// turn off one-chip mode
	tnt_writeb( tnt_priv, NODMA, HSSEL );
	tnt_writeb( tnt_priv, 0, ACCWR );
	// make sure we are in 7210 mode
	tnt_writeb( tnt_priv,AUX_7210, AUXCR);
	udelay(1);
	// registers might be swapped, so write it to the swapped address too
	tnt_writeb( tnt_priv,AUX_7210, SWAPPED_AUXCR);
	udelay(1);
	// turn on one-chip mode
	if( nec_priv->type == TNT4882 )
		tnt_writeb(tnt_priv, NODMA | TNT_ONE_CHIP_BIT, HSSEL);
	else
		tnt_writeb(tnt_priv, NODMA, HSSEL);

	nec7210_board_reset( nec_priv, board );
	// read-clear isr0
	tnt_readb( tnt_priv, ISR0 );

	// enable passing of nat4882 interrupts
	tnt_priv->imr3_bits = HR_TLCI;
	tnt_writeb( tnt_priv, tnt_priv->imr3_bits, IMR3 );

	// enable interrupt
	tnt_writeb( tnt_priv, 0x1, INTRT );

	// force immediate holdoff
	write_byte( &tnt_priv->nec7210_priv, AUX_HLDI, AUXMR );
	set_bit( RFD_HOLDOFF_BN, &nec_priv->state );

	tnt_priv->auxg_bits = AUXRG | NTNL_BIT;
	write_byte( &tnt_priv->nec7210_priv, tnt_priv->auxg_bits, AUXMR );

	nec7210_board_online( nec_priv, board );
	// enable interface clear interrupt for event queue
	tnt_priv->imr0_bits = TNT_IMR0_ALWAYS_BITS | TNT_ATNI_BIT | TNT_IFCIE_BIT;
	tnt_writeb( tnt_priv, tnt_priv->imr0_bits, IMR0 );
}

int ni_pci_attach(gpib_board_t *board, gpib_board_config_t config)
{
	tnt4882_private_t *tnt_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = IRQF_SHARED;
	int retval;
	struct mite_struct *mite;

	board->status = 0;

	if(tnt4882_allocate_private(board))
		return -ENOMEM;
	tnt_priv = board->private_data;
	tnt_priv->io_writeb = writeb_wrapper;
	tnt_priv->io_readb = readb_wrapper;
	tnt_priv->io_writew = writew_wrapper;
	tnt_priv->io_readw = readw_wrapper;
	nec_priv = &tnt_priv->nec7210_priv;
	nec_priv->type = TNT4882;
	nec_priv->read_byte = nec7210_locking_iomem_read_byte;
	nec_priv->write_byte = nec7210_locking_iomem_write_byte;
	nec_priv->offset = atgpib_reg_offset;

	if(mite_devices == NULL)
	{
		printk("no National Instruments PCI boards found\n");
		return -1;
	}

	for(mite = mite_devices; mite; mite = mite->next)
	{
		short found_board;

		if(mite->used) continue;
		if( board->pci_bus >=0 && board->pci_bus !=
			mite->pcidev->bus->number )
			continue;
		if( board->pci_slot >= 0 && board->pci_slot !=
			PCI_SLOT(mite->pcidev->devfn))
			continue;
		switch(mite_device_id(mite))
		{
		case PCI_DEVICE_ID_NI_GPIB:
		case PCI_DEVICE_ID_NI_GPIB_PLUS:
		case PCI_DEVICE_ID_NI_PXIGPIB:
		case PCI_DEVICE_ID_NI_PMCGPIB:
		case PCI_DEVICE_ID_NI_PCIEGPIB:
			found_board = 1;
			break;
		default:
			found_board = 0;
			break;
		}
		if(found_board) break;
	}
	if(mite == NULL)
	{
		printk("no NI PCI-GPIB boards found\n");
		return -1;
	}
	tnt_priv->mite = mite;
	retval = mite_setup(tnt_priv->mite);
	if(retval < 0)
	{
		printk("tnt4882: error setting up mite.\n");
		return retval;
	}

	nec_priv->iobase = tnt_priv->mite->daq_io_addr;

	// get irq
	if(request_irq(mite_irq(tnt_priv->mite), tnt4882_interrupt, isr_flags, "ni-pci-gpib", board))
	{
		printk("gpib: can't request IRQ %d\n", mite_irq( tnt_priv->mite ) );
		return -1;
	}
	tnt_priv->irq = mite_irq(tnt_priv->mite);
	printk( "tnt4882: irq %i\n", tnt_priv->irq );

	tnt4882_init( tnt_priv, board );

	return 0;
}

void ni_pci_detach(gpib_board_t *board)
{
	tnt4882_private_t *tnt_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(tnt_priv)
	{
		nec_priv = &tnt_priv->nec7210_priv;

		if(nec_priv->iobase)
		{
			tnt4882_board_reset( tnt_priv, board );
		}
		if(tnt_priv->irq)
		{
			free_irq(tnt_priv->irq, board);
		}
		if(tnt_priv->mite)
		{
			mite_unsetup(tnt_priv->mite);
		}
	}
	tnt4882_free_private(board);
}

int ni_isapnp_find( struct pnp_dev **dev )
{
	*dev = pnp_find_dev( NULL, ISAPNP_VENDOR_ID_NI,
		ISAPNP_FUNCTION( ISAPNP_ID_NI_ATGPIB_TNT ), NULL );
	if(*dev == NULL || (*dev)->card == NULL)
	{
		printk( "tnt4882: failed to find isapnp board\n" );
		return -ENODEV;
	}
	if(pnp_device_attach(*dev) < 0)
 	{
		printk( "tnt4882: atgpib/tnt board already active, skipping\n" );
		return -EBUSY;
	}
	if(pnp_activate_dev(*dev) < 0 )
	{
		pnp_device_detach(*dev);
		printk( "tnt4882: failed to activate() atgpib/tnt, aborting\n" );
		return -EAGAIN;
	}
	if(!pnp_port_valid(*dev, 0) || !pnp_irq_valid(*dev, 0))
	{
		pnp_device_detach(*dev);
		printk( "tnt4882: invalid port or irq for atgpib/tnt, aborting\n" );
		return -ENOMEM;
	}
	return 0;
}

int ni_isa_attach_common( gpib_board_t *board, enum nec7210_chipset chipset )
{
	tnt4882_private_t *tnt_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = 0;
	void *iobase;

	board->status = 0;

	if(tnt4882_allocate_private(board))
		return -ENOMEM;
	tnt_priv = board->private_data;
	tnt_priv->io_writeb = outb_wrapper;
	tnt_priv->io_readb = inb_wrapper;
	tnt_priv->io_writew = outw_wrapper;
	tnt_priv->io_readw = inw_wrapper;
	nec_priv = &tnt_priv->nec7210_priv;
	nec_priv->type = chipset;
	nec_priv->read_byte = nec7210_locking_ioport_read_byte;
	nec_priv->write_byte = nec7210_locking_ioport_write_byte;
	nec_priv->offset = atgpib_reg_offset;

	// look for plug-n-play board
	if( board->ibbase == 0 )
	{
		struct pnp_dev *dev;
		int retval;

		retval = ni_isapnp_find( &dev );
		if( retval < 0 ) return retval;
		tnt_priv->pnp_dev = dev;
		iobase = (void*)(pnp_port_start(dev, 0));
		board->ibirq = pnp_irq(dev, 0);
	}else
		iobase = board->ibbase;

	// allocate ioports
	if(request_region((unsigned long)(iobase), atgpib_iosize, "atgpib") == NULL)
	{
		printk("tnt4882: failed to allocate ioports\n");
		return -1;
	}
	nec_priv->iobase = iobase;

	// get irq
	if(request_irq(board->ibirq, tnt4882_interrupt, isr_flags, "atgpib", board))
	{
		printk("gpib: can't request IRQ %d\n", board->ibirq);
		return -1;
	}
	tnt_priv->irq = board->ibirq;

	tnt4882_init( tnt_priv, board );

	return 0;
}

int ni_tnt_isa_attach( gpib_board_t *board, gpib_board_config_t config )
{
	return ni_isa_attach_common( board, TNT4882 );
}

int ni_nat4882_isa_attach( gpib_board_t *board, gpib_board_config_t config )
{
	return ni_isa_attach_common( board, NAT4882 );
}

int ni_nec_isa_attach( gpib_board_t *board, gpib_board_config_t config )
{
	return ni_isa_attach_common( board, NEC7210 );
}

void ni_isa_detach(gpib_board_t *board)
{
	tnt4882_private_t *tnt_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(tnt_priv)
	{
		nec_priv = &tnt_priv->nec7210_priv;
		if(nec_priv->iobase)
		{
			tnt4882_board_reset( tnt_priv, board );
		}
		if( tnt_priv->irq )
		{
			free_irq( tnt_priv->irq, board );
		}
		if( nec_priv->iobase )
		{
			release_region((unsigned long)(nec_priv->iobase), atgpib_iosize);
		}
		if(tnt_priv->pnp_dev)
		{
			pnp_device_detach(tnt_priv->pnp_dev);
		}
	}
	tnt4882_free_private(board);
}

static const struct pci_device_id tnt4882_pci_table[] =
{
	{PCI_DEVICE(PCI_VENDOR_ID_NATINST, PCI_DEVICE_ID_NI_GPIB)},
	{PCI_DEVICE(PCI_VENDOR_ID_NATINST, PCI_DEVICE_ID_NI_GPIB_PLUS)},
	{PCI_DEVICE(PCI_VENDOR_ID_NATINST, PCI_DEVICE_ID_NI_PXIGPIB)},
	{PCI_DEVICE(PCI_VENDOR_ID_NATINST, PCI_DEVICE_ID_NI_PMCGPIB)},
	{PCI_DEVICE(PCI_VENDOR_ID_NATINST, PCI_DEVICE_ID_NI_PCIEGPIB)},
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, tnt4882_pci_table);

static const struct pnp_device_id tnt4882_pnp_table[] __devinitdata =
{
	{.id = "NICC601"},
	{.id = ""}
};
MODULE_DEVICE_TABLE(pnp, tnt4882_pnp_table);

static int __init tnt4882_init_module( void )
{
	gpib_register_driver(&ni_isa_interface, THIS_MODULE);
	gpib_register_driver(&ni_isa_accel_interface, THIS_MODULE);
	gpib_register_driver(&ni_nat4882_isa_interface, THIS_MODULE);
	gpib_register_driver(&ni_nat4882_isa_accel_interface, THIS_MODULE);
	gpib_register_driver(&ni_nec_isa_interface, THIS_MODULE);
	gpib_register_driver(&ni_nec_isa_accel_interface, THIS_MODULE);
	gpib_register_driver(&ni_pci_interface, THIS_MODULE);
	gpib_register_driver(&ni_pci_accel_interface, THIS_MODULE);
#if defined(GPIB_CONFIG_PCMCIA)
	gpib_register_driver(&ni_pcmcia_interface, THIS_MODULE);
	gpib_register_driver(&ni_pcmcia_accel_interface, THIS_MODULE);
	if( init_ni_gpib_cs() < 0 )
		return -1;
#endif

	mite_init();
	mite_list_devices();

	return 0;
}

static void __exit tnt4882_exit_module( void )
{
	gpib_unregister_driver(&ni_isa_interface);
	gpib_unregister_driver(&ni_isa_accel_interface);
	gpib_unregister_driver(&ni_nat4882_isa_interface);
	gpib_unregister_driver(&ni_nat4882_isa_accel_interface);
	gpib_unregister_driver(&ni_nec_isa_interface);
	gpib_unregister_driver(&ni_nec_isa_accel_interface);
	gpib_unregister_driver(&ni_pci_interface);
	gpib_unregister_driver(&ni_pci_accel_interface);
#if defined(GPIB_CONFIG_PCMCIA)
	gpib_unregister_driver(&ni_pcmcia_interface);
	gpib_unregister_driver(&ni_pcmcia_accel_interface);
	exit_ni_gpib_cs();
#endif

	mite_cleanup();
}

module_init( tnt4882_init_module );
module_exit( tnt4882_exit_module );
