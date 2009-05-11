/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
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
 *  @(#) $Id: sys_config.c,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール（MPC860T TB6102S用）
 *　　　　　カーネル内部で使用する定義
 *　　　　　　C言語関数の実体
 */

#include "jsp_kernel.h"
#include <mpc860_sil.h>

/*
 *  割込み発生時にスタックに待避された状態のIPMのデータ構造
 *    4バイト境界を維持するため、ダミー領域が入る
 */
typedef struct ipm_stack {
	UH	ipm;
	UH	dummy;
} IPM_STACK;

/*
 *  割込みマスク用擬似ベクタテーブル
 *	ipm_table自体は$(CPU)とのインターフェースに含まれない
 *　　　（define_ipmにより、隠蔽されている）
 */
IPM ipm_table[TMAX_EXCNO];

/*
 *  SILを使用したときのログ機能
 */
#ifdef SIL_DEBUG

BOOL sil_debug_on = FALSE;
char sil_buffer[SIL_BUFFER_SIZE];
int sil_pos = -1;
UW sil_log_id = 0;

#endif	/* SIL_DEBUG */

/*
 *  ターゲットシステム依存の初期化
 */
void
sys_initialize()
{
    	INHNO j;
	
	/*
	 *  通信プロセッサCPMの初期化
	 */
	
	/*  リセット・コマンドの発行  */
    	mpc860_wrh_mem(CPCR, CPCR_RST | CPCR_FLG);
	
	/*
	 *  割込みコントローラの初期化
	 *　　IRL：SIUに対する割込みレベル
	 *　　IEN=1：CPM割込みイネーブル
	 */
	mpc860_wrw_mem(CICR, CICR_IRL0 | CICR_IEN);

	/*  IPMの初期化：CPM割込みの許可  */
	mpc860_wrh_mem(SIMASK, ENABLE_CPM);


	/*
	 *  内部データの初期化
	 */

    	/* IPMテーブルの初期化 */
    	for( j = 0; j < TMAX_EXCNO; j++ ) {
        	ipm_table[j] = DEFAULT_IPM;
    	}
    	
    	/*  CPM割込みのIPMを設定  */
    	ipm_table[INHNO_CPM] = IPM_CPM;
	
	/*
	 *  プロセッサ識別のための変数の初期化
	 */
	/*  未実装  */
}

/*
 *  ターゲットシステムの終了
 */
void
sys_exit()
{
	tb6102s_exit();
}

/*
 *  ターゲットシステムの文字出力
 */
void
sys_putc(char c)
{
	if (c == '\n') {
		tb6102s_putc('\r');
	}
	tb6102s_putc(c);
}

/*
 * 登録されていない割込み発生時のログ出力
 * 　　マクロPROC_ICUの中で呼び出される
 * 　　関数名や引数の型はプロセッサ毎に決めて良い
 *
 *     MPC860では割込みコントローラがカスケード接続されているため、
 *　　 各割込みコントローラに専用の関数を用意した。
 *
 */

/*  SIU割込み（CPM割込み以外）  */
void no_reg_SIU_interrupt(INHNO intno, UW *sp)
{
    syslog(LOG_EMERG, "SIU interrupt error occurs.");
    syslog(LOG_EMERG, "Interrupt No.=%d", intno);
    syslog(LOG_EMERG, "IPM=0x%04x", ((IPM_STACK *)sp)->ipm);

    /*  
     *  スタック上のデータの表示
     *    割込みマスクをスタックに積んでいるので
     *　  その分、スキップする
     */
    ++sp;
    syslog_data_on_stack((EXCSTACK *)sp);
    
    while(1);
}

/*  CPM割込み  */
void no_reg_CPM_interrupt(INHNO intno, UW *sp)
{
    syslog(LOG_EMERG, "CPM interrupt error occurs.");
    syslog(LOG_EMERG, "Interrupt No.=%d(total %d)", 
           intno, intno + TMAX_SIU_INTNO);
    syslog(LOG_EMERG, "IPM=0x%04x", ((IPM_STACK *)sp)->ipm);

    /*  
     *  スタック上のデータの表示
     *    割込みマスクをスタックに積んでいるので
     *　  その分、スキップする
     */
    ++sp;
    syslog_data_on_stack((EXCSTACK *)sp);
    
    while(1);
}

/*  end of file  */
