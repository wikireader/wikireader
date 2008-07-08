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
 *  @(#) $Id: t_syslog.h,v 1.4 2004/01/05 05:15:01 hiro Exp $
 */

/*
 *	システムログ機能
 *
 *  システムログサービスは，システムのログ情報を出力するためのサービス
 *  である．カーネルからのログ情報の出力にも用いるため，内部で待ち状態
 *  にはいることはない．
 *
 *  ログ情報は，カーネル内のログバッファに書き込むか，低レベルの文字出
 *  力関数を用いて出力する．どちらを使うかは，拡張サービスコールで切り
 *  換えることができる．
 *
 *  ログバッファ領域がオーバフローした場合には，古いログ情報を消して上
 *  書きする．
 *
 *  アセンブリ言語のソースファイルやシステムコンフィギュレーションファ
 *  イルからこのファイルをインクルードする時は，_MACRO_ONLY を定義して
 *  おくことで，マクロ定義以外の記述を除くことができる．
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  他のファイルから直接インクルードされることはない．
 *
 *  このファイルをインクルードする前に，t_stddef.h と itron.h をインク
 *  ルードしておくことが必要である．
 */

#ifndef _T_SYSLOG_H_
#define _T_SYSLOG_H_

/*
 *  ログ情報の種別の定義
 *
 *  LOG_TYPE_CYC，LOG_TYPE_ASSERT 以外は，デバッギングインタフェース仕
 *  様と合致している．
 */
#define LOG_TYPE_INH		0x01u	/* 割込みハンドラ */
#define LOG_TYPE_ISR		0x02u	/* 割込みサービスルーチン */
#define LOG_TYPE_CYC		0x03u	/* 周期ハンドラ */
#define LOG_TYPE_EXC		0x04u	/* CPU例外ハンドラ */
#define LOG_TYPE_TEX		0x05u	/* タスク例外処理ルーチン */
#define LOG_TYPE_TSKSTAT	0x06u	/* タスク状態変化 */
#define LOG_TYPE_DSP		0x07u	/* ディスパッチャ */
#define LOG_TYPE_SVC		0x08u	/* サービスコール */
#define LOG_TYPE_COMMENT	0x09u	/* コメント */
#define LOG_TYPE_ASSERT		0x0au	/* アサーションの失敗 */

#define LOG_ENTER		0x00u	/* 入口／開始 */
#define LOG_LEAVE		0x80u	/* 出口／終了 */

/*
 *  ログ情報の重要度の定義
 */
#define LOG_EMERG	0u		/* シャットダウンに値するエラー */
#define LOG_ALERT	1u
#define LOG_CRIT	2u
#define LOG_ERROR	3u		/* システムエラー */
#define LOG_WARNING	4u		/* 警告メッセージ */
#define LOG_NOTICE	5u
#define LOG_INFO	6u
#define LOG_DEBUG	7u		/* デバッグ用メッセージ */

#ifndef _MACRO_ONLY

/*
 *  ログ情報のデータ構造
 */

#define TMAX_LOGINFO	6

typedef struct {
		UINT	logtype;		/* ログ情報の種別 */
		SYSTIM	logtim;			/* ログ時刻 */
		VP_INT	loginfo[TMAX_LOGINFO];	/* その他のログ情報 */
	} SYSLOG;

/*
 *  ログ情報の重要度のビットマップを作るためのマクロ
 */
#define LOG_MASK(prio)	(1u << (prio))
#define LOG_UPTO(prio)	((1u << ((prio) + 1)) - 1)

#ifndef OMIT_SYSLOG

/*
 *  ログ情報の出力
 */
extern ER	vwri_log(UINT prio, SYSLOG *p_log) throw();

/*
 *  ログバッファからのログ情報の読出し
 */
extern ER_UINT	vrea_log(SYSLOG *p_log) throw();

/* 
 *  出力すべきログ情報の重要度の設定
 */
extern ER	vmsk_log(UINT logmask, UINT lowmask) throw();

/*
 *  ログ情報を出力するためのライブラリ関数
 */

Inline ER
_syslog_0(UINT prio, UINT type)
{
	SYSLOG	log;

	log.logtype = type;
	return(vwri_log(prio, &log));
}

Inline ER
_syslog_1(UINT prio, UINT type, VP_INT arg1)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	return(vwri_log(prio, &log));
}

Inline ER
_syslog_2(UINT prio, UINT type, VP_INT arg1, VP_INT arg2)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	return(vwri_log(prio, &log));
}

