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

#ifndef _KZ_VR4131PCI_01_
#define _KZ_VR4131PCI_01_

#define KZ_VR4131PCI_01

/*
 *  デバイスの割り当てベースアドレス
 */

#define VR4131_BASE_ADDR	0xa0000000

/*
 *  デバイスに供給されるクロック
 */
/*
 *  VR4131 は起動時に CLKSEL(2:0) 端子を見て、パイプラインクロックPClockを
 *  決定する。今回のターゲットボードのデフォルトは、
 *        CLKSEL(2:0) = 110
 *  パワーマネジメントユニットの、PMUTCLKDIVREGに変更は加えないため下記となる。
 *        PClock 199.1[MHz], VTClock 33.2[MHz], TClock 16.6[MHz]
 */
/* シリアルに供給されるクロック周波数 [Hz]（ボーレート設定で使用）
 * （18.432MHz = 18432000 Hz ; ハードウェア編p193参照） */
#define XIN_CLOCK	18432000u	/* [Hz] */

/* タイマに供給されるクロック周波数 [kHz]（TO_CLOCKで使用）
 * （TO_CLOCK で1ミリ秒周期(1kHz)を発生させるために使用）
 * （VTClock=33.2MHz = 33200kHz ; ハードウェア編p22参照） */
#define TIMER_CLOCK	33200u	/* [kHz] */

/*
 *  デバイスの割込みマスクの定義
 *
 *  優先度 : (低) DSIU -> TClockカウンタ -> バッテリ残量不足検知 (高)
 */
/* MSYSINT1 の BATINTR の設定については、ハードウェア編p224参照  */

/*
 *  DSIU 割込みの割込みマスク（送受信共用）
 *  ・割込みレベルは送信も受信も同じレベルに設定している
 */
#define CORE_IPM_DSIU		INIT_CORE_IPM	/* MIPS3コア(SR)用データ */

/* BATINTR、TCLKINTR 割込みのみを許可する。 */
#define ICU_IPM_SYSINT1_DSIU	BATINTR		/* 割込みコントローラ用データ */
#define ICU_IPM_SYSINT2_DSIU	TCLKINTR

/*  設定する割込みマスク（IPM型で記述する） */
#define IPM_DSIU	{ CORE_IPM_DSIU, {ICU_IPM_SYSINT1_DSIU, ICU_IPM_SYSINT2_DSIU} }

/*
 *  RTC 関連の割込みマスク
 */
#define CORE_IPM_TIMER		INIT_CORE_IPM	/* MIPS3コア(SR)用データ */

/* BATINTR 割込みのみを許可する。 */
#define ICU_IPM_SYSINT1_TCLK	BATINTR		/* 割込みコントローラ用データ */
#define ICU_IPM_SYSINT2_TCLK	0u

/*  設定する割込みマスク（IPM型で記述する） */
#define IPM_TIMER	{ CORE_IPM_TIMER, {ICU_IPM_SYSINT1_TCLK, ICU_IPM_SYSINT2_TCLK} }

#endif /* _KZ_VR4131PCI_01_ */
