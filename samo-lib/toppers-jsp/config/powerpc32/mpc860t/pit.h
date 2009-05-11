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
 *  @(#) $Id: pit.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	タイマドライバ
 *　　　　　MPC860T内蔵の周期割込みタイマPITを使用
 */

#ifndef _PIT_H_
#define _PIT_H_

#include <mpc860_sil.h>

/*
 *  SIUの内部割込みレベル（ビットパターン）
 */
#define SIU_INT_PIT	LEVEL_TO_BIT_PATTERN(INTLVL_PIT)


/*
 *  タイマ値の内部表現の型
 */
typedef UH	CLOCK;

/*
 *	タイマに供給されるクロック周波数[kHz]
 *   	TIMER_CLOCK=SYSTEM_CLOCK[MHz] / 4
 */
#define	TIMER_CLOCK		(SYSTEM_CLOCK * 1000 / 4)

/*
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換
 */
#define	TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define	TO_USEC(clock)		((clock) * 1000 / TIMER_CLOCK)
#define CLOCK_PER_TICK	((CLOCK) (TO_CLOCK(TIC_NUME, TIC_DENO) - 1))

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define	MAX_CLOCK	((CLOCK) 0xffffu)

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define	GET_TOLERANCE	100	/* 処理遅れの見積り値（単位は内部表現）*/
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  コントロールレジスタの定義
 */

/*  周期割込みステータスおよび制御レジスタ  */
#define PISCR       (VH *)(IMMR_BASE + 0x240)
#define PISCRK      (VW *)(IMMR_BASE + 0x340)  /*  キーレジスタ  */
#define PISCR_PIRQ  0xff00      /*  周期割込み要求レベルビット  */  
#define PISCR_PS    BIT8_16     /*  周期割込みステータス  */    
#define PISCR_PIE   BIT13_16    /*  周期割込みイネーブル  */
#define PISCR_PITF  BIT14_16    /*  PITフリーズ・イネーブル  */
#define PISCR_PTE   BIT15_16    /*  周期タイマ・イネーブル  */

/*  PITカウントレジスタ（上位2バイトのみ）  */
#define PITC        (VH *)(IMMR_BASE + 0x244)
#define PITCK       (VW *)(IMMR_BASE + 0x344)  /*  キーレジスタ  */

/*  PITレジスタ（上位2バイトのみ）  */
#define PITR        (VH *)(IMMR_BASE + 0x248)



#ifndef _MACRO_ONLY

/*
 *  内部レジスタ保護のロックとアンロック
 */

/*  PISCRロック  */
Inline void
lock_piscr()
{
    	/*  値はUNLOCK_KEY以外であれば良い  */
    mpc860_wrw_mem(PISCRK, 0);
}

/*  PISCRアンロック  */
Inline void
unlock_piscr()
{
    mpc860_wrw_mem(PISCRK, UNLOCK_KEY);
}

/*  PITCロック  */
Inline void
lock_pitc()
{
    	/*  値はUNLOCK_KEY以外であれば良い  */
    mpc860_wrw_mem(PITCK, 0);
}

/*  PITCアンロック  */
Inline void
unlock_pitc()
{
    mpc860_wrw_mem(PITCK, UNLOCK_KEY);
}

/*
 *  タイマスタート
 */
Inline void
pit_start()
{
    unlock_piscr();         	/*  PISCRアンロック  */
    mpc860_orh_mem(PISCR, PISCR_PTE);
    lock_piscr();         	/*  PISCRロック  */
}

/*
 *  タイマ一時停止
 */
Inline void
pit_stop()
{
    unlock_piscr();         	/*  PISCRアンロック  */
    mpc860_andh_mem(PISCR, ~PISCR_PTE);
    lock_piscr();         	/*  PISCRロック  */
}

/*
 *  タイマ割込み要求のクリア
 *          PISCRレジスタのPSビットに1を書き込む
 */
Inline void
pit_int_clear()
{
	/*
	 *  SILを使用したときのログ機能をオフに
	 */
#ifdef SIL_DEBUG
    BOOL sil_debug_tmp = sil_debug_on;
    sil_debug_on = FALSE;
#endif /* SIL_DEBUG */

    unlock_piscr();         	/*  PISCRアンロック  */
    mpc860_orh_mem(PISCR, PISCR_PS);
    lock_piscr();         	/*  PISCRロック  */

	/*
	 *  SILを使用したときのログ機能を復元
	 */
#ifdef SIL_DEBUG
    sil_debug_on = sil_debug_tmp;
#endif /* SIL_DEBUG */

}


/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
pit_initialize()
{
    VH piscr;
    VW sccr;
    
    /* タイマ上限値=変数の最大値なのでチェックは省略 */

    pit_stop();		/* タイマ停止 */
    
    /*
     *  タイマ関連の設定
     */
    
    /*  クロックソース選択、分周比設定  */
    unlock_sccr();              /*  SCCRアンロック  */
    sccr = mpc860_rew_mem(SCCR);
    sccr = (sccr & ~SCCR_RTDIV) /*  リアルタイムクロック分周:4  */
            | SCCR_RTSEL;       /*  クロックソース：EXTCLK  */
    mpc860_wrw_mem(SCCR, sccr);
    lock_sccr();                /*  SCCRロック  */


    /*  カウンタ目標値設定  */
    unlock_pitc();                  /*  PITCアンロック  */
    mpc860_wrh_mem(PITC, CLOCK_PER_TICK);
    lock_pitc();                    /*  PITCロック  */
    
    
    unlock_piscr();                 /*  PISCRアンロック  */
    piscr = mpc860_reh_mem(PISCR);
    piscr = (piscr & ~PISCR_PIRQ)
            | (SIU_INT_PIT << 8)	/*  割込みレベル設定  */
            | PISCR_PIE;                /*  PIT割込み許可  */
    mpc860_wrh_mem(PISCR, piscr);
    lock_piscr();                   /*  PISCRロック  */
}

/*
 *  タイマの停止処理
 *
 *  　タイマの動作を停止させる．
 */
Inline void
pit_terminate()
{
    pit_stop();				/* タイマを停止 	*/
    pit_int_clear();			/* 割り込み要求をクリア */

    unlock_piscr();                 	/*  PISCRアンロック  */
    mpc860_andh_mem(PISCR, ~PISCR_PIE);	/* タイマ割込みを禁止   */
    lock_piscr();                   	/*  PISCRロック  */
    
    /*  本当はタイマへのクロック供給を停止する  */
}

/*
 *  タイマの現在値の読出し
 *　　　割込み禁止区間中で呼び出すこと
 */
Inline CLOCK
pit_get_current()
{
    CLOCK pitr;
    
    pitr = mpc860_reh_mem(PITR);
    return(CLOCK_PER_TICK - pitr);
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
pit_fetch_interrupt()
{
    VH piscr;
    
    /*  読み出しなので、アンロックしなくて良い  */
    piscr = mpc860_reh_mem(PISCR);
    return(piscr & PISCR_PS);
}

#endif /* _MACRO_ONLY */
#endif /* _PIT_H_ */
/*  end of file  */
