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
 *  @(#) $Id: mpc860_smc.c,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *   MPC860内蔵シリアル・マネージメントコントローラSMC1用 簡易ドライバ
 *　　　　
 *　　　　SMC1のみをサポートしている
 *
 *　　　　smc.{c,h}とhw_serial.hの分割の基準
 *　　　　　　・smc.{c,h}：シリアルデバイスに依存する部分のみ記述
 *　　　　　　・hw_serial.h：割込みコントローラ依存
 *　　　　　　
 *　　　　　　・クロック周波数依存について
 *　　　　　　　　・デバイス依存のパラメータはmpc860_smc.c内で用意する。
 *　　　　　　　　・大元のクロック周波数はコンパイルオプションとして与える
 */

#include <s_services.h>
#include <mpc860_smc.h>

/*
 *  コントロールレジスタの定義
 */

/*  SMCモードレジスタ  */
#define SMCMR1          (VH *)(IMMR_BASE + 0xa82)
#define SMCMR_SM_UART   BIT10_16    /*  SMCモード：UART  */
#define SMCMR_TEN       BIT14_16    /*  SMC送信イネーブル  */
#define SMCMR_REN       BIT15_16    /*  SMC受信イネーブル  */


/*
 *  30.2.3 パラメータRAM
 */
/*  バッファ・ディスクリプタBDの先頭アドレス  */
#define BD1_BASE    (IMMR_BASE + 0x3e80)
#define RBASE1      (VH *)BD1_BASE          /*  RxBDベースアドレス  */
#define TBASE1      (VH *)(BD1_BASE+2)      /*  TxBDベースアドレス  */
#define RFCR1       (VB *)(BD1_BASE+4)      /*  受信ファンクションコード  */
					    /*  バイト順（受信）  */
#if SIL_ENDIAN == SIL_ENDIAN_BIG	    /* ビッグエンディアン */
#define RFCR_BO     0x10
#else /* SIL_ENDIAN == SIL_ENDIAN_BIG */    /* リトルエンディアン */
#define RFCR_BO	    0x08
#endif /* SIL_ENDIAN == SIL_ENDIAN_BIG */

#define TFCR1       (VB *)(BD1_BASE+5)      /*  送信ファンクションコード  */
					    /*  バイト順（送信）  */
#if SIL_ENDIAN == SIL_ENDIAN_BIG	    /* ビッグエンディアン */
#define TFCR_BO     0x10
#else /* SIL_ENDIAN == SIL_ENDIAN_BIG */    /* リトルエンディアン */
#define TFCR_BO	    0x08
#endif /* SIL_ENDIAN == SIL_ENDIAN_BIG */

/*
 *  30.3.2 UARTの場合のパラメータRAM
 */
#define MRBLR1      (VH *)(BD1_BASE+6)      /*  最大受信バッファ長  */
#define RBPTR1      (VH *)(BD1_BASE+0x10)   /*  RxBDポインタ  */
#define TBPTR1      (VH *)(BD1_BASE+0x20)   /*  TxBDポインタ  */
                                            /*  最大アイドルキャラクタ数  */
#define MAX_IDL1    (VH *)(BD1_BASE+0x28)
#define IDLC1       (VH *)(BD1_BASE+0x2a)   /*  一時アイドル・カウンタ  */
#define BRKLN1      (VH *)(BD1_BASE+0x2c)   /*  最終受信ブレーク長  */
                                            /*  受信ブレーク条件カウンタ  */
#define BRKEC1      (VH *)(BD1_BASE+0x2e)
                                    /*  ブレークカウントレジスタ（送信）  */
#define BRKCR1      (VH *)(BD1_BASE+0x30)
#define R_MASK1     (VH *)(BD1_BASE+0x32)   /*  一時ビット・マスク  */


/*
 *  デュアルポート・メモリ上に確保する領域
 */

/*
 *  バッファ・ディスクリプタBDのベースアドレス
 */
#define RxBD_BASE_ADDRESS   0x0     /*  RxBDベースアドレス  */
#define TxBD_BASE_ADDRESS   0x8     /*  TxBDベースアドレス  */

/*  30.3.10 受信用バッファディスクリプタRxBD  */
#define RxBD        (DUAL_PORT_RAM + RxBD_BASE_ADDRESS)
#define RxBD_STATUS (VH *)RxBD     	/*  ステータスと制御  */
#define RxBD_LENGTH (VH *)(RxBD+2) 	/*  データ長  */
#define RxBD_BUFFP  (VW *)(RxBD+4) 	/*  受信バッファポインタ  */

/*  30.3.11 送信用バッファディスクリプタTxBD  */
#define TxBD        (DUAL_PORT_RAM + TxBD_BASE_ADDRESS)
#define TxBD_STATUS (VH *)TxBD     	/*  ステータスと制御  */
#define TxBD_LENGTH (VH *)(TxBD+2) 	/*  データ長  */
#define TxBD_BUFFP  (VW *)(TxBD+4) 	/*  送信バッファポインタ  */

/*  受信用バッファディスクリプタRxBD  */
/*  　「ステータスと制御」フィールドの定数  */
#define RxBD_STATUS_E       BIT0_16     /*  空白  */
#define RxBD_STATUS_W       BIT2_16     /*  ラップ  */
#define RxBD_STATUS_I       BIT3_16     /*  割込み  */
#define RxBD_STATUS_CM      BIT6_16     /*  継続モード  */
#define RxBD_STATUS_ID      BIT7_16     /*  アイドル受信  */
#define RxBD_STATUS_BR      BIT10_16    /*  ブレーク受信  */
#define RxBD_STATUS_FR      BIT11_16    /*  フレーミング・エラー  */
#define RxBD_STATUS_PR      BIT12_16    /*  パリティ・エラー  */
#define RxBD_STATUS_OV      BIT14_16    /*  オーバーラン  */

/* 送信用バッファディスクリプタTxBD  */
/*  　「ステータスと制御」フィールドの定数  */
#define TxBD_STATUS_R       BIT0_16     /*  レディ  */
#define TxBD_STATUS_W       BIT2_16     /*  ラップ  */
#define TxBD_STATUS_I       BIT3_16     /*  割込み  */
#define TxBD_STATUS_CM      BIT6_16     /*  継続モード  */
#define TxBD_STATUS_P       BIT7_16     /*  プリアンブル  */

/*  30.3.12 SMC UARTイベントレジスタ  */
#define SMCE1           (VB *)(IMMR_BASE + 0xa86)
#define SMCE_BRKE       BIT1_8      /*  ブレーク最終  */
#define SMCE_BRK        BIT3_8      /*  ブレークキャラクタ受信  */
#define SMCE_BSY        BIT5_8      /*  ビジー条件  */
#define SMCE_TX         BIT6_8      /*  送信バッファ  */
#define SMCE_RX         BIT7_8      /*  受信バッファ  */

/*  SMC UARTマスクレジスタ  */
#define SMCM1           (VB *)(IMMR_BASE + 0xa8a)
#define SMCM_BRKE       BIT1_8      /*  ブレーク最終  */
#define SMCM_BRK        BIT3_8      /*  ブレークキャラクタ受信  */
#define SMCM_BSY        BIT5_8      /*  ビジー条件  */
#define SMCM_TX         BIT6_8      /*  送信バッファ  */
#define SMCM_RX         BIT7_8      /*  受信バッファ  */

#define PIC_BUFFER_SIZE     1      		/*  バッファサイズ  */

volatile static UB Rx_buffer[PIC_BUFFER_SIZE];	/*  受信バッファ  */
volatile static UB Tx_buffer[PIC_BUFFER_SIZE];	/*  送信バッファ  */


/*
 *  ボーレート設定用定数
 *  
 *  　非同期の場合は×16クロックを用いるので16で割っている
 *  （ユーザーの設定で16分周しているわけではない）
 *  
 */
#define MHZ		1000000

#define BRGC1_CD_VAL		\
	(((SYSTEM_CLOCK * MHZ * 10 + 10) / (16 * BAUD_RATE * 10)) - 1) 

/*  BRGC1レジスタのCDフィールドは20ビットに制限されている  */
#if (BRGC1_CD_VAL & ~0xfffff) != 0
　ここでコンパイルエラー
#endif


/*
 *  分周比とビットレートの設定
 *
 */


/*
 *  ボーレート設定後、クロックが安定するまでの待ち時間
 *  　（最初の1ビット分）　
 *  BPS=9600bpsの場合
 *  　t = 1 / BPS = 104,167 = 105,000[nsec]
 */
#define SMC_1BIT_TIME 	105000


/*
 *  シリアルI/Oポート管理ブロックの定義
 *  　2chサポートに拡張する場合は初期値用のデータも含める
 */
struct sio_port_control_block {
	VP_INT		exinf;		/* 拡張情報 */
	BOOL		openflag;	/* オープン済みフラグ */
};

/*
 *  シリアルI/Oポート管理ブロックのエリア
 *  　　ID = 1 をSMC1に対応させている．
 */
static SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	((UINT)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))


