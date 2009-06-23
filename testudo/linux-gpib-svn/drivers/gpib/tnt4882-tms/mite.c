/*
	tnt4882/mite.c
	Hardware driver for NI Mite PCI interface chip,
	adapted from COMEDI

	Copyright (C) 1997-8 David A. Schleef <ds@stm.lbl.gov>
	Copyright (C) 2002 Frank Mori Hess <fmhess@users.sourceforge.net>

	The PCI-MIO E series driver was originally written by
	Tomasz Motylewski <...>, and ported to comedi by ds.

	References for specifications:

	   321747b.pdf  Register Level Programmer Manual (obsolete)
	   321747c.pdf  Register Level Programmer Manual (new)
	   DAQ-STC reference manual

	Other possibly relevant info:

	   320517c.pdf  User manual (obsolete)
	   320517f.pdf  User manual (new)
	   320889a.pdf  delete
	   320906c.pdf  maximum signal ratings
	   321066a.pdf  about 16x
	   321791a.pdf  discontinuation of at-mio-16e-10 rev. c
	   321808a.pdf  about at-mio-16e-10 rev P
	   321837a.pdf  discontinuation of at-mio-16de-10 rev d
	   321838a.pdf  about at-mio-16de-10 rev N

***********************************************************************

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <linux/slab.h>

#include "mite.h"


#define PCI_MITE_SIZE		4096
#define PCI_DAQ_SIZE		4096


struct mite_struct *mite_devices = NULL;
	
#define TOP_OF_PAGE(x) ((x)|(~(PAGE_MASK)))

void mite_init(void)
{
	struct pci_dev *pcidev;
	struct mite_struct *mite;

	pci_for_each_dev(pcidev){
		if(pcidev->vendor==PCI_VENDOR_ID_NATINST){
			mite=kmalloc(sizeof(*mite),GFP_KERNEL);
			if(!mite){
				printk("mite: allocation failed\n");
				return;
			}
			memset(mite,0,sizeof(*mite));

			mite->pcidev=pcidev;

			mite->next=mite_devices;
			mite_devices=mite;
		}
	}
}

int mite_setup(struct mite_struct *mite)
{
	unsigned long			start, length;
	u32				addr;

	if(pci_enable_device(mite->pcidev)){
		printk("error enabling mite\n");
		return -EIO;
	}
	pci_set_master(mite->pcidev);
	addr=pci_resource_start(mite->pcidev, 0);
	mite->mite_phys_addr=addr;
	start = mite->mite_phys_addr & PCI_BASE_ADDRESS_MEM_MASK;
	length = pci_resource_len(mite->pcidev, 0);
// kernel automatically allocates regions for PCI-GPIB for some reason
	// check and request io memory region
	if(check_mem_region(start, length)){

		printk("io memory region already in use\n");
		return -EIO;
	}
	request_mem_region(start, length, "mite");
	mite->mite_io_addr = ioremap(start, length);
	printk("MITE:0x%08lx mapped to %p ",mite->mite_phys_addr,mite->mite_io_addr);

	addr=pci_resource_start(mite->pcidev, 1);
	mite->daq_phys_addr=addr;
	start = mite->daq_phys_addr & PCI_BASE_ADDRESS_MEM_MASK;
	length = pci_resource_len(mite->pcidev, 1);
	// check and request io memory region
	if(check_mem_region(start, length)){

		printk("io memory region already in use\n");
		return -EIO;
	}
	request_mem_region(start, length, "mite (gpib)");
	mite->daq_io_addr = ioremap(start, length);
	printk("DAQ:0x%08lx mapped to %p\n", mite->daq_phys_addr,
		mite->daq_io_addr);

	/* XXX don't know what the 0xc0 and 0x80 mean */
	/* It must be here for the driver to work though */
	writel(mite->daq_phys_addr | 0x80 , mite->mite_io_addr + 0xc0 );

	mite->used = 1;

	return 0;
}


void mite_cleanup(void)
{
	struct mite_struct *mite,*next;

	for(mite=mite_devices;mite;mite=next){
		next=mite->next;
		kfree(mite);
	}
}

void mite_unsetup(struct mite_struct *mite)
{
	unsigned long start, length;

	if(!mite)return;

	if(mite->mite_io_addr){
		iounmap(mite->mite_io_addr);
		mite->mite_io_addr=NULL;
		// release io memory region
		start = mite->mite_phys_addr & PCI_BASE_ADDRESS_MEM_MASK;
		length = pci_resource_len(mite->pcidev, 0);
		release_mem_region(start, length);
	}
	if(mite->daq_io_addr){
		iounmap(mite->daq_io_addr);
		mite->daq_io_addr=NULL;
		// release io memory region
		start = mite->daq_phys_addr & PCI_BASE_ADDRESS_MEM_MASK;
		length = pci_resource_len(mite->pcidev, 1);
		release_mem_region(start, length);
	}

	mite->used = 0;
}


