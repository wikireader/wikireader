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

#ifndef	_CPU_INSN_H_
#define	_CPU_INSN_H_

/* 下記にて利用している、MAX_IPM は cpu_config.h で定義 */

#ifndef _MACRO_ONLY

/*
 *  制御レジスタの操作関数
 */

#include <machine.h>	/*  処理系の組み込み関数  */

/*
 *  コンデションコードレジスタ（CCR）の現在値の読出し
 *　　UB current_ccr(void);
 *　　　→　unsigned char get_ccr(void);
 */
#define current_ccr	get_ccr

/*
 *  コンデションコードレジスタ（CCR）の現在値の変更
 *　　組み込み関数のset_ccr()をそのまま使う。
 *　　void set_ccr(unsigned char ccr);
 */

/*
 *  エクステンドレジスタ（EXR）の現在値の読出し
 *　　UB current_exr(void);
 *　　　→　unsigned char get_exr(void);
 */
#define current_exr	get_exr

/*
 *  エクステンドレジスタ（EXR）の現在値の変更
 *　　組み込み関数のset_exr()をそのまま使う。
 *　　void set_exr(unsigned char exr);
 */

/*
 *  割込みマスク操作ライブラリ (割込みモード２用)
 */

/*
 *  割込みマスクの現在値の読出し
 *　　IPM current_intmask(void);
 *　　　→　unsigned char get_imask_exr(void);
 */
#define current_intmask		(IPM)get_imask_exr

/*
 *  割込みマスクの設定
 *　　　・引数intmaskの範囲チェックは省略している。
 */
#define set_intmask(intmask)	set_imask_exr(intmask)

/* 用語定義
 *　　カーネル管理下の割込み ＝ 割込みレベルがMAX_IPM 以下の割込み
 *　　カーネル管理外の割込み ＝ プライオリティレベル(MAX_IPM+1)
 *　　　　　　　　　　　　　　　以上の割込みとNMI
 */

/*
 *  カーネル管理下の割込みを禁止
 *　　　・EXRレジスタのトレースビットTの保存は省略している。
 */
#define disint()	set_intmask( (UB)MAX_IPM )

/*
 *  カーネル管理下の割込みを許可
 *　　　・EXRレジスタのトレースビットTの保存は省略している。
 *　　　・この処理は、事実上、カーネル管理下の有無を問わず、
 *　　　　すべての割込みを許可することになっている。
 */
#define enaint()	set_intmask( (UB)0 )

/*
 *  割込みロック状態の制御用関数
 */

/*
 *  カーネル管理外も含めてすべての割込みを禁止 (NMIを除く)
 *　　　・EXRレジスタのトレースビットTの保存は省略している。
 */
#define _disint_()	set_exr( EXR_I_MASK )


#endif /* _MACRO_ONLY */

#endif /* _CPU_INSN_H_ */
