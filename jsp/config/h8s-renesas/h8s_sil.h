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

#ifndef _H8S_SIL_H_
#define _H8S_SIL_H_

#ifndef _MACRO_ONLY
#include <sil.h>
#endif /* _MACRO_ONLY */

/*
 *  H8S内部レジスタ・アクセスインタフェース
 *　　ベースアドレスH8S_BASE_ADDRはh8s.hで定義し、
 *　　CPU動作モードにより、使い分ける。
 */

/*
 *  ベースアドレスの加算
 *　　UHでキャストしているのは、２つの意味がある。
 *　　　・下位16ビットを取り出す
 *　　　・加算の際の符号拡張を防ぐ
 */
#define ADD_BASE_ADDR(addr)	(VP)(H8S_BASE_ADDR + (UW)(UH)(addr))

/*  レジスタへのアクセス  */
#define h8s_reb_reg(addr)	sil_reb_mem(ADD_BASE_ADDR(addr) )
#define h8s_wrb_reg(addr, val)	sil_wrb_mem(ADD_BASE_ADDR(addr), (VB)(val))

#define h8s_reh_reg(addr)	sil_reh_mem(ADD_BASE_ADDR(addr) )
#define h8s_wrh_reg(addr, val)	sil_wrh_mem(ADD_BASE_ADDR(addr), (VH)(val))

/*
 *  論理演算
 */
#define h8s_orb_reg(addr, val)	h8s_wrb_reg(addr, h8s_reb_reg(addr) | (val))
#define h8s_andb_reg(addr, val) h8s_wrb_reg(addr, h8s_reb_reg(addr) & (val))

#define h8s_orh_reg(addr, val)	h8s_wrh_reg(addr, h8s_reh_reg(addr) | (val))
#define h8s_andh_reg(addr, val)	h8s_wrh_reg(addr, h8s_reh_reg(addr) & (val))

#endif /* _H8S_SIL_H_ */
