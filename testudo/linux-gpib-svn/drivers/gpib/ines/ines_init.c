/***************************************************************************
                          nec7210/ines_init.c  -  description
                             -------------------
  Initialization stuff for ines GPIB boards

    copyright            : (C) 1999 Axel Dziemba (axel.dziemba@ines.de)
                           (C) 2002 by Frank Mori Hess
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

#include "ines.h"

#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <asm/io.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

int ines_pci_attach(gpib_board_t *board, gpib_board_config_t config);
int ines_pci_accel_attach(gpib_board_t *board, gpib_board_config_t config);
int ines_isa_attach(gpib_board_t *board, gpib_board_config_t config);

void ines_pci_detach(gpib_board_t *board);
void ines_isa_detach(gpib_board_t *board);

enum ines_pci_vendor_ids
{
	PCI_VENDOR_ID_INES_QUICKLOGIC = 0x16da
};

enum ines_pci_device_ids
{
	PCI_DEVICE_ID_INES_GPIB_AMCC = 0x8507,
	PCI_DEVICE_ID_INES_GPIB_QL5030 = 0x11,
};

enum ines_pci_subdevice_ids
{
	PCI_SUBDEVICE_ID_INES_GPIB = 0x1072
};

static struct pci_device_id ines_pci_table[] =
{
	{PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_9050, PCI_VENDOR_ID_PLX, PCI_SUBDEVICE_ID_INES_GPIB, 0, 0, 0},
	{PCI_VENDOR_ID_AMCC, PCI_DEVICE_ID_INES_GPIB_AMCC, PCI_VENDOR_ID_AMCC, PCI_SUBDEVICE_ID_INES_GPIB, 0, 0, 0},
	{PCI_VENDOR_ID_INES_QUICKLOGIC, PCI_DEVICE_ID_INES_GPIB_QL5030, PCI_VENDOR_ID_INES_QUICKLOGIC, PCI_DEVICE_ID_INES_GPIB_QL5030, 0, 0, 0},
	{PCI_DEVICE(PCI_VENDOR_ID_QUANCOM, PCI_DEVICE_ID_QUANCOM_GPIB)},
	{0}
};
MODULE_DEVICE_TABLE(pci, ines_pci_table);

typedef struct
{
	unsigned int vendor_id;
	unsigned int device_id;
	int subsystem_vendor_id;
	int subsystem_device_id;
	unsigned int gpib_region;
	unsigned int io_offset;
	enum ines_pci_chip pci_chip_type;
} ines_pci_id;

ines_pci_id pci_ids[] =
{
	{
		vendor_id: PCI_VENDOR_ID_PLX,
		device_id: PCI_DEVICE_ID_PLX_9050,
		subsystem_vendor_id: PCI_VENDOR_ID_PLX,
		subsystem_device_id: PCI_SUBDEVICE_ID_INES_GPIB,
		gpib_region: 2,
		io_offset: 1,
		pci_chip_type: PCI_CHIP_PLX9050,
	},
	{
		vendor_id: PCI_VENDOR_ID_AMCC,
		device_id: PCI_DEVICE_ID_INES_GPIB_AMCC,
		subsystem_vendor_id: PCI_VENDOR_ID_AMCC,
		subsystem_device_id: PCI_SUBDEVICE_ID_INES_GPIB,
		gpib_region: 1,
		io_offset: 1,
		pci_chip_type: PCI_CHIP_AMCC5920,
	},
	{
		vendor_id: PCI_VENDOR_ID_INES_QUICKLOGIC,
		device_id: PCI_DEVICE_ID_INES_GPIB_QL5030,
		subsystem_vendor_id: PCI_VENDOR_ID_INES_QUICKLOGIC,
		subsystem_device_id: PCI_DEVICE_ID_INES_GPIB_QL5030,
		gpib_region: 1,
		io_offset: 1,
		pci_chip_type: PCI_CHIP_QUICKLOGIC5030,
	},
	{
		vendor_id: PCI_VENDOR_ID_QUANCOM,
		device_id: PCI_DEVICE_ID_QUANCOM_GPIB,
		subsystem_vendor_id: -1,
		subsystem_device_id: -1,
		gpib_region: 0,
		io_offset: 4,
		pci_chip_type: PCI_CHIP_QUANCOM,
	},
};

static const int num_pci_chips = sizeof(pci_ids) / sizeof(pci_ids[0]);


// wrappers for interface functions
int ines_read(gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	ines_private_t *priv = board->private_data;
	nec7210_private_t *nec_priv = &priv->nec7210_priv;
	ssize_t retval;
	int dummy;

	retval = nec7210_read(board, &priv->nec7210_priv, buffer, length, end, bytes_read);
	if( retval < 0 )
	{
		write_byte( nec_priv, INES_RFD_HLD_IMMEDIATE, AUXMR );
		set_bit( RFD_HOLDOFF_BN, &nec_priv->state );
		nec7210_read_data_in( board, nec_priv, &dummy );
	}
	return retval;
}
int ines_write(gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	ines_private_t *priv = board->private_data;
	return nec7210_write(board, &priv->nec7210_priv, buffer, length, send_eoi, bytes_written);
}
ssize_t ines_command(gpib_board_t *board, uint8_t *buffer, size_t length)
{
	ines_private_t *priv = board->private_data;
	return nec7210_command(board, &priv->nec7210_priv, buffer, length);
}
int ines_take_control(gpib_board_t *board, int synchronous)
{
	ines_private_t *priv = board->private_data;
	return nec7210_take_control(board, &priv->nec7210_priv, synchronous);
}
int ines_go_to_standby(gpib_board_t *board)
{
	ines_private_t *priv = board->private_data;
	return nec7210_go_to_standby(board, &priv->nec7210_priv);
}
void ines_request_system_control( gpib_board_t *board, int request_control )
{
	ines_private_t *priv = board->private_data;
	nec7210_request_system_control( board, &priv->nec7210_priv, request_control );
}
void ines_interface_clear(gpib_board_t *board, int assert)
{
	ines_private_t *priv = board->private_data;
	nec7210_interface_clear(board, &priv->nec7210_priv, assert);
}
void ines_remote_enable(gpib_board_t *board, int enable)
{
	ines_private_t *priv = board->private_data;
	nec7210_remote_enable(board, &priv->nec7210_priv, enable);
}
int ines_enable_eos(gpib_board_t *board, uint8_t eos_byte, int compare_8_bits)
{
	ines_private_t *priv = board->private_data;
	return nec7210_enable_eos(board, &priv->nec7210_priv, eos_byte, compare_8_bits);
}
void ines_disable_eos(gpib_board_t *board)
{
	ines_private_t *priv = board->private_data;
	nec7210_disable_eos(board, &priv->nec7210_priv);
}
unsigned int ines_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	ines_private_t *priv = board->private_data;
	return nec7210_update_status( board, &priv->nec7210_priv, clear_mask );
}
void ines_primary_address(gpib_board_t *board, unsigned int address)
{
	ines_private_t *priv = board->private_data;
	nec7210_primary_address(board, &priv->nec7210_priv, address);
}
void ines_secondary_address(gpib_board_t *board, unsigned int address, int enable)
{
	ines_private_t *priv = board->private_data;
	nec7210_secondary_address(board, &priv->nec7210_priv, address, enable);
}
int ines_parallel_poll(gpib_board_t *board, uint8_t *result)
{
	ines_private_t *priv = board->private_data;
	return nec7210_parallel_poll(board, &priv->nec7210_priv, result);
}
void ines_parallel_poll_configure(gpib_board_t *board, uint8_t config)
{
	ines_private_t *priv = board->private_data;
	nec7210_parallel_poll_configure(board, &priv->nec7210_priv, config);
}
void ines_parallel_poll_response(gpib_board_t *board, int ist)
{
	ines_private_t *priv = board->private_data;
	nec7210_parallel_poll_response(board, &priv->nec7210_priv, ist);
}
void ines_serial_poll_response(gpib_board_t *board, uint8_t status)
{
	ines_private_t *priv = board->private_data;
	nec7210_serial_poll_response(board, &priv->nec7210_priv, status);
}
uint8_t ines_serial_poll_status( gpib_board_t *board )
{
	ines_private_t *priv = board->private_data;
	return nec7210_serial_poll_status( board, &priv->nec7210_priv );
}
void ines_return_to_local( gpib_board_t *board )
{
	ines_private_t *priv = board->private_data;
	nec7210_return_to_local( board, &priv->nec7210_priv );
}

gpib_interface_t ines_pci_unaccel_interface =
{
	name: "ines_pci_unaccel",
	attach: ines_pci_attach,
	detach: ines_pci_detach,
	read: ines_read,
	write: ines_write,
	command: ines_command,
	take_control: ines_take_control,
	go_to_standby: ines_go_to_standby,
	request_system_control: ines_request_system_control,
	interface_clear: ines_interface_clear,
	remote_enable: ines_remote_enable,
	enable_eos: ines_enable_eos,
	disable_eos: ines_disable_eos,
	parallel_poll: ines_parallel_poll,
	parallel_poll_configure: ines_parallel_poll_configure,
	parallel_poll_response: ines_parallel_poll_response,
	line_status: ines_line_status,
	update_status: ines_update_status,
	primary_address: ines_primary_address,
	secondary_address: ines_secondary_address,
	serial_poll_response: ines_serial_poll_response,
	serial_poll_status: ines_serial_poll_status,
	t1_delay: ines_t1_delay,
	return_to_local: ines_return_to_local,
};

gpib_interface_t ines_pci_interface =
{
	name: "ines_pci",
	attach: ines_pci_accel_attach,
	detach: ines_pci_detach,
	read: ines_accel_read,
	write: ines_accel_write,
	command: ines_command,
	take_control: ines_take_control,
	go_to_standby: ines_go_to_standby,
	request_system_control: ines_request_system_control,
	interface_clear: ines_interface_clear,
	remote_enable: ines_remote_enable,
	enable_eos: ines_enable_eos,
	disable_eos: ines_disable_eos,
	parallel_poll: ines_parallel_poll,
	parallel_poll_configure: ines_parallel_poll_configure,
	parallel_poll_response: ines_parallel_poll_response,
	line_status: ines_line_status,
	update_status: ines_update_status,
	primary_address: ines_primary_address,
	secondary_address: ines_secondary_address,
	serial_poll_response: ines_serial_poll_response,
	serial_poll_status: ines_serial_poll_status,
	t1_delay: ines_t1_delay,
	return_to_local: ines_return_to_local,
};

gpib_interface_t ines_pci_accel_interface =
{
	name: "ines_pci_accel",
	attach: ines_pci_accel_attach,
	detach: ines_pci_detach,
	read: ines_accel_read,
	write: ines_accel_write,
	command: ines_command,
	take_control: ines_take_control,
	go_to_standby: ines_go_to_standby,
	request_system_control: ines_request_system_control,
	interface_clear: ines_interface_clear,
	remote_enable: ines_remote_enable,
	enable_eos: ines_enable_eos,
	disable_eos: ines_disable_eos,
	parallel_poll: ines_parallel_poll,
	parallel_poll_configure: ines_parallel_poll_configure,
	parallel_poll_response: ines_parallel_poll_response,
	line_status: ines_line_status,
	update_status: ines_update_status,
	primary_address: ines_primary_address,
	secondary_address: ines_secondary_address,
	serial_poll_response: ines_serial_poll_response,
	serial_poll_status: ines_serial_poll_status,
	t1_delay: ines_t1_delay,
	return_to_local: ines_return_to_local,
};

gpib_interface_t ines_isa_interface =
{
	name: "ines_isa",
	attach: ines_isa_attach,
	detach: ines_isa_detach,
	read: ines_accel_read,
	write: ines_accel_write,
	command: ines_command,
	take_control: ines_take_control,
	go_to_standby: ines_go_to_standby,
	request_system_control: ines_request_system_control,
	interface_clear: ines_interface_clear,
	remote_enable: ines_remote_enable,
	enable_eos: ines_enable_eos,
	disable_eos: ines_disable_eos,
	parallel_poll: ines_parallel_poll,
	parallel_poll_configure: ines_parallel_poll_configure,
	parallel_poll_response: ines_parallel_poll_response,
	line_status: ines_line_status,
	update_status: ines_update_status,
	primary_address: ines_primary_address,
	secondary_address: ines_secondary_address,
	serial_poll_response: ines_serial_poll_response,
	serial_poll_status: ines_serial_poll_status,
	t1_delay: ines_t1_delay,
	return_to_local: ines_return_to_local,
};

int ines_allocate_private(gpib_board_t *board)
{
	ines_private_t *priv;

	board->private_data = kmalloc(sizeof(ines_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -1;
	priv = board->private_data;
	memset( priv, 0, sizeof(ines_private_t));
	init_nec7210_private( &priv->nec7210_priv );
	return 0;
}

void ines_free_private(gpib_board_t *board)
{
	if(board->private_data)
	{
		kfree(board->private_data);
		board->private_data = NULL;
	}
}

int ines_generic_attach(gpib_board_t *board)
{
	ines_private_t *ines_priv;
	nec7210_private_t *nec_priv;

	board->status = 0;

	if(ines_allocate_private(board))
		return -ENOMEM;
	ines_priv = board->private_data;
	nec_priv = &ines_priv->nec7210_priv;
	nec_priv->read_byte = nec7210_ioport_read_byte;
	nec_priv->write_byte = nec7210_ioport_write_byte;
	nec_priv->offset = 1;
	nec_priv->type = iGPIB7210;
	ines_priv->pci_chip_type = PCI_CHIP_NONE;

	return 0;
}

void ines_online( ines_private_t *ines_priv, const gpib_board_t *board, int use_accel )
{
	nec7210_private_t *nec_priv = &ines_priv->nec7210_priv;

	/* ines doesn't seem to use internal count register */
	write_byte( nec_priv, ICR | 0, AUXMR );

	write_byte( nec_priv, INES_AUX_XMODE, AUXMR );
	write_byte( nec_priv, INES_RFD_HLD_IMMEDIATE, AUXMR );
	set_bit( RFD_HOLDOFF_BN, &nec_priv->state );
	write_byte( nec_priv, INES_AUXD | 0, AUXMR );
	ines_outb( ines_priv, 0, XDMA_CONTROL );
	ines_priv->extend_mode_bits = 0;
	ines_outb( ines_priv, ines_priv->extend_mode_bits, EXTEND_MODE );
	if( use_accel )
	{
		ines_outb( ines_priv, 0x80, OUT_FIFO_WATERMARK );
		ines_outb( ines_priv, 0x80, IN_FIFO_WATERMARK );
		ines_outb( ines_priv, IFC_ACTIVE_BIT | ATN_ACTIVE_BIT | FIFO_ERROR_BIT | XFER_COUNT_BIT, IMR3 );
		ines_outb( ines_priv, IN_FIFO_WATERMARK_BIT | IN_FIFO_FULL_BIT | OUT_FIFO_WATERMARK_BIT |
			OUT_FIFO_EMPTY_BIT, IMR4 );
	}else
	{
		nec7210_set_reg_bits( nec_priv, ADMR, IN_FIFO_ENABLE_BIT | OUT_FIFO_ENABLE_BIT, 0 );
		ines_outb( ines_priv, IFC_ACTIVE_BIT | FIFO_ERROR_BIT, IMR3 );
		ines_outb( ines_priv, 0, IMR4 );
	}

	nec7210_board_online( nec_priv, board );
	if( use_accel )
		nec7210_set_reg_bits( nec_priv, IMR1, HR_DOIE | HR_DIIE, 0 );
}

