/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Takagi Nobuhisa
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
 *  @(#) $Id: cxxrt.c,v 1.3 2004/09/17 09:11:44 honda Exp $
 */
#include "../kernel/jsp_kernel.h"
#include "../kernel/task.h"
#include "kernel_id.h"

typedef int _toppers_cxxrt_sync_t;
typedef volatile char _toppers_cxxrt_once_t;

static inline int _get_tid()
{
	return runtsk - tcb_table + TMIN_TSKID;
}

/*
 *	For Syncronization
 */
int _toppers_cxxrt_lock(_toppers_cxxrt_sync_t *sync)
{
	if (iniflg)
		dis_dsp();
	return 0;
}

int _toppers_cxxrt_trylock(_toppers_cxxrt_sync_t *sync)
{
	return _toppers_cxxrt_lock(sync);
}

int _toppers_cxxrt_unlock(_toppers_cxxrt_sync_t *sync)
{
	if (iniflg)
		ena_dsp();
	return 0;
}

int _toppers_cxxrt_get_tskid(void)
{
	return iniflg ? _get_tid() : 0;
}

/*
 *	For recursive semaphore lock
 */
static ID cxxrt_holder;
static UINT cxxrt_counter;

static inline void _toppers_cxxrt_recursive_semaphore_lock(ID semid)
{
	if (iniflg && !sns_dsp())
	{
		ID tskid;
		tskid = _get_tid();

		if (cxxrt_holder != tskid)
		{
			wai_sem(semid);
			cxxrt_holder = tskid;
		}
		++cxxrt_counter;
	}
}

static inline void _toppers_cxxrt_recursive_semaphore_unlock(ID semid)
{
	if (iniflg && !sns_dsp())
	{
		if (--cxxrt_counter == 0)
		{
			cxxrt_holder = 0;
			sig_sem(semid);
		}
	}
}


/*
 *	For function call once
 */
int _toppers_cxxrt_once(_toppers_cxxrt_once_t *once, void (*func)(void))
{
	if (!*once)
	{
		static _toppers_cxxrt_sync_t sync;

		_toppers_cxxrt_recursive_semaphore_lock(_CXXRT_SEM);
		if (!*once)
		{
			(*func)();
			*once = 1;
		}
		_toppers_cxxrt_recursive_semaphore_unlock(_CXXRT_SEM);
	}
	return 0;
}


/*
 *	For task-local storage
 */
#ifndef CXXRT_KEY_MAX
#define CXXRT_KEY_MAX	2
#endif

struct _toppers_cxxrt_tls
{
	void **data;
	void (*dtor)(void*);
};

extern const ID tmax_tskid;

static void **tls_data[CXXRT_KEY_MAX];
static struct _toppers_cxxrt_tls tls[CXXRT_KEY_MAX];

int _toppers_cxxrt_key_create(struct _toppers_cxxrt_tls **key, void (*dtor)(void*))
{
	struct _toppers_cxxrt_tls *p;

	for (p = &tls[0]; p < &tls[CXXRT_KEY_MAX]; p++)
	{
		if (p->data == NULL)
		{
			p->data = tls_data[p - &tls[0]];
			p->dtor = dtor;
			*key = p;
			return 0;
		}
	}
	return -1;
}

int _toppers_cxxrt_key_delete(struct _toppers_cxxrt_tls *key)
{
	int i;

	for (i = 0; i <= tmax_tskid; i++)
	{
		if (key->dtor != 0)
			(*key->dtor)(key->data[i]);
	}
	key->data = NULL;
	key->dtor = 0;
	return 0;
}

/* JSP 1.4との互換性のために_toppers_cxxrt_reset_specificを残しておく */
void _toppers_cxxrt_reset_specific(void)
{
}

void _toppers_cxxrt_init(void)
{
	int i, tmax = tmax_tskid + 1;
	void **p = (void**)malloc(sizeof(void*) * CXXRT_KEY_MAX * tmax);

	for (i = 0; i < CXXRT_KEY_MAX * tmax; i++)
		p[i] = NULL;
	for (i = 0; i < CXXRT_KEY_MAX; i++)
		tls_data[i] = p + i * tmax;
}

