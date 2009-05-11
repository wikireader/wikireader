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
 *  @(#) $Id: itron.h,v 1.16 2004/12/22 03:38:00 hiro Exp $
 */

/*
 *	ITRON仕様共通規定のデータ型・定数・マクロ
 *
 *  このファイルには，スタンダードプロファイルには必要ない定義も含んで
 *  いる．データ型の定義は，スタンダードプロファイルを満たすちょうどの
 *  長さにはしていない．
 *
 *  アセンブリ言語のソースファイルやシステムコンフィギュレーションファ
 *  イルからこのファイルをインクルードする時は，_MACRO_ONLY を定義して
 *  おくことで，マクロ定義以外の記述を除くことができる．
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  また，ITRON仕様共通規定に準拠するソフトウェア部品のインクルードファ
 *  イルは，このファイルを直接インクルードしてもよい．この例外を除いて，
 *  他のファイルから直接インクルードされることはない．
 *
 *  このファイルをインクルードする前に，t_stddef.h をインクルードして
 *  おくことが必要である．
 */

#ifndef _ITRON_H_
#define _ITRON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  開発環境に依存する定義
 */
#include <tool_defs.h>

/*
 *  開発環境の標準インクルードファイル（NULL と size_t の定義が必要）
 *
 *  C++/EC++ では，標準仕様上は stddef.h がサポートされているとは限らな
 *  いが，ほとんどの処理系でサポートされている．
 */
#ifndef _MACRO_ONLY
#include <stddef.h>
#endif /* _MACRO_ONLY */

/*
 *  コンパイラ依存のデータ型のデフォルト定義
 */
#ifndef _bool_
#define	_bool_		int		/* ブール型 */
#endif /* _bool_ */

/*
 *  ITRON仕様共通データ型
 */
#ifndef _MACRO_ONLY

#ifdef _int8_
typedef	signed _int8_		B;	/* 符号付き8ビット整数 */
typedef	unsigned _int8_		UB;	/* 符号無し8ビット整数 */
typedef	_int8_			VB;	/* 型が定まらない8ビットの値 */
#endif /* _int8_ */

#ifdef _int16_
typedef	signed _int16_		H;	/* 符号付き16ビット整数 */
typedef	unsigned _int16_ 	UH;	/* 符号無し16ビット整数 */
typedef	_int16_			VH;	/* 型が定まらない16ビットの値 */
#endif /* _int16_ */

typedef	signed _int32_		W;	/* 符号付き32ビット整数 */
typedef	unsigned _int32_	UW;	/* 符号無し32ビット整数 */
typedef	_int32_			VW;	/* 型が定まらない32ビットの値 */

#ifdef _int64_
typedef	signed _int64_		D;	/* 符号付き64ビット整数 */
typedef	unsigned _int64_	UD;	/* 符号無し64ビット整数 */
typedef	_int64_			VD;	/* 型が定まらない64ビットの値 */
#endif /* _int64_ */

typedef	void		*VP;		/* 型が定まらないものへのポインタ */
typedef	void		(*FP)();	/* プログラムの起動番地（ポインタ） */

typedef signed int	INT;		/* 自然なサイズの符号付き整数 */
typedef unsigned int	UINT;		/* 自然なサイズの符号無し整数 */

typedef _bool_		BOOL;		/* 真偽値 */

typedef INT		FN;		/* 機能コード */
typedef	INT		ER;		/* エラーコード */
typedef	INT		ID;		/* オブジェクトのID番号 */
typedef	UINT		ATR;		/* オブジェクトの属性 */
typedef	UINT		STAT;		/* オブジェクトの状態 */
typedef	UINT		MODE;		/* サービスコールの動作モード */
typedef	INT		PRI;		/* 優先度 */
typedef	size_t		SIZE;		/* メモリ領域のサイズ */

typedef	INT		TMO;		/* タイムアウト指定 */
typedef	UINT		RELTIM;		/* 相対時間 */
typedef	UW		SYSTIM;		/* システム時刻 */

#ifdef _vp_int_
typedef	_vp_int_	VP_INT;		/* VP または INT */
#else /* _vp_int_ */
typedef	VP		VP_INT;		/* VP または INT */
#endif /* _vp_int_ */

typedef	INT		ER_BOOL;	/* ER または BOOL */
typedef	INT		ER_ID;		/* ER または ID */
typedef	INT		ER_UINT;	/* ER または UINT */

#endif /* _MACRO_ONLY */

/*
 *  ITRON仕様共通定数
 */

/*
 *  一般
 *
 *  _MACRO_ONLY の時には，NULL を定義しない．これは，_MACRO_ONLY の時
 *  はstddef.h をインクルードしないため，そうでない時と NULL の定義が
 *  食い違う可能性があるためである．また，システムコンフィギュレーショ
 *  ンファイルを処理する場合には NULL を定義してはならないため，その点
 *  からも定義しない方が都合がよい．
 */
#ifndef _MACRO_ONLY
#ifndef NULL				/* stddef.h に含まれているはず */
#define	NULL		0		/* 無効ポインタ */
#endif /* NULL */
#endif /* _MACRO_ONLY */

#define	TRUE		1		/* 真 */
#define	FALSE		0		/* 偽 */
#define	E_OK		0		/* 正常終了 */

/*
 *  エラーコード
 */
#define	E_SYS		(-5)		/* システムエラー */
#define	E_NOSPT		(-9)		/* 未サポート機能 */
#define	E_RSFN		(-10)		/* 予約機能コード */
#define	E_RSATR		(-11)		/* 予約属性 */
#define	E_PAR		(-17)		/* パラメータエラー */
#define	E_ID		(-18)		/* 不正ID番号 */
#define	E_CTX		(-25)		/* コンテキストエラー */
#define	E_MACV		(-26)		/* メモリアクセス違反 */
#define	E_OACV		(-27)		/* オブジェクトアクセス違反 */
#define	E_ILUSE		(-28)		/* サービスコール不正使用 */
#define	E_NOMEM		(-33)		/* メモリ不足 */
#define	E_NOID		(-34)		/* ID番号不足 */
#define	E_OBJ		(-41)		/* オブジェクト状態エラー */
#define	E_NOEXS		(-42)		/* オブジェクト未生成 */
#define	E_QOVR		(-43)		/* キューイングオーバーフロー */
#define	E_RLWAI		(-49)		/* 待ち状態の強制解除 */
#define	E_TMOUT		(-50)		/* ポーリング失敗またはタイムアウト */
#define	E_DLT		(-51)		/* 待ちオブジェクトの削除 */
#define	E_CLS		(-52)		/* 待ちオブジェクトの状態変化 */
#define	E_WBLK		(-57)		/* ノンブロッキング受付け */
#define	E_BOVR		(-58)		/* バッファオーバーフロー */

/*
 *  オブジェクト属性
 */
#define	TA_NULL		0u		/* オブジェクト属性を指定しない */

/*
 *  タイムアウト指定
 */
#define TMO_POL		0		/* ポーリング */
#define TMO_FEVR	(-1)		/* 永久待ち */
#define TMO_NBLK	(-2)		/* ノンブロッキング */

/*
 *  ITRON仕様共通マクロ
 */

/*
 *  エラーコード生成・分解マクロ
 *
 *  ANSI C言語の規格では，右シフト演算子（>>）が符号拡張されることを保
 *  証していないため，SERCDマクロの定義を単に ((ercd) >> 8) とすると，
 *  右シフト演算子を符号拡張しないコンパイラでは，SERCD の返値が正の値
 *  になってしまう．
 */
#define	ERCD(mercd,sercd)	(((sercd) << 8) | ((mercd) & 0xff))

#ifdef _int8_
#define	MERCD(ercd)		((ER)((B)(ercd)))
#define	SERCD(ercd)		((ER)((B)((ercd) >> 8)))
#else /* _int8_ */
#define	MERCD(ercd)		(((ercd) & 0x80) == 0 ? ((ercd) & 0xff) \
							: ((ercd) | ~0xff))
#define	SERCD(ercd)		((~((~0) >> 8)) | ((ercd) >> 8))
#endif /* _int8_ */

#ifdef __cplusplus
}
#endif

#endif /* _ITRON_H_ */
