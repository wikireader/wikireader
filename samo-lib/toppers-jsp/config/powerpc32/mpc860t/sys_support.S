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
 *  @(#) $Id: sys_support.S,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール アセンブリ言語部
 *　　　　　カーネル内部で使用する定義（MPC860T TB6102S用）
 *  
 *  　　ボード依存を分離すると、このファイルはほとんど空になって
 *  　　しまうので、そのままにしている。
 *  
 */

#define _MACRO_ONLY
#include "jsp_kernel.h"

/*
 *  メモリコマンドレジスタMCRに設定するWRITEオペコード
 *
 *  　　　　OP=00：WRITE
 *  　　　　UM=0：UPMA
 *  　　　　MB=000：RUNコマンドのみ有効
 *  　　　　MCLF=0：RUNコマンドのみ有効
 *  　　　　MAD=0：メモリアレイインデックス
 *  　　　　　　　　（空にしておいて、別のデータを入れる）
 *  
 */
#define MCR_WRITE_OP	0

/*
 *  低レベルのターゲットシステム依存の初期化
 *
 *  スタートアップモジュールの中で，メモリの初期化の前に呼び出される．
 *  
 *  レジスタ割り当て
 *  　　r10：内部レジスタ領域の先頭アドレス
 *  
 */
    .text
    .globl hardware_init_hook
hardware_init_hook: 
    /*  ipm_tableをSPRG3にコピー  */
    LI32(r4, ipm_table)
    mtspr   SPRG3, r4

    /*  内部メモリマップレジスタIMMRの初期化  */
    /* 　内部レジスタ領域の先頭アドレスの設定 */  
    lis     r10, IMMR_UPPER_2BYTE /*  上位2バイト  */
    mtspr   IMMR, r10   /*  下位2バイトは上書きしても破壊されない  */
    
#ifdef DOWNLOAD_TO_RAM      /*  デバッグ用  */
    
    /*
     *  RAM上にダウンロードする場合
     *
     *  　　例外ベクタのオフセットを0x000x,xxxxにする。
     */
    li      r5, 0
    mtmsr   r5		/*  MSR.IP←0  */
    blr
    
#else /* DOWNLOAD_TO_RAM */ /*  ROM化  */
    

    /*  バスコントローラの初期化  */    

    /*
     *  ソフトウェア・ウォッチドックタイマの初期化
     */

    /*  ウォッチドックタイマをクリア  */
    li      r6, SWSR_CLEAR1
    sth     r6, TADR_SIU_SWSR(r10)
    ori     r6, r0, SWSR_CLEAR2
    sth     r6, TADR_SIU_SWSR(r10)
    
    /*
     *  　ウォッチドックタイマをディセーブルにする
     *  　  SYPCR.SWE←0
     */
    lwz     r5, TADR_SIU_SYPCR(r10)
    ori	    r5, r5, SYPCR_SWE
    xori    r5, r5, SYPCR_SWE
    stw     r5, TADR_SIU_SYPCR(r10)


    /*
     *  FlashROMバスコントローラの初期化
     *  　2MB
     *  　アドレスマップ：0x1000,0000-0x101f,ffff
     *  　チップセレクト信号：CS0
     *  　メモリコントローラ：汎用チップ・セレクト・マシンGPCM
     *  　16ビットアクセス
     *  　
     *  　ブートチップセレクトCS0に関しては
     *  　BR0→OR0の順に設定しなければならない
     *  　
     */

    /*
     *  外部バス分周係数の設定
     *  　SCCR.EBDF=00：CLKOUTはGCLK2の1の分周比
     */
    LI32(r4, UNLOCK_KEY)
    stw     r4, TADR_SCCRK(r10)     /* SCCRアンロック */
    lwz     r5, TADR_SCCR(r10)
    oris    r6, r5, (SCCR_EBDF>>16)
    xoris   r7, r6, (SCCR_EBDF>>16)
    stw     r7, TADR_SCCR(r10)
    li      r0, 0
    stw     r0, TADR_SCCRK(r10)     /* SCCRロック */


    /*
     *  ベースレジスタBR0 = 0x1000,0801
     *  　　BA=0x1000(0b)：アクセス範囲 0x1000,0000〜
     *  　　PS=10：ポートサイズ 16ビット
     *  　　PARE=0：パリティ・ディセーブル
     *  　　WP=0：リード／ライト許可（保護なし）　要検討
     *  　　MS=00：UPMAを使用
     *  　　V=1：BR0,OR0が有効
     *
     *　　　ハードウェア・マニュアルにはBR0はリードオンリーと
     *　　　あるが、誤訳らしい
     */
    LI32(r3, 0x10000801)
    stw     r3, TADR_BR0(r10)

    /*
     *  オプション・レジスタOR0 = 0xffe0,0936
     *  　　MA=0xffe0(0b)：アクセス範囲21ビット（＝2MB）
     *  　　CSNT=1：チップ・セレクト・ネゲート期間
     *  　　ACS=00：CSをアドレスラインと同時に出力
     *  　　BIH=1：バースト禁止（GPCMならセットしなければならない）
     *  　　SYC=3：サイクル長
     *  　　SETA=0：内部／外部の転送アクノリッジは最初のアクセスを認識
     *  　　TRLX=1：タイミング緩和
     *  　　EHTR=1：リード時のホールド時間延長
     */
    LI32(r4, 0xffe00936)
    stw     r4, TADR_OR0(r10)

    /*  MSR.IP=0(A20=L)の場合の絶対番地にジャンプ  */
    LI32(r5, activate_A20_line)
    mtctr   r5
    bctr
    
    /*  A20信号の活性化  */
activate_A20_line:
    /*  汎用I/O機能選択：PBPAR.DD27←0  */
    lhz     r6, (TADR_PBPAR + 2)(r10)
    andi.   r6, r6, (~PBPAR_DD27 & 0xffff)
    sth     r6, (TADR_PBPAR + 2)(r10)

    /*  入出力方向選択（出力）：PBDIR.DR27←1  */
    lhz     r7, (TADR_PBDIR + 2)(r10)
    ori     r7, r7, PBDIR_DR27
    sth     r7, (TADR_PBDIR + 2)(r10)
    
    /*  アクティブにドライブ：PBODR.OD27←0  */
    lhz     r8, (TADR_PBODR + 2)(r10)
    andi.   r8, r8, (~PBODR_OD27 & 0xffff)
    sth     r8, (TADR_PBODR + 2)(r10)
    
    /*  A20=1を出力：PBDAT.D27←1  */
    lhz     r9, (TADR_PBDAT + 2)(r10)
    ori     r9, r9, PBDAT_D27
    sth     r9, (TADR_PBDAT + 2)(r10)
    nop
    nop
    nop


    /*
     *  SDRAMバスコントローラの初期化
     *  　50MHz
     *  　16MB
     *  　アドレスマップ：0x0000,0000-0x00ff,ffff
     *  　チップセレクト信号：CS2
     *  　メモリコントローラ：ユーザ・プログラマブル・マシンUPMA
     *  　32ビットアクセス
     *  　
     *  　レジスタ割り当て
     *  　　r10：内部レジスタ領域の先頭アドレス
     *  　
     *  　備考
     *  　　　UPMレジスタ→ORx→BRxの順に設定する
     *  　
     *  　
     */

    /*
     *  メモリコントローラUPMAの設定レジスタ初期化
     *  　
     *  　リフレッシュ周期＝1CLK周期×プリスケーラ値×タイマ周期
     *  　　　　　　　　　＝20nsec　×　　 32 　　　×　24
     *  　　　　　　　　　＝15.36μsec
     */

    /*
     *  メモリ周期タイマ・プリスケーラ・レジスタMPTPR = 0x0200
     *  　　PTP=0x2：32による分周
     */
    ori     r3, r0, 0x0200
    sth     r3, TADR_MPTPR(r10)

    /*
     *  マシンAモード・レジスタMAMR = 0x1880,2111
     *  　　PTx=24：リフレッシュ周期
     *  　　PTxE=1：周期タイマイネーブル
     *  　　AMx=0：アドレス多重サイズ
     *  　　DSx=00：ディセーブル期間 1サイクル
     *  　　G0CLx=001：GPL0に対するアドレスライン出力はA11
     *  　　GPLx4DIS=0：UPWAIT/GPL_x4はGPL_x4に定義
     *  　　RLFx=0001：リードサイクルのループ 1回
     *  　　WLFx=0001：ライトサイクルのループ 1回
     *  　　TLFx=0001：周期タイマサービスサイクルのループ 1回
     */
    LI32(r4, 0x18802111)
    stw     r4, TADR_MAMR(r10)

    /*
     *  オプション・レジスタOR2 = 0xff00,0600
     *  　　MA=0xff00(0b)：アクセス範囲24ビット（＝16MB）
     *  　　SAM=0：多重化アドレスなし
     *  　　G5LA=1：GPL_A5で内部GPL5信号を出力
     *  　　G5LS=1：GPL5はGCLK1_50の立ち上がりエッジでHにドライブ
     *  　　BIH=0：バーストアクセスをサポート
     */
    LI32(r5, 0xff000600)
    stw     r5, TADR_OR2(r10)

    /*
     *  ベース・レジスタBR2 = 0x0000,0081
     *  　　BA=0x0000(0b)：アクセス範囲 0x0000,0000〜
     *  　　PS=00：ポートサイズ 32ビット
     *  　　PARE=0：パリティ・ディセーブル
     *  　　WP=0：リード／ライト許可（保護なし）
     *  　　MS=10：UPMAを使用
     *  　　V=1：BR2,OR2が有効
     */
    ori     r6, r0, 0x0081
    stw     r6, TADR_BR2(r10)
    
    /*
     *  RAMアレイの初期化
     *  　RAMアレイはMPC860の内蔵メモリであり、外部メモリにアクセス
     *  　する際の各サイクルにおけるピンの状態を規定する。
     *  　
     *  　レジスタ割り当て
     *  　　r3：ポインタテーブルへのポインタ
     *  　　r4：書き込むワード数（残り数）
     *  　　r5：元データのアドレス
     *  　　r6：転送先のインデックス
     *  　　r10：内部レジスタ領域の先頭アドレス
     */
	    /*  　lwzu命令が使いやすくなるよう  */
	    /*  　アドレスを-4している  */
    LI32(r3, (RAM_array_index-4))

    /*  ブロックデータ書き込みループ  */
RAM_array_block_init_loop:
    lwzu    r4, 4(r3)       /*  ワード数  */
    			    /*    r3がインクリメントされる点に注意 */
    cmpwi   crf0, r4, 0     /*  テーブルに指定されたワード数が  */
    beq     SDRAM_init      /*  ゼロならば、デバイスの初期化へ  */
    lwzu    r5, 4(r3)       /*  元データのアドレス  */
    			    /*  　lwzu命令が使いやすくなるよう  */
    			    /*  　アドレスは-4されて格納されている  */
    lwzu    r6, 4(r3)       /*  転送先のインデックス  */
    			    /*    r3がインクリメントされる点に注意 */

    /*
     *  ワードデータ書き込みループ
     *  　メモリデータレジスタMDRに書き込んだデータが
     *  　WRITEオペコードによりRAMアレイに転送される
     */
RAM_array_word_init_loop:
    lwzu    r8, 4(r5)   /*  元データ  */
			/*    r5がインクリメントされる点に注意 */
    stw     r8, TADR_MDR(r10)   /*  書き込みデータ準備  */
    ori     r9, r6, MCR_WRITE_OP
    stw     r9, TADR_MCR(r10)   /*  WRITEコマンド発行  */
    subi    r4, r4, 1
    addi    r6, r6, 1
    cmpwi   crf0, r4, 0         /*  書き込むべきデータが残っていれば  */
    bne     RAM_array_word_init_loop    /*  次のワードデータの処理へ  */
                    /*  データが残っていなければ、次のブロックの処理へ  */
    b       RAM_array_block_init_loop


    /*  SDRAM（デバイス自体）の初期化  */
SDRAM_init: 
    /*
     *  全バンク・プリチャージ
     *  
     *  　　メモリコマンドレジスタMCR = 0x8000,4105
     *  　　　　OP=10：RUN
     *  　　　　UM=0：UPMA
     *  　　　　MB=010：CS2
     *  　　　　MCLF=1：ループ1回
     *  　　　　MAD=5：メモリアレイインデックス
     *  　　　　　　　　（全バンクプリチャージの命令が入っている）
     */
    LI32(r3, 0x80004105)
    stw     r3, TADR_MCR(r10)
        
    /*
     *  オートリフレッシュ 8回
     *  
     *  　　メモリコマンドレジスタMCR = 0x8000,4830
     *  　　　　OP=10：RUN
     *  　　　　UM=0：UPMA
     *  　　　　MB=010：CS2
     *  　　　　MCLF=8：ループ 8回 （リフレッシュ回数）
     *  　　　　MAD=0x30：メモリアレイインデックス
     *  　　　　　　　　（オートリフレッシュの命令が入っている）
     */
    LI32(r4, 0x80004830)
    stw     r4, TADR_MCR(r10)
    
    
    /*
     *  動作モード設定
     *  　CASレイテンシ：2
     *  　バースト長　 ：4
     *  
     *  　　メモリアドレスレジスタMAR = 0x0000,0088
     *  　　　　アドレスバスに出力され、SDRAMへの命令となる
     *  
     *  　　メモリコマンドレジスタMCR = 0x8000,4106
     *  　　　　OP=10：RUN
     *  　　　　UM=0：UPMA
     *  　　　　MB=010：CS2
     *  　　　　MCLF=1：ループ 1回 
     *  　　　　MAD=6：メモリアレイインデックス
     *  　　　　　　　　（動作モード設定の命令が入っている）
     */
    ori     r5, r0, 0x0088
    stw     r5, TADR_MAR(r10)
    LI32(r6, 0x80004106)
    stw     r6, TADR_MCR(r10)
    

/*
 *  例外ベクタのコピー
 */
copy_exception_vector:
	/*  lwzu命令を使うため-4している  */
	LI32(r7, EXCEPTION_VECTOR_START_SRC - 4)
	LI32(r8, EXCEPTION_VECTOR_END_SRC - 4)
	LI32(r9, EXCEPTION_VECTOR_START_DST - 4)
copy_exception_vector_1:
	lwzu	r10, 4(r7)	/*  r7の更新も1命令で実行される  */
	stwu	r10, 4(r9)	/*  r9の更新も1命令で実行される  */
	cmpw	crf0, r7, r8
	blt	copy_exception_vector_1
	
        /*
         *  MSR.IP←0
         *　　例外プレフィックス：0x000n,nnnn
         */
	li	r3, MSR_IP
	mfmsr	r4
	or	r4, r4, r3
	xor	r4, r4, r3
	mtmsr	r4
	
	LI32(r10, start_1)
	mtctr	r10
	bctr
	nop
	nop
	nop
	nop

/*
 *  RAMアレイの初期化に使用するデータへのポインタテーブル
 *  　ワード数、元データのアドレス、転送先のインデックスで
 *  　1セット（1ブロック分）
 *  　元データのアドレスはlwzu命令が使いやすくなるよう
 *  　-4している。
 *  　
 *  　最後はワード数０のデータを配置し、ループ終了の判定に用いる
 */
RAM_array_index:    
    /*
     *  リード・シングル・ビート要求（計 16ワード）
     *  　　・リード・シングル・ビート要求（5ワード）
     *  　　・全バンクプリチャージ（1ワード）
     *  　　・モード設定（2ワード）
     *  　　・リード・バースト要求（8ワード）
     */
    .int    16                  /*  ワード数  */
    .int    RAM_array_RSS - 4   /*  元データのアドレス  */
    .int    0x0                 /*  転送先のインデックス  */

    /*
     *  ライト・シングル・ビート要求
     */
    .int    4                   /*  ワード数  */
    .int    RAM_array_WSS - 4   /*  元データのアドレス  */
    .int    0x18                /*  転送先のインデックス  */

    /*
     *  ライト・バースト要求
     */
    .int    7                   /*  ワード数  */
    .int    RAM_array_WBS - 4   /*  元データのアドレス  */
    .int    0x20                /*  転送先のインデックス  */

    /*
     *  周期タイマ要求（オートリフレッシュ）
     */
    .int    5                   /*  ワード数  */
    .int    RAM_array_TPS - 4   /*  元データのアドレス  */
    .int    0x30                /*  転送先のインデックス  */

    /*
     *  例外条件要求
     */
    .int    1                   /*  ワード数  */
    .int    RAM_array_EXS - 4   /*  元データのアドレス  */
    .int    0x3c                /*  転送先のインデックス  */

    /*
     *  ループ終了判定用データ
     */
    .int    0                   /*  ワード数  */
    

/*
 *  RAMアレイにコピーするデータ
 */

    /*
     *  以下の４つの命令群を連続した領域に配置（計 16ワード）
     *  　　・リード・シングル・ビート要求（5ワード）
     *  　　・全バンクプリチャージ（1ワード）
     *  　　・モード設定（2ワード）
     *  　　・リード・バースト要求（8ワード）
     */
RAM_array_RSS:  
    /*  リード・シングル・ビート要求  */
    .int    0x1f07fc04                      /*  0  */
    .int    0xeeaef004                      /*  1  */
    .int    0x11adf004                      /*  2  */
    .int    0xefbbbc00                      /*  3  */
    .int    0x1ff77c47                      /*  4  */
    /*  全バンクプリチャージ  */
    .int    0x1ff77c07                      /*  5  */
    /*  モード設定用データ  */
    .int    0xefcab034                      /*  6  */
    .int    0x1f357405                      /*  7  */
    /*  リード・バースト要求  */
    .int    0x1f07fc04                      /*  8  */
    .int    0xeeaef004                      /*  9  */
    .int    0x10adf004                      /*  a  */
    .int    0xf0affc00                      /*  b  */
    .int    0xf0affc00                      /*  c  */
    .int    0xf1affc00                      /*  d  */
    .int    0xefbbbc00                      /*  e  */
    .int    0x1ff77c47                      /*  f  */

    /*
     *  ライト・シングル・ビート要求
     */
RAM_array_WSS:
    .int    0x1f07fc04                      /*  0x18 + 0  */
    .int    0xeeaeb000                      /*  0x18 + 1  */
    .int    0x01b93004                      /*  0x18 + 2  */
    .int    0x1ff77c47                      /*  0x18 + 3  */

    /*
     *  ライト・バースト要求
     */
RAM_array_WBS:
    .int    0x1f07fc04                      /*  0x20 + 0  */
    .int    0xeeaeb000                      /*  0x20 + 1  */
    .int    0x10ad7000                      /*  0x20 + 2  */
    .int    0xf0affc00                      /*  0x20 + 3  */
    .int    0xf0affc00                      /*  0x20 + 4  */
    .int    0xe1bbbc04                      /*  0x20 + 5  */
    .int    0x1ff77c47                      /*  0x20 + 6  */

    /*
     *  周期タイマ要求（オートリフレッシュ）
     */
RAM_array_TPS:
    .int    0x1ff5fc84                      /*  0x30 + 0  */
    .int    0xfffffc04                      /*  0x30 + 1  */
    .int    0xfffffc04                      /*  0x30 + 2  */
    .int    0xfffffc84                      /*  0x30 + 3  */
    .int    0xfffffc05                      /*  0x30 + 4  */

    /*
     *  例外条件要求
     */
RAM_array_EXS:
    .int    0xfffffc07                      /*  0x3c + 0  */


#endif  /*  DOWNLOAD_TO_RAM  */
/*  end of file  */
