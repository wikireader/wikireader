/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: t_stddef.h,v 1.7 2004/01/05 05:11:37 hiro Exp $
 */

/*
 *	カーネル・アプリケーション 共通インクルードファイル
 *
 *  このインクルードファイルは，kernel.h と sil.h の先頭でインクルード
 *  される．他のファイルから直接インクルードされることはない．
 *
 *  この中でインクルードしているファイルを除いて，他のインクルードファ
 *  イルに依存していない．
 */

#ifndef _T_STDDEF_H_
#define _T_STDDEF_H_

/*
 *  開発環境に依存する定義
 */
#include <tool_defs.h>

/*
 *  開発環境の標準インクルードファイル（CHAR_BIT の定義が必要）
 *
 *  C++/EC++ では，標準仕様上は limits.h がサポートされているとは限らな
 *  いが，ほとんどの処理系でサポートされている．
 */
#ifndef _MACRO_ONLY
#include <limits.h>
#endif /* _MACRO_ONLY */

/*
 *  コンパイラの拡張機能のためのマクロのデフォルト定義
 */
#ifndef Inline
#define	Inline		static inline
#endif /* Inline */

/*
 *  C言語/EC++で throw() に対処
 */
#if !defined(__cplusplus) || defined(__embedded_cplusplus)
#define throw()
#endif

/*
 *  assertマクロの定義
 */
#undef assert
#ifndef NDEBUG
#define assert(exp) \
	((void)(!(exp) ? (_syslog_3(LOG_EMERG, LOG_TYPE_ASSERT,		\
				(VP_INT)(__FILE__), (VP_INT)(__LINE__),	\
				(VP_INT)(#exp)),			\
				kernel_abort(), 0) : 0))
#else /* NDEBUG */
#define	assert(exp)	((void) 0)
#endif /* NDEBUG */

#endif /* _T_STDDEF_H_ */
