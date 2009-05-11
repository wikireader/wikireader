/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2004 by Industrial Technology Institute,
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
 *   ビット番号の定義
 *   　注意：PowerPCのビット番号は通常と逆になっている
 *   　　　　また、32ビットレジスタと16ビットレジスタでは
 *   　　　　最下位ビットのビット番号が異なる点にも注意
 */   

/*  32ビットレジスタ用  */
#define BIT0_32    0x80000000
#define BIT1_32    0x40000000
#define BIT2_32    0x20000000
#define BIT3_32    0x10000000
#define BIT4_32     0x8000000
#define BIT5_32     0x4000000
#define BIT6_32     0x2000000
#define BIT7_32     0x1000000
#define BIT8_32      0x800000
#define BIT9_32      0x400000
#define BIT10_32     0x200000
#define BIT11_32     0x100000
#define BIT12_32      0x80000
#define BIT13_32      0x40000
#define BIT14_32      0x20000
#define BIT15_32      0x10000
#define BIT16_32       0x8000
#define BIT17_32       0x4000
#define BIT18_32       0x2000
#define BIT19_32       0x1000
#define BIT20_32        0x800
#define BIT21_32        0x400
#define BIT22_32        0x200
#define BIT23_32        0x100
#define BIT24_32         0x80
#define BIT25_32         0x40
#define BIT26_32         0x20
#define BIT27_32         0x10
#define BIT28_32          0x8
#define BIT29_32          0x4
#define BIT30_32          0x2
#define BIT31_32          0x1

/*  16ビットレジスタ用  */
#define BIT0_16        0x8000
#define BIT1_16        0x4000
#define BIT2_16        0x2000
#define BIT3_16        0x1000
#define BIT4_16         0x800
#define BIT5_16         0x400
#define BIT6_16         0x200
#define BIT7_16         0x100
#define BIT8_16          0x80
#define BIT9_16          0x40
#define BIT10_16         0x20
#define BIT11_16         0x10
#define BIT12_16          0x8
#define BIT13_16          0x4
#define BIT14_16          0x2
#define BIT15_16          0x1

/*  8ビットレジスタ用  */
#define BIT0_8           0x80
#define BIT1_8           0x40
#define BIT2_8           0x20
#define BIT3_8           0x10
#define BIT4_8            0x8
#define BIT5_8            0x4
#define BIT6_8            0x2
#define BIT7_8            0x1

/*  シンボル名の文字列化  */
#define _TO_STRING(arg)	#arg
#define TO_STRING(arg)	_TO_STRING(arg)

#endif /* _UTIL_H_ */
/*  end of file  */
