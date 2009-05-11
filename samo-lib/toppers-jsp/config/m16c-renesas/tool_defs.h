/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
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
 *  @(#) $Id: tool_defs.h,v 1.3 2005/11/24 12:41:23 honda Exp $
 */

/*
 *	開発環境に依存する定義
 *
 *  このインクルードファイルは，t_stddef.h と itron.h の先頭でインクルー
 *  ドされる．他のファイルからは直接インクルードすることはない．他のイ
 *  ンクルードファイルに先立って処理されるため，他のインクルードファイ
 *  ルに依存してはならない．
 */

#ifndef _TOOL_DEFS_H_
#define _TOOL_DEFS_H_

/*
 *  コンパイラ依存のデータ型の定義
 */
#define	_int8_		char		/* 8ビットの整数型	*/
#define	_int16_		short		/* 16ビットの整数型	*/
#define	_int32_		long		/* 32ビットの整数型	*/

/*
 *  コンパイラの拡張機能のためのマクロ定義
 */

#define	Inline		_inline

#ifndef __cplusplus			/* C++ には asm がある */
#define	asm		_asm
#endif /* __cplusplus */

#define	Asm		_asm

/*
 *  MYTOOL用の設定
 */
#define	NC30

/*
 *  VP_INTをFARポインタとして定義
 */

#define	_vp_int_	void _far *

#endif /* _TOOL_DEFS_H_ */