int ines_common_pci_attach( gpib_board_t *board )
{
	ines_private_t *ines_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = 0;
	int retval;
	ines_pci_id found_id;
	unsigned int i;
	struct pci_dev *pdev;

	memset(&found_id, 0, sizeof(found_id));

	retval = ines_generic_attach(board);
	if(retval) return retval;

	ines_priv = board->private_data;
	nec_priv = &ines_priv->nec7210_priv;

	// find board
	ines_priv->pci_device = NULL;
	for(i = 0; i < num_pci_chips && ines_priv->pci_device == NULL; i++)
	{
		pdev = NULL;
		do
		{
			if( pci_ids[i].subsystem_vendor_id >= 0 && pci_ids[i].subsystem_device_id >= 0 )
				pdev = pci_get_subsys(pci_ids[i].vendor_id, pci_ids[i].device_id,
					pci_ids[i].subsystem_vendor_id, pci_ids[i].subsystem_device_id, pdev);
			else
				pdev = pci_get_device(pci_ids[i].vendor_id, pci_ids[i].device_id, pdev);
			if( pdev == NULL )
				break;
			if( board->pci_bus >=0 && board->pci_bus != pdev->bus->number )
				continue;
			if( board->pci_slot >= 0 && board->pci_slot !=
				PCI_SLOT( pdev->devfn ) )
				continue;
			found_id = pci_ids[i];
			ines_priv->pci_device = pdev;
			break;
		}while( 1 );
	}
	if(ines_priv->pci_device == NULL)
	{
		printk("gpib: could not find ines PCI board\n");
		return -1;
	}

	if(pci_enable_device(ines_priv->pci_device))
	{
		printk("error enabling pci device\n");
		return -1;
	}

	if(pci_request_regions(ines_priv->pci_device, "ines-gpib"))
		return -1;
	nec_priv->iobase = (void*)(pci_resource_start(ines_priv->pci_device, found_id.gpib_region));

	ines_priv->pci_chip_type = found_id.pci_chip_type;
	nec_priv->offset = found_id.io_offset;
	switch( ines_priv->pci_chip_type )
	{
		case PCI_CHIP_PLX9050:
			ines_priv->plx_iobase = pci_resource_start(ines_priv->pci_device, 1);
			break;
		case PCI_CHIP_AMCC5920:
			ines_priv->amcc_iobase = pci_resource_start(ines_priv->pci_device, 0);
			break;
		case PCI_CHIP_QUANCOM:
			break;
		case PCI_CHIP_QUICKLOGIC5030:
			break;
		default:
			printk("gpib: unspecified chip type? (bug)\n");
			nec_priv->iobase = 0;
			pci_release_regions(ines_priv->pci_device);
			return -1;
			break;
	}

	nec7210_board_reset( nec_priv, board );
#if 0
	if(ines_priv->pci_chip_type == PCI_CHIP_QUANCOM)
	{
		/* change interrupt polarity */
		nec_priv->auxb_bits |= HR_INV;
		ines_outb(ines_priv, nec_priv->auxb_bits, AUXMR);
	}
#endif
	isr_flags |= IRQF_SHARED;
	if(request_irq(ines_priv->pci_device->irq, ines_pci_interrupt, isr_flags, "pci-gpib", board))
	{
		printk("gpib: can't request IRQ %d\n",ines_priv->pci_device->irq);
		return -1;
	}
	ines_priv->irq = ines_priv->pci_device->irq;

	// enable interrupts on pci chip
	switch(ines_priv->pci_chip_type)
	{
		case PCI_CHIP_PLX9050:
			outl(PLX9050_LINTR1_EN_BIT | PLX9050_LINTR1_POLARITY_BIT | PLX9050_PCI_INTR_EN_BIT,
				ines_priv->plx_iobase + PLX9050_INTCSR_REG);
			break;
		case PCI_CHIP_AMCC5920:
			{
				static const int region = 1;
				static const int num_wait_states = 7;
				uint32_t bits;

				bits = amcc_prefetch_bits(region, PREFETCH_DISABLED);
				bits |= amcc_PTADR_mode_bit(region);
				bits |= amcc_disable_write_fifo_bit(region);
				bits |= amcc_wait_state_bits(region, num_wait_states);
				outl(bits, ines_priv->amcc_iobase + AMCC_PASS_THRU_REG);
				outl(AMCC_ADDON_INTR_ENABLE_BIT, ines_priv->amcc_iobase + AMCC_INTCS_REG);
			}
			break;
		case PCI_CHIP_QUANCOM:
			outb( QUANCOM_IRQ_ENABLE_BIT, (unsigned long)(nec_priv->iobase) + QUANCOM_IRQ_CONTROL_STATUS_REG );
			break;
		case PCI_CHIP_QUICKLOGIC5030:
			break;
		default:
			printk("gpib: unspecified chip type? (bug)\n");
			return -1;
			break;
		}

	return 0;
}

