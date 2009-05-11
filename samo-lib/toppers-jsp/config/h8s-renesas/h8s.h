/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
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

#ifndef _H8S_H_
#define _H8S_H_

#include <util.h>

/*
 *  H8S/2600, H8S/2000 CPU 用定義
 *    割込み関連とI/Oポートはチップ毎に異なるので
 *    h8sxxxx.hで定義している。
 */

/*
 *  ベースアドレス
 */
#ifndef H8S_NORMAL_MODE
#define H8S_BASE_ADDR	0xff0000	/* アドバンスドモード */
#else /* H8S_NORMAL_MODE */
#define H8S_BASE_ADDR	0x000000	/* ノーマルモード */
#endif /* H8S_NORMAL_MODE */

/*
 *  コンディションコードレジスタ(CCR)
 */
#define CCR_I		BIT7
#define CCR_UI		BIT6
#define CCR_H		BIT5
#define CCR_U		BIT4
#define CCR_N		BIT3
#define CCR_Z		BIT2
#define CCR_V		BIT1
#define CCR_C		BIT0

/*
 *  エクステンドレジスタ(EXR)
 */
#define EXR_T		BIT7		/* トレースビット */
#define EXR_I2		BIT2		/* 割込みマスクビット */
#define EXR_I1		BIT1		/* 割込みマスクビット */
#define EXR_I0		BIT0		/* 割込みマスクビット */

/* 割込みマスクビット取得用マスク */
#define EXR_I_MASK	(EXR_I2|EXR_I1|EXR_I0)

/*
 *  MCU動作モード関連のレジスタ
 */

/* レジスタのアドレス */
#define MDCR		0xff3b		/*  モードコントロールレジスタ  */
#define SYSCR		0xff39		/*  システムコントロールレジスタ  */

/* 各レジスタのビットパターン */

/* MDCR */
#define MDS2		BIT2
#define MDS1		BIT1
#define MDS0		BIT0

/* SYSCR */
#define INTM1		BIT5
#define INTM0		BIT4
#define NMIEG		BIT3
#define LWROD		BIT2
#define IRQPAS		BIT1
#define RAME		BIT0

#define INT_MODE2	INTM1	/*  割込み制御モード２  */
#define RAME_BIT	0x0

#endif /* _H8S_H_ */
