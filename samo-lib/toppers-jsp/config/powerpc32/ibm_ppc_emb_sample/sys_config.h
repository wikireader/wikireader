/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2003 by Industrial Technology Institute,
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
 * 
 *  @(#) $Id: sys_config.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュールのサンプル
 *　　　　　カーネル内部で使用する定義
 *　　　　　　データ型、マクロ、関数のプロトタイプ宣言
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
 */

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/*
 *  ユーザー定義情報
 */
#include <user_config.h>

/*
 *  カーネルの内部識別名のリネーム
 */
#include <sys_rename.h>

/*
 *  ターゲットシステムのハードウェア資源の定義
 */
//#include <******.h>
//#include <******.h>


/*
 *  起動メッセージのターゲットシステム名
 */
#define	TARGET_NAME	"Target name"

/*
 *  微少時間待ちのための定義
 */
#define	SIL_DLY_TIM1	1425
#define	SIL_DLY_TIM2	 350

#ifndef _MACRO_ONLY

/*
 *  ターゲットシステム依存の初期化
 */
extern void	sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．
 *　　ROMモニタ／GDB STUB呼出しは未実装
 */
extern void	sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．
 *　　ROMモニタ／GDB STUB呼出しは未実装
 */
extern void	sys_putc(char c) throw();

#endif /* _MACRO_ONLY */

/*  --------------------------------------------------------------  */
/*
 * PowerPC共通部とのインターフェースの定義
 */

/*
 *   MSRの初期値
 */
#define INIT_MSR    	0

/*
 *  割込みの本数
 */
#define TMAX_INTNO       10

/*
 *  割込み処理のICU依存部分
 *  　処理内容
 *  　　・割込み要因の判別
 *  　　・割込みマスクの設定
 *  　　・割込み許可
 *  　　・C言語ルーチン呼び出し
 *  　　・割込み禁止
 *  　　・（必要であれば）割り込み要求フラグのクリア
 *  　
 *  　引数
 *  　  label：ラベル識別用文字列
 *  　  　　　　（複数箇所でこのマクロを使用できるようにするため）
 */
#define _PROC_ICU(label)

/* マクロ引数labelを確実に展開するため、マクロを2重にしている */
#define PROC_ICU(label) _PROC_ICU(label)


/*
 * 割り込みコントローラのマスクIPM関連の定義
 */

/*  IPMの待避  */
    /*  注意：spを4バイト境界に維持する  */
#define PUSH_ICU_IPM

/*  IPMの復元  */
    /*  注意：spを4バイト境界に維持する  */
#define POP_ICU_IPM


#ifdef SUPPORT_CHG_IPM
/*
 * ICUに設定するIPMパラメータのチェック
 *
 *　C言語の
 *  　if (ipmの値が不正) {
 *	ercd = E_PAR;
 *	goto exit;
 *    }
 *　と同等の処理を行うマクロ
 */
#define CHECK_IPM(ipm)

/*
 * 割り込みコントローラのマスク取得
 */
#define GET_IPM(p_ipm)	

/*
 * 割り込みコントローラのマスク設定
 */
#define CHG_IPM(ipm)

#endif	/*  SUPPORT_CHG_IPM  */


#ifndef _MACRO_ONLY

/*
 *  割込みマスク用擬似ベクタテーブル
 *	ipm_table自体は$(CPU)とのインターフェースに含まれない
 *　　　（define_ipmにより、隠蔽されている）
 */
extern IPM ipm_table[];

/*
 *  割り込みレベルの設定
 *	割込み番号inhnoの割込みを受け付けたときに割込みコントローラの
 *	IPMに設定する値を定義する。
 *	デバイスドライバの初期化処理で使用されることを想定している。
 *
 *　　　引数
 *　　　　inhno：割込みハンドラ番号
 *　　　　ipm：割込みマスク
 *
 */
Inline void
define_ipm(INHNO inhno, IPM ipm)
{
	ipm_table[inhno] = ipm;
}

#endif /* _MACRO_ONLY */

/*  
 *  省電力モードへの移行と割込み許可
 *  　実行すべきタスクがなくて割込み待ちになるときに
 *  　ディスパッチャの中で使用される。
 */ 
#define SAVE_POWPER	/*  処理内容をアセンブラで記述する  */


/* ----------------------------------------------------------------- */
/*
 * The IBM PowerPC Embedded Environment用の定義
 */

/*
 * 例外処理ルーチンの定義（必須部分）
 */

/*
 *  Critical Interrupt
 *  　オフセット：0x100
 *  　Critical Interruptはインプリメンテーション依存なので
 *  　処理内容はマクロ定義しておく。
 *  　　マクロ名：CRITICAL_INTERRUPT_EXCEPTION_PROC
 *  　この処理内容は0x100バイトを超えてはいけない。
 *  　（超える場合は残りの部分を別の場所に配置して、
 *  　　そこに分岐すること）
 *
 *  　Critical Interruptはカーネル管理外の例外とする。
 *  　（この処理ルーチン内でサービスコールを使用しない。）
 *
 *　　例外クラス：Critical
 *  　　リターン命令はrfci(Return From Critical Interrupt)
 *  　　を用いる。
 */
#define CRITICAL_INTERRUPT_EXCEPTION_PROC	rfci
				/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */


/*
 *  マシン・チェック例外
 *  　この処理内容は0x100バイトを超えてはいけない。
 *  　（超える場合は残りの部分を別の場所に配置して、
 *  　　そこに分岐すること）
 *
 *  　カーネル管理外の例外とする。
 *  　（この処理ルーチン内でサービスコールを使用しない。）
 *
 *　　例外クラス：Critical
 *  　　リターン命令はrfci(Return From Critical Interrupt)
 *  　　を用いる。
 */
#define MACHINE_CHECK_PROC	rfci
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */

/*
 *  Programmable Interval timer
 *  　オフセット：0x1000
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
#define PROGRAMMABLE_INTERVAL_TIMER_PROC
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */

/*
 *  Fixed Interval timer
 *  　オフセット：0x1010
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
#define FIXED_INTERVAL_TIMER_PROC
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */

/*
 *  Watchdog timer
 *  　オフセット：0x1020
 *
 *　　例外クラス：Critical
 *  　　リターン命令はrfci(Return From Critical Interrupt)
 *  　　を用いる。
 *
 *  　0x10バイトしか領域が割り当てられていないので注意
 *  　カーネル管理外の例外とする。
 *  　（この処理ルーチン内でサービスコールを使用しない。）
 */
#define WATCHDOG_TIMER_PROC	rfci
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */

/*
 *  Data TLB miss
 *  　オフセット：0x1100
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
#define DATA_TLB_MISS_PROC
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */

/*
 *  Instruction TLB miss
 *  　オフセット：0x1200
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
#define INSTRUCTION_TLB_MISS_PROC
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */


/*
 *  Debug例外
 *  　オフセット：0x2000
 *
 *　　例外クラス：Critical
 *  　　リターン命令はrfci(Return From Critical Interrupt)
 *  　　を用いる。
 *  　
 *  　要因は以下の7種類
 *  　　Trap
 *  　　Instruction address compare
 *  　　Data address compare
 *  　　Instruction complete
 *  　　Branch taken
 *  　　Exception
 *  　　Unconditional debug event
 *  　
 *  　カーネル管理外の例外とする。
 *  　（この処理ルーチン内でサービスコールを使用しない。）
 *  　
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
#define DEBUG_PROC	rfci
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */

/*
 *  インプリメンテーション専用の処理（オフセット：0xd00-0xff0）
 *    インプリメンテーション固有の例外ベクタ
 * 　 オフセットの指定方法に注意すること。
 *  　具体的には
 *     .org オフセット - EXCEPTION_VECTOR_BASE
 * 　 のように指定する。詳しくは他の例外要因の該当個所を参照。
 *    例えば、PowerPC405ではオフセット0xf20に
 *    APU Unavailable例外の処理を配置する
 */
//#define IMPLEMENT_EXCEPTION_D00_PROC
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */

/*
 * 例外処理ルーチンの定義（オプション部分）
 */
/*
 *  インプリメンテーション専用の処理（オフセット：0x2010-0x2ff0）
 *    インプリメンテーション固有の例外ベクタ（１つとは限らない）や
 *    他の用途に使用される
 * 　 オフセットの指定方法に注意すること。
 *  　具体的には
 *     .org オフセット - EXCEPTION_VECTOR_BASE
 * 　 のように指定する。詳しくは他の例外要因の該当個所を参照。
 */
// #define IMPLEMENT_EXCEPTION_02010_PROC
						/*  アセンブラで記述する  */
/*  このマクロ定義はデバイスに合わせて各自書き換えて下さい。  */


#endif /* _SYS_CONFIG_H_ */

/*  end of file  */
