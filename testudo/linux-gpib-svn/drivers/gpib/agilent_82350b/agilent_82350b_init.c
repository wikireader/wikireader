/***************************************************************************
                          agilent_82350b/agilent_82350b.c  -  description
                             -------------------

    copyright            : (C) 2002, 2004 by Frank Mori Hess
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

#include "agilent_82350b.h"
#include <linux/delay.h>
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

int agilent_82350b_unaccel_attach(gpib_board_t *board, gpib_board_config_t config);
int agilent_82350b_accel_attach(gpib_board_t *board, gpib_board_config_t config);

void agilent_82350b_detach( gpib_board_t *board );

static const char *driver_name = "agilent_82350b";

int read_transfer_counter(agilent_82350b_private_t *a_priv)
{
	int lo, mid, value;
	lo = readb(a_priv->gpib_base + XFER_COUNT_LO_REG);
	mid =readb(a_priv->gpib_base + XFER_COUNT_MID_REG);
	value = (lo & 0xff) | ((mid << 8) & 0x7f00);
	value = ~(value - 1) & 0x7fff;
	return value;
}

void set_transfer_counter(agilent_82350b_private_t *a_priv, int count)
{
	int complement = -count;
	writeb(complement & 0xff, a_priv->gpib_base + XFER_COUNT_LO_REG);
	writeb((complement >> 8) & 0xff, a_priv->gpib_base + XFER_COUNT_MID_REG);
	//I don't think the hi count reg is even used, but oh well
	writeb((complement >> 16) & 0xf, a_priv->gpib_base + XFER_COUNT_HI_REG);
}

// wrappers for interface functions
int agilent_82350b_read( gpib_board_t *board, uint8_t *buffer, size_t length, int *end, size_t *bytes_read)
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_read( board, &priv->tms9914_priv, buffer, length, end, bytes_read);
}
int agilent_82350b_write( gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi, size_t *bytes_written)
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_write( board, &priv->tms9914_priv, buffer, length, send_eoi, bytes_written);
}
ssize_t agilent_82350b_command( gpib_board_t *board, uint8_t *buffer, size_t length )
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_command( board, &priv->tms9914_priv, buffer, length );
}
int agilent_82350b_take_control( gpib_board_t *board, int synchronous )
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_take_control( board, &priv->tms9914_priv, synchronous );
}
int agilent_82350b_go_to_standby( gpib_board_t *board )
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_go_to_standby( board, &priv->tms9914_priv );
}
void agilent_82350b_request_system_control( gpib_board_t *board, int request_control )
{
	agilent_82350b_private_t *a_priv = board->private_data;

	if(request_control)
	{
		a_priv->card_mode_bits |= CM_SYSTEM_CONTROLLER_BIT;
		if(a_priv->model != MODEL_82350A)
			writeb(IC_SYSTEM_CONTROLLER_BIT, a_priv->gpib_base + INTERNAL_CONFIG_REG);
	}else
	{
		a_priv->card_mode_bits &= ~CM_SYSTEM_CONTROLLER_BIT;
		if(a_priv->model != MODEL_82350A)
			writeb(0, a_priv->gpib_base + INTERNAL_CONFIG_REG);
	}
	writeb(a_priv->card_mode_bits, a_priv->gpib_base + CARD_MODE_REG);
	tms9914_request_system_control(board, &a_priv->tms9914_priv, request_control);
}
void agilent_82350b_interface_clear( gpib_board_t *board, int assert )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_interface_clear( board, &priv->tms9914_priv, assert );
}
void agilent_82350b_remote_enable( gpib_board_t *board, int enable )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_remote_enable( board, &priv->tms9914_priv, enable );
}
int agilent_82350b_enable_eos( gpib_board_t *board, uint8_t eos_byte, int compare_8_bits )
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_enable_eos( board, &priv->tms9914_priv, eos_byte, compare_8_bits );
}
void agilent_82350b_disable_eos( gpib_board_t *board )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_disable_eos( board, &priv->tms9914_priv );
}
unsigned int agilent_82350b_update_status( gpib_board_t *board, unsigned int clear_mask )
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_update_status( board, &priv->tms9914_priv, clear_mask );
}
void agilent_82350b_primary_address( gpib_board_t *board, unsigned int address )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_primary_address( board, &priv->tms9914_priv, address );
}
void agilent_82350b_secondary_address( gpib_board_t *board, unsigned int address, int enable )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_secondary_address( board, &priv->tms9914_priv, address, enable );
}
int agilent_82350b_parallel_poll( gpib_board_t *board, uint8_t *result )
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_parallel_poll( board, &priv->tms9914_priv, result );
}
void agilent_82350b_parallel_poll_configure( gpib_board_t *board, uint8_t config )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_parallel_poll_configure( board, &priv->tms9914_priv, config );
}
void agilent_82350b_parallel_poll_response( gpib_board_t *board, int ist )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_parallel_poll_response( board, &priv->tms9914_priv, ist );
}
void agilent_82350b_serial_poll_response( gpib_board_t *board, uint8_t status )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_serial_poll_response( board, &priv->tms9914_priv, status );
}
uint8_t agilent_82350b_serial_poll_status( gpib_board_t *board )
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_serial_poll_status( board, &priv->tms9914_priv );
}
int agilent_82350b_line_status( const gpib_board_t *board )
{
	agilent_82350b_private_t *priv = board->private_data;
	return tms9914_line_status( board, &priv->tms9914_priv );
}
unsigned int agilent_82350b_t1_delay( gpib_board_t *board, unsigned int nanosec )
{
	agilent_82350b_private_t *a_priv = board->private_data;
	static const int nanosec_per_clock = 30;
	unsigned value;

	tms9914_t1_delay(board, &a_priv->tms9914_priv, nanosec);

	value = (nanosec + nanosec_per_clock - 1) / nanosec_per_clock;
	if(value > 0xff) value = 0xff;
	writeb(value, a_priv->gpib_base + T1_DELAY_REG);
	return value * nanosec_per_clock;
}
void agilent_82350b_return_to_local( gpib_board_t *board )
{
	agilent_82350b_private_t *priv = board->private_data;
	tms9914_return_to_local( board, &priv->tms9914_priv );
}

gpib_interface_t agilent_82350b_unaccel_interface =
{
	name: "agilent_82350b_unaccel",
	attach: agilent_82350b_unaccel_attach,
	detach: agilent_82350b_detach,
	read: agilent_82350b_read,
	write: agilent_82350b_write,
	command: agilent_82350b_command,
	request_system_control: agilent_82350b_request_system_control,
	take_control: agilent_82350b_take_control,
	go_to_standby: agilent_82350b_go_to_standby,
	interface_clear: agilent_82350b_interface_clear,
	remote_enable: agilent_82350b_remote_enable,
	enable_eos: agilent_82350b_enable_eos,
	disable_eos: agilent_82350b_disable_eos,
	parallel_poll: agilent_82350b_parallel_poll,
	parallel_poll_configure: agilent_82350b_parallel_poll_configure,
	parallel_poll_response: agilent_82350b_parallel_poll_response,
	line_status: agilent_82350b_line_status,
	update_status: agilent_82350b_update_status,
	primary_address: agilent_82350b_primary_address,
	secondary_address: agilent_82350b_secondary_address,
	serial_poll_response: agilent_82350b_serial_poll_response,
	t1_delay: agilent_82350b_t1_delay,
	return_to_local: agilent_82350b_return_to_local,
};

gpib_interface_t agilent_82350b_interface =
{
	name: "agilent_82350b",
	attach: agilent_82350b_accel_attach,
	detach: agilent_82350b_detach,
	read: agilent_82350b_accel_read,
	write: agilent_82350b_accel_write,
	command: agilent_82350b_command,
	request_system_control: agilent_82350b_request_system_control,
	take_control: agilent_82350b_take_control,
	go_to_standby: agilent_82350b_go_to_standby,
	interface_clear: agilent_82350b_interface_clear,
	remote_enable: agilent_82350b_remote_enable,
	enable_eos: agilent_82350b_enable_eos,
	disable_eos: agilent_82350b_disable_eos,
	parallel_poll: agilent_82350b_parallel_poll,
	parallel_poll_configure: agilent_82350b_parallel_poll_configure,
	parallel_poll_response: agilent_82350b_parallel_poll_response,
	line_status: agilent_82350b_line_status,
	update_status: agilent_82350b_update_status,
	primary_address: agilent_82350b_primary_address,
	secondary_address: agilent_82350b_secondary_address,
	serial_poll_response: agilent_82350b_serial_poll_response,
	t1_delay: agilent_82350b_t1_delay,
	return_to_local: agilent_82350b_return_to_local,
};

int agilent_82350b_allocate_private( gpib_board_t *board )
{
	board->private_data = kmalloc(sizeof(agilent_82350b_private_t), GFP_KERNEL);
	if(board->private_data == NULL)
		return -ENOMEM;
	memset(board->private_data, 0, sizeof(agilent_82350b_private_t));
	return 0;
}

void agilent_82350b_free_private( gpib_board_t *board )
{
	if(board->private_data)
	{
		kfree(board->private_data);
		board->private_data = NULL;
	}
}

int init_82350a_hardware(gpib_board_t *board, const gpib_board_config_t *config)
{
	agilent_82350b_private_t *a_priv = board->private_data;
	static const unsigned firmware_length = 5302;
	unsigned borg_status;
	static const unsigned timeout = 1000;
	int i, j;
	const char *firmware_data = config->init_data;
	const unsigned plx_cntrl_static_bits = PLX9050_WAITO_NOT_USER0_SELECT_BIT |
		PLX9050_USER0_OUTPUT_BIT |
		PLX9050_LLOCK_NOT_USER1_SELECT_BIT |
		PLX9050_USER1_OUTPUT_BIT |
		PLX9050_USER2_OUTPUT_BIT |
		PLX9050_USER3_OUTPUT_BIT |
		PLX9050_PCI_READ_MODE_BIT |
		PLX9050_PCI_WRITE_MODE_BIT |
		PLX9050_PCI_RETRY_DELAY_BITS(64) |
		PLX9050_DIRECT_SLAVE_LOCK_ENABLE_BIT;
	// load borg data
	borg_status = readb(a_priv->borg_base);
	if((borg_status & BORG_DONE_BIT)) return 0;
	// need to programme borg
	if(config->init_data == NULL ||  config->init_data_length != firmware_length)
	{
		printk("%s: the 82350a requires a firmware after powering on.\n", driver_name);
		return -EIO;
	}
	printk("%s: Loading firmware... ", driver_name);

	// tickle the borg
	writel(plx_cntrl_static_bits | PLX9050_USER3_DATA_BIT, a_priv->plx_base + PLX9050_CNTRL_REG);
	msleep(1);
	writel(plx_cntrl_static_bits, a_priv->plx_base + PLX9050_CNTRL_REG);
	msleep(1);
	writel(plx_cntrl_static_bits | PLX9050_USER3_DATA_BIT, a_priv->plx_base + PLX9050_CNTRL_REG);
	msleep(1);

	for(i = 0; i < config->init_data_length; ++i)
	{
		for(j = 0; j < timeout && (readb(a_priv->borg_base) & BORG_READY_BIT) == 0; ++j)
		{
			if(need_resched()) schedule();
			udelay(10);
		}
		if(j == timeout)
		{
			printk("%s: timed out loading firmware.\n", driver_name);
			return -ETIMEDOUT;
		}
		writeb(firmware_data[i], a_priv->gpib_base + CONFIG_DATA_REG);
	}
	for(j = 0; j < timeout && (readb(a_priv->borg_base) & BORG_DONE_BIT) == 0; ++j)
	{
		if(need_resched()) schedule();
		udelay(10);
	}
	if(j == timeout)
	{
		printk("%s: timed out waiting for firmware load to complete.\n", driver_name);
		return -ETIMEDOUT;
	}
	printk("done.\n");
	return 0;
}

int test_sram(gpib_board_t *board)
{
	agilent_82350b_private_t *a_priv = board->private_data;
	unsigned i;
	const unsigned sram_length = pci_resource_len(a_priv->pci_device, SRAM_82350A_REGION);
	// test SRAM
	const unsigned byte_mask = 0xff;
	for(i = 0; i < sram_length; ++i)
	{
		writeb(i & byte_mask, a_priv->sram_base + i);
		if(need_resched()) schedule();
	}
	for(i = 0; i < sram_length; ++i)
	{
		unsigned read_value = readb(a_priv->sram_base + i);
		if((i & byte_mask) != read_value)
		{
			printk("%s: SRAM test failed at %d wanted %d got %d\n",
				driver_name, i, (i & byte_mask), read_value);
			return -EIO;
		}
		if(need_resched()) schedule();
	}
	return 0;
}

int agilent_82350b_generic_attach(gpib_board_t *board, const gpib_board_config_t *config, int use_fifos)
{
	agilent_82350b_private_t *a_priv;
	tms9914_private_t *tms_priv;
	int retval;

	board->status = 0;

	if(agilent_82350b_allocate_private(board))
		return -ENOMEM;
	a_priv = board->private_data;
	tms_priv = &a_priv->tms9914_priv;
	tms_priv->read_byte = tms9914_iomem_read_byte;
	tms_priv->write_byte = tms9914_iomem_write_byte;
	tms_priv->offset = 1;

	// find board
	a_priv->pci_device = gpib_pci_get_device(board, PCI_VENDOR_ID_AGILENT,
		PCI_DEVICE_ID_82350B, NULL);
	if(a_priv->pci_device)
	{
		a_priv->model = MODEL_82350B;
	}else
	{
		a_priv->pci_device = gpib_pci_get_subsys(board, PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_9050,
			PCI_VENDOR_ID_HP,  PCI_SUBDEVICE_ID_82350A, a_priv->pci_device);
		if(a_priv->pci_device)
		{
			a_priv->model = MODEL_82350A;
		}else
		{
			printk("agilent_82350b: no 82350 board found\n");
			return -ENODEV;
		}
	}
	if(pci_enable_device(a_priv->pci_device))
	{
		printk("error enabling pci device\n");
		return -EIO;
	}
	if(pci_request_regions(a_priv->pci_device, driver_name))
		return -EIO;
	switch(a_priv->model)
	{
	case MODEL_82350A:
		a_priv->plx_base = ioremap(pci_resource_start(a_priv->pci_device, PLX_MEM_REGION),
			pci_resource_len(a_priv->pci_device, PLX_MEM_REGION));
		printk("%s: plx base address remapped to 0x%p\n", __FUNCTION__, a_priv->plx_base );
		a_priv->gpib_base = ioremap(pci_resource_start(a_priv->pci_device, GPIB_82350A_REGION),
			pci_resource_len(a_priv->pci_device, GPIB_82350A_REGION));
		printk("%s: gpib base address remapped to 0x%p\n", __FUNCTION__, a_priv->gpib_base );
		tms_priv->iobase = a_priv->gpib_base + TMS9914_BASE_REG;
		a_priv->sram_base = ioremap(pci_resource_start(a_priv->pci_device, SRAM_82350A_REGION),
			pci_resource_len(a_priv->pci_device, SRAM_82350A_REGION));
		printk("%s: sram base address remapped to 0x%p\n", __FUNCTION__, a_priv->sram_base );
		a_priv->borg_base = ioremap(pci_resource_start(a_priv->pci_device, BORG_82350A_REGION),
			pci_resource_len(a_priv->pci_device, BORG_82350A_REGION));
		printk("%s: borg base address remapped to 0x%p\n", __FUNCTION__, a_priv->borg_base );
		retval = init_82350a_hardware(board, config);
		if(retval < 0) return retval;
		break;
	case MODEL_82350B:
		a_priv->gpib_base = ioremap(pci_resource_start(a_priv->pci_device, GPIB_REGION),
			pci_resource_len(a_priv->pci_device, GPIB_REGION));
		printk("%s: gpib base address remapped to 0x%p\n", __FUNCTION__, a_priv->gpib_base );
		tms_priv->iobase = a_priv->gpib_base + TMS9914_BASE_REG;
		a_priv->sram_base = ioremap(pci_resource_start(a_priv->pci_device, SRAM_REGION),
			pci_resource_len(a_priv->pci_device, SRAM_REGION));
		printk("%s: sram base address remapped to 0x%p\n", __FUNCTION__, a_priv->sram_base );
		a_priv->misc_base = ioremap(pci_resource_start(a_priv->pci_device, MISC_REGION),
			pci_resource_len(a_priv->pci_device, MISC_REGION));
		printk("%s: misc base address remapped to 0x%p\n", __FUNCTION__, a_priv->misc_base );
		break;
	default:
		BUG();
	}

	retval = test_sram(board);
	if(retval < 0) return retval;

	if(request_irq(a_priv->pci_device->irq, agilent_82350b_interrupt, IRQF_SHARED, driver_name, board))
	{
		printk("gpib: can't request IRQ %d\n", a_priv->pci_device->irq);
		return -EIO;
	}
	a_priv->irq = a_priv->pci_device->irq;
	printk( "agilent_82350b: IRQ %d\n", a_priv->irq );

	writeb(0, a_priv->gpib_base + SRAM_ACCESS_CONTROL_REG);
	a_priv->card_mode_bits = ENABLE_PCI_IRQ_BIT;
	writeb(a_priv->card_mode_bits, a_priv->gpib_base + CARD_MODE_REG);

	if(a_priv->model == MODEL_82350A)
	{
		// enable PCI interrupts for 82350a
		writel(PLX9050_LINTR1_EN_BIT | PLX9050_LINTR2_POLARITY_BIT | PLX9050_PCI_INTR_EN_BIT,
			a_priv->plx_base + PLX9050_INTCSR_REG);
	}
	
	if(use_fifos)
	{
		writeb(ENABLE_BUFFER_END_EVENTS_BIT | ENABLE_TERM_COUNT_EVENTS_BIT,
			a_priv->gpib_base + EVENT_ENABLE_REG);
		writeb(ENABLE_TERM_COUNT_INTERRUPT_BIT | ENABLE_BUFFER_END_INTERRUPT_BIT | ENABLE_TMS9914_INTERRUPTS_BIT,
			a_priv->gpib_base + INTERRUPT_ENABLE_REG);
		//write-clear event status bits
		writeb(BUFFER_END_STATUS_BIT | TERM_COUNT_STATUS_BIT,
			a_priv->gpib_base + EVENT_STATUS_REG);
	}else
	{
		writeb(0, a_priv->gpib_base + EVENT_ENABLE_REG);
		writeb(ENABLE_TMS9914_INTERRUPTS_BIT,
			a_priv->gpib_base + INTERRUPT_ENABLE_REG);
	}
	board->t1_nano_sec = agilent_82350b_t1_delay(board, 2000);
	tms9914_board_reset(tms_priv);

	tms9914_online( board, tms_priv );

	return 0;
}

int agilent_82350b_unaccel_attach(gpib_board_t *board, gpib_board_config_t config)
{
	return agilent_82350b_generic_attach(board, &config, 0);
}

int agilent_82350b_accel_attach(gpib_board_t *board, gpib_board_config_t config)
{
	return agilent_82350b_generic_attach(board, &config, 1);
}

void agilent_82350b_detach(gpib_board_t *board)
{
	agilent_82350b_private_t *a_priv = board->private_data;
	tms9914_private_t *tms_priv;

	if(a_priv)
	{
		if (a_priv->plx_base) // disable interrupts
			writel(0, a_priv->plx_base + PLX9050_INTCSR_REG);

		tms_priv = &a_priv->tms9914_priv;
		if(a_priv->irq)
		{
			free_irq(a_priv->irq, board);
		}
		if(a_priv->gpib_base)
		{
			tms9914_board_reset(tms_priv);
			if(a_priv->misc_base) iounmap((void *)a_priv->misc_base);
			if(a_priv->borg_base) iounmap((void *)a_priv->borg_base);
			if(a_priv->sram_base) iounmap((void *)a_priv->sram_base);
			if(a_priv->gpib_base) iounmap((void *)a_priv->gpib_base);
			if(a_priv->plx_base) iounmap((void *)a_priv->plx_base);
			pci_release_regions(a_priv->pci_device);
		}
		if(a_priv->pci_device)
			pci_dev_put(a_priv->pci_device);
	}
	agilent_82350b_free_private( board );
}

static const struct pci_device_id agilent_82350b_pci_table[] =
{
	{ PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_9050, PCI_VENDOR_ID_HP,  PCI_SUBDEVICE_ID_82350A, 0, 0, 0 },
	{ PCI_VENDOR_ID_AGILENT, PCI_DEVICE_ID_82350B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, agilent_82350b_pci_table);

static int __init agilent_82350b_init_module( void )
{
	gpib_register_driver(&agilent_82350b_unaccel_interface, THIS_MODULE);
	gpib_register_driver(&agilent_82350b_interface, THIS_MODULE);
	return 0;
}

static void __exit agilent_82350b_exit_module( void )
{
	gpib_unregister_driver(&agilent_82350b_interface);
	gpib_unregister_driver(&agilent_82350b_unaccel_interface);
}

module_init( agilent_82350b_init_module );
module_exit( agilent_82350b_exit_module );

