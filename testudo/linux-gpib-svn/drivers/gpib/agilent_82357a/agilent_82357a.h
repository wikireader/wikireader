/***************************************************************************
                              ni_usb_gpib.h
                             -------------------

    begin                : Oct 2004
    copyright            : (C) 2004 by Frank Mori Hess
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

#ifndef _AGILENT_82357_H
#define _AGILENT_82357_H

#include <linux/usb.h>
#include <linux/timer.h>
#include "gpibP.h"
#include "tms9914.h"

enum usb_vendor_ids
{
	USB_VENDOR_ID_AGILENT = 0x0957
};

enum usb_device_ids
{
	USB_DEVICE_ID_AGILENT_82357A = 0x0107,
	USB_DEVICE_ID_AGILENT_82357A_PREINIT = 0x0007,	// device id before firmware is loaded
	USB_DEVICE_ID_AGILENT_82357B = 0x0718,	// device id before firmware is loaded
	USB_DEVICE_ID_AGILENT_82357B_PREINIT = 0x0518,	// device id before firmware is loaded
};

enum endpoint_addresses
{
	AGILENT_82357_CONTROL_ENDPOINT = 0x0,
	AGILENT_82357_BULK_IN_ENDPOINT = 0x2,
	AGILENT_82357A_BULK_OUT_ENDPOINT = 0x4,
	AGILENT_82357A_INTERRUPT_IN_ENDPOINT = 0x6,
	AGILENT_82357B_BULK_OUT_ENDPOINT = 0x6,
	AGILENT_82357B_INTERRUPT_IN_ENDPOINT = 0x8,
};

enum bulk_commands
{
	DATA_PIPE_CMD_WRITE = 0x1,
	DATA_PIPE_CMD_READ = 0x3,
	DATA_PIPE_CMD_WR_REGS = 0x4,
	DATA_PIPE_CMD_RD_REGS = 0x5
};

enum agilent_82357a_read_flags
{
	ARF_END_ON_EOI = 0x1,
	ARF_NO_ADDRESS = 0x2,
	ARF_END_ON_EOS_CHAR = 0x4,
	ARF_SPOLL = 0x8
};

enum agilent_82357a_trailing_read_flags
{
	ATRF_EOI = 0x1,
	ATRF_ATN = 0x2,
	ATRF_IFC = 0x4,
	ATRF_EOS = 0x8,
	ATRF_ABORT = 0x10,
	ATRF_COUNT = 0x20,
	ATRF_DEAD_BUS = 0x40,
	ATRF_UNADDRESSED = 0x80
};

enum agilent_82357a_write_flags
{
	AWF_SEND_EOI = 0x1,
	AWF_NO_FAST_TALKER_FIRST_BYTE = 0x2,
	AWF_NO_FAST_TALKER = 0x4,
	AWF_NO_ADDRESS = 0x8,
	AWF_ATN = 0x10,
	AWF_SEPARATE_HEADER = 0x80
};
                        
enum agilent_82357a_interrupt_flag_bit_numbers
{
	AIF_SRQ_BN = 0,
	AIF_WRITE_COMPLETE_BN = 1,
	AIF_READ_COMPLETE_BN = 2,
};

enum agilent_82357_error_codes
{
	UGP_SUCCESS = 0,
	UGP_ERR_INVALID_CMD = 1,
	UGP_ERR_INVALID_PARAM = 2,
	UGP_ERR_INVALID_REG = 3,
	UGP_ERR_GPIB_READ = 4,
	UGP_ERR_GPIB_WRITE = 5,
	UGP_ERR_FLUSHING = 6,
	UGP_ERR_FLUSHING_ALREADY = 7,
	UGP_ERR_UNSUPPORTED = 8,
	UGP_ERR_OTHER  = 9
};

enum agilent_82357_control_values
{
	XFER_ABORT = 0xa0,
	XFER_STATUS = 0xb0,
};

enum xfer_status_bits
{
	XS_COMPLETED = 0x1,
	XS_READ = 0x2,
};

enum xfer_status_completion_bits
{
	XSC_EOI = 0x1,
	XSC_ATN = 0x2,
	XSC_IFC = 0x4,
	XSC_EOS = 0x8,
	XSC_ABORT = 0x10,
	XSC_COUNT = 0x20,
	XSC_DEAD_BUS = 0x40,
	XSC_BUS_NOT_ADDRESSED = 0x80
};

enum xfer_abort_type
{
	XA_FLUSH = 0x1
};

// struct which defines local data for each 82357 device
typedef struct
{
	struct usb_interface *bus_interface;
	unsigned short eos_char;
	unsigned short eos_mode;
	unsigned short hw_control_bits;
	unsigned long interrupt_flags;
	struct urb *bulk_urb;
	struct urb *interrupt_urb;
	uint8_t interrupt_buffer[0x8];	
	struct semaphore bulk_transfer_lock;
	struct semaphore bulk_alloc_lock;
	struct semaphore interrupt_alloc_lock;
	struct semaphore control_alloc_lock;
	unsigned bulk_out_endpoint;
	unsigned interrupt_in_endpoint;
	unsigned is_cic : 1;
} agilent_82357a_private_t;

typedef struct
{
	struct semaphore complete;
	unsigned timed_out : 1;
} agilent_82357a_urb_context_t;

struct agilent_82357a_register_pairlet
{
	short address;
	unsigned short value;
};

enum firmware_registers
{
	HW_CONTROL = 0xa,
	LED_CONTROL = 0xb,
	RESET_TO_POWERUP = 0xc,
	PROTOCOL_CONTROL = 0xd,
	FAST_TALKER_T1 = 0xe
};

enum hardware_control_bits
{
	NOT_TI_RESET = 0x1,
	SYSTEM_CONTROLLER = 0x2,
	NOT_PARALLEL_POLL = 0x4,
	OSCILLATOR_5V_ON = 0x8,
	OUTPUT_5V_ON = 0x20,
	CPLD_3V_ON = 0x80,
};

enum led_control_bits
{
	FIRMWARE_LED_CONTROL = 0x1,
	FAIL_LED_ON = 0x20,
	READY_LED_ON = 0x40,
	ACCESS_LED_ON = 0x80
};

enum reset_to_powerup_bits
{
	RESET_SPACEBALL = 0x1,	// wait 2 millisec after sending
};

enum protocol_control_bits
{
	WRITE_COMPLETE_INTERRUPT_EN = 0x1,
};

static const int agilent_82357a_control_request = 0x4;

#endif	// _AGILENT_82357_H
