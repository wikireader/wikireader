/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
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

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */
#include <sys_rename.h>

/*
 *  割込みコントローラ・ボード依存のインクルードファイルの読み込み
 */
#include <pic_icu.h>	/* 割込みコントローラ関係の情報を<CPU>に渡す為 */
#include <vr5500.h>	/* NOP_FOR_CP0_HAZARDの情報をcpu_support.Sに渡す為 */

/*
 *  vgxet_tim をサポートするかどうかの定義
 */
#define	SUPPORT_VXGET_TIM

/*
 *  シリアルポート数の定義
 */
#ifndef GDB_STUB
#define TNUM_SIOP	2u	/* サポートするシリアルポートの数 */
#define TNUM_PORT	2u	/* サポートするシリアルI/Oポートの数 */
#else  /* GDB_STUB */
#define TNUM_SIOP	1u	/* サポートするシリアルポートの数 */
#define TNUM_PORT	1u	/* サポートするシリアルI/Oポートの数 */
				/* GDB_STUB を利用するときは、SCC1を GDB_STUB が
				   利用するために、SCC0のみしか利用できない。 */
#endif /* GDB_STUB */


/*
 *  システムタスクに関する定義
 */
#define	LOGTASK_PORTID	1u	/* システムログを出力するシリアルポート番号 */

/*
 *  微少時間待ちのための定義
 */
#define	SIL_DLY_TIM1	2865
#define	SIL_DLY_TIM2	398

/*
 *  割込みマスクの初期値 (MIPS3コアのもターゲットシステム依存なので、ここで定義)
 */

/*  MIPS3コアの関係 */
/*  実質的な割込み制御は、外部割込みコントローラで行うため、MIPS3コアに関しては
    可能な分を許可する。*/
/*  RTE-VR5500-CBでは2本の割込み線が接続されている。 */
#define INIT_CORE_IPM	( Cause_Int1 | Cause_Int0 )

/*  外部割込みコントローラ関係  */
/*  (割込み線０において、タイマ０のみ割込み許可） */
#define INIT_INT0M	TIMER0
#define INIT_INT1M	0u

#ifndef _MACRO_ONLY

/*
 *  ターゲットシステム依存の初期化
 */
extern void	sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．ROMモニタ呼出しで実現することを想定し
 *  ている．
 */
extern void	sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．ROMモニタ呼出しで実現するこ
 *  とを想定している．
 */

extern void	sys_putc(char c) throw();

#endif /* _MACRO_ONLY */

/*============================================================================*/
/* 以下は、共通ドキュメントに無い独自の定義 */

/*
 *  ステータスレジスタの初期値
 */
#define INIT_SR		(SR_BEV | INIT_CORE_IPM | SR_DE)
	/*
	 *  BEV = 1：例外ベクタをkseg0セグメントに配置
	 *  IM = 0：CPUロック状態 (割込み禁止)
	 *  DE = 1 : キャッシュパリティエラーによる例外発生の禁止
	 */

/*
 *  コンフィグレジスタ初期値設定用マスク
 */
#define INIT_CONFIG_MASK	0x0

/*
 *  シリアルコントローラのボーレートの設定 (分周比設定データ；[bps]で指定)
 */
#define DEVIDE_RATIO	9600u	/*  9600bpsを選択  */

#endif /* _SYS_CONFIG_H_ */
