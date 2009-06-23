/*
    tnt4882/mite.h
    Hardware driver for NI Mite PCI interface chip

    Copyright (C) 1999 David A. Schleef <ds@stm.lbl.gov>

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

#ifndef _MITE_H_
#define _MITE_H_

#include <linux/pci.h>

#define PCI_VENDOR_ID_NATINST		0x1093

//#define DEBUG_MITE

#ifdef DEBUG_MITE
#define MDPRINTK(format,args...)	printk(format , ## args )
#else
#define MDPRINTK(format,args...)
#endif

#define MITE_RING_SIZE 3000
struct mite_dma_chain{
	u32 count;
	u32 addr;
	u32 next;
};

struct mite_struct{
	struct mite_struct *next;
	int used;

	struct pci_dev *pcidev;
	unsigned long mite_phys_addr;
	void *mite_io_addr;
	unsigned long daq_phys_addr;
	void *daq_io_addr;

	int DMA_CheckNearEnd;

	struct mite_dma_chain ring[MITE_RING_SIZE];
};

extern struct mite_struct *mite_devices;

extern inline unsigned int mite_irq(struct mite_struct *mite)
{
	return mite->pcidev->irq;
};
extern inline unsigned int mite_device_id(struct mite_struct *mite)
{
	return mite->pcidev->device;
};
extern inline unsigned long mite_iobase(struct mite_struct *mite)
{
	return (unsigned long)mite->daq_io_addr;
};


void mite_init(void);
void mite_cleanup(void);
int mite_setup(struct mite_struct *mite);
void mite_unsetup(struct mite_struct *mite);
void mite_list_devices(void);

int mite_dma_tcr(struct mite_struct *mite);

void mite_dma_arm(struct mite_struct *mite);
void mite_dma_disarm(struct mite_struct *mite);

void mite_dump_regs(struct mite_struct *mite);
void mite_setregs(struct mite_struct *mite,unsigned long ll_start,int chan, int dir);
int mite_bytes_transferred(struct mite_struct *mite, int chan);

#define CHAN_OFFSET(x)			(0x100*(x))

/* DMA base for chan 0 is 0x500, chan 1 is 0x600 */

#define MITE_CHOR		0x500
#define CHOR_DMARESET			(1<<31)
#define CHOR_SET_SEND_TC		(1<<11)
#define CHOR_CLR_SEND_TC		(1<<10)
#define CHOR_SET_LPAUSE			(1<<9)
#define CHOR_CLR_LPAUSE			(1<<8)
#define CHOR_CLRDONE			(1<<7)
#define CHOR_CLRRB			(1<<6)
#define CHOR_CLRLC			(1<<5)
#define CHOR_FRESET			(1<<4)
#define CHOR_ABORT			(1<<3)
#define CHOR_STOP			(1<<2)
#define CHOR_CONT			(1<<1)
#define CHOR_START			(1<<0)
#define CHOR_PON			(CHOR_CLR_SEND_TC|CHOR_CLR_LPAUSE)

#define MITE_CHCR		0x504
#define CHCR_SET_DMA_IE			(1<<31)
#define CHCR_CLR_DMA_IE			(1<<30)
#define CHCR_SET_LINKP_IE		(1<<29)
#define CHCR_CLR_LINKP_IE		(1<<28)
#define CHCR_SET_SAR_IE			(1<<27)
#define CHCR_CLR_SAR_IE			(1<<26)
#define CHCR_SET_DONE_IE		(1<<25)
#define CHCR_CLR_DONE_IE		(1<<24)
#define CHCR_SET_MRDY_IE		(1<<23)
#define CHCR_CLR_MRDY_IE		(1<<22)
#define CHCR_SET_DRDY_IE		(1<<21)
#define CHCR_CLR_DRDY_IE		(1<<20)
#define CHCR_SET_LC_IE			(1<<19)
#define CHCR_CLR_LC_IE			(1<<18)
#define CHCR_SET_CONT_RB_IE		(1<<17)
#define CHCR_CLR_CONT_RB_IE		(1<<16)
#define CHCR_FIFODIS			(1<<15)
#define CHCR_FIFO_ON			0
#define CHCR_BURSTEN			(1<<14)
#define CHCR_NO_BURSTEN			0
#define CHCR_NFTP(x)			((x)<<11)
#define CHCR_NFTP0			CHCR_NFTP(0)
#define CHCR_NFTP1			CHCR_NFTP(1)
#define CHCR_NFTP2			CHCR_NFTP(2)
#define CHCR_NFTP4			CHCR_NFTP(3)
#define CHCR_NFTP8			CHCR_NFTP(4)
#define CHCR_NFTP16			CHCR_NFTP(5)
#define CHCR_NETP(x)			((x)<<11)
#define CHCR_NETP0			CHCR_NETP(0)
#define CHCR_NETP1			CHCR_NETP(1)
#define CHCR_NETP2			CHCR_NETP(2)
#define CHCR_NETP4			CHCR_NETP(3)
#define CHCR_NETP8			CHCR_NETP(4)
#define CHCR_CHEND1			(1<<5)
#define CHCR_CHEND0			(1<<4)
#define CHCR_DIR			(1<<3)
#define 	CHCR_DEV_TO_MEM		CHCR_DIR
#define 	CHCR_MEM_TO_DEV		0
#define CHCR_NORMAL			((0)<<0)
#define CHCR_CONTINUE			((1)<<0)
#define CHCR_RINGBUFF			((2)<<0)
#define CHCR_LINKSHORT			((4)<<0)
#define CHCR_LINKLONG			((5)<<0)
#define CHCRPON				(CHCR_CLR_DMA_IE | CHCR_CLR_LINKP_IE | CHCR_CLR_SAR_IE | CHCR_CLR_DONE_IE | CHCR_CLR_MRDY_IE | CHCR_CLR_DRDY_IE | CHCR_CLR_LC_IE | CHCR_CLR_CONT_IE)

