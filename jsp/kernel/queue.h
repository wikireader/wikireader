/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: queue.h,v 1.5 2003/06/04 01:46:16 hiro Exp $
 */

/*
 *	キュー操作ライブラリ
 *
 *  このキュー操作ライブラリでは，キューヘッダを含むリング構造のダブル
 *  リンクキューを扱う．具体的には，キューヘッダの次エントリはキューの
 *  先頭のエントリ，前エントリはキューの末尾のエントリとする．また，キ
 *  ューの先頭のエントリの前エントリと，キューの末尾のエントリの次エン
 *  トリは，キューヘッダとする．空のキューは，次エントリ，前エントリと
 *  も自分自身を指すキューヘッダであらわす．
 */

#ifndef	_QUEUE_H_
#define	_QUEUE_H_

/*
 *  キューのデータ構造の定義
 */
typedef struct queue {
	struct queue *next;		/* 次エントリへのポインタ */
	struct queue *prev;		/* 前エントリへのポインタ */
} QUEUE;

/*
 *  キューの初期化
 *
 *  queue にはキューヘッダを指定する．
 */
Inline void
queue_initialize(QUEUE *queue)
{
	queue->prev = queue->next = queue;
}

/*
 *  キューの前エントリへの挿入
 *
 *  queue の前に entry を挿入する．queue にキューヘッダを指定した場合
 *  には，キューの末尾に entry を挿入することになる．
 */
Inline void
queue_insert_prev(QUEUE *queue, QUEUE *entry)
{
	entry->prev = queue->prev;
	entry->next = queue;
	queue->prev->next = entry;
	queue->prev = entry;
}

/*
 *  エントリの削除
 *
 *  entry をキューから削除する．
 */
Inline void
queue_delete(QUEUE *entry)
{
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
}

/*
 *  キューの次エントリの取出し
 *
 *  queue の次エントリをキューから削除し，削除したエントリを返す．queue
 *  にキューヘッダを指定した場合には，キューの先頭のエントリを取り出す
 *  ことになる．queue に空のキューを指定して呼び出してはならない．
 */
Inline QUEUE *
queue_delete_next(QUEUE *queue)
{
	QUEUE	*entry;

	assert(queue->next != queue);
	entry = queue->next;
	queue->next = entry->next;
	entry->next->prev = queue;
	return(entry);
}

/*
 *  キューが空かどうかのチェック
 *
 *  queue にはキューヘッダを指定する．
 */
Inline BOOL
queue_empty(QUEUE *queue)
{
	if (queue->next == queue) {
		assert(queue->prev == queue);
		return(TRUE);
	}
	return(FALSE);
}

#endif /* _QUEUE_H_ */
