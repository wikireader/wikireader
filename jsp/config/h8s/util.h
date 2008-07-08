/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 */

#ifndef _UTIL_H_
#define _UTIL_H_

/*
 *  このヘッダファイルに含まれているマクロは、原則として、カーネル開発者に対して
 *  公開されているものである。そのため、パラメータチェックなどが不十分である。
 *  ユーザーにあっては、自己責任において利用すること。
 */

/*
 *  ビット番号の定義
 */
#define BIT0              0x1
#define BIT1              0x2
#define BIT2              0x4
#define BIT3              0x8
#define BIT4             0x10
#define BIT5             0x20
#define BIT6             0x40
#define BIT7             0x80
#define BIT8            0x100
#define BIT9            0x200
#define BIT10           0x400
#define BIT11           0x800
#define BIT12          0x1000
#define BIT13          0x2000
#define BIT14          0x4000
#define BIT15          0x8000

/*
 *  バイトデータ操作用マクロ
 */
/* 以下において、x：不定 */

/* 以下のような書き方と、どっちが良いか？
     (UH) (((UW) (c)) & 0x0000ffff)
     (UH) (((UW) (c)) & 0xffff0000) >> 16
*/

/* (UH) xxaa -> (UB) aa */			/* 下位1バイト取り出し */
#define LO8(c)		(UB)( (UH)(c) & 0xff )
/* (UH) aaxx -> (UB) aa */			/* 上位1バイト取り出し */
#define HI8(c)		LO8( (UH)(c) >>  8 )

/* (UW) xxxxaaaa -> (UH) aaaa */		/* 下位2バイト取り出し */
#define LO16(c)		(UH)( (UW)(c) & 0xffff )
/* (UW) aaaaxxxx -> (UH) aaaa */		/* 上位2バイト取り出し */
#define HI16(c)		LO16( (UW)(c) >> 16 )

/* (UH) aaaa, (UH) bbbb -> (UW) aaaabbbb */	/* 2バイトデータの結合 */
#define JOIN16(hi, lo)	(UW)( ((UW)(hi) << 16) | (lo) )
/* (UB) aa, (UB) bb -> (UH) aabb */		/* 1バイトデータの結合 */
#define JOIN8(hi, lo)	(UH)( ((UH)(hi) <<  8) | (lo) )

/* (BIN) : ２進数 */
/* (BIN) xxxxaaaa -> (BIN) 0000aaaa */		/* 0-3ビットに配置  */
#define TO_LO4(c)	( (UB)(c) & 0xf )
/* (BIN) xxxxaaaa -> (BIN) aaaa0000 */		/* 4-7ビットに配置  */
#define TO_HI4(c)	(((UB)(c) << 4) & 0xf0)

/*
 *  数値データ文字列化用マクロ
 */
#define _TO_STRING(arg)	#arg
#define TO_STRING(arg)	_TO_STRING(arg)

#endif /* _UTIL_H_ */
