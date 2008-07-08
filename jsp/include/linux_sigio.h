/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: linux_sigio.h,v 1.10 2003/12/06 06:18:55 hiro Exp $
 */

/*
 *  LINUX用 ノンブロッキングI/O サポートモジュール
 *
 *  ノンブロッキングI/O サポートモジュールは，SIGIO シグナルにより，ユー
 *  ザが登録したコールバック関数を呼び出す機能を持つ．
 *
 *  SIGIO シグナルによりコールバック関数を実行させたい場合には，SIGIO 
 *  通知イベントブロックを用意し，その callback フィールドにコールバッ
 *  ク関数，arg フィールドにコールバック関数へ渡す引数を設定し，
 *  eneuque_sigioeb を用いて SIGIO 通知イベントキューに登録する．
 *
 *  呼び出されたコールバック関数が 0 を返すと，関連する SIGIO 通知イベ
 *  ントブロック はキューに登録されたままとなり，続く SIGIO シグナルで
 *  再び同じコー ルバック関数が呼び出される．コールバック関数が 0 以外
 *  を返すと，SIGIO 通知イベントブロックはキューから削除され，コールバッ
 *  ク関数はそれ以降呼び出されなくなる．
 */

#ifndef	_LINUX_SIGIO_H_
#define	_LINUX_SIGIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  SIGIO 通知イベントブロックの定義
 */
typedef BOOL	(*SIGIO_CBACK)(VP);	/* SIGIO コールバック関数の型 */

typedef struct bsd_sigio_event_block {
	VP		queue[2];	/* SIGIO 通知イベントキューエリア */
	SIGIO_CBACK	callback;	/* SIGIO コールバック関数 */
	VP		arg;		/* コールバック関数へ渡す引数 */
} SIGIOEB;

/*
 *  SIGIO 通知イベントブロックの登録
 */
extern ER	enqueue_sigioeb(SIGIOEB *sigioeb) throw();

/*
 *  システム起動時用 SIGIO 通知イベントブロックの登録
 */
extern ER	enqueue_sigioeb_initialize(SIGIOEB *sigioeb) throw();

/*
 *   ノンブロッキングI/O モジュール起動ルーチン
 */

extern void linux_sigio_initialize(VP_INT exinf) throw();

/*
 *  割込みハンドラのベクタ番号
 */
#define	INHNO_SIGIO	SIGIO

/*
 *  SIGIO割り込みハンドラ
 */

extern void linux_sigio_handler() throw();
    
/*
 * SIGIOタスクの設定
 */
#define LINUX_SIGIO_PRIORITY      2
#define LINUX_SIGIO_STACK_SIZE 8192

extern void linux_sigio_task(void) throw();

#ifdef __cplusplus
}
#endif

#endif /* _LINUX_SIGIO_H_ */
