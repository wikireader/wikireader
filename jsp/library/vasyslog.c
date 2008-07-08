/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2005 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: vasyslog.c,v 1.7 2005/11/24 11:46:02 hiro Exp $
 */

/*
 *	可変数引数のシステムログライブラリ
 */

#include <t_services.h>
#include <stdarg.h>

ER
syslog(UINT prio, const char *format, ...)
{
	SYSLOG	log;
	va_list	ap;
	int	i;
	int	c;
	BOOL	lflag;

	log.logtype = LOG_TYPE_COMMENT;
	log.loginfo[0] = (VP_INT) format;
	i = 1;
	va_start(ap, format);

	while ((c = *format++) != '\0' && i < TMAX_LOGINFO) {
		if (c != '%') {
			continue;
		}

		lflag = FALSE;
		c = *format++;
		while ('0' <= c && c <= '9') {
			c = *format++;
		}
		if (c == 'l') {
			lflag = TRUE;
			c = *format++;
		}
		switch (c) {
		case 'd':
			log.loginfo[i++] = lflag ?
					(VP_INT) va_arg(ap, long) :
					(VP_INT) va_arg(ap, int);
			break;
		case 'u':
		case 'x':
		case 'X':
			log.loginfo[i++] = lflag ?
					(VP_INT) va_arg(ap, unsigned long) :
					(VP_INT) va_arg(ap, unsigned int);
			break;
		case 'p':
			log.loginfo[i++] = (VP_INT) va_arg(ap, void *);
			break;
		case 'c':
			log.loginfo[i++] = (VP_INT) va_arg(ap, int);
			break;
		case 's':
			log.loginfo[i++] = (VP_INT) va_arg(ap, const char *);
			break;
		case '\0':
			format--;
			break;
		default:
			break;
		}
	}
	va_end(ap);
	return(vwri_log(prio, &log));
}
