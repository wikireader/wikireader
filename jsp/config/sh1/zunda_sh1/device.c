/*
 *  TOPPERS/JSP Rose-RT Link Programs
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by CHUO ELECTRIC WORKS Co.,LTD. JAPAN
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 */
/*
 *  (株)中央製作所製μITRON搭載SH1CPUボード用
 *  LED,SW 制御関数
 */
#include <s_services.h>
#include <sh1_sil.h>
#include "zunda_sh1.h"
#include "device.h"

/*
 *  LEDの初期化
 */
void
initial_led(void)
{
	set_led( LED1, OFF );
	set_led( LED2, OFF );
	set_led( LED3, OFF );
}

/*
 *  LEDの設定を行う
 *  arg1: LED1 - 3
 *  arg2: ON|OFF
 */
void
set_led( int led, int req )
{
	if ( req != ON && req != OFF ) {
		return;
	}
	
	switch ( led ) {
	case LED1:
		if( req == OFF ) {
			sh1_orh_reg( PA_DR, LED_CTL1 );
		} else {
			sh1_anh_reg( PA_DR, ~LED_CTL1 );
		}
		break;
	case LED2:
		if( req == OFF ) {
			sh1_orh_reg( PB_DR, LED_CTL2 );
		} else {
			sh1_anh_reg( PB_DR, ~LED_CTL2 );
		}
		break;
	case LED3:
		if( req == OFF ) {
			sh1_orh_reg( PB_DR, LED_CTL3 );
		} else {
			sh1_anh_reg( PB_DR, ~LED_CTL3 );
		}
		break;
	default:
		break;
	}
}

/*
 *  LEDの状態取得
 *  arg1: LED1 - 3
 */
int
get_led( int led )
{
	switch ( led ) {
	case LED1:
		if ( (sil_reh_mem( PA_DR ) & LED_CTL1) == 0 ) {
			return ON;
		}
		break;
	case LED2:
		if ( (sil_reh_mem( PB_DR ) & LED_CTL2) == 0 ) {
			return ON;
		}
		break;
	case LED3:
		if ( (sil_reh_mem( PB_DR ) & LED_CTL3) == 0 ) {
			return ON;
		}
		break;
	default:
		break;
	}
	return OFF;
}

/*
 *  ディップスイッチの状態取得
 *  return:ON OFF
 *  arg1:DSW11 〜 DSW14
 */
int
get_dsw( int sw )
{
	UH	key = 0;
	int	result = OFF;

	key = sil_reh_mem( SW_DR );
	switch ( sw ) {
	case DSW11:
		if ((key & DSW11) == 0 ) {
			result = ON;
		}
		break;
	case DSW12:
		if ((key & DSW12) == 0 ) {
			result = ON;
		}
		break;
	case DSW13:
		if ((key & DSW13) == 0 ) {
			result = ON;
		}
		break;
	case DSW14:
		if ((key & DSW14) == 0 ) {
			result = ON;
		}
		break;
	default:
		break;
	}
	return result;
}

/*
 *  ロータリースイッチの値取得
 *  return:スイッチの値
 */
int
get_rsw(void)
{
	return (sil_reh_mem( SW_DR ) & RSW_CTL);
}

