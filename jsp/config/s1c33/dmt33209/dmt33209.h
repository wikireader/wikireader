/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
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
 *  ハードウェア資源の定義
 *  definitions of hardware resource.
 */
#ifndef _DMT33209_H_
#define _DMT33209_H_

#include "s1c33.h"

#ifndef _MACRO_ONLY
extern int __START_bss[];			/* リンカスクリプトで定義される	*/
extern int __END_bss[];				/* シンボル			*/
extern int __START_data[];
extern int __END_data[];
extern int __START_vector[];
extern int __END_vector[];
extern int __START_data_lma[];
extern int __END_data_lma[];
extern int __START_vector_lma[];
extern int __END_vector_lma[];
#endif /* _MACRO_ONLY */

/*
 *	エリア先頭/終端アドレス
 *  Top/tail address of each area.
 */
#define STACKTOP	((void *)0x00002000)

#define BSS_START	__START_bss		/* RAM領域の先頭 		*/
#define BSS_END		__END_bss		/* RAM領域の終端		*/
#define DATA_START	__START_data		/* RAM上の初期化変数領域先頭	*/
#define IDATA_START	__START_data_lma	/* ROM上の初期化変数領域先頭	*/
#define IDATA_END	__END_data_lma		/* ROM上の初期化変数領域終端	*/
#define VECTOR_START	__START_vector		/* RAM上のベクタテーブル領域先頭*/
#define IVECTOR_START	__START_vector_lma	/* ROM上のベクタテーブル領域先頭*/
#define IVECTOR_END	__END_vector_lma	/* ROM上のベクタテーブル領域終端*/

#endif /*  _DMT33209_H_ */
