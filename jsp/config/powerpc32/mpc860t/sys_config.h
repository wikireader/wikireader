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
 *  @(#) $Id: sys_config.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール（MPC860T TB6102S用）
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
#include <tb6102s.h>
#include <mpc860t.h>


/*
 *  起動メッセージのターゲットシステム名
 */
#define	TARGET_NAME	"TANBAC TB6102S CompactPCI CPU Board"

/*
 *  微少時間待ちのための定義
 */
#ifdef DOWNLOAD_TO_RAM      /*  デバッグ用  */

#define	SIL_DLY_TIM1	1425
#define	SIL_DLY_TIM2	 350

#else /* DOWNLOAD_TO_RAM */ /*  ROM化  */

#define	SIL_DLY_TIM1	3445
#define	SIL_DLY_TIM2	 867

#endif /* DOWNLOAD_TO_RAM */

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

/*
 * 　CPMへの割込み要求のクリア
 * 　　CPM割込みインサービスレジスタCISRの該当ビットに１をセットすると
 * 　　クリアされる。
 */
#define CLEAR_IRQ_TO_ICU(device)		\
	mpc860_wrw_mem(CISR, CISR_BIT(device))	


/*
 * 　登録されていないSIU割込みにおける引数設定
 * 　　初段の割込みと多重割込みで、spの取り出し方が異なる
 */
#define SET_NO_REG_SIU_INTERRUPT_ARG_FROM_TASK	\
	lwz	r4, 0(sp)	/*  割込みスタックからspを復元  */

#define SET_NO_REG_SIU_INTERRUPT_ARG_FROM_INT	\
	addi	r4, sp, 8	/*  sp+8  */

/*
 * 　登録されていないCPM割込みにおける引数設定
 * 　　初段の割込みと多重割込みで、spの取り出し方が異なる
 */
#define SET_NO_REG_CPM_INTERRUPT_ARG_FROM_TASK	\
	lwz	r4, 8(sp)	/*  割込みスタックからspを復元  */

#define SET_NO_REG_CPM_INTERRUPT_ARG_FROM_INT	\
	addi	r4, sp, 16	/*  sp+16  */


#ifndef _MACRO_ONLY
/*
 * 登録されていない割込み発生時のログ出力
 * 　　マクロPROC_ICUの中で呼び出される。
 *     関数名や引数の型はプロセッサ毎に決めて良い。
 *
 *     MPC860では割込みコントローラがカスケード接続されているため、
 *　　 各割込みコントローラに専用の関数を用意した。
 *
 */

/*  SIU割込み（CPM割込み以外）  */
extern void     no_reg_SIU_interrupt(INHNO intno, UW *sp);

/*  CPM割込み  */
extern void     no_reg_CPM_interrupt(INHNO intno, UW *sp);

#endif /* _MACRO_ONLY */

#define EXCEPTION_VECTOR_START_SRC	0x10000100
#define EXCEPTION_VECTOR_END_SRC	0x10002000
#define EXCEPTION_VECTOR_START_DST	0x100



/*  --------------------------------------------------------------  */
/*
 * PowerPC共通部とのインターフェースの定義
 */

/*
 *   MSRの初期値
 */
#define INIT_MSR    	0

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
 *  　
 *
 *  　MPC860の割込みアーキテクチャは
 *  　　・システム・インターフェース・ユニットSIU
 *  　　・通信プロセッサモジュールCPM
 *  　の2階層になっている
 *  　
 *  　　MPC860ではシステムインターフェースユニットSIUの割込みマスク
 *  　　（SIU割込みマスクレジスタSIMASK）のみをカーネルで管理する。
 *  　　通信プロセッサモジュールCPMの割込みマスクの管理はすべて
 *  　　ハードウェアが行っている。
 *  　
 *  　CPM割込みコントローラ（ハードウェア）の動作
 *  　  　割込みが発生すると
 *  　  　　・割込み保留レジスタCIPRの該当ビットの該当ビットをセット
 *  　  　　・発生した割込みの優先度を
 *  　  　　　　・割込みマスクレジスタCIMR
 *  　  　　　　・インサービスレジスタCISR
 *  　  　　　　と比較してからSIUに通知
 *  　  　コアがベクタレジスタCIVRのIACKビットをセットすると
 *  　  　　・SIUへの割込み要求をクリア
 *  　  　　・ベクタレジスタCIVRにベクタ番号をセット
 *  　  　　・ベクタレジスタCIVRのIACKビットをクリア
 *  　  　　・割込み保留レジスタCIPRの該当ビットをクリア
 *  　  　　・インサービスレジスタCISRの該当ビットをセット
 *  　  　　　　以後、これより下位の割込みをマスクする
 *  　  　　　　　　→CPMについてはソフトウェアによるマスク操作の必要なし
 *  　  　　
 *  　CPM割込みに関しては、SIUのIPMに一括して同じ値を設定する
 *  　  　　
 *  　
 *  　レジスタ割り当て：
 *  　  SPRG1:擬似ベクタテーブルの先頭アドレスint_table
 *  　  SPRG3:IPMテーブルの先頭アドレスipm_table
 *  　  r0：値０
 *  　  r3：内部レジスタ領域の先頭アドレス
 *  　  r4：割込みベクタ番号×４
 *  　
 */
#define _PROC_ICU(label)                                                \
    /*  SIUの割込み要因判別  */                                         \
    lis     r3, IMMR_UPPER_2BYTE; /* 内部レジスタ領域の先頭アドレス */  \
        /*  ベクタ番号の読み出し  */                                    \
        /*  SIVECレジスタには"ベクタ番号×4"が格納されている  */        \
    lbz     r4, TADR_SIU_SIVEC(r3); /*  r4←ベクタ番号×4  */           \
                                                                        \
    /*  割込みマスクの設定  */                                          \
    /*  　SIU割込み：割込み要因毎の値をIPMに設定する  */      		\
    /*  　CPM割込み：一括して同じ値をSIUのIPMに設定する  */   		\
    mfspr   r5, SPRG3;      /*  r5←ipm_table  */                       \
    srwi    r6, r4, 1;      /*  r6←r4>>1（割込み番号×2倍）  */        \
    lhzx    r7, r5, r6;     /*  r7：割込みマスク  */                    \
    	/*  現在の割込みマスクを確認して、  */				\
    	/*  元々、許可されていない割込みは許可しないようにする  */	\
    lhz     r8, TADR_SIU_SIMASK(r3);					\
    and     r9, r8, r7;							\
    sth     r9, TADR_SIU_SIMASK(r3);/*  割込みマスク設定  */            \
                                                                        \
    /*  CPM割込みならば分岐  */                                         \
    cmpwi   crf0, r4, INHNO_CPM*4;                                      \
    beq     label##_CPM_INTERRUPT;                                      \
                                                                        \
    /*  SIU割込み（CPM割込み以外）の場合  */                            \
    /*  （SIUに直接、接続されているデバイスからの割込みの場合）  */     \
    mfspr   r8, SPRG1;  /*  r8←SPRG1：int_table  */                    \
                        /*  r4：割込み番号×4  */                       \
    lwzx    r9, r8, r4; /*  r9：C言語ルーチンの先頭アドレス  */         \
    mtctr   r9;         /*  CTR←r9  */                                 \
    mfmsr   r10;                                                        \
    ori     r11, r10, MSR_EE;   /*  EEビットをセット  */                \
    mtmsr   r11;                /*  割込み許可  */                      \
    cmpwi   crf0, r9, 0;	/*  割込みハンドラが登録されているか */ \
    beq     label##_call_no_reg_SIU_interrupt; /*  チェック  */         \
    bctrl;                      /*  C言語ルーチン呼び出し  */           \
    mfmsr   r10;                                                        \
    xori    r11, r10, MSR_EE;   /*  EEビットをクリア  */                \
    mtmsr   r11;        /*  割込み禁止  */                              \
    b       label##_exit;/*  PROC_ICUマクロの出口へジャンプ  */         \
                                                                        \
    /*                                                                  \
　　 *　　SIU割込み（CPM割込み以外）で割込みハンドラが                  \
　　 *　　登録されていない場合                                          \
　　 */                                                                 \
label##_call_no_reg_SIU_interrupt:;                                     \
    srwi    r3, r4, 2;          /*  r3←割込み番号  */                  \
    SET_NO_REG_SIU_INTERRUPT_ARG_##label;                               \
    LI32(r9, no_reg_SIU_interrupt);                                     \
    mtctr   r9;         /*  CTR←r9  */                                 \
    bctr;                                                               \
    /*  ここには戻ってこない  */                                        \
                                                                        \
    /*                                                                  \
　　 *　　CPM割込みで割込みハンドラが                                   \
　　 *　　登録されていない場合                                          \
　　 */                                                                 \
label##_call_no_reg_CPM_interrupt:;                                     \
    srwi    r3, r4, 2;          /*  r3←割込み番号  */                  \
    SET_NO_REG_CPM_INTERRUPT_ARG_##label;                               \
    LI32(r9, no_reg_CPM_interrupt);                                     \
    mtctr   r9;         /*  CTR←r9  */                                 \
    bctr;                                                               \
    /*  ここには戻ってこない  */                                        \
                                                                        \
                                                                        \
    /*  CPM割込みの場合  */                                             \
    /*  　　SIUのIPMは設定済み  */                                      \
    /*  　　CPMのIPM設定はハードウェアが行う  */                        \
label##_CPM_INTERRUPT:;                                                 \
    /*  CIVRレジスタのIACKビットをセット  */                            \
    li      r5, TA_CPM_CIVR_IACK;                                       \
    sth     r5, TADR_CPM_CIVR(r3);                                      \
    /*  CPM割込みベクタ番号読み出し  */                                 \
            /*  割込みベクタレジスタCIVRにはベクタ番号が  */            \
            /*  11ビット左シフトされた状態で格納されている  */          \
    lhz     r6, TADR_CPM_CIVR(r3);  /*  r6←ベクタ番号<<11  */          \
    srwi    r4, r6, 9;  /*  r4←r6>>9（割込み番号×4倍）  */            \
    stwu    r4, -STACK_MARGIN(sp); /*  ベクタ番号を保存  */             \
    		/*  spを8バイトずらすのは、C言語ルーチンが  */		\
    		/*  sp+4番地に書き込みを行うため  */			\
        /*  CPMローカルのベクタ番号をSIU全体のベクタ番号に変換  */      \
    addi    r5, r4, TMAX_SIU_INTNO*4;                                   \
    mfspr   r8, SPRG1;  /*  r8←SPRG1：int_table  */                    \
                        /*  r4：割込み番号×4  */                       \
    lwzx    r9, r8, r5; /*  r9：C言語ルーチンの先頭アドレス  */         \
    mtctr   r9;         /*  CTR←r9  */                                 \
    mfmsr   r10;                                                        \
    ori     r11, r10, MSR_EE;   /*  EEビットをセット  */                \
    mtmsr   r11;                /*  割込み許可  */                      \
    cmpwi   crf0, r9, 0;	/*  割込みハンドラが登録されているか */ \
    beq     label##_call_no_reg_CPM_interrupt; /*  チェック  */         \
    bctrl;                      /*  C言語ルーチン呼び出し  */           \
    mfmsr   r10;                                                        \
    xori    r11, r10, MSR_EE;   /*  EEビットをクリア  */                \
    mtmsr   r11;        /*  割込み禁止  */                              \
    /*  インサービスレジスタCISRの該当ビットをクリア  */                \
    /*      （１を書き込むとクリアされる）  */                          \
    lwz     r4, 0(sp);  /*  ベクタ番号×4を読み出し  */                 \
    addi    sp, sp, STACK_MARGIN;                                       \
    srwi    r5, r4, 2;  /*  r5←r4>>2（割込み番号）  */                 \
    li      r6, 1;                                                      \
    slw     r7, r6, r5; /*  r7←1<<(割込み番号)  */                     \
    lis     r3, IMMR_UPPER_2BYTE; /* 内部レジスタ領域の先頭アドレス */  \
    stw     r7, TADR_CPM_CISR(r3);                                      \
label##_exit:    /*  PROC_ICUの出口ラベル  */

/* マクロ引数labelを確実に展開するため、マクロを2重にしている */
#define PROC_ICU(label) _PROC_ICU(label)


/*
 * 割り込みコントローラのマスクIPM関連の定義
 *  　　MPC860ではシステムインターフェースユニットSIUの割込みマスク
 *  　　（SIU割込みマスクレジスタSIMASK）のみカーネルで管理する。
 *  　　通信プロセッサモジュールCPMの割込みマスクの管理はすべてハード
 *  　　ウェアが行っている。
 */

/*  IPMの待避  */
#define PUSH_ICU_IPM                	\
    lis     r4, IMMR_UPPER_2BYTE;      	\
    lhz     r5, TADR_SIU_SIMASK(r4);    \
    sthu    r5, -4(sp)
    /*  ストアとspの更新が1命令で実行される  */
    /*  保存するデータは2バイトであるが、  */
    /*  spを4バイト境界に維持するため、オフセットを-4としている  */

/*  IPMの復元  */
#define POP_ICU_IPM                 			\
    lhz     r4, 0(sp);              			\
    lis     r5, IMMR_UPPER_2BYTE;   			\
    addi    sp, sp, 4;           /*  上記参照  */	\
    sth     r4, TADR_SIU_SIMASK(r5) 


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
	/*  MPC860では何もチェックしない  */

/*
 * 割り込みコントローラのマスク取得
 */
#define GET_IPM(p_ipm)	*(p_ipm) = sil_reh_mem((VP)SIMASK)

/*
 * 割り込みコントローラのマスク設定
 */
#define CHG_IPM(ipm)	sil_wrh_mem((VP)SIMASK, ipm)

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
 *
 *  　・割込みから戻ってきた後に割込み禁止も行う
 *  　・r0〜r4の内容を破壊してはならない
 *  　
 *  　MCP860ではドーズ・ロー・モードを用いる
 *  　
 *  　レジスタ割り当て
 *  　　r0：値０
 *  　　r5：内部レジスタの先頭アドレス
 *  　　r6：ロック・キーワード0x55ccaa33
 */ 
#define SAVE_POWPER                                                 \
    lis     r5, IMMR_UPPER_2BYTE;                                   \
    LI32(r6, UNLOCK_KEY);                                           \
    stw     r6, TADR_SCCRK(r5);   /* SCCRアンロック */              \
                                                                    \
    /* CRQEN=1：CPが動作するとき、ドーズ・ハイ・モードへ    */      \
    /* PRQEN=1：割込み要求でノーマル・ハイ・モードへ        */      \
    lwz     r7, TADR_SCCR(r5);                                      \
    oris    r7, r7, (SCCR_CRQEN | SCCR_PRQEN)>>16;                  \
    stw     r7, TADR_SCCR(r5);                                      \
                                                                    \
    stw     r0, TADR_SCCRK(r5);     /* SCCRロック */                \
                                                                    \
    stw     r6, TADR_PLPRCRK(r5);   /* PLPRCアンロック */           \
                                                                    \
    /* PLPRCR                                   */                  \
    /*    TMIST=0：割込みソースをクリアしても   */                  \
    /*           ドーズ・ロー・モードに戻らない */                  \
    lwz     r8, TADR_PLPRCR(r5);                                    \
    ori     r8, r8, PLPRCR_TIMIST;                                  \
    xori    r8, r8, PLPRCR_TIMIST;                                  \
    stw     r8, TADR_PLPRCR(r5);                                    \
                                                                    \
    /* ドーズ・ロー・モードに遷移するための値設定   */              \
    /*      CSRC=1：                                */              \
    /*      LPM=01：                                */              \
    ori     r8, r8, (PLPRCR_CSRC | PLPRCR_LPM);                     \
    xori    r8, r8, PLPRCR_LPM10;                                   \
    stw     r8, TADR_PLPRCR(r5);                                    \
                                                                    \
    stw     r0, TADR_PLPRCRK(r5);   /* PLPRCロック */               \
                                                                    \
    /* 割込み許可とドーズ・ロー・モードへの移行 */                  \
    mfmsr   r9;                                                     \
    ori     r9, r9, MSR_EE;         /*  EEビットセット  */          \
    oris    r9, r9, MSR_POW>>16;    /*  POWビットセット  */         \
    mtmsr   r9;                                                     \
    /*  ここで割込みが入る（ドーズ・ロー・モードで待つ）  */        \
    /*  　割込みから戻ったときはr9も元の値になっている  */          \
                                                                    \
    /*  割込み禁止  */                                              \
    xori    r9, r9, MSR_EE;         /*  EEビットクリア  */          \
    xoris   r9, r9, MSR_POW>>16;    /*  POWビットクリア  */         \
    mtmsr   r9


/*
 * インプリメンテーション固有の例外0x01000の処理内容を定義したマクロ
 * 　
 * マクロIMPLEMENT_EXCEPTION_01000が定義されている場合のみ有効となる。
 * 処理内容はプロセッサのインプリメンテーション固有のものなので、
 * カーネルによる出入口処理は挿入されず、このマクロで定義した処理内容が
 * 直接呼び出される。（つまり、カーネルの管理外扱い）
 * そのため、この処理中でカーネルの機能を呼び出した場合の動作は未定義で
 * ある。
 * 　
 * オフセット0x01000以降に配置される例外要因は１つとは限らない。2つ以上
 * の例外ベクタが配置される場合、オフセットの指定方法に注意すること。
 * 　具体的には
 *     .org オフセット - EXCEPTION_VECTOR_BASE
 * のように指定する。詳しくはcpu_support.Sの該当個所を参照。
 * 　
 */
#define MAKE_EXCEPTION_VECTOR(offset, name)	\
    .org offset - EXCEPTION_VECTOR_BASE;	\
name:;						\
  MAKE_EXCEPTION_ENTRY(offset)


#define IMPLEMENT_EXCEPTION_01000_PROC				\
software_emulation:;						\
  MAKE_EXCEPTION_ENTRY(0x1000);					\
								\
MAKE_EXCEPTION_VECTOR(0x1100, instruction_TLB_miss);		\
MAKE_EXCEPTION_VECTOR(0x1200, data_TLB_miss);			\
MAKE_EXCEPTION_VECTOR(0x1300, instruction_TLB_error);		\
MAKE_EXCEPTION_VECTOR(0x1400, data_TLB_error);			\
MAKE_EXCEPTION_VECTOR(0x1c00, data_break_point);		\
MAKE_EXCEPTION_VECTOR(0x1d00, instruction_break_point);		\
MAKE_EXCEPTION_VECTOR(0x1e00, peripheral_break_point);		\
MAKE_EXCEPTION_VECTOR(0x1f00, non_maskable_developement_port)


#endif /* _SYS_CONFIG_H_ */

/*  end of file  */