#define MITE_TCR		0x508

/* CR bits */
#define CR_RL(x)			((x)<<21)
#define CR_RL0				CR_RL(0)
#define CR_RL1				CR_RL(1)
#define CR_RL2				CR_RL(2)
#define CR_RL4				CR_RL(3)
#define CR_RL8				CR_RL(4)
#define CR_RL16				CR_RL(5)
#define CR_RL32				CR_RL(6)
#define CR_RL64				CR_RL(7)
#define CR_RD(x)			((x)<<19)
#define CR_RD0				CR_RD(0)
#define CR_RD32				CR_RD(1)
#define CR_RD512			CR_RD(2)
#define CR_RD8192			CR_RD(3)
#define CR_REQS(x)			((x)<<16)
#define CR_REQSDRQ0			CR_REQS(4)
#define CR_REQSDRQ1			CR_REQS(5)
#define CR_REQSDRQ2			CR_REQS(6)
#define CR_REQSDRQ3			CR_REQS(7)
#define CR_ASEQx(x)			((x)<<10)
#define CR_ASEQx0			CR_ASEQx(0)
#define 	CR_ASEQDONT		CR_ASEQx0
#define CR_ASEQxP1			CR_ASEQx(1)
#define 	CR_ASEQUP		CR_ASEQxP1
#define CR_ASEQxP2			CR_ASEQx(2)
#define 	CR_ASEQDOWN		CR_ASEQxP2
#define CR_ASEQxP4			CR_ASEQx(3)
#define CR_ASEQxP8			CR_ASEQx(4)
#define CR_ASEQxP16			CR_ASEQx(5)
#define CR_ASEQxP32			CR_ASEQx(6)
#define CR_ASEQxP64			CR_ASEQx(7)
#define CR_ASEQxM1			CR_ASEQx(9)
#define CR_ASEQxM2			CR_ASEQx(10)
#define CR_ASEQxM4			CR_ASEQx(11)
#define CR_ASEQxM8			CR_ASEQx(12)
#define CR_ASEQxM16			CR_ASEQx(13)
#define CR_ASEQxM32			CR_ASEQx(14)
#define CR_ASEQxM64			CR_ASEQx(15)
#define CR_PSIZEBYTE			(1<<8)
#define CR_PSIZEHALF			(2<<8)
#define CR_PSIZEWORD			(3<<8)
#define CR_PORTCPU			(0<<6)
#define CR_PORTIO			(1<<6)
#define CR_PORTVXI			(2<<6)
#define CR_PORTMXI			(3<<6)
#define CR_AMDEVICE			(1<<0)

#define CHSR_INT			0x80000000
#define CHSR_DONE			0x02000000
#define CHSR_LINKC			0x00080000

#define MITE_MCR		0x50c
#define	MCRPON				0

#define MITE_MAR		0x510

#define MITE_DCR		0x514
#define DCR_NORMAL			(1<<29)
#define DCRPON				0

#define MITE_DAR		0x518

#define MITE_LKCR		0x51c

#define MITE_LKAR		0x520
#define MITE_LLKAR		0x524
#define MITE_BAR		0x528
#define MITE_BCR		0x52c
#define MITE_SAR		0x530
#define MITE_WSCR		0x534
#define MITE_WSER		0x538
#define MITE_CHSR		0x53c
#define MITE_FCR		0x540

#define MITE_FIFO		0x80
#define MITE_FIFOEND		0xff

#define MITE_AMRAM		0x00
#define MITE_AMDEVICE		0x01
#define MITE_AMHOST_A32_SINGLE	0x09
#define MITE_AMHOST_A24_SINGLE	0x39
#define MITE_AMHOST_A16_SINGLE	0x29
#define MITE_AMHOST_A32_BLOCK	0x0b
#define MITE_AMHOST_A32D64_BLOCK	0x08
#define MITE_AMHOST_A24_BLOCK	0x3b

#endif

