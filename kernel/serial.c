/*
 * mahatma - a simple kernel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <guilib.h>
#include <wikilib.h>
#include <input.h>

#include <msg.h>
#include "regs.h"
#include "touchscreen.h"
#include "serial.h"
#include "irq.h"


static u8 console_buffer[16];
static unsigned int console_read;
static unsigned int console_write;

serial_buffer_type *send_queue_head;
serial_buffer_type *send_queue_tail;
const char *transmit;
volatile bool sending;
volatile bool linefeed;


#define BUFFER_FULL(w, r, b) ((((w) + 1) % ARRAY_SIZE(b)) == (r))
#define BUFFER_EMPTY(w, r, b) ((w) == (r))
#define BUFFER_NEXT(p, b) (p) = (((p) + 1) % ARRAY_SIZE(b))

// Bits for: REG_INT_ESIF01
#define ESTX1  (1 << 5)
#define ESRX1  (1 << 4)
#define ESERR1 (1 << 3)
#define ESTX0  (1 << 2)
#define ESRX0  (1 << 1)
#define ESERR0 (1 << 0)

// Bits for: REG_INT_FSIF01
#define FSTX1  (1 << 5)
#define FSRX1  (1 << 4)
#define FSERR1 (1 << 3)
#define FSTX0  (1 << 2)
#define FSRX0  (1 << 1)
#define FSERR0 (1 << 0)

// Bits for: REG_EFSIFx_STATUS
#define RXDxNUM1 (1 << 7)
#define RXDxNUM0 (1 << 6)
#define TENDx    (1 << 5)
#define FERx     (1 << 4)
#define PERx     (1 << 3)
#define OERx     (1 << 2)
#define TDBEx    (1 << 1)
#define RDBFx    (1 << 0)

void serial_init(void)
{
  static bool initialised = false;
  if (!initialised)
  {
    //REG_INT_ESIF01 = 0x36;
    //REG_INT_ESIF01 = 0x6;
    REG_INT_ESIF01 = ESRX0;

    REG_INT_PLCDC_PSIO0 = 0x70;
    console_read = 0;
    console_write = 0;
    send_queue_head = NULL;
    send_queue_tail = NULL;
    sending = false;
    linefeed = false;

    initialised = true;
  }
}


bool serial_output_pending(void)
{
  return sending || (0 != (REG_EFSIF0_STATUS & TENDx));
}


void serial_put(serial_buffer_type *buffer)
{
  if (NULL != buffer)
  {
    // critical code - disable uart tx interrupts
    REG_INT_ESIF01 &= ~ESTX0;
    buffer->link = NULL;
    if (NULL != send_queue_tail)
    {
      send_queue_tail->link = buffer;
    }
    send_queue_tail = buffer;
    if (NULL == send_queue_head)
    {
      send_queue_head = send_queue_tail;
      transmit = send_queue_head->text;
      REG_INT_FSIF01 = FSTX0;
      {
        u8 c = *transmit++;
        if ('\n' == c)
        {
          c = '\r';
          linefeed = true;
        }
        REG_EFSIF0_TXD = c;
        sending = true;
      }

    }
    REG_INT_ESIF01 |= ESTX0;
  }
}


void serial_drained_0(void)
{
  if (!linefeed && '\0' == *transmit)
  {
    serial_buffer_type *p = send_queue_head;
    send_queue_head = send_queue_head->link;
    if (NULL != p->callback)
    {
      p->callback(p);
    }
    if (NULL == send_queue_head)
    {
      transmit = NULL;
      send_queue_tail = NULL;
    }
    else
    {
      transmit = send_queue_head->text;
    }
  }
  if (NULL == transmit)
  {
    REG_INT_ESIF01 &= ~ESTX0;
    sending = false;
  }
  else
  {
    REG_INT_FSIF01 |= FSTX0;
    if (linefeed)
    {
      linefeed = false;
      REG_EFSIF0_TXD = '\n';
    }
    else
    {
      u8 c = *transmit++;
      if ('\n' == c)
      {
        c = '\r';
        linefeed = true;
      }
      REG_EFSIF0_TXD = c;
    }
  }
}


void serial_filled_0(void)
{
	while (REG_EFSIF0_STATUS & 0x1) {
		u8 c = REG_EFSIF0_RXD;
		if (c == 0)
			continue;

		console_buffer[console_write] = c;
		console_write++;
                if (console_write >= sizeof(console_buffer))
                {
                  console_write  = 0;
                }
	}
}

void serial_out(int port, char c)
{
	if (port != 0)
		return;

        DISABLE_IRQ();
	REG_EFSIF0_TXD = c;
        ENABLE_IRQ();
}

int serial_get_event(struct wl_input_event *ev)
{
	if (console_read == console_write)
		return 0;

//	msg(MSG_INFO, " OUT. %d %d    %p %p\n", console_read, console_write,
//		ev, &ev->type);

	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = console_buffer[console_read];
	ev->key_event.value = 1;
	console_read = (console_read + 1) % ARRAY_SIZE(console_buffer);

	/* Override for scrolling... */
	if (ev->key_event.keycode == KEY_PLUS) {
		ev->type = WL_INPUT_EV_TYPE_CURSOR;
		ev->key_event.keycode = WL_INPUT_KEY_CURSOR_DOWN;
	} else if (ev->key_event.keycode == KEY_MINUS) {
		ev->type = WL_INPUT_EV_TYPE_CURSOR;
		ev->key_event.keycode = WL_INPUT_KEY_CURSOR_UP;
	}

	return 1;
}