void mite_list_devices(void)
{
	struct mite_struct *mite,*next;

	printk("Available NI device IDs:");
	if(mite_devices)for(mite=mite_devices;mite;mite=next){
		next=mite->next;
		printk(" 0x%04x",mite_device_id(mite));
		if(mite->used)printk("(used)");
	}
	printk("\n");

}

int mite_bytes_transferred(struct mite_struct *mite, int chan)
{
	int dar, fcr;
	
	dar = readl(mite->mite_io_addr+MITE_DAR+CHAN_OFFSET(chan));
	fcr = readl(mite->mite_io_addr+MITE_FCR+CHAN_OFFSET(chan)) & 0x000000FF;
	return dar-fcr;
}

int mite_dma_tcr(struct mite_struct *mite)
{
	int tcr;
	int lkar;

	lkar=readl(mite->mite_io_addr+CHAN_OFFSET(0)+MITE_LKAR);
	tcr=readl(mite->mite_io_addr+CHAN_OFFSET(0)+MITE_TCR);
	MDPRINTK("lkar=0x%08x tcr=%d\n",lkar,tcr);

	return tcr;
}

void mite_dma_disarm(struct mite_struct *mite)
{
	int chor;

	/* disarm */
	chor = CHOR_ABORT;
	writel(chor,mite->mite_io_addr+CHAN_OFFSET(0)+MITE_CHOR);
}

void mite_dump_regs(struct mite_struct *mite)
{
	unsigned long mite_io_addr = (unsigned long) mite->mite_io_addr;
	unsigned long addr=0;
	unsigned long temp=0;

	printk("mite address is  =0x%08lx\n",mite_io_addr);
		
	addr = mite_io_addr+MITE_CHOR+CHAN_OFFSET(0);
	printk("mite status[CHOR]at 0x%08lx =0x%08lx\n",addr, temp=readl(addr));
	//mite_decode(mite_CHOR_strings,temp);
	addr = mite_io_addr+MITE_CHCR+CHAN_OFFSET(0);
	printk("mite status[CHCR]at 0x%08lx =0x%08lx\n",addr, temp=readl(addr));
	//mite_decode(mite_CHCR_strings,temp);
	addr = mite_io_addr+MITE_TCR+CHAN_OFFSET(0);
	printk("mite status[TCR] at 0x%08lx =0x%08x\n",addr, readl(addr));
	addr = mite_io_addr+MITE_MCR+CHAN_OFFSET(0);
	printk("mite status[MCR] at 0x%08lx =0x%08lx\n",addr, temp=readl(addr));
	//mite_decode(mite_MCR_strings,temp);
	
	addr = mite_io_addr+MITE_MAR+CHAN_OFFSET(0);
	printk("mite status[MAR] at 0x%08lx =0x%08x\n",addr, readl(addr));
	addr = mite_io_addr+MITE_DCR+CHAN_OFFSET(0);
	printk("mite status[DCR] at 0x%08lx =0x%08lx\n",addr, temp=readl(addr));
	//mite_decode(mite_CR_strings,temp);
	addr = mite_io_addr+MITE_DAR+CHAN_OFFSET(0);
	printk("mite status[DAR] at 0x%08lx =0x%08x\n",addr, readl(addr));
	addr = mite_io_addr+MITE_LKCR+CHAN_OFFSET(0);
	printk("mite status[LKCR]at 0x%08lx =0x%08lx\n",addr, temp=readl(addr));
	//mite_decode(mite_CR_strings,temp);
	addr = mite_io_addr+MITE_LKAR+CHAN_OFFSET(0);
	printk("mite status[LKAR]at 0x%08lx =0x%08x\n",addr, readl(addr));

	addr = mite_io_addr+MITE_CHSR+CHAN_OFFSET(0);
	printk("mite status[CHSR]at 0x%08lx =0x%08lx\n",addr, temp=readl(addr));
	//mite_decode(mite_CHSR_strings,temp);
	addr = mite_io_addr+MITE_FCR+CHAN_OFFSET(0);
	printk("mite status[FCR] at 0x%08lx =0x%08x\n\n",addr, readl(addr));
}

