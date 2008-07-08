/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: sys_config.h,v 1.7 2007/03/23 07:58:33 honda Exp $
 */

/*
 *　ターゲットシステム依存モジュール（北斗電子 HSB8F3048BF25用）
 *　　　カーネル内部で使用する定義
 *　　　　データ型、マクロ、関数のプロトタイプ宣言
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
 */

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */

#include <sys_rename.h>

/*
 *  ユーザー・コンフィギュレーション情報の定義
 */
#include <user_config.h>


/*
 *  ターゲットシステムのハードウェア資源の定義
 */

#include <h8_3048f.h>

/*
 *  起動メッセージのターゲット名
 */

#define TARGET_NAME     "Hokuto Electronic H8/3048F-ONE STARTER KIT"


#ifndef _MACRO_ONLY

/*
 *  ターゲットシステム依存の初期化
 */

extern void     sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．ROMモニタ呼出しで実現することを想定し
 *  ている．
 */

extern void     sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．ROMモニタ呼出しで実現するこ
 *  とを想定している．
 */
extern void     sys_putc(char c) throw();

#endif /* _MACRO_ONLY */

/*
 *  SOFTWARE_TERM_HOOKマクロ：atexitの処理
 *  　処理関数の外部定義があれば、その関数名をSOFTWARE_TERM_HOOK
 *　　マクロとして定義する。
 */
/*  今回は定義なし  */


/*
 *  (1) シリアルポートの設定
 *  (2) タイマーの設定
 *  (3) 外部アドレス空間制御
 */

/*
 *  SCI 共通定数、マクロ
 */

        /* SCI をクローズする前の待ち時間の計算に使用する。*/
#define H8_MIN_BAUD_RATE        9600ul

/*
 *  SYSTEM 用 SCI の設定値
 */
                                /* システムログを出力するシリアルポート番号     */
#define LOGTASK_PORTID          SYSTEM_PORTID   /*  user_config.h  */

/*  設定レジスタのベースアドレス  */
#define SCI_PORT1_BASE          H8SCI0

/*  プライオリティレベル設定用のデータ  */
#define SCI_PORT1_IPR          H8IPRB
#define SCI_PORT1_IPR_BIT      H8IPR_SCI0_BIT

#define SCI_PORT1_SMR          0u
                        /* 送受信フォーマット                   */
                        /* 調歩同期式                           */
                        /* キャラクタレングス：8ビット          */
                        /* パリティなし                         */
                        /* ストップビットレングス：1            */
                        /* クロックセレクト（分周比）:1         */

#define INHNO_SERIAL_IN         IRQ_RXI0
#define INHNO_SERIAL_OUT        IRQ_TXI0
#define INHNO_SERIAL_ERR        IRQ_ERI0


#if TNUM_PORT == 2

#define SCI_PORT2_BASE          H8SCI1

/*  プライオリティレベル設定用のデータ  */
#define SCI_PORT2_IPR          H8IPRB
#define SCI_PORT2_IPR_BIT      H8IPR_SCI1_BIT

#define SCI_PORT2_SMR          0u
                        /* 送受信フォーマット                   */
                        /* 調歩同期式                           */
                        /* キャラクタレングス：8ビット          */
                        /* パリティなし                         */
                        /* ストップビットレングス：1            */
                        /* クロックセレクト（分周比）:1         */

#define INHNO_SERIAL2_IN        IRQ_RXI1
#define INHNO_SERIAL2_OUT       IRQ_TXI1
#define INHNO_SERIAL2_ERR       IRQ_ERI1

#endif  /* of #if TNUM_PORT == 2u */

/*
 *  タイマの設定
 */

#define SYSTEM_TIMER            H8ITU0

#define SYSTEM_TIMER_CNT        (SYSTEM_TIMER + H8TCNT)
#define SYSTEM_TIMER_TCR        (SYSTEM_TIMER + H8TCR)
#define SYSTEM_TIMER_TIOR       (SYSTEM_TIMER + H8TIOR)
#define SYSTEM_TIMER_IER        (SYSTEM_TIMER + H8TIER)
#define SYSTEM_TIMER_IFR        (SYSTEM_TIMER + H8TSR)
#define SYSTEM_TIMER_TSTR       H8ITU_TSTR
#define SYSTEM_TIMER_GR         (SYSTEM_TIMER + H8GRA)

#define SYSTEM_TIMER_STR        H8TSTR_STR0
#define SYSTEM_TIMER_STR_BIT    H8TSTR_STR0_BIT
#define SYSTEM_TIMER_IE         H8TIER_IMIEA            /* interrupt mask */
#define SYSTEM_TIMER_IE_BIT     H8TIER_IMIEA_BIT
#define SYSTEM_TIMER_IF         H8TSR_IMIFA             /* match flag */
#define SYSTEM_TIMER_IF_BIT     H8TSR_IMIFA_BIT

#define INHNO_TIMER             IRQ_IMIA0

#define SYSTEM_TIMER_TCR_BIT    (H8TCR_CCLR0 | H8TCR_TPSC1 | H8TCR_TPSC0)
#define SYSTEM_TIMER_TIOR_BIT   (0u)

/*  プライオリティレベル設定用のデータ  */
#define SYSTEM_TIMER_IPR                H8IPRA
#define SYSTEM_TIMER_IP_BIT             H8IPR_ITU0_BIT

/*
 *  I/Oポート DDRの初期値
 *      h8_sil.cのDDRアクセスルーチンで使用する。
 *      ポート７は入力専用のため、省略している。
 */
#define H8P1DDR0        0xffu	/*  アドレス・バス  */
#define H8P2DDR0        0xffu	/*  アドレス・バス  */
#define H8P3DDR0        0u	    /*  データ・バス　  */
#define H8P4DDR0        0u      /*  データ・バス　  */
#define H8P5DDR0        0x0fu	/*  アドレス・バス  */
#define H8P6DDR0        0u
#define H8P8DDR0        H8P8DDR_CS1     /*  P83をCS1に設定  */
#define H8P9DDR0        0u
#define H8PADDR0        0u
#define H8PBDDR0        0u

/*
 *  SYSCRの初期値
 *      $SYSディレクトリでSYSCR_SYSとして定義する内容
 *      　ビット6-4 STS：スタンバイ・タイマ・セレクト
 *      　ビット2 NMIEG：NMIエッジ
 */
#define SYSCR_SYS	0u


#endif /* _SYS_CONFIG_H_ */
