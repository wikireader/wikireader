/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2003-2004 by CHUO ELECTRIC WORKS Co.,LTD. JAPAN
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
 */

/*
 *   SH1内蔵シリアルコミュニケーションインタフェースSCI用 簡易ドライバ
 *
 *　　　　SCI0/1サポート
 *
 *　　　　sh1sci.{c,h}とhw_serial.hの分割の基準
 *　　　　　　・sh1sci.{c,h}：シリアルデバイスに依存する部分のみ記述
 *　　　　　　・hw_serial.h：シリアルデバイス以外に起因する事項は
 *　　　　 　　　　　　　　　hw_serial.hに記述する
 *　　　　　　　　　　　　　　割込みコントローラ依存など
 */

#ifndef _SH1SCI2_H_
#define _SH1SCI2_H_

#include <t_services.h>
#include <sh1_sil.h>

/*
 *  シリアルポートインターフェース
 */
#define	PORT1			1
#define	PORT2			2

#define RS232C_INTERFACE	0	/* RS232C I/Fの場合、送信許可制御 */
#define RS485_INTERFACE		1	/* RS485 I/Fの場合、送信許可制御 */

/*
 *  シリアルポートの初期化ブロック
 */
typedef struct sio_port_initialization_block {
	UW 	base;		/* SCI のベースアドレス		*/
	UW	baudrate;	/* ボーレイト			*/	
	VH  	*ipr;		/* 割り込みコントローラアドレス */
	UH	shift;		/* 割り込みコントローラシフト	*/
	UB	smr_init;	/* SMR の設定値			*/
	UINT	delay;		/* ボーレートの設定後待ち	*/
	UB	rs485if;	/* RS232C | RS485		*/
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロックの定義
 *  　2chに拡張
 */
typedef struct sio_port_control_block {
	const SIOPINIB	*inib;		/* 初期値用のデータ 	*/
	VP_INT		exinf;		/* 拡張情報 		*/
	BOOL		openflag;	/* オープン済みフラグ 	*/
} SIOPCB;

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND	1u		/* 送信可能コールバック */
#define SIO_ERDY_RCV	2u		/* 受信通知コールバック */

/*
 *  SIOドライバの初期化ルーチン
 */
extern void	sh1sci_initialize(void);

/*
 *  オープンしているポートがあるか？
 */
extern BOOL	sh1sci_openflag(ID siopid);

/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB	*sh1sci_opn_por(ID siopid, VP_INT exinf);

/*
 *  シリアルI/Oポートのクローズ
 */
extern void	sh1sci_cls_por(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL	sh1sci_snd_chr(SIOPCB *siopcb, char c);

/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT	sh1sci_rcv_chr(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void	sh1sci_ena_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void	sh1sci_dis_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  SIOの割込みサービスルーチン
 */
extern void	sh1sci_isr_in(void);
extern void	sh1sci_isr_out(void);

/*
 *  SIOの受信エラー割込みサービスルーチン
 */
extern void	sh1sci_isr_error(void);

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void	sh1sci_ierdy_snd(VP_INT exinf);

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void	sh1sci_ierdy_rcv(VP_INT exinf);



#endif /* _SH1SCI2_H_ */
