/*
 *  TOPPERS/JSP Kernel
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
 
#ifndef _DEVICE_H_
#define _DEVICE_H_

/*
 *  LED ID定義
 */
#define LED1		1		/* LED1のID定義 */
#define LED2		2		/* LED2のID定義 */
#define LED3		3		/* LED3のID定義 */

/*
 *  LED制御ビット
 */
#define LED_CTL1	0x0100		/* LED1 制御 */
#define LED_CTL2	0x0040		/* LED2 制御 */
#define LED_CTL3	0x1000		/* LED3 制御 */

/*
 *  スイッチリードアドレス
 */
#define SW_DR		((VB *)0x6000200)

/*
 *  ディップスイッチ取得用のマスクビット
 */
#define DSW11		0x10
#define DSW12		0x20
#define DSW13		0x40
#define DSW14		0x80

/*
 *  ロータリースイッチ取得用のマスクビット
 */
#define RSW_CTL		0x0F

/*
 *  ON OFF 定義
 */
#define ON		1
#define OFF		0

/*
 *  ウオッチドッグタイマクリア
 */
Inline void
wdt_clear(void)
{
	sil_wrh_mem(PA_DR, (sil_reh_mem(PA_DR) ^ (1<<5)));
}

/*
 *  デバイスのプロトタイプ宣言
 */
void	initial_led(void );
void	set_led( int led, int req );
int	get_led( int led );
int	get_dsw( int sw );
int	get_rsw( void );

#endif /* _DEVICE_H_ */
