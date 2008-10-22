/*
 *  keyboard input routines for TOPPERS/JSP Kernel
 *  Copyright (C) 2008 Daniel Mack <daniel@caiaq.de>
 *
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 * 
 */

#include <s1c33.h>
#include <t_services.h>
#include <string.h>

#include "kernel_id.h"
#include "sample1.h"
#include "keyboard.h"

#define MAXKEYS 256

static unsigned char current_state[MAXKEYS/8];


static void key_event(int key, int state)
{
	syslog(LOG_INFO, "KEY %03d -> %d", key, state);
}

static void scan_matrix(void)
{
	unsigned char row, col, diff, i;
	volatile s1c33PEPort_t *portbase = ((volatile s1c33PEPort_t *) S1C33_PORT_BASE);

	for (row = 1; row < 7; row++) {
		unsigned char mask = 1 << row;

		/* only drive one row at a time */
		*(IOREG *) (S1C33_EXTPORT_BASE + 0) = (mask & 0x1f);
		*(IOREG *) (S1C33_EXTPORT_BASE + 2) = (mask & 0xe0) >> 5;

	        *(IOREG *) (S1C33_EXTPORT_BASE + 1) = ~(mask & 0x1f);
	        *(IOREG *) (S1C33_EXTPORT_BASE + 3) = ~(mask & 0xe0) >> 5;

		/* and read the input columns */
		col = (portbase->stPPort[0].bData & 0xf0)
		    | (portbase->stPPort[6].bData & 0x0f);

		diff = current_state[row] ^ col;
		if (!diff)
			continue;

		//syslog(LOG_NOTICE, " ---> row %d diff %02x\n", row, diff);

		for (i = 0; i < 8; i++)
			if ((diff >> (7 - i)) & 1)
				key_event(row * 8 + i,
					  (~col >> (7 - i) & 1));

		current_state[row] = col;
	}

	/* drive all rows low again */
	*(IOREG *) (S1C33_EXTPORT_BASE + 0) = 0x1f;
	*(IOREG *) (S1C33_EXTPORT_BASE + 2) = 0x0f;
	*(IOREG *) (S1C33_EXTPORT_BASE + 1) = 0;
	*(IOREG *) (S1C33_EXTPORT_BASE + 3) = 0;
}

/* おしょくじをおたのしみください！ */
void kint0_handler(void)
{
	volatile s1c33PEPort_t *portbase = ((volatile s1c33PEPort_t *) S1C33_PORT_BASE);
	portbase->bScpk[0] = portbase->stPPort[6].bData & 0xf;
	syslog(LOG_NOTICE, "%s()", __func__);
	scan_matrix();
	clr_int(S1C33_INHNO_KINT0);
}

void kint1_handler(void)
{
	volatile s1c33PEPort_t *portbase = ((volatile s1c33PEPort_t *) S1C33_PORT_BASE);
	portbase->bScpk[1] = portbase->stPPort[0].bData >> 4;
	syslog(LOG_NOTICE, "%s()", __func__);
	scan_matrix();
	clr_int(S1C33_INHNO_KINT1);
}

void keyboard_init(void)
{
	volatile s1c33PEPort_t *portbase = ((volatile s1c33PEPort_t *) S1C33_PORT_BASE);

	syslog(LOG_NOTICE, "%s()", __func__);
	memset(current_state, 0xff, sizeof(current_state));

	/* SSPK1[2:0] = 000 (P0[7:4])
	 * SSPK0[2:0] = 100 (P6[4:0]) */
	portbase->bSppk01 = 0x04;

	/* interrupt input mask */
	portbase->bSmpk[0] = 0x0f;
	portbase->bSmpk[1] = 0x0f;

	/* compare register: all 1 */
	portbase->bScpk[0] = 0x0f;
	portbase->bScpk[1] = 0x0f;

	/* ROWS: set port functions for PA[0:4] and PB[0:3] to output */
	*(IOREG *) (S1C33_EXTPORT_BASE + 0) = 0x1f;
	*(IOREG *) (S1C33_EXTPORT_BASE + 2) = 0x0f;

	/* ... and drive them low */
	*(IOREG *) (S1C33_EXTPORT_BASE + 1) = 0;
	*(IOREG *) (S1C33_EXTPORT_BASE + 3) = 0;
	
	/* COLUMNS: set port functions for P0[4:7] and P6[0:3] to input */
	portbase->stFuncSelect[0].bCfp47 = 0;
	portbase->stFuncSelect[6].bCfp03 = 0;
	portbase->stPPort[0].bControl &= 0x0f;
	portbase->stPPort[6].bControl &= 0xf0;

	/* enable interrupts */
	ena_int(S1C33_INHNO_KINT0);
	ena_int(S1C33_INHNO_KINT1);
}

