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
 *  @(#) $Id: sys_defs.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	ターゲットシステムに依存する定義（MPC860T TB6102S用）
 *　　　　　アプリケーションが必要とする定義
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  他のファイルから直接インクルードすることはない．このファイルをイン
 *  クルードする前に，t_stddef.h と itron.h がインクルードされるので，
 *  それらに依存してもよい．
 */

#ifndef _SYS_DEFS_H_
#define _SYS_DEFS_H_

/*
 *  ユーザー定義情報
 */
#include <user_config.h>

#define TB6102S			/* システム略称 */
#define MPC860T			/* プロセッサ略称 */


#ifndef _MACRO_ONLY

/*
 *  ICUに設定するIPMの型定義
 *  　　MPC860ではシステムインターフェースユニットSIUの割込みマスク
 *  　　（SIU割込みマスクレジスタSIMASK）のみカーネルで管理する。
 *  　　通信プロセッサモジュールCPMの割込みマスクの管理はすべてハード
 *  　　ウェアが行っている。
 */
typedef UH IPM;

/*
 *  割込み番号の型と割込みの禁止／許可
 */
 	/*  
 	 * 　 PowerPC MPC860T版ではサポートしていない。
	 *  　実装にかかる工数は割込みコントローラの構造に
	 *  　大きく依存する  
	 */
#if 0
typedef	UINT	INTNO;		/* 割込み番号 */

extern ER	dis_int(INTNO intno) throw();
extern ER	ena_int(INTNO intno) throw();

#endif /* 0 */

#endif /* _MACRO_ONLY */

#ifndef _MACRO_ONLY

/*
 *  性能評価用システム時刻の参照
 */
#ifdef SUPPORT_VXGET_TIM

typedef	UD	SYSUTIM;	/* 性能評価用システム時刻 */
extern ER	vxget_tim(SYSUTIM *p_sysutim) throw();

#endif	/*  SUPPORT_VXGET_TIM  */

/*
 *  システムの停止処理
 */
#ifdef GDB_STUB

Inline void
kernel_abort()
{
	/*  未実装  */
}

#else /* GDB_STUB */

Inline void
kernel_abort()
{
	while(1);
}

#endif /* GDB_STUB */
#endif /* _MACRO_ONLY */
#endif /* _SYS_DEFS_H_ */
/*  end of file  */