#define	CODE_LF		0x0a		/*  改行コード  */

/*  キャラクタ送信後の待ち時間  */
#ifdef DOWNLOAD_TO_RAM      /*  デバッグ用  */

#define	DELAY_CH	      0		/*  通常のキャラクタ  */
#define	DELAY_LF	5000000		/*  改行コード  */

#else /* DOWNLOAD_TO_RAM */ /*  ROM化  */

#define	DELAY_CH	4000000		/*  通常のキャラクタ  */
#define	DELAY_LF	1000000		/*  改行コード  */

#endif /* DOWNLOAD_TO_RAM */



/*
 *  文字を受信できるか？
 */
Inline BOOL
smc_getready(SIOPCB *siopcb)
{
	VH status;
	
	status = mpc860_reh_mem(RxBD_STATUS);
	return(!(status & RxBD_STATUS_E));
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
smc_putready(SIOPCB *siopcb)
{
	VH status;

	status = mpc860_reh_mem(TxBD_STATUS);
	return(!(status & TxBD_STATUS_R));
}

/*
 *  受信した文字の取出し
 */
Inline char
smc_getchar(SIOPCB *siopcb)
{
	char c;
	SIL_PRE_LOC;
	
	SIL_LOC_INT();		/*  割込み禁止  */
    	c = Rx_buffer[0];
    	
    	/*  注意：受信バッファサイズが１の場合のみ有効  */
    	mpc860_orh_mem(RxBD_STATUS, RxBD_STATUS_E);
    	SIL_UNL_INT();		/*  割込み許可  */
    	return(c);
}

/*
 *  送信する文字の書込み
 */
Inline void
smc_putchar(SIOPCB *siopcb, char c)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();		/*  割込み禁止  */
    	Tx_buffer[0] = c;
    	
    	/*  注意：送信バッファサイズが１の場合のみ有効  */
    	mpc860_orh_mem(TxBD_STATUS, TxBD_STATUS_R);
    	
    	SIL_UNL_INT();		/*  割込み許可  */

	/*
	 *  文字化けを防ぐため、待ち時間を入れる
	 */
	sil_dly_nse(DELAY_CH);

    	/*  改行コードの場合  */
    	if (c == CODE_LF) {
		sil_dly_nse(DELAY_LF);
	}
}