int ines_pci_attach( gpib_board_t *board, gpib_board_config_t config )
{
	ines_private_t *ines_priv;
	int retval;

	retval = ines_common_pci_attach( board );
	if( retval < 0 ) return retval;

	ines_priv = board->private_data;
	ines_online( ines_priv, board, 0 );

	return 0;
}

int ines_pci_accel_attach( gpib_board_t *board, gpib_board_config_t config )
{
	ines_private_t *ines_priv;
	int retval;

	retval = ines_common_pci_attach( board );
	if( retval < 0 ) return retval;

	ines_priv = board->private_data;
	ines_online( ines_priv, board, 1 );

	return 0;
}

int ines_isa_attach( gpib_board_t *board, gpib_board_config_t config )
{
	ines_private_t *ines_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = 0;
	int retval;

	retval = ines_generic_attach(board);
	if(retval) return retval;

	ines_priv = board->private_data;
	nec_priv = &ines_priv->nec7210_priv;


	if(request_region((unsigned long)(board->ibbase), ines_isa_iosize, "ines_gpib") == 0)
	{
		printk("ines_gpib: ioports at 0x%p already in use\n", board->ibbase);
		return -1;
	}
	nec_priv->iobase = board->ibbase;
	nec_priv->offset = 1;
	nec7210_board_reset(nec_priv, board);
	if(request_irq(board->ibirq, ines_pci_interrupt, isr_flags, "ines_gpib", board))
	{
		printk("ines_gpib: failed to allocate IRQ %d\n", board->ibirq);
		return -1;
	}
	ines_priv->irq = board->ibirq;
	ines_online(ines_priv, board, 1);
	return 0;
}

