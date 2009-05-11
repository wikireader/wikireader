/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000,2001 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2002 by Hokkaido Industrial Research Institute, JAPAN
 *
 *  上記著作権者は，Free Software Foundation によって公表されている
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の条件のいずれかを満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 *
 *  @(#) $Id: sh7145.h,v 1.5 2005/07/06 00:45:07 honda Exp $
 */

#ifndef _SH7145_H_
#define _SH7145_H_

#include <sil.h>

/*
 *  割込みのベクタ番号定義
 */

#define ERI0	128				/*  SCI0    */
#define RXI0	129				/*  SCI0    */
#define TXI0	130				/*  SCI0    */
#define ERI1	132				/*  SCI1    */
#define RXI1	133				/*  SCI1    */
#define TXI1	134				/*  SCI1    */
#define CMI0	144				/*  CMT    */
#define ERI2	168				/*  SCI2    */
#define RXI2	169				/*  SCI2    */
#define TXI2	170				/*  SCI2    */

#ifndef _MACRO_ONLY

/*
 *  SH2の内部レジスタ定義
 */
/* モジュールスタンバイレジスタ */
#define MSTCR1	((VH *)0xffff861c)
#define MSTCR2	((VH *)0xffff861e)

#define BCR1	((VH *)0xffff8620)
#define BCR2	((VH *)0xffff8622)
#define WCR1	((VH *)0xffff8624)
#define PACRH	((VH *)0xffff8388)
#define PACRL1	((VH *)0xffff838c)
#define PACRL2	((VH *)0xffff838e)
#define PBCR1	((VH *)0xffff8398)
#define PBCR2	((VH *)0xffff839a)
#define PCCR	((VH *)0xffff839c)
#define PDCRH1	((VH *)0xffff83a8)
#define PDCRH2	((VH *)0xffff83aa)
#define PDCRL1	((VH *)0xffff83ac)
#define PDCRL2	((VH *)0xffff83ae)
#define PECRL1	((VH *)0xffff83b8)
#define PECRL2	((VH *)0xffff83ba)
#define PAIORL	((VH *)0xffff8386)
#define PBIOR	((VH *)0xffff8394)
#define PEIORL	((VH *)0xffff83b4)
#define PEDRL	((VH *)0xffff83b0)

/* 割り込み優先度レベル設定レジスタ */
#define IPRA    ((VH *)0xffff8348)
#define IPRB    ((VH *)0xffff834a)
#define IPRC    ((VH *)0xffff834c)
#define IPRD    ((VH *)0xffff834e)
#define IPRE    ((VH *)0xffff8350)
#define IPRF    ((VH *)0xffff8352)
#define IPRG    ((VH *)0xffff8354)
#define IPRH    ((VH *)0xffff8356)
#define IPRI    ((VH *)0xffff835c)
#define IPRJ    ((VH *)0xffff835e)

/*
 * 割り込みコントローラの初期化
 */
Inline void sh2_init_intcontorller(void)
{
	sil_wrh_mem (IPRA, 0x0000);
	sil_wrh_mem (IPRB, 0x0000);
	sil_wrh_mem (IPRC, 0x0000);
	sil_wrh_mem (IPRD, 0x0000);
	sil_wrh_mem (IPRE, 0x0000);
	sil_wrh_mem (IPRF, 0x0000);
	sil_wrh_mem (IPRG, 0x0000);
	sil_wrh_mem (IPRH, 0x0000);
	sil_wrh_mem (IPRI, 0x0000);
	sil_wrh_mem (IPRJ, 0x0000);
}
#endif /* _MACRO_ONLY */

#endif /* _SH7145_H_ */
