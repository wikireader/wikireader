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
 *  @(#) $Id: powerpc.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*  
 *   PowerPCアーキテクチャ依存の定義
 *   　注意：一部、32ビット・インプリメント専用になっている
 *   　
 *   　PowerPCアーキテクチャの定義は以下の３つのレベルから成る
 *   　・USIA:User Instruction Set Architecture
 *   　・VEA: Virtual Environment Architecture
 *   　・OEA: Operating Environment Architecture
 *   　
 *   　USIAについては全機種共通であるが、VEAとOEAについては
 *   　オリジナルのPowerPCアーキテクチャとThe IBM PowerPC 
 *   　Embedded Environmentそれぞれ別に定義されているため、
 *   　VEAとOEAの定義はファイルを分けて、それをインクルード
 *   　している。
 */   

#ifndef _POWERPC_H_
#define _POWERPC_H_

#include <util.h>


/*  
 *   VEAとOEAの定義
 */   

#ifndef IBM_PPC_EMB_ENV
/*  
 *   オリジナルのPowerPCアーキテクチャの場合
 *   　　モトローラMPCシリーズ、IPM PowerPC6xx/7xxシリーズは
 *   　　こちらに該当する。
 */   
#include <vea_oea.h>

#else	/*  IBM_PPC_EMB_ENV  */
/*  
 *   The IBM PowerPC Embedded Environmentの場合
 *   　IBM系PowerPC40xシリーズ対応
 */   
#include <vea_oea_emb.h>

#endif	/*  IBM_PPC_EMB_ENV  */


/*  
 *   UISAの定義
 */   

/*  
 *  レジスタ番号の定義
 *  　　C言語の変数名とぶつかりそうなので、アセンブラ内に限定する
 */   
#ifdef _MACRO_ONLY

/* CPUコアの内部レジスタの名称 */

/*  汎用レジスタ  */
#define r0       0      /*  zeroまたは汎用レジスタ  */
#define r1       1
#define sp       r1     /*  スタックポインタ  */

#define r2       2      /*  TOC：Table of Contents  */
#define r3       3      /*  r3-4：引数、戻り値  */
#define r4       4
#define r5       5      /*  r5-10：引数  */
#define r6       6
#define r7       7
#define r8       8
#define r9       9
#define r10     10
#define r11     11
#define r12     12
#define r13     13      /*  r13：small data  */
#define r14     14      /*  r14-30：ローカル変数  */
#define r15     15
#define r16     16
#define r17     17
#define r18     18
#define r19     19
#define r20     20
#define r21     21
#define r22     22
#define r23     23
#define r24     24
#define r25     25
#define r26     26
#define r27     27
#define r28     28
#define r29     29
#define r30     30
#define r31     31
#define fp      r31     /*  フレームポインタ  */

/*  浮動小数点レジスタ  */
#define f0       0
#define f1       1      /*  f1：引数、戻り値  */
#define f2       2      /*  f2-13：引数  */
#define f3       3
#define f4       4
#define f5       5
#define f6       6
#define f7       7
#define f8       8
#define f9       9
#define f10     10
#define f11     11
#define f12     12
#define f13     13
#define f14     14      /*  f14-31：ローカル変数  */
#define f15     15
#define f16     16
#define f17     17
#define f18     18
#define f19     19
#define f20     20
#define f21     21
#define f22     22
#define f23     23
#define f24     24
#define f25     25
#define f26     26
#define f27     27
#define f28     28
#define f29     29
#define f30     30
#define f31     31

/*  特殊レジスタ  */
#define XER      1      /* Fixed Point Exception Register  */
			/*  整数オペレーションの条件識別レジスタ */
			/*  （キャリーやオーバフローなど）  */
#define LR       8      /*  リンク・レジスタ  */
#define CTR      9      /*  カウント・レジスタ  */

#endif  /* _MACRO_ONLY */

#define crf0        0   /*  コンディションレジスタCRのフィールド指定  */

#endif /* _POWERPC_H_ */
/*  end of file  */
