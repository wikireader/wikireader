/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 */

#define _MACRO_ONLY
#include <jsp_kernel.h>
#include <hw_serial.h>


/*
 *  割込みベクタテーブルの定義
 *    ベクタテーブルは固定なので、割込みの出入り口処理のラベルは
 *    以下のベクタテーブルに直接書き込む必要がある。
 */

	.h8300s
	.section .vectors
	.align 4

/*	.global _vectors*/

_vectors:
	.long	start				/*  0, 0x0000: パワーオンリセット	*/
	.long	start    			/*  1, 0x0004: マニュアルリセット	*/
	/*  ----------------------------------------------------------  */
	.long	no_reg_exception		/*  2, 0x0008: システム予約		*/
	.long	no_reg_exception		/*  3, 0x000c: システム予約		*/
	.long	no_reg_exception		/*  4, 0x0010: システム予約		*/
	/*  ---トレース-----------------------------------------------  */
	.long	no_reg_exception		/*  5, 0x0014: トレース			*/
	/*  ----------------------------------------------------------  */
	.long	no_reg_exception		/*  6, 0x0018: システム予約		*/
	/*  ---NMI----------------------------------------------------  */
	.long	start				/*  7, 0x001c: 外部割込み  NMI		*/
	/*  ---トラップ命令-------------------------------------------  */
	.long	no_reg_exception		/*  8, 0x0020: トラップ命令		*/
	.long	no_reg_exception		/*  9, 0x0024: トラップ命令		*/
	.long	no_reg_exception		/* 10, 0x0028: トラップ命令		*/
	.long	no_reg_exception		/* 11, 0x002c: トラップ命令		*/
	/*  ----------------------------------------------------------  */
	.long	no_reg_exception		/* 12, 0x0030: システム予約		*/
	.long	no_reg_exception		/* 13, 0x0034: システム予約		*/
	.long	no_reg_exception		/* 14, 0x0038: システム予約		*/
	.long	no_reg_exception		/* 15, 0x003c: システム予約		*/
	/*  ---外部割込み---------------------------------------------  */
	.long	no_reg_exception		/* 16, 0x0040: IRQ0	*/
	.long	no_reg_exception		/* 17, 0x0044: IRQ1	*/
	.long	no_reg_exception		/* 18, 0x0048: IRQ2	*/
	.long	no_reg_exception		/* 19, 0x004c: IRQ3 	*/
	.long	no_reg_exception		/* 20, 0x0050: IRQ4	*/
	.long	no_reg_exception		/* 21, 0x0054: IRQ5	*/
	.long	no_reg_exception		/* 22, 0x0058: IRQ6	*/
        .long	no_reg_exception		/* 23, 0x005c: IRQ7	*/
        /*  ---DTC----------------------------------------------------  */
        .long	no_reg_exception		/* 24, 0x0060: SWDTEND(ソフトウェア起動データ転送終了)	*/
        /*  ウォッチドッグタイマ--------------------------------------  */
	.long	no_reg_exception		/* 25, 0x0064: WOVI(インターバルタイマ)	*/
	/*  ---リフレッシュコントローラ-------------------------------  */
	.long	no_reg_exception		/* 26, 0x0068: CMI(コンペアマッチ)	*/
        /*  ----------------------------------------------------------  */
	.long	no_reg_exception		/* 27, 0x006c: リザーブ	*/
 	/*  ---A/D----------------------------------------------------  */
	.long	no_reg_exception		/* 28, 0x0070: ADI(A/D変換の終了)	*/
	/*  ----------------------------------------------------------  */
	.long	no_reg_exception		/* 29, 0x0074: リザーブ	*/
	.long	no_reg_exception		/* 30, 0x0078: リザーブ	*/
	.long	no_reg_exception		/* 31, 0x007c: リザーブ	*/
	/*  ---TPUチャネル0-------------------------------------------  */
	.long	_INT_ENTRY(timer_handler)	/* 32, 0x0080: TGI0A	*/
	.long	no_reg_exception		/* 33, 0x0084: TGI0B	*/
	.long	no_reg_exception		/* 34, 0x0088: TGI0C	*/
	.long	no_reg_exception		/* 35, 0x008c: TGI0D	*/
	.long	no_reg_exception		/* 36, 0x0090: TCI0V	*/
	/*  ----------------------------------------------------------  */
	.long	no_reg_exception		/* 37, 0x0094: リザーブ	*/
	.long	no_reg_exception		/* 38, 0x0098: リザーブ	*/
	.long	no_reg_exception		/* 39, 0x009c: リザーブ	*/
	/*  ---TPUチャネル1-------------------------------------------  */
	.long	no_reg_exception		/* 40, 0x00a0: TGI1A	*/
	.long	no_reg_exception		/* 41, 0x00a4: TGI1B	*/
	.long	no_reg_exception		/* 42, 0x00a8: TCI1V	*/
	.long	no_reg_exception		/* 43, 0x00ac: TCI1U	*/
	/*  ---TPUチャネル2-------------------------------------------  */
	.long	no_reg_exception		/* 44, 0x00b0: TGI2A	*/
	.long	no_reg_exception		/* 45, 0x00b4: TGI2B	*/
	.long	no_reg_exception		/* 46, 0x00b8: TCI2V	*/
	.long	no_reg_exception		/* 47, 0x00bc: TCI2U	*/
	/*  --TPUチャネル3--------------------------------------------  */
	.long	no_reg_exception		/* 48, 0x00c0: TGI3A	*/
	.long	no_reg_exception		/* 49, 0x00c4: TGI3B	*/
	.long	no_reg_exception		/* 50, 0x00c8: TGI3C	*/
	.long	no_reg_exception		/* 51, 0x00cc: TGI3D	*/
	.long	no_reg_exception		/* 52, 0x00d0: TCI3V	*/
	/*  ----------------------------------------------------------  */
 	.long	no_reg_exception		/* 53, 0x00d4: リザーブ	*/
	.long	no_reg_exception		/* 54, 0x00d8: リザーブ	*/
	.long	no_reg_exception		/* 55, 0x00dc: リザーブ	*/
	/*  ---TPUチャネル4-------------------------------------------  */
	.long	no_reg_exception		/* 56, 0x00e0: TGI4A	*/
	.long	no_reg_exception		/* 57, 0x00e4: TGI4B	*/
	.long	no_reg_exception		/* 58, 0x00e8: TCI4V	*/
	.long	no_reg_exception		/* 59, 0x00ec: TCI4U	*/
	/*  ---TPUチャネル5-------------------------------------------  */
	.long	no_reg_exception		/* 60, 0x00f0: TGI5A	*/
	.long	no_reg_exception		/* 61, 0x00f4: TGI5B	*/
	.long	no_reg_exception		/* 62, 0x00f8: TCI5V	*/
	.long	no_reg_exception		/* 63, 0x00fc: TCI5U    */
     	/*  ----------------------------------------------------------  */
	.long	no_reg_exception		/* 64, 0x0100: リザーブ	*/
	.long	no_reg_exception		/* 65, 0x0104: リザーブ	*/
	.long	no_reg_exception		/* 66, 0x0108: リザーブ	*/
	.long	no_reg_exception		/* 67, 0x010c: リザーブ	*/
	.long	no_reg_exception		/* 68, 0x0110: リザーブ	*/
	.long	no_reg_exception		/* 69, 0x0114: リザーブ	*/
	.long	no_reg_exception		/* 70, 0x0118: リザーブ	*/
	.long	no_reg_exception		/* 71, 0x011c: リザーブ	*/
	/*  ---DMAC---------------------------------------------------  */
	.long	no_reg_exception		/* 72, 0x0120: DEND0A	*/
	.long	no_reg_exception		/* 73, 0x0124: DEND0B	*/
	.long	no_reg_exception		/* 74, 0x0128: DEND1A	*/
	.long	no_reg_exception		/* 75, 0x012c: DEND1B	*/
	/*  ----------------------------------------------------------  */
	.long	no_reg_exception		/* 76, 0x0130: リザーブ	*/
	.long	no_reg_exception		/* 77, 0x0134: リザーブ	*/
	.long	no_reg_exception		/* 78, 0x0138: リザーブ	*/
	.long	no_reg_exception		/* 79, 0x013c: リザーブ	*/
	/*  ---SCIチャネル0-------------------------------------------  */
	.long	_INT_ENTRY(sio1_handler_error)	/* 80, 0x0140: ERI0	*/
	.long	_INT_ENTRY(sio1_handler_in)	/* 81, 0x0144: RXI0	*/
	.long	_INT_ENTRY(sio1_handler_out)	/* 82, 0x0148: TXI0	*/
	.long	no_reg_exception		/* 83, 0x014c: TEI0	*/
        /*  ---SCIチャネル1-------------------------------------------  */
#if TNUM_SIOP >= 2
	.long	_INT_ENTRY(sio2_handler_error)	/* 84, 0x0150: ERI1	*/
	.long	_INT_ENTRY(sio2_handler_in)	/* 85, 0x0154: RXI1	*/
	.long	_INT_ENTRY(sio2_handler_out)	/* 86, 0x0158: TXI1	*/
	.long	no_reg_exception		/* 87, 0x015c: TEl1	*/
#else
	.long	no_reg_exception		/* 84, 0x0150: ERI1	*/
	.long	no_reg_exception		/* 85, 0x0154: RXI1	*/
	.long	no_reg_exception		/* 86, 0x0158: TXI1	*/
	.long	no_reg_exception		/* 87, 0x015c: TEl1	*/
#endif /* TNUM_SIOP >= 2 */
        /*  ----------------------------------------------------------  */
	.long	no_reg_exception		/* 88, 0x0160: リザーブ	*/
	.long	no_reg_exception		/* 89, 0x0164: リザーブ	*/
	.long	no_reg_exception		/* 90, 0x0168: リザーブ	*/
	.long	no_reg_exception		/* 91, 0x016c: リザーブ	*/