/*
 *  SIOドライバの初期化ルーチン
 */
void
smc_initialize()
{
	SIOPCB	*siopcb;
	UINT	i;

	/*
	 *  シリアルI/Oポート管理ブロックの初期化
	 */
	for (siopcb = siopcb_table, i = 0; i < TNUM_SIOP; siopcb++, i++) {
		siopcb->openflag = FALSE;
	}
}


/*
 *  オープンしているポートがあるか？
 */
BOOL
smc_openflag(void)
{
	return(siopcb_table[0].openflag);
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
smc_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB *siopcb;

	siopcb = get_siopcb(siopid);
	     					/*  送受信停止  */
	mpc860_andh_mem(SMCMR1, ~(SMCMR_TEN | SMCMR_REN));

	/*
	 *  ピン・アサイン
	 *  　　ポートBの設定
	 *　　　　　BP25ピン：SMTXD1
	 *　　　　　BP24ピン：SMRXD1
	 */
         				/*  専用ペリフェラル機能選択  */
     	mpc860_orw_mem(PBPAR, (PBPAR_DD24 | PBPAR_DD25));
        				/*  ペリフェラル機能0選択  */
     	mpc860_andw_mem(PBDIR, ~(PBDIR_DR24 | PBDIR_DR25));
        				/*  アクティブにドライブ  */
     	mpc860_andw_mem(PBODR, ~(PBODR_OD24 | PBODR_OD25));

    	/*
    	 *  VCOUTからBRGCLKへの分周係数設定
    	 *      SCCR.DFBRG=00：分周比１
    	 */
    	unlock_sccr();          		/*  SCCRアンロック  */
    	mpc860_andw_mem(SCCR, ~SCCR_DFBRG);    	/*  分周比設定  */
    	lock_sccr();            		/*  SCCRロック  */

    	/*
    	 *  ボーレート設定（BRG1）
    	 *  　　EN=1：BRGカウントイネーブル
    	 *  　　EXTC=00：クロックソースにBRGCLKを選択
    	 *  　　ATB=0：オートボー機能ディセーブル
    	 *  　　CD：分周比
    	 *  　　DIV16：16分周するか否か
    	 */
    	mpc860_wrw_mem(BRGC1, BRGC1_EN | (BRGC1_CD_VAL<<1));
    
    	/*
    	 *  BRG1とSMC1の接続
    	 *  　　SMC1=0：NMSIモード（多重化しない）
    	 *  　　SMC1CS=000：クロックソースBRG1
    	 */
    	mpc860_andw_mem(SIMODE, ~(SIMODE_SMC1 | SIMODE_SMC1CS));
    
    	/*　クロックが安定するまで待つ */
    	sil_dly_nse(SMC_1BIT_TIME);

    	/*
    	 *  バッファ・ディスクリプタBDの設定
    	 */
    					/*  RxBDベースアドレス  */
    	mpc860_wrh_mem(RBASE1, RxBD_BASE_ADDRESS);
         				/*  TxBDベースアドレス  */
    	mpc860_wrh_mem(TBASE1, TxBD_BASE_ADDRESS);

    	/*
    	 *  通信プロセッサCPにINIT RX AND TX PARAMETERSコマンドを発行
    	 */
    	mpc860_CP_command(CPCR_INIT_RX_TX_PARAMETERS, CPCR_CH_NUM_SMC1);
    
    	/*  SDMAのUバス調停優先度を設定  */
    	mpc860_wrw_mem(SDCR, SDCR_RAID_RB5);   /*  優先度5：通常処理  */

    	/*  バッファ・ディスクリプタBDのエンディアン設定  */
    	mpc860_wrb_mem(RFCR1, RFCR_BO);		/*  受信  */
    	mpc860_wrb_mem(TFCR1, TFCR_BO);		/*  送信  */
    	    			/*  最大受信バッファ長  */
    	mpc860_wrh_mem(MRBLR1, PIC_BUFFER_SIZE);
       		/*  アイドルキャラクタ数検出機能ディセーブル  */
    	mpc860_wrh_mem(MAX_IDL1, 0);
    
    	/*  受信ブレークに関する設定は必要ないのでクリア  */
    	mpc860_wrh_mem(BRKLN1, 0);
    	mpc860_wrh_mem(BRKEC1, 0);
    
    	/*  STOP TRANSMITコマンドで送信されるブレークキャラクタの数を設定  */
    	mpc860_wrh_mem(BRKCR1, 1);

    	/*  受信バッファ・ディスクリプタRxBDの初期化  */
    	mpc860_wrh_mem(RxBD_STATUS,
                 RxBD_STATUS_E | RxBD_STATUS_W | RxBD_STATUS_I);
                /*  Eフラグセット  */
                /*  W=1：テーブル内の最終BD  */
                /*  I=1：受信割込み発生  */

    	mpc860_wrh_mem(RxBD_LENGTH, PIC_BUFFER_SIZE);  /*  データ長  */
       					/*  受信バッファポインタ  */
    	mpc860_wrw_mem(RxBD_BUFFP, (VW)Rx_buffer);

    	/*  送信バッファ・ディスクリプタTxBDの初期化  */
    	mpc860_wrh_mem(TxBD_STATUS,
    		TxBD_STATUS_R | TxBD_STATUS_W | TxBD_STATUS_I);
                /*  Rフラグセット  */
                /*  W=1：テーブル内の最終BD  */
                /*  I=1：送信割込み発生  */

    	mpc860_wrh_mem(TxBD_LENGTH, PIC_BUFFER_SIZE);  /*  データ長  */
    					/*  送信バッファポインタ  */
    	mpc860_wrw_mem(TxBD_BUFFP, (VW)Tx_buffer);
    
    	/*  以前のイベントをクリア  */
    	mpc860_wrb_mem(SMCE1,
    		SMCE_BRKE | SMCE_BRK | SMCE_BSY | SMCE_TX | SMCE_RX);

    	/*  すべてのUART割込みを許可  */
    	mpc860_wrb_mem(SMCM1,
     		SMCM_BRKE | SMCM_BRK | SMCM_BSY | SMCM_TX | SMCM_RX);
    
    	/*  SMCモード設定  */
    	mpc860_wrh_mem(SMCMR1, (VH)((0x9<<11) | SMCMR_SM_UART));
        	/*  
        	 *  CLEN=9：キャラクタ長
        	 *  SL=0：1ストップビット
        	 *  PEN=0：パリティなし
        	 *  SM=10：SMCモード UART
        	 *  DM=00：診断モード 通常動作
        	 *  TEN=0：SMC送信ディセーブル
        	 *  REN=0：SMC受信ディセーブル
        	 *   この時点ではまだ、送信も受信もイネーブルではない
        	 */
    
    	mpc860_orh_mem(SMCMR1, (SMCMR_TEN | SMCMR_REN));
        	/*  
        	 *  上記に加えて
        	 *  　TEN=1：SMC送信イネーブル
        	 *  　REN=1：SMC受信イネーブル
        	 */

	siopcb->exinf = exinf;
	siopcb->openflag = TRUE;
	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
smc_cls_por(SIOPCB *siopcb)
{
         					/*  送受信停止  */
	mpc860_andh_mem(SMCMR1, ~(SMCMR_TEN | SMCMR_REN));
	siopcb->openflag = FALSE;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
smc_snd_chr(SIOPCB *siopcb, char c)
{
	if (smc_putready(siopcb)) {
		smc_putchar(siopcb, c);
		return(TRUE);
	}
	return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
smc_rcv_chr(SIOPCB *siopcb)
{
	if (smc_getready(siopcb)) {
		return((INT)(UB) smc_getchar(siopcb));
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
smc_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	SIL_PRE_LOC;
	
	switch (cbrtn) {
	case SIO_ERDY_SND:	/* 送信割り込み要求を許可 */
		SIL_LOC_INT();
		mpc860_orh_mem(SMCMR1, SMCMR_TEN);
		SIL_UNL_INT();
		break;
	case SIO_ERDY_RCV:	/* 受信割り込み要求を許可 */
		SIL_LOC_INT();
		mpc860_orh_mem(SMCMR1, SMCMR_REN);
		SIL_UNL_INT();
		break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
smc_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	SIL_PRE_LOC;

	switch (cbrtn) {
	case SIO_ERDY_SND:	/* 送信割り込み要求を禁止 */
		SIL_LOC_INT();
		mpc860_andh_mem(SMCMR1, ~SMCMR_TEN);
		SIL_UNL_INT();
		break;
	case SIO_ERDY_RCV:	/* 受信割り込み要求を禁止 */
		SIL_LOC_INT();
		mpc860_andh_mem(SMCMR1, ~SMCMR_REN);
		SIL_UNL_INT();
		break;
	}
}

/*
 *  シリアルI/Oポートに対する送信割込み処理
 */
Inline void
smc_isr_siop_out(SIOPCB *siopcb)
{
	/*  SMCE1での割込み要因のチェックは済んでいる  */
	
	if (smc_putready(siopcb)) {
		/*
		 *  送信通知コールバックルーチンを呼び出す．
		 */
		smc_ierdy_snd(siopcb->exinf);
	}
}

/*
 *  シリアルI/Oポートに対する受信割込み処理
 */
Inline void
smc_isr_siop_in(SIOPCB *siopcb)
{
	/*  SMCE1での割込み要因のチェックは済んでいる  */

	if (smc_getready(siopcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		smc_ierdy_rcv(siopcb->exinf);
	}
}

/*
 *  SILを使用したときのログ機能
 */
#ifdef SIL_DEBUG
#define ENTER_SMC_ISR						\
	if (sil_debug_on) {					\
		syslog(LOG_EMERG, "Enter selial interrupt.");	\
	}

#else /* SIL_DEBUG */

#define ENTER_SMC_ISR

#endif /* SIL_DEBUG */


/*
 *  SIO送信割込みサービスルーチン
 *  
 *  　送受信共通
 *  
 */
void
smc_isr()
{
    	UB smce1, smcm1, event;
    	SIL_PRE_LOC;

	ENTER_SMC_ISR
	
    	if (!siopcb_table[0].openflag) goto exit_label;
    
    	smce1 = mpc860_reb_mem(SMCE1);
    	smcm1 = mpc860_reb_mem(SMCM1);
    	event = smce1 & smcm1;
    	
    	if (event & (SMCE_BRKE | SMCE_BRK | SMCE_BSY)) {
    		/*  エラー処理：未完成  */
    		goto exit_label;
    	}
    	
    	if (event & SMCE_RX) {         /*  受信割込み  */
	        	/*  1をセットすることにより、イベントをクリア  */
        	mpc860_wrb_mem(SMCE1, SMCE_RX);
        	smc_isr_siop_in(&siopcb_table[0]);
    	}
    	if (event & SMCE_TX) {         /*  送信割込み  */
	        	/*  1をセットすることにより、イベントをクリア  */
        	mpc860_wrb_mem(SMCE1, SMCE_TX);
        	smc_isr_siop_out(&siopcb_table[0]);
    	}

exit_label:
    	/*
    	 * 　割込み要求のクリア
    	 *  　　割込みコントローラ依存
    	 *  　　（sys_config.hで定義）
    	 */
    	SIL_LOC_INT();
	CLEAR_IRQ_TO_ICU(SMC1);	
    	SIL_UNL_INT();
}

/*
 * ポーリングによる文字の送信
 */
void
smc_putc_pol(char c)
{
	BOOL is_ten_ok;
	VH smcmr;
	if (smc_openflag()) {	/*  初期化されているかチェック  */
		smcmr = mpc860_reh_mem(SMCMR1);
		is_ten_ok = smcmr & SMCMR_TEN;
		if (!is_ten_ok) {
						/*  送信許可  */
			mpc860_orh_mem(SMCMR1, SMCMR_TEN);
		}
				/*  バッファが空になるのを待つ  */
		while(!smc_putready(&siopcb_table[0]));	
		smc_putchar(&siopcb_table[0], c);	/*  1文字送信  */

		if (!is_ten_ok) {
				/*  バッファが空になるのを待つ  */
			while(!smc_putready(&siopcb_table[0]));
				/*  2キャラクタ分待つ  */
			sil_dly_nse(SMC_1BIT_TIME*18*10);	
						/*  送信禁止  */
			mpc860_andh_mem(SMCMR1, ~SMCMR_TEN);
		}
	}
}

/*  end of file  */