void ines_pci_detach(gpib_board_t *board)
{
	ines_private_t *ines_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(ines_priv)
	{
		nec_priv = &ines_priv->nec7210_priv;
		if(ines_priv->irq)
		{
			// disable interrupts
			switch( ines_priv->pci_chip_type )
			{
			case PCI_CHIP_AMCC5920:
				if( ines_priv->plx_iobase )
					outl( 0, ines_priv->plx_iobase + PLX9050_INTCSR_REG );
				break;
			case PCI_CHIP_QUANCOM:
				if( nec_priv->iobase )
					outb( 0, (unsigned long)(nec_priv->iobase) + QUANCOM_IRQ_CONTROL_STATUS_REG );
				break;
			default:
				break;
			}
			free_irq(ines_priv->irq, board);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset( nec_priv, board );
			pci_release_regions(ines_priv->pci_device);
		}
		if(ines_priv->pci_device)
			pci_dev_put(ines_priv->pci_device);
	}
	ines_free_private(board);
}

void ines_isa_detach(gpib_board_t *board)
{
	ines_private_t *ines_priv = board->private_data;
	nec7210_private_t *nec_priv;

	if(ines_priv)
	{
		nec_priv = &ines_priv->nec7210_priv;
		if(ines_priv->irq)
		{
			free_irq(ines_priv->irq, board);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset( nec_priv, board );
			release_region((unsigned long)(nec_priv->iobase), ines_isa_iosize);
		}
	}
	ines_free_private(board);
}

