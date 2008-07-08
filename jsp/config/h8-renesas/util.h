/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2007 by Industrial Technology Institute,
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

#ifndef _UTIL_H_
#define _UTIL_H_

/*
 *  ビット番号の定義
 */
#define BIT0              0x1u
#define BIT1              0x2u
#define BIT2              0x4u
#define BIT3              0x8u
#define BIT4             0x10u
#define BIT5             0x20u
#define BIT6             0x40u
#define BIT7             0x80u
#define BIT8            0x100u
#define BIT9            0x200u
#define BIT10           0x400u
#define BIT11           0x800u
#define BIT12          0x1000u
#define BIT13          0x2000u
#define BIT14          0x4000u
#define BIT15          0x8000u
#define BIT16         0x10000ul
#define BIT17         0x20000ul
#define BIT18         0x40000ul
#define BIT19         0x80000ul
#define BIT20        0x100000ul
#define BIT21        0x200000ul
#define BIT22        0x400000ul
#define BIT23        0x800000ul
#define BIT24       0x1000000ul
#define BIT25       0x2000000ul
#define BIT26       0x4000000ul
#define BIT27       0x8000000ul
#define BIT28      0x10000000ul
#define BIT29      0x20000000ul
#define BIT30      0x40000000ul
#define BIT31      0x80000000ul


/*
 *  シンボル名の結合
 */
#define JOINT(a,b)      _JOINT(a,b)
#define _JOINT(a,b)     a##b

#define JOINT3(a,b,c)   _JOINT3(a,b,c)
#define _JOINT3(a,b,c)  JOINT(a,JOINT(b,c))

#define JOINT4(a,b,c,d)   _JOINT4(a,b,c,d)
#define _JOINT4(a,b,c,d)  JOINT(a,JOINT3(b,c,d))

#define HI2(data)	(UH)(data >> 16)
#define LO2(data)	(UH)(data)

#endif /* _UTIL_H_ */
