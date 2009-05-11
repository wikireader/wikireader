/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
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

/*
 *  ターゲットシステム依存モジュール(LUXUN2)
 */

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */

#include "sys_rename.h"

/*
 *  処理時間測定用サービスコールをサポートしない
 */
#define OMIT_VGET_TIM

/*
 *  起動メッセージのターゲット名
 */
#define TARGET_NAME	"EPSON S1C33 Standard Macro"

/*
 *  サポートするシリアルディバイスの数は最大 1
 */
#define TNUM_PORT		1
#define LOGTASK_PORTID		1

/*
 *  シリアル割り込みが入力/出力で異なるかどうかの定義
 */
#define SEPARATE_SIO_INT

#include "s1c33.h"
#include "luxun2.h"

#ifndef _MACRO_ONLY

/*
 *  ターゲットシステム依存の初期化
 */
extern void sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．
 */
extern void sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．
 *
 */
extern void sys_putc(char c);

/*
 *  割込み制御レジスタのオフセット位置取得
 */
/*
 *  割り込みプライオリティレジスタ位置特定処理
 */
Inline ER
get_Offset(INTNO intno)
{
	INT		iOffset;
	switch(intno)
	{
		case S1C33_INHNO_PINT0:			/* Key, Port0-3 group */
		case S1C33_INHNO_PINT1:
		case S1C33_INHNO_PINT2:
		case S1C33_INHNO_PINT3:
		case S1C33_INHNO_KINT0:
		case S1C33_INHNO_KINT1:
			iOffset = 0;
			break;

		case S1C33_INHNO_HSDMA0:		/* DMA group */
		case S1C33_INHNO_HSDMA1:
		case S1C33_INHNO_HSDMA2:
		case S1C33_INHNO_HSDMA3:
		case S1C33_INHNO_IDMA:
			iOffset = 1;
			break;

		case S1C33_INHNO_P16TIMER0B:		/* 16bit timer0/1 group */
		case S1C33_INHNO_P16TIMER0A:
		case S1C33_INHNO_P16TIMER1B:
		case S1C33_INHNO_P16TIMER1A:
			iOffset = 2;
			break;

		case S1C33_INHNO_P16TIMER2B:		/* 16bit timer2/3 group */
		case S1C33_INHNO_P16TIMER2A:
		case S1C33_INHNO_P16TIMER3B:
		case S1C33_INHNO_P16TIMER3A:
			iOffset = 3;
			break;

		case S1C33_INHNO_P16TIMER4B:		/* 16bit timer4/5 group */
		case S1C33_INHNO_P16TIMER4A:
		case S1C33_INHNO_P16TIMER5B:
		case S1C33_INHNO_P16TIMER5A:
			iOffset = 4;
			break;

		case S1C33_INHNO_P8TIMER0:		/* 8bit timer group */
		case S1C33_INHNO_P8TIMER1:
		case S1C33_INHNO_P8TIMER2:
		case S1C33_INHNO_P8TIMER3:
			iOffset = 5;
			break;

		case S1C33_INHNO_SERIAL0ERR:		/* Serial I/O group */
		case S1C33_INHNO_SERIAL0RX:
		case S1C33_INHNO_SERIAL0TX:
		case S1C33_INHNO_SERIAL1ERR:
		case S1C33_INHNO_SERIAL1RX:
		case S1C33_INHNO_SERIAL1TX:
			iOffset = 6;
			break;

		case S1C33_INHNO_AD:			/* A/DConv, clocktimer, */
		case S1C33_INHNO_CLKTIMER:		/* Port 4-7 group	*/
		case S1C33_INHNO_PINT4:
		case S1C33_INHNO_PINT5:
		case S1C33_INHNO_PINT6:
		case S1C33_INHNO_PINT7:
			iOffset = 7;
			break;

		default:
			iOffset = E_PAR;
	}

	return (iOffset);
}

/*
 *  割り込み制御ビット位置特定処理
 */
Inline ER
get_BitFlag(INTNO intno)
{
	INT	iBitFlag;

	switch(intno)
	{
		case S1C33_INHNO_PINT0:			/* D0 group*/
		case S1C33_INHNO_HSDMA0:
		case S1C33_INHNO_P8TIMER0:
		case S1C33_INHNO_SERIAL0ERR:
		case S1C33_INHNO_AD:
			iBitFlag = 0x01;
			break;

		case S1C33_INHNO_PINT1:			/* D1 group*/
		case S1C33_INHNO_HSDMA1:
		case S1C33_INHNO_P8TIMER1:
		case S1C33_INHNO_SERIAL0RX:
		case S1C33_INHNO_CLKTIMER:
			iBitFlag = 0x02;
			break;

		case S1C33_INHNO_PINT2:			/* D2 group*/
		case S1C33_INHNO_HSDMA2:
		case S1C33_INHNO_P16TIMER0B:
		case S1C33_INHNO_P16TIMER2B:
		case S1C33_INHNO_P16TIMER4B:
		case S1C33_INHNO_P8TIMER2:
		case S1C33_INHNO_SERIAL0TX:
		case S1C33_INHNO_PINT4:
			iBitFlag = 0x04;
			break;

		case S1C33_INHNO_PINT3:			/* D3 group*/
		case S1C33_INHNO_HSDMA3:
		case S1C33_INHNO_P16TIMER0A:
		case S1C33_INHNO_P16TIMER2A:
		case S1C33_INHNO_P16TIMER4A:
		case S1C33_INHNO_P8TIMER3:
		case S1C33_INHNO_SERIAL1ERR:
		case S1C33_INHNO_PINT5:
			iBitFlag = 0x08;
			break;

		case S1C33_INHNO_KINT0:			/* D4 group*/
		case S1C33_INHNO_IDMA:
		case S1C33_INHNO_SERIAL1RX:
		case S1C33_INHNO_PINT6:
			iBitFlag = 0x10;
			break;

		case S1C33_INHNO_KINT1:			/* D5 group*/
		case S1C33_INHNO_SERIAL1TX:
		case S1C33_INHNO_PINT7:
			iBitFlag = 0x20;
			break;

		case S1C33_INHNO_P16TIMER1B:		/* D6 group*/
		case S1C33_INHNO_P16TIMER3B:
		case S1C33_INHNO_P16TIMER5B:
			iBitFlag = 0x40;
			break;

		case S1C33_INHNO_P16TIMER1A:		/* D7 group*/
		case S1C33_INHNO_P16TIMER3A:
		case S1C33_INHNO_P16TIMER5A:
			iBitFlag = 0x80;
			break;

		default:
			iBitFlag = E_PAR;
	}

	return (iBitFlag);
}

#endif /* _MACRO_ONLY */
#endif /* _SYS_CONFIG_H_ */
