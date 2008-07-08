/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005,2006 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: t_services.h,v 1.7 2006/01/13 00:34:58 hiro Exp $
 */

/*
 *	アプリケーション用 標準インクルードファイル
 *
 *  このインクルードファイルは，カーネル上で動作するプログラムのソース
 *  ファイルでインクルードする標準インクルードファイルである．この中で，
 *  kernel.h（さらにここから，t_stddef.h，itron.h，tool_defs.h，
 *  sys_defs.h，cpu_defs.h，t_syslog.h）とserial.hをインクルードしてい
 *  る．また，アプリケーションに有益と思われる定義をいくつか含んでいる．
 *
 *  アセンブリ言語のソースファイルやシステムコンフィギュレーションファ
 *  イルからこのファイルをインクルードする時は，_MACRO_ONLY を定義して
 *  おくことで，マクロ定義以外の記述を除くことができる．
 */

#ifndef _T_SERVICES_H_
#define _T_SERVICES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  カーネルを呼び出すために必要なインクルードファイル
 */
#include <kernel.h>

#ifndef _MACRO_ONLY

/*
 *  シリアルインタフェースモジュール
 */
#include <serial.h>

/*
 *  カーネルの終了処理
 */
extern void	kernel_exit(void) throw();

/*
 *  syscallマクロ，_syscall マクロの定義
 */

extern void	t_perror(UINT prio, const char *file, int line,
					const char *expr, ER ercd) throw();

Inline ER
_t_perror(const char *file, int line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
	return(ercd);
}

Inline ER
_t_panic(const char *file, int line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_EMERG, file, line, expr, ercd);
		kernel_abort();
	}
	return(ercd);
}

#define syscall(s)	_t_perror(__FILE__, __LINE__, #s, (s))
#define _syscall(s)	_t_panic(__FILE__, __LINE__, #s, (s))

#endif /* _MACRO_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _T_SERVICES_H_ */
