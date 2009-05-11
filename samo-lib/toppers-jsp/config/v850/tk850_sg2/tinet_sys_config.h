/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2003 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: tinet_sys_config.h,v 1.2 2007/01/05 02:10:17 honda Exp $
 */

#ifndef _TINET_SYS_CONFIG_H_
#define _TINET_SYS_CONFIG_H_

#include <v850es_sg2.h>

/*
 *  データリンク層 (ネットワークインタフェース) に関する定義
 */

/*
 *  NIC (SMCS LAN91C111) に関する定義
 */


#define NUM_IF_ED_TXBUF		1			/* 送信バッファ数 */

#define INHNO_IF_ED			0x0A		/* IRQ=INTP1 */

#define ED_INT_PRIORITY		1			/* 割り込み優先度(0が最高7が最低) */

/*
 * NIC(SMSC LAN91C111)に関する定義
 */
#define ED_BASE_ADDRESS		(0x100600)	/* NIC のレジスタベースアドレス */
#define ED_REG_OFFSET		4			/* NICレジスターのOFFSET */

#define TMO_IF_ED_GET_NET_BUF	1		/* [ms]、受信用 net_buf 獲得タイムアウト	*/

/*
 * 以下の定義はターゲットCPUの動作周波数に依存する時間調整パラメータ
 *   この定義はV850/SG2の20MHz動作用
 */

#define WAIT_LOOP_CNT		8000		/* 微小時間のポーリング回数 */

#define uWAIT()		nop(1);				/* 約1usecの微小時間待ち */

#endif /* _TINET_SYS_CONFIG_H_ */
