/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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
 */

#ifndef _RTE_VR5500_CB_
#define _RTE_VR5500_CB_

#define RTE_VR5500_CB

/*
 *  デバイスの割り当てベースアドレス
 */

/* シリアルコントローラ (TL16PIR552) */
#define UART_CH01	0xb9001000
#define UART_CH02	0xb9002000
#define PRINTER_PPCS	0xb9003000
#define PRINTER_ECPCS	0xb9004000

/* タイマ (uPD71054) */
#define TIMER_BASE_ADDR	0xb9005000

/* 割込みコントローラ(PIC) */
#define ICU_BASE_ADDR	0xb9000000

/*
 *  デバイスに供給されるクロック
 */
/* シリアルに供給されるクロック周波数 [Hz]（ボーレート設定で使用）
 * （16MHz = 16000000 Hz ; ボードのマニュアルp21参照） */
#define XIN_CLOCK	16000000u

/* タイマに供給されるクロック周波数 [kHz]
 *（TO_CLOCK で1ミリ秒周期(1kHz)を発生させるために使用）
 * (2MHz = 2000kHz ; ボードのマニュアルp22参照) */
#define TIMER_CLOCK	2000u

/*
 *  デバイスの割込みマスクの定義
 *
 *  優先度 : (低) シリアル１ -> シリアル０ -> タイマ０ (高)
 */

/*
 *  シリアル０割込みの割込みマスク（送受信共用）
 *  ・割込みレベルは送信も受信も同じレベルに設定している
 */
#define CORE_IPM_SCC		INIT_CORE_IPM	/* MIPS3コア(SR)用データ */

#define ICU_IPM_INT0_SCC0	TIMER0		/* 割込みコントローラ用データ */
#define ICU_IPM_INT1_SCC0	0u

/*  設定する割込みマスク（IPM型で記述する） */
#define IPM_SCC0	{ CORE_IPM_SCC, {ICU_IPM_INT0_SCC0, ICU_IPM_INT1_SCC0} }

/*
 *  シリアル１割込みの割込みマスク（送受信共用）
 *  ・割込みレベルは送信も受信も同じレベルに設定している
 *  ・MIPS3コア用のデータは、SCC0と共通
 */
#define ICU_IPM_INT0_SCC1	(TIMER0 | SERIAL0) /* 割込みコントローラ用データ */
#define ICU_IPM_INT1_SCC1	0u

/*  設定する割込みマスク（IPM型で記述する） */
#define IPM_SCC1	{ CORE_IPM_SCC, {ICU_IPM_INT0_SCC1, ICU_IPM_INT1_SCC1} }

/*
 *  タイマ０割込みの割込みマスク
 *  本実装では、タイマ割込みが入ったときは、全ての外部割込みを禁止している。
 */
#define CORE_IPM_TIMER		INIT_CORE_IPM	/* MIPS3コア(SR)用データ */

#define ICU_IPM_INT0_TIMER0	0u		/* 割込みコントローラ用データ */
#define ICU_IPM_INT1_TIMER0	0u

/*  設定する割込みマスク（IPM型で記述する） */
#define IPM_TIMER0	{ CORE_IPM_TIMER, {ICU_IPM_INT0_TIMER0, ICU_IPM_INT1_TIMER0} }

#endif /* _PIC_ICU_H_ */