static int __init ines_init_module( void )
{
	int err = 0;

	gpib_register_driver(&ines_pci_interface, THIS_MODULE);
	gpib_register_driver(&ines_pci_unaccel_interface, THIS_MODULE);
	gpib_register_driver(&ines_pci_accel_interface, THIS_MODULE);
	gpib_register_driver(&ines_isa_interface, THIS_MODULE);
#if defined(GPIB_CONFIG_PCMCIA)
	gpib_register_driver(&ines_pcmcia_interface, THIS_MODULE);
	gpib_register_driver(&ines_pcmcia_unaccel_interface, THIS_MODULE);
	gpib_register_driver(&ines_pcmcia_accel_interface, THIS_MODULE);
	err += ines_pcmcia_init_module();
#endif
	if(err)
		return -1;

	return 0;
}

static void __exit ines_exit_module( void )
{
	gpib_unregister_driver(&ines_pci_interface);
	gpib_unregister_driver(&ines_pci_unaccel_interface);
	gpib_unregister_driver(&ines_pci_accel_interface);
	gpib_unregister_driver(&ines_isa_interface);
#if defined(GPIB_CONFIG_PCMCIA)
	gpib_unregister_driver(&ines_pcmcia_interface);
	gpib_unregister_driver(&ines_pcmcia_unaccel_interface);
	gpib_unregister_driver(&ines_pcmcia_accel_interface);
	ines_pcmcia_cleanup_module();
#endif
}

module_init( ines_init_module );
module_exit( ines_exit_module );