Inline ER
_syslog_3(UINT prio, UINT type, VP_INT arg1, VP_INT arg2, VP_INT arg3)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	log.loginfo[2] = arg3;
	return(vwri_log(prio, &log));
}

Inline ER
_syslog_4(UINT prio, UINT type, VP_INT arg1, VP_INT arg2,
				VP_INT arg3, VP_INT arg4)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	log.loginfo[2] = arg3;
	log.loginfo[3] = arg4;
	return(vwri_log(prio, &log));
}

Inline ER
_syslog_5(UINT prio, UINT type, VP_INT arg1, VP_INT arg2,
				VP_INT arg3, VP_INT arg4, VP_INT arg5)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	log.loginfo[2] = arg3;
	log.loginfo[3] = arg4;
	log.loginfo[4] = arg5;
	return(vwri_log(prio, &log));
}

Inline ER
_syslog_6(UINT prio, UINT type, VP_INT arg1, VP_INT arg2, VP_INT arg3,
				VP_INT arg4, VP_INT arg5, VP_INT arg6)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	log.loginfo[2] = arg3;
	log.loginfo[3] = arg4;
	log.loginfo[4] = arg5;
	log.loginfo[5] = arg6;
	return(vwri_log(prio, &log));
}

#else /* OMIT_SYSLOG */

#define vwri_log(prio, p_log)		E_OK
#define vrea_log(p_log)			E_OK
#define vmsk_log(logmask, lowmask)	E_OK

#define _syslog_0(prio, type)						E_OK
#define _syslog_1(prio, type, arg1)					E_OK
#define _syslog_2(prio, type, arg1, arg2)				E_OK
#define _syslog_3(prio, type, arg1, arg2, arg3)				E_OK
#define _syslog_4(prio, type, arg1, arg2, arg3, arg4)			E_OK
#define _syslog_5(prio, type, arg1, arg2, arg3, arg4, arg5)		E_OK
#define _syslog_6(prio, type, arg1, arg2, arg3, arg4, arg5, arg6)	E_OK

#endif /* OMIT_SYSLOG */

/*
 *  ログ情報（コメント）を出力するためのマクロ
 *
 *  format および後続の引数から作成したメッセージを，重大度 prio で
 *  ログ情報として出力するためのマクロ．arg1〜argn は VP_INT型にキャ
 *  ストするため，VP_INT型に型変換できる任意の型でよい．
 */

#define	syslog_0(prio, format) \
		_syslog_1(prio, LOG_TYPE_COMMENT, (VP_INT) format)

#define	syslog_1(prio, format, arg1) \
		_syslog_2(prio, LOG_TYPE_COMMENT, (VP_INT) format, \
							(VP_INT)(arg1))

#define	syslog_2(prio, format, arg1, arg2) \
		_syslog_3(prio, LOG_TYPE_COMMENT, (VP_INT) format, \
					(VP_INT)(arg1), (VP_INT)(arg2))

#define	syslog_3(prio, format, arg1, arg2, arg3) \
		_syslog_4(prio, LOG_TYPE_COMMENT, (VP_INT) format, \
			(VP_INT)(arg1), (VP_INT)(arg2), (VP_INT)(arg3))

#define	syslog_4(prio, format, arg1, arg2, arg3, arg4) \
		_syslog_5(prio, LOG_TYPE_COMMENT, (VP_INT) format, \
			(VP_INT)(arg1), (VP_INT)(arg2), (VP_INT)(arg3), \
							(VP_INT)(arg4))

#define	syslog_5(prio, format, arg1, arg2, arg3, arg4, arg5) \
		_syslog_6(prio, LOG_TYPE_COMMENT, (VP_INT) format, \
			(VP_INT)(arg1), (VP_INT)(arg2), (VP_INT)(arg3), \
					(VP_INT)(arg4), (VP_INT)(arg5))

/*
 *  ログ情報（コメント）を出力するためのライブラリ関数（vasyslog.c）
 */
extern ER	syslog(UINT prio, const char *format, ...) throw();

/* 
 *  ログ情報のフォーマット出力（log_output.c）
 */
extern void	syslog_printf(const char *format, VP_INT *args,
						void (*putc)(char)) throw();
extern void	syslog_print(SYSLOG *p_log, void (*putc)(char)) throw();
extern void	syslog_output(void (*putc)(char)) throw();

#endif /* _MACRO_ONLY */
#endif /* _T_SYSLOG_H_ */
