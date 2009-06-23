/***************************************************************************
                              lib/ibConf.h
                             -------------------

    copyright            : (C) 2001,2002 by Frank Mori Hess
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

#ifndef _IBCONF_H
#define _IBCONF_H

#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

/* meaning for flags */

#define CN_SDCL    (1<<1)             /* Send DCL on init                */
#define CN_SLLO    (1<<2)             /* Send LLO on init                */
#define CN_NETWORK (1<<3)             /* is a network device             */
#define CN_AUTOPOLL (1<<4)            /* Auto serial poll devices        */
#define CN_EXCLUSIVE (1<<5)           /* Exclusive use only */

/*---------------------------------------------------------------------- */

struct async_operation
{
	pthread_t thread;	/* thread used for asynchronous io operations */
	pthread_mutex_t lock;
	pthread_mutex_t join_lock;
	pthread_cond_t condition;
	uint8_t *buffer;
	volatile long buffer_length;
	volatile int iberr;
	volatile int ibsta;
	volatile long ibcntl;
	volatile short in_progress;
	volatile short abort;
};

typedef struct
{
	int pad;	/* device primary address */
	int sad;	/* device secodnary address (negative disables) */
	int board;	/* board index */
	unsigned int usec_timeout;
	unsigned int spoll_usec_timeout;
	unsigned int ppoll_usec_timeout;
	char eos;                           /* eos character */
	int eos_flags;
	int ppoll_config;	/* current parallel poll configuration */
	unsigned send_eoi : 1;	/* assert EOI at end of writes */
	unsigned local_lockout : 1;	/* send local lockout when device is brought online */
	unsigned local_ppc : 1;	/* enable local configuration of board's parallel poll response */
	unsigned readdr : 1;	/* useless, exists for compatibility only at present */
}descriptor_settings_t;

typedef struct ibConfStruct
{
	int handle;
	char name[100];		/* name of the device (for ibfind())     */
	descriptor_settings_t defaults;	/* initial settings stored so ibonl() can restore them */
	descriptor_settings_t settings;	/* various software settings for this descriptor */
	char init_string[100];               /* initialization string (optional) */
	int flags;                         /* some flags, deprecated          */
	struct async_operation async;	/* used by asynchronous operations ibcmda(), ibrda(), etc. */
	unsigned end : 1;	/* EOI asserted or EOS received at end of IO operation */
	unsigned is_interface : 1;	/* is interface board */
	unsigned board_is_open : 1;
	unsigned has_lock : 1;
	unsigned timed_out : 1;		/* io operation timed out */
} ibConf_t;

/*---------------------------------------------------------------------- */

typedef struct ibBoardStruct {
	char board_type[100];	/* name (model) of interface board */
	unsigned long base;                          /* base configuration */
	unsigned int irq;
	unsigned int dma;
	int pci_bus;
	int pci_slot;
	int fileno;                        /* device file descriptor           */
	char device[100];	/* name of device file ( /dev/gpib0, etc.) */
	unsigned int open_count;	/* reference count */
	unsigned is_system_controller : 1;	/* board is busmaster or not */
	unsigned use_event_queue : 1;	/* use event queue, or DTAS/DCAS */
	unsigned autospoll : 1; /* do auto serial polling */
} ibBoard_t;

#endif	/* _IBCONF_H */

















