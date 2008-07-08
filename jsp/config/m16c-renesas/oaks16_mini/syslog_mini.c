/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000,2001 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: syslog_mini.c,v 1.3 2005/11/24 12:41:23 honda Exp $
 */

/*
 *  システムログ機能簡略版
 */

#undef OMIT_SYSLOG
#include "jsp_kernel.h"
#include "time_event.h"
#include "serial.h"

/*
 *  出力すべきログ情報の重要度（ビットマップ）
 */
static UINT	syslog_logmask;		/* 記録／出力すべき重要度 */
static UINT	syslog_lowmask;		/* 低レベル出力すべき重要度 */

/*
 *  システムログ機能の初期化
 */
void syslog_initialize(void)
{
	syslog_logmask = LOG_UPTO(LOG_NOTICE);
	syslog_lowmask = LOG_UPTO(LOG_NOTICE);
}

/* 
 *  ログ情報の出力
 */
SYSCALL ER
vwri_log(UINT prio, SYSLOG *p_log)
{
	BOOL	locked;

	p_log->logtim = systim_offset + current_time;
	if ((syslog_logmask & LOG_MASK(prio)) != 0) {
		/*
	 	*  低レベル出力への書出し
	 	*/
		syslog_print(p_log, sys_putc);
		sys_putc('\n');
	}
	return(E_OK);
}

/* 
 *  出力すべきログ情報の重要度の設定
 */
SYSCALL ER
vmsk_log(UINT logmask, UINT lowmask)
{
	syslog_logmask = logmask;
	syslog_lowmask = lowmask;
	return(E_OK);
}

/* 
 *  システムログ機能の終了処理
 *
 *  ログバッファに記録されたログ情報を，低レベル出力機能を用いて出力す
 *  る．
 */
void
syslog_terminate(void){}

/*
 *  数値を文字列に変換
 */
static void
convert(unsigned long val, int radix, const char *radchar, int width,
		int minus, int padzero, void (*putc)(char))
{
	char	buf[12];
	int	i, j;

	i = 0;
	do {
		buf[i++] = radchar[val % radix];
		val /= radix;
	} while (val != 0);

	width -= minus;
	if (minus > 0 && padzero > 0) {
		(*putc)('-');
	}
	for (j = i; j < width; j++) {
		(*putc)((char)(padzero > 0 ? '0' : ' '));
	}
	if (minus > 0 && padzero <= 0) {
		(*putc)('-');
	}
	while (i > 0) {
		(*putc)(buf[--i]);
	}
}

/*
 *  ログ情報フォーマット用ライブラリ関数
 */
static char const raddec[] = "0123456789";
static char const radhex[] = "0123456789abcdef";
static char const radHEX[] = "0123456789ABCDEF";

void
syslog_print(SYSLOG *p_syslog, void (*putc)(char))
{
	char const	*format;
	int	argno;
	int	c;
	int	width;
	int	padzero;
	long	val;
	char const	*str;

	format = (char const *)(p_syslog->loginfo[0]);
	argno = 1;

	while ((c = *format++) != '\0') {
		if (c != '%') {
			(*putc)((char) c);
			continue;
		}

		width = padzero = 0;
		if ((c = *format++) == '0') {
			padzero = 1;
			c = *format++;
		}
		while ('0' <= c && c <= '9') {
			width = width*10 + c - '0';
			c = *format++;
		}
		switch (c) {
		case 'd':
			val = (long)(p_syslog->loginfo[argno++]);
			if (val >= 0) {
				convert(val, 10, raddec, width,
						0, padzero, putc);
			}
			else {
				convert(-val, 10, raddec, width,
						1, padzero, putc);
			}
			break;
		case 'u':
			val = (long)(p_syslog->loginfo[argno++]);
			convert(val, 10, raddec, width, 0, padzero, putc);
			break;
		case 'x':
			val = (long)(p_syslog->loginfo[argno++]);
			convert(val, 16, radhex, width, 0, padzero, putc);
			break;
		case 'X':
			val = (long)(p_syslog->loginfo[argno++]);
			convert(val, 16, radHEX, width, 0, padzero, putc);
			break;
		case 'c':
			(*putc)((char)(int)(p_syslog->loginfo[argno++]));
			break;
		case 's':
			str = (char const *)(p_syslog->loginfo[argno++]);
			while ((c = *str++) != '\0') {
				(*putc)((char) c);
			}
			break;
		case '%':
			(*putc)('%');
			break;
		case '\0':
			format--;
			break;
		default:
			break;
		}
	}
}

