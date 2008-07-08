/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 Takagi Nobuhisa
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
 *  @(#) $Id: cxx_sample2.cpp,v 1.4 2004/09/17 09:11:34 honda Exp $
 */

/*
 *  C++サンプルプログラム(2)の本体
 *
 *  このサンプルプログラムは「哲学者の食事」をベースとして、C++の各機能
 *  のデモを行っている。
 *  5人の哲学者が左右のフォークを取る際、故意にデッドロックを発生させ、
 *  タイムアウトを検出した時点でC++の例外を送出している。
 *
 *  動作中に'q'を入力すると動作を停止することができ、restart? [y|n]に対
 *  して'y'を入力すれば再起動、'n'を入力すれば終了する。
 *  また、動作中に'a'を入力すればアボートする。
 */

#include <t_services.h>
#include <cstdlib>
#include <new>
#include "kernel_id.h"
#include "cxx_sample2.h"

//	非マルチタスクテストクラス
//	静的オブジェクトのコンストラクタとデストラクタの動作サンプル
class non_multitask_test
{
	int* x_;
public:
	non_multitask_test()
		: x_(new int(12345))	// カーネル非動作状態でのnew演算子
	{
	}
	~non_multitask_test()
	{
		if (*x_== 12345)
			syslog(LOG_NOTICE,"non-multitask test succeeded");
		else
			syslog(LOG_NOTICE,"non-multitask test failed");
		delete x_;				// カーネル非動作状態でのdelete演算子
		x_ = 0;
	}
} test;

class timeout_error
{
};

// 擬似乱数
int rnd()
{
	static unsigned int seed = 1;
	loc_cpu();
	seed = seed * 1566083941UL + 1;
	unl_cpu();
	return (seed >> 16) % 0x7fff;
}

//	フォーククラス
class fork
{
	ID semid_;
	bool used_;
public:
	explicit fork(int semid)
		: semid_(semid), used_(false)
	{
	}
	~fork()
	{
		if (used_)
			give();
	}
	ID id() const { return semid_; }
	bool is_used() const { return used_; }
	void take()
	{
		if (twai_sem(semid_, 500*5) == E_TMOUT)
			throw timeout_error();
		used_ = true;
	}
	void give()
	{
		used_ = false;
		sig_sem(semid_);
	}
};

fork* p_fork[5];

//	哲学者クラス
class philosopher
{
	ID		tskid_;
	fork* left_;
	fork* right_;
public:
	explicit philosopher(int tskid, fork* left, fork* right)
		: tskid_(tskid),
			left_(left), right_(right)
	{
		syslog(LOG_NOTICE,"philosofer #%d", tskid);
	}
	void think()
	{
		syslog(LOG_NOTICE, "#%d thinking...", tskid_);
		dly_tsk(100 * (rnd() % 5 + 1));
	}
	void eat()
	{
		syslog(LOG_NOTICE, "#%d eat up", tskid_);
		dly_tsk(100 * (rnd() % 5 + 1));
	}
	void run()
	{
		for (;;)
		{
			try
			{
				//	意図的にデッドロックを発生させる。
				left_->take();
				syslog(LOG_NOTICE, "#%d take left fork(%d)", tskid_, left_->id());

				dly_tsk(100 * (rnd() % 5 + 1));

				right_->take();
				syslog(LOG_NOTICE, "#%d take right fork(%d)", tskid_, right_->id());

				eat();

				left_->give();
				syslog(LOG_NOTICE, "#%d give left fork(%d)", tskid_, left_->id());
				right_->give();
				syslog(LOG_NOTICE, "#%d give right fork(%d)", tskid_, right_->id());
				think();
			}
			catch (timeout_error&)
			{
				//	タイムアウトによりデッドロックを検出すると、フォークを放す。
				syslog(LOG_NOTICE, "#%d !!!! timeout error !!!!", tskid_);
				if (left_->is_used())
				{
					left_->give();
					syslog(LOG_NOTICE, "#%d give left fork(%d)", tskid_, left_->id());
				}
				if (right_->is_used())
				{
					right_->give();
					syslog(LOG_NOTICE, "#%d give right fork(%d)", tskid_, right_->id());
				}
				rot_rdq(TPRI_SELF);
			}
		}
	}
};

void task(VP_INT exinf)
{
	_toppers_cxxrt_reset_specific();	// タスクの再起動を可能にするための初期化処理
	ID		tskid = ID(exinf);
	fork* left	= p_fork[(tskid - 1) % 5];
	fork* right = p_fork[(tskid - 1 + 4) % 5];
	philosopher phil(tskid, left, right);
	phil.run();
}

//	std::atexitで登録する終了時関数
void finish()
{
	syslog(LOG_NOTICE, "finish");
}


//	メインタスク
void main_task(VP_INT exinf)
{
	serial_ctl_por(TASK_PORTID, (IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV));
	syslog(LOG_NOTICE,"Sample program starts (exinf = %d)", exinf);

	std::atexit(finish);

	try
	{
		for (;;)
		{
			for (ID semid = 1; semid <= 5; semid++)
				p_fork[semid - 1] = new fork(semid);

			for (ID tskid = 1; tskid <= 5; tskid++)
				act_tsk(tskid);

			char c;
			do
			{
				serial_rea_dat(TASK_PORTID, &c, 1);
				if (c == 'a')
					std::abort();
			} while (c != 'q' && c != 'Q');

			for (ID tskid = 1; tskid <= 5; tskid++)
			{
				ter_tsk(tskid);
			}

			for (ID semid = 1; semid <= 5; semid++)
			{
				delete p_fork[semid - 1];
				p_fork[semid - 1] = 0;
			}

			do
			{
				syslog(LOG_NOTICE, "restart? [y|n] ");
				serial_rea_dat(TASK_PORTID, &c, 1);
			} while (c != 'y' && c != 'n');

			if (c == 'n')
				break;
		}

		syslog(LOG_NOTICE, "multitask test succeeded");
	}
	catch (std::bad_alloc&)
	{
		syslog(LOG_NOTICE, "multitask test failed");
	}

	std::exit(0);
}

