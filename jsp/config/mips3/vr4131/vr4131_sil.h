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
 */

#ifndef _MIPS3_SIL_H_
#define _MIPS3_SIL_H_

#include <kz_vr4131pci_01.h>	/* VR4131_BASE_ADDR */

#ifndef _MACRO_ONLY
#include <sil.h>
#endif /* _MACRO_ONLY */

/*
 *  VR4131用一般アクセスインタフェース
 */
#define vr4131_reb_mem(x)	sil_reb_mem( (VP)(VR4131_BASE_ADDR + x) )
#define vr4131_wrb_mem(x, y)	sil_wrb_mem( (VP)(VR4131_BASE_ADDR + x), y )

#define vr4131_reh_mem(x)	sil_reh_mem( (VP)(VR4131_BASE_ADDR + x) )
#define vr4131_wrh_mem(x, y)	sil_wrh_mem( (VP)(VR4131_BASE_ADDR + x), y )

/* アセンブラ用 SIL */
#define ASM_SIL(x)	(VR4131_BASE_ADDR + x)

/*
 *  VR4131用一般レジスタ操作関数
 */
#define vr4131_orb( mem, val )	vr4131_wrb_mem( mem, vr4131_reb_mem( mem ) | val )
#define vr4131_andb( mem, val )	vr4131_wrb_mem( mem, vr4131_reb_mem( mem ) & val )

#define vr4131_orh( mem, val )	vr4131_wrh_mem( mem, vr4131_reh_mem( mem ) | val )
#define vr4131_andh( mem, val )	vr4131_wrh_mem( mem, vr4131_reh_mem( mem ) & val )

#endif /* _MIPS3_SIL_H_ */
