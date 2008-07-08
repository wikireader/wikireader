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
 *  @(#) $Id: sh1.h,v 1.8 2004/10/07 17:13:56 honda Exp $
 */

#ifndef _SH1_H_
#define _SH1_H_

/*
 *  割込みのベクタ番号定義
 */
#define GII	 4	/*  一般不当命令:General Illegal Instruction  */
#define SII	 6	/*  スロット不当命令:Slot Illegal Instruction */
#define CAE	 9	/*  CPUアドレスエラー:CPU Address Error       */
#define DAE	10	/*  DMAアドレスエラー:DMA Address Error       */

#define NMI	11	/*  NMI  		*/
#define USBK	12	/*  ユーザーブレーク  	*/
#define IRQ0	64	/*  外部割込み要求  	*/
#define IRQ1	65
#define IRQ2	66
#define IRQ3	67
#define IRQ4	68
#define IRQ5	69
#define IRQ6	70
#define IRQ7	71

	/*  DMAC:ダイレクトメモリアクセスコントローラ  */
#define DEI0	72	/*  DMAC0  */
#define DEI1	74	/*  DMAC1  */
#define DEI2	76	/*  DMAC2  */
#define DEI3	78	/*  DMAC3  */

	/*  ITU:インテクレーテッドタイマパルスユニット  */
#define IMIA0	80	/*  ITU0  */
#define IMIB0	81
#define OVI0	82
#define IMIA1	84	/*  ITU1  */
#define IMIB1	85
#define OVI1	86
#define IMIA2	88	/*  ITU2  */
#define IMIB2	89
#define OVI2	90
#define IMIA3	92	/*  ITU3  */
#define IMIB3	93
#define OVI3	94
#define IMIA4	96	/*  ITU4  */
#define IMIB4	97
#define OVI4	98

	/*  SCI:シリアルコミュニケーションインターフェース  */
#define ERI0	100	/*  SCI0  */
#define RXI0	101
#define TXI0	102
#define TEI0	103
#define ERI1	104	/*  SCI1  */
#define RXI1	105
#define TXI1	106
#define TEI1	107

#define PEI	108	/*  PRT:バスコントローラのパリティ制御  */
#define ADI	109	/*  A/Dコンバータ  			*/
#define ITI	112	/*  WDT:ウォッチドックタイマ  		*/
#define CMI	113	/*  REF:DRAMリフレッシュ制御  		*/



#ifndef _MACRO_ONLY

/*
 *  SH1の内部レジスタ定義
 *  
 *  　コンパイラの型チェック機能を有効にするため、ポインタ型には
 *  　VPではなく、VB *,VH *, VW *を用いている。
 */

/*
 * ピンファンクションコントローラ （サイズは2バイト）
 */
#define	PAIOR	((VH *)0x5ffffc4)	/*  ポートA I/Oレジスタ  	  */
#define	PACR1	((VH *)0x5ffffc8)	/*  ポートA コントロールレジスタ1 */
#define	PACR2	((VH *)0x5ffffca)	/*  ポートA コントロールレジスタ2 */
#define	PBIOR	((VH *)0x5ffffc6)	/*  ポートB I/Oレジスタ  	  */
#define	PBCR1	((VH *)0x5ffffcc)	/*  ポートB コントロールレジスタ1 */
#define	PBCR2	((VH *)0x5ffffce)	/*  ポートB コントロールレジスタ2 */
#define	PADR	((VH *)0x5ffffc0)	/*  ポートA データレジスタ  	  */

#define PBCR1_TD0_RD0_MASK 	~0xfu	/*  TxD0,RxD0端子設定用マスク	*/
#define PBCR1_TD0 		 0x8u	/*  TxD0端子設定用マクロ  	*/
#define PBCR1_RD0 		 0x2u	/*  RxD0端子設定用マクロ  	*/


/*
 * 割り込みコントローラレジスタ
 */
#define	IPRA	((VH *)0x5ffff84)	/*  割込みレベル設定レジスタ  */
#define	IPRB	((VH *)0x5ffff86)	/*  （サイズは2バイト）  */
#define	IPRC	((VH *)0x5ffff88)
#define	IPRD	((VH *)0x5ffff8a)
#define	IPRE	((VH *)0x5ffff8c)
#define	ICR	((VH *)0x5ffff8e)	/*  割込みコントロールレジスタ  */
					/*  （サイズは2バイト）  */

#endif /* _MACRO_ONLY */

#endif /* _SH1_H_ */
