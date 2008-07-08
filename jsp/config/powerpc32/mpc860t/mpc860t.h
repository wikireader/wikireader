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
 *  @(#) $Id: mpc860t.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *  MPC860のハードウェア資源の定義
 */

#ifndef _MPC860_H_
#define _MPC860_H_

/*
 *  割込みの本数
 */
#define TMAX_SIU_INTNO      16  /*  システムインターフェースユニットSIU  */
#define TMAX_CPM_INTNO      32  /*  通信プロセッサモジュールCPM  */
                                /*  MPC860全体  */
#define TMAX_INTNO          (TMAX_SIU_INTNO+TMAX_CPM_INTNO)

/*
 *  割込み関連の定義
 *  　割込み要因毎に3種類の値を管理している
 *  　１．割込み番号
 *  　２．SIUへの割込み要求レベル
 *  　　　内部レベルの設定
 *  　　　CPM割込みは全体でひとまとめの扱い
 *  　　　　(1) No.：0〜7の数値
 *  　　　　　　　人間には分かりやすいが、プログラム中では
 *  　　　　　　　直接、使用されない
 *  　　　　(1-2) 割込み要因の識別
 *  　　　　　　　擬似ベクタテーブルのインデックスとして読み出す
 *  　　　　　　　だけなので、即値としてマクロ定義しておく必要はない
 *  　　　　　　　予め2ビットシフトされている
 *  　　　　(2) 1バイトのビットパターン 
 *  　　　　　　　割込み要求レベルを設定する際にレジスタにセットする値
 *  　　　　　　　該当する1ビットだけをセットする
 *  　３．SIUのIPM
 *  　　　2バイトのビットパターン
 *  　　　CPM割込みは全体でひとまとめの扱い
 *  　　　　(1) 割込みマスク
 *  　　　　　　　SIU割込みレジスタSIMASKに設定する値
 *  　　　　　　　自分よりレベルの低い割込み要求をすべてマスクする
 *  　　　　(2) 保留中の割込み要因
 *  　　　　　　　該当するビットだけがセットされている
 *  
 *  　内部レベル2-(1)から割込み番号、割込みレベル設定用ビットパターン、
 *  　IPMを生成する方針で実装
 *  
 */

/*
 *  割込み番号の定義
 *      ハードウェアマニュアルにはSIU割込みソースの16〜31番は予約済
 *      みとあるが、擬似ベクタテーブルが無駄に大きくなるだけなので、
 *      CPM割込みソースを割り当てている
 *      
 *      割込み番号と優先度の関係がSIU割込みとCPM割込み（デフォルト）
 *      で逆なので注意
 *      　SIU割込み
 *      　　割込み番号　　0：最上位
 *      　　割込み番号　 15：最下位
 *      　CPM割込み（デフォルト）
 *      　　割込み番号　 0x0：最下位
 *      　　割込み番号　0x1f：最上位
 */

    /*  SIU割込みソース  */
#define INTNO_IRQ0      0x0     /*  IRQ0  */
#define INTNO_LVL0      0x1     /*  内部レベル0  */
#define INTNO_IRQ1      0x2     /*  IRQ1  */
#define INTNO_LVL1      0x3     /*  内部レベル1  */
#define INTNO_IRQ2      0x4     /*  IRQ2  */
#define INTNO_LVL2      0x5     /*  内部レベル2  */
#define INTNO_IRQ3      0x6     /*  IRQ3  */
#define INTNO_LVL3      0x7     /*  内部レベル3  */
#define INTNO_IRQ4      0x8     /*  IRQ4  */
#define INTNO_LVL4      0x9     /*  内部レベル4  */
#define INTNO_IRQ5      0xa     /*  IRQ5  */
#define INTNO_LVL5      0xb     /*  内部レベル5  */
#define INTNO_IRQ6      0xc     /*  IRQ6 */
#define INTNO_LVL6      0xd     /*  内部レベル6  */
#define INTNO_IRQ7      0xe     /*  IRQ7  */
#define INTNO_LVL7      0xf     /*  内部レベル7  */

    /*  CPM割込みソース  */
#define INTNO_ERR       0x10    /*  エラー：0x0  */
#define INTNO_PC4       0x11    /*  パラレルI/O PC4：0x1  */
#define INTNO_PC5       0x12    /*  パラレルI/O PC5：0x2  */
#define INTNO_SMC2      0x13    /*  SMC2/PIP：0x3  */
#define INTNO_SMC1      0x14    /*  SMC1：0x4  */
#define INTNO_SPI       0x15    /*  SPI：0x5  */
#define INTNO_PC6       0x16    /*  パラレルI/O PC6：0x6  */
#define INTNO_TIMER4    0x17    /*  タイマ4：0x7  */
/*  CPM割込み番号8は予約済み  */
#define INTNO_PC7       0x19    /*  パラレルI/O PC7：0x9  */
#define INTNO_PC8       0x1a    /*  パラレルI/O PC8：0xa  */
#define INTNO_PC9       0x1b    /*  パラレルI/O PC9：0xb  */
#define INTNO_TIMER3    0x1c    /*  タイマ3：0xc  */
/*  CPM割込み番号0xdは予約済み  */
#define INTNO_PC10      0x1e    /*  パラレルI/O PC10：0xe  */
#define INTNO_PC11      0x1f    /*  パラレルI/O PC11：0xf  */
#define INTNO_I2C       0x20    /*  I2C：0x10  */
#define INTNO_RISC      0x21    /*  RISCタイマテーブル：0x11  */
#define INTNO_TIMER2    0x22    /*  タイマ2：0x12  */
/*  CPM割込み番号0x13は予約済み  */
#define INTNO_IDMA2     0x24    /*  IDMA2：0x14  */
#define INTNO_IDMA1     0x25    /*  IDMA1：0x15  */
#define INTNO_SDMA      0x26    /*  SDMAチャネルバスエラー：0x16  */
#define INTNO_PC12      0x27    /*  パラレルI/O PC12：0x17  */
#define INTNO_PC13      0x28    /*  パラレルI/O PC13：0x18  */
#define INTNO_TIMER1    0x29    /*  タイマ1：0x19  */
#define INTNO_PC14      0x2a    /*  パラレルI/O PC14：  */
#define INTNO_SCC4      0x2b    /*  SCC4：0x1a  */
#define INTNO_SCC3      0x2c    /*  SCC3：0x1c  */
#define INTNO_SCC2      0x2d    /*  SCC2：0x1d  */
#define INTNO_SCC1      0x2e    /*  SCC1：0x1e  */
#define INTNO_PC15      0x2f    /*  パラレルI/O PC15：0x1f  */

/*
 *  SIUのIPMの定義
 *    2バイトのビットパターン
 *    ハードウェアのデフォルトの優先度に合わせている
 */
#define IPM_IRQ0	0x00u			/*  IRQ0  */
#define IPM_LVL0	BIT0_16			/*  内部レベル0  */
#define IPM_IRQ1	(IPM_LVL0 | BIT1_16)	/*  IRQ1  */
#define IPM_LVL1	(IPM_IRQ1 | BIT2_16)	/*  内部レベル1  */
#define IPM_IRQ2	(IPM_LVL1 | BIT3_16)	/*  IRQ2  */
#define IPM_LVL2	(IPM_IRQ2 | BIT4_16)	/*  内部レベル2  */
#define IPM_IRQ3	(IPM_LVL2 | BIT5_16)	/*  IRQ3  */
#define IPM_LVL3	(IPM_IRQ3 | BIT6_16)	/*  内部レベル3  */
#define IPM_IRQ4	(IPM_LVL3 | BIT7_16)	/*  IRQ4  */
#define IPM_LVL4	(IPM_IRQ4 | BIT8_16)	/*  内部レベル4  */
#define IPM_IRQ5	(IPM_LVL4 | BIT9_16)	/*  IRQ5  */
#define IPM_LVL5	(IPM_IRQ5 | BIT10_16)	/*  内部レベル5  */
#define IPM_IRQ6	(IPM_LVL5 | BIT11_16)	/*  IRQ6  */
#define IPM_LVL6	(IPM_IRQ6 | BIT12_16)	/*  内部レベル6  */
#define IPM_IRQ7	(IPM_LVL6 | BIT13_16)	/*  IRQ7  */
#define IPM_LVL7	(IPM_IRQ7 | BIT14_16)	/*  内部レベル7  */


/*  割込みレベルから割込み番号への変換  */
#define _LEVEL_TO_INHNO(level)      INTNO_LVL##level
#define  LEVEL_TO_INHNO(level)      _LEVEL_TO_INHNO(level)

/*  割込みレベルから登録用ビットパターンへの変換  */
#define _LEVEL_TO_BIT_PATTERN(level)      (0x1<<(7-(level)))
#define  LEVEL_TO_BIT_PATTERN(level)      _LEVEL_TO_BIT_PATTERN(level)

/*  割込みレベルからIPMへの変換  */
#define _LEVEL_TO_IPM(level)      IPM_LVL##level
#define  LEVEL_TO_IPM(level)      _LEVEL_TO_IPM(level)

/*  割込みレベルから割込み許可ビットへの変換  */
#define _LEVEL_TO_ENABLE_BIT(level)  SIMASK_LVM##level
#define  LEVEL_TO_ENABLE_BIT(level)  _LEVEL_TO_ENABLE_BIT(level)


/*
 *  CPUの内部レジスタ
 */

/*
 *   8. 命令キャッシュ・データキャッシュ関連の定義
 */

/*  制御レジスタ */

/*  命令キャッシュ関連 */
#define IC_CST	560	/*  制御およびステータス・レジスタ */
#define IC_ADR	561	/*  アドレス・レジスタ  */
#define IC_DAT	562	/*  データ・ポート・レジスタ  */

/*  データ・キャッシュ関連 */
#define DC_CST	568	/*  制御およびステータス・レジスタ */
#define DC_ADR	569	/*  アドレス・レジスタ  */
#define DC_DAT	570	/*  データ・ポート・レジスタ  */

/*
 *   9. メモリ管理ユニットMMU関連の定義
 */
/*  制御レジスタ  */
#define MI_CTR	784	/*  IMMU 制御レジスタ  */
#define MD_CTR	792	/*  DMMU 制御レジスタ  */

/*  TLBソース・レジスタ  */
#define MI_EPN	787	/*  IMMU 実効ページ番号レジスタ  */
#define MD_EPN	795	/*  DMMU 実効ページ番号レジスタ  */
#define MI_TWC	789	/*  IMMU テーブルウォーク制御レジスタ  */
#define MD_TWC	797	/*  DMMU テーブルウォーク制御レジスタ  */
#define MI_RPN	790	/*  IMMU 実（物理）ページ番号ポート  */
#define MD_RPN	798	/*  DMMU 実（物理）ページ番号ポート  */

/*  テーブルウォーク・ベース・レジスタ  */
#define M_TWD	796	/*  MMU テーブルウォーク・ベース・レジスタ  */

/*  保護レジスタ  */
#define M_CASID	793	/*  MMU カレント・アドレス空間ID レジスタ  */
#define MI_AP	786	/*  IMMU アクセス保護レジスタ  */
#define MD_AP	794	/*  DMMU アクセス保護レジスタ  */

/*  スクラッチ・レジスタ  */
#define M_TB	799	/*  MMU テーブルウォーク・スペシャル・レジスタ  */

/*  デバッグ・レジスタ  */
#define MI_CAM	816	/*  IMMU CAM エントリ・リード・レジスタ  */
#define MI_RAM0	817	/*  IMMU RAM エントリ・リード・レジスタ 0  */
#define MI_RAM1	818	/*  IMMU RAM エントリ・リード・レジスタ 1  */
#define MD_CAM	824	/*  DMMU CAM エントリ・リード・レジスタ  */
#define MD_RAM0	825	/*  DMMU RAM エントリ・リード・レジスタ 0  */
#define MD_RAM1	826	/*  DMMU RAM エントリ・リード・レジスタ 1  */

/*
 *  11. システムインターフェースユニットSIU関連の定義
 */


/*
 *  内部メモリマップレジスタ
 *  　　内部レジスタ群の先頭アドレス
 *  　　
 *  　　　リセット直後は0x0000,0000番地を指しているがSDRAMと
 *  　　　ぶつかるので、別のアドレスに変える必要がある
 */
#define IMMR        638                 /*  レジスタ番号  */
#define IMMR_UPPER_2BYTE  0xff00              /*  上位16ビット  */
#define IMMR_BASE   (IMMR_UPPER_2BYTE << 16)  /*  ベースアドレス  */  

#define TADR_SIU_SIUMCR  0x0    /* SIUモジュールコンフィギュレーション */
                                /*                     レジスタSIUMCR  */

/*
 *  11.5 SIU割込みコントローラ関連の定義
 */
#define TADR_SIU_SIPEND 0x10    /* SIU割込み保留レジスタSIPEND */
#define TADR_SIU_SIMASK 0x14    /* SIU割込みマスクレジスタSIMASK */

				/*  上位2バイトのみ使用する  */
#define SIMASK		(VH *)(IMMR_BASE + TADR_SIU_SIMASK)
#define SIMASK_IRM0	BIT0_16		/*  IRQ0  */
#define SIMASK_LVM0	BIT1_16		/*  内部レベル0  */
#define SIMASK_IRM1	BIT2_16		/*  IRQ1  */
#define SIMASK_LVM1	BIT3_16		/*  内部レベル1  */
#define SIMASK_IRM2	BIT4_16		/*  IRQ2  */
#define SIMASK_LVM2	BIT5_16		/*  内部レベル2  */
#define SIMASK_IRM3	BIT6_16		/*  IRQ3  */
#define SIMASK_LVM3	BIT7_16		/*  内部レベル3  */
#define SIMASK_IRM4	BIT8_16		/*  IRQ4  */
#define SIMASK_LVM4	BIT9_16		/*  内部レベル4  */
#define SIMASK_IRM5	BIT10_16	/*  IRQ5  */
#define SIMASK_LVM5	BIT11_16	/*  内部レベル5  */
#define SIMASK_IRM6	BIT12_16	/*  IRQ6  */
#define SIMASK_LVM6	BIT13_16	/*  内部レベル6  */
#define SIMASK_IRM7	BIT14_16	/*  IRQ7  */
#define SIMASK_LVM7	BIT15_16	/*  内部レベル7  */

#define TADR_SIU_SIEL   0x18    /* SIU割込みエッジ／レベルレジスタSIEL */
#define TADR_SIU_SIVEC  0x1c    /* SIU割込みベクタレジスタSIVEC */
#define TADR_SIU_TESR   0x20    /* 転送エラーステータスレジスタTESR */
#define TADR_SIU_SDCR   0x30    /* SDMAコンフィギュレーションレジスタSDCR */

/*
 *  11.7 ソフトウェア・ウォッチドックタイマ関連の定義
 */
#define TADR_SIU_SYPCR   0x4    /* システム保護コントロールレジスタSYPCR */
#define SYPCR_SWE 	 BIT29_32  /*  イネーブル  */
#define SYPCR_SWRI 	 BIT30_32  /*  リセット／割込み選択  */ 
#define TADR_SIU_SWSR    0xe       /* ソフトウェアサービスレジスタSWSR */
#define SWSR_CLEAR1      0x556c    /* クリア時に書き込む定数１ */
#define SWSR_CLEAR2      0xaa39    /* クリア時に書き込む定数２ */

/*
 *  12 リセット関連の定義
 */
#define TADR_SIU_RSR  0x288  /* リセット・ステータス・レジスタRSR */
#define TADR_SIU_RSRK 0x388  /* リセット・ステータス・レジスタ・キー */
#define RSR_EHRS BIT0_32    /* 外部ハード・リセット・ステータス */
#define RSR_ESRS BIT1_32    /* 外部ソフト・リセット・ステータス */
#define RSR_LLRS BIT2_32    /* ロック解除リセット・ステータス */
#define RSR_SWRS BIT3_32    /* ソフトウェア・ウォッチドック・リセット */
#define RSR_CSRS BIT4_32    /* チェック・ストップ・リセット・ステータス */
    		/* デバッグ・ポート・ハード・リセット・ステータス */
#define RSR_DBHRS BIT5_32
    		/* デバッグ・ポート・ソフト・リセット・ステータス */
#define RSR_DBSRS BIT6_32
#define RSR_JTRS BIT4_32    /* JTAGリセット・ステータス */


/*
 *  通信プロセッサモジュールCPM関連
 */

/*
 *  35. CPM割込みコントローラ関連の定義
 */
#define TADR_CPM_CIVR   0x930   /* CPM割込みベクタレジスタCIVR */
#define TADR_CPM_CICR   0x940   /* CPM割込みコンフィギュレーション */
                                /*                   レジスタCICR  */
#define TADR_CPM_CIPR   0x944   /* CPM割込み保留レジスタCIPR */
#define TADR_CPM_CIMR   0x948   /* CPM割込みマスクレジスタCIMR */
#define TADR_CPM_CISR   0x94c   /* CPM割込みインサービスレジスタCISR */

/* CPM割込みコンフィギュレーション レジスタCICR */
#define CICR            (VW *)(IMMR_BASE + TADR_CPM_CICR)
#define CICR_IEN        BIT24_32

/* CPM割込みマスクレジスタCIMR */
#define CIMR            (VW *)(IMMR_BASE + TADR_CPM_CIMR)
#define CIMR_PC15       BIT0_32     
#define CIMR_SCC1       BIT1_32     
#define CIMR_SCC2       BIT2_32     
#define CIMR_SCC3       BIT3_32     
#define CIMR_SCC4       BIT4_32     
#define CIMR_PC14       BIT5_32     
#define CIMR_TIMER1     BIT6_32     
#define CIMR_PC13       BIT7_32     
#define CIMR_PC12       BIT8_32     
#define CIMR_SDMA       BIT9_32     
#define CIMR_IDMA1      BIT10_32        
#define CIMR_IDMA2      BIT11_32        
#define CIMR_TIMER2     BIT13_32        
#define CIMR_RTT        BIT14_32        
#define CIMR_I2C        BIT15_32        
#define CIMR_PC11       BIT16_32        
#define CIMR_PC10       BIT17_32        
#define CIMR_TIMER3     BIT19_32        
#define CIMR_PC9        BIT20_32        
#define CIMR_PC8        BIT21_32        
#define CIMR_PC7        BIT22_32        
#define CIMR_TIMER4     BIT24_32        
#define CIMR_PC6        BIT25_32        
#define CIMR_SPI        BIT26_32        
#define CIMR_SMC1       BIT27_32        
#define CIMR_SMC2       BIT28_32        
#define CIMR_PC5        BIT29_32        
#define CIMR_PC4        BIT30_32        

/* CPM割込みインサービスレジスタCISR */
#define CISR            (VW *)(IMMR_BASE + TADR_CPM_CISR)
#define CISR_BIT(device)	_CISR_BIT(device)
#define _CISR_BIT(device)	CIMR_##device

/*
 *  CPM割込み制御ビットの定義
 */
    /*  CPM割込みベクタレジスタCIVRレジスタのIACKビット  */
#define TA_CPM_CIVR_IACK    0x1 


/*
 *  15. クロック選択と電力制御関連のレジスタ定義
 */

/*  システムクロックおよびリセット制御レジスタ  */
#define TADR_SCCR   0x280
#define SCCR        (VW *)(IMMR_BASE + TADR_SCCR)
#define TADR_SCCRK  0x380       /*  キーレジスタ  */
#define SCCRK       (VW *)(IMMR_BASE + TADR_SCCRK)

#define SCCR_RTDIV  BIT7_32     /*  リアルタイムクロック分周  */
                                /*  0：4で分周、1：512で分周  */
#define SCCR_RTSEL  BIT8_32     /*  リアルタイムクロック選択  */
                                /*  0：OSCM（水晶オシレータ）、1：EXTCLK  */
#define SCCR_CRQEN  BIT9_32     /*  CPM要求イネーブル  */
#define SCCR_PRQEN  BIT10_32    /*  パワーマネジメント要求イネーブル  */
#define SCCR_EBDF   (BIT13_32 | BIT14_32)   /*  外部バス分周係数  */
#define SCCR_DFBRG  (BIT19_32 | BIT20_32)   /*  BRGCLKの分周係数  */

/*  SPLL 、低電力、およびリセット制御レジスタ  */
#define TADR_PLPRCRK    0x384   /*  レジスタ・キー  */
#define TADR_PLPRCR     0x284   
#define PLPRCR_TIMIST   BIT19_32    /*  タイマ割込みステータス  */
#define PLPRCR_CSRC     BIT21_32    /*  クロックソース  */
                                    /*  低電力モード  */
#define PLPRCR_LPM      (BIT22_32 | BIT23_32)
#define PLPRCR_LPM10    BIT22_32


/*
 *  16. メモリコントローラのレジスタ定義
 */

/*  ベースレジスタBRx  */
#define TADR_BR0    0x100
#define TADR_BR1    0x108
#define TADR_BR2    0x110
#define TADR_BR3    0x118
#define TADR_BR4    0x120
#define TADR_BR5    0x128
#define TADR_BR6    0x130
#define TADR_BR7    0x138

/*  オプションレジスタORx  */
#define TADR_OR0    0x104
#define TADR_OR1    0x10c
#define TADR_OR2    0x114
#define TADR_OR3    0x11c
#define TADR_OR4    0x124
#define TADR_OR5    0x12c
#define TADR_OR6    0x134
#define TADR_OR7    0x13c

#define TADR_MAMR   0x170   /*  マシンAモード・レジスタ  */
#define TADR_MBMR   0x174   /*  マシンBモード・レジスタ  */
#define TADR_MSTAT  0x178   /*  メモリステータス・レジスタMSTAT  */

#define TADR_MCR    0x168   /*  メモリ・コマンド・レジスタMCR  */
#define TADR_MDR    0x17c   /*  メモリ・データ・レジスタMDR  */
#define TADR_MAR    0x164   /*  メモリ・アドレス・レジスタMAR  */
#define TADR_MPTPR  0x17a   /*  メモリ周期タイマ・プリスケーラ  */
                            /*  　　　　　　　　　　レジスタMPTPR  */

/*
 *  19. 通信プロセッサCP関連のレジスタ定義
 */

/*  CPコマンドレジスタ  */
#define CPCR        (VH *)(IMMR_BASE + 0x9c0)
#define CPCR_RST            BIT0_16  /*  リセット・フラグ  */
#define CPCR_FLG            BIT15_16 /*  コマンド・セマフォ・フラグ */
#define CPCR_CH_NUM_SMC1    0x9      /*  SMC1のチャネル番号  */

/*  CPコマンドのオペコード  */
#define CPCR_INIT_RX_TX_PARAMETERS	0x0
#define CPCR_STOP_TX			0x4
#define CPCR_RESTART_TX			0x6



/*  デュアル・ポートRAMの先頭アドレス  */
#define DUAL_PORT_RAM   (IMMR_BASE + 0x2000)


/*
 *  20. SDMAおよびIDMAエミュレーション
 *  　　　　SDMA：シリアルDMA
 *  　　　　IDMA：仮想SDMA
 */

/*  SDMAコンフィギュレーション・レジスタ  */
#define SDCR            (VW *)(IMMR_BASE + 0x30)
	/* RISCコントローラ（CP）の調停ID */
	/* SDMA のU バス調停優先度5：通常処理 */
#define SDCR_RAID_RB5   0x1

/*
 *  21. シリアルインターフェースSI関連のレジスタ定義
 */

/*  SIモード・レジスタ  */
#define SIMODE      (VW *)(IMMR_BASE + 0xae0)
#define SIMODE_SMC1 BIT16_32    /*  SMC1の接続  */
                                /*  　0:NMSIモード  */
                                /*  　1:多重処理モード  */

                                /*  SMC1クロックソース  */
#define SIMODE_SMC1CS   (BIT17_32 | BIT18_32 | BIT19_32)    

/*  ボーレート・ジェネレータ・コンフィギュレーション・レジスタ  */
#define BRGC1       (VW *)(IMMR_BASE + 0x9f0)
#define BRGC1_RST   BIT14_32    /*  BRGリセット  */
#define BRGC1_EN    BIT15_32    /*  BRGカウントイネーブル  */
#define BRGC1_EXTC  (BIT16_32 | BIT17_32)   /*  外部クロックソース  */
#define BRGC1_ATB   BIT18_32    /*  オートボー  */
#define BRGC1_CD    0x1ffe      /*  クロック・デバイダ  */
#define BRGC1_DIV16 BIT31_32    /*  16分周  */


/*
 *  34. パラレルI/O関連のレジスタ定義
 */

/*  ポートBピン・アサイン・レジスタ  */
#define TADR_PBPAR  0xabc
#define PBPAR       (VW *)(IMMR_BASE + TADR_PBPAR)
#define PBPAR_DD24  BIT24_32    /*  PB24ピン：専用ペリフェラル機能  */
#define PBPAR_DD25  BIT25_32    /*  PB25ピン：専用ペリフェラル機能  */
#define PBPAR_DD27  BIT27_32    /*  PB27ピン：専用ペリフェラル機能  */

/*  ポートBデータ・ディレクション・レジスタ  */
#define TADR_PBDIR  0xab8
#define PBDIR       (VW *)(IMMR_BASE + TADR_PBDIR)
#define PBDIR_DR24  BIT24_32    /*  PB24ピン：ペリフェラル機能1  */
#define PBDIR_DR25  BIT25_32    /*  PB25ピン：ペリフェラル機能1  */
#define PBDIR_DR27  BIT27_32    /*  PB25ピン：ペリフェラル機能1  */

/*  ポートBオープン・ドレイン・レジスタ  */
#define TADR_PBODR  0xac0
#define PBODR       (VW *)(IMMR_BASE + TADR_PBODR)
#define PBODR_OD24  BIT24_32    /*  PB24ピン：オープン・ドレイン・ドライブ  */
#define PBODR_OD25  BIT25_32    /*  PB25ピン：オープン・ドレイン・ドライブ  */
#define PBODR_OD27  BIT27_32    /*  PB25ピン：オープン・ドレイン・ドライブ  */

/*  ポートBデータレジスタ  */
#define TADR_PBDAT  0xac4
#define PBDAT_D27   BIT27_32


/*  アンロックの際にキーレジスタに書き込む定数  */
#define UNLOCK_KEY  0x55ccaa33

/*
 *  内部レジスタ保護のロックとアンロック
 *
 *　　シリアルドライバとタイマドライバの両方で用いるため、
 *　　このファイルに入れている
 */

/*  SCCRロック  */
    	/*  値はUNLOCK_KEY以外であれば良い  */
#define lock_sccr()     mpc860_wrw_mem(SCCRK, 0)

/*  SCCRアンロック  */
#define unlock_sccr()	mpc860_wrw_mem(SCCRK, UNLOCK_KEY)


#endif /* _MPC860_H_ */
/*  end of file  */
