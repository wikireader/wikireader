/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cpu_config.c,v 1.14 2004/09/17 09:58:17 honda Exp $
 */

/*
 *	プロセッサ依存モジュール（ARM4vT用）
 */

#include "jsp_kernel.h"
#include "check.h"
#include <armv4.h>
#include "task.h"

/*
 * 例外ベクタに書き込まれたジャンプ命令が参照するアドレス
 * 
 */
UW *arm_vector_add[8];


/*
 * 例外に応じたハンドラの起動番地
 */
UW arm_handler_add[8];


/*
 * 割り込みのネスト回数のカウント
 */
UW interrupt_count;


/*
 * CPU/割込みハンドラの出入口処理で一時的に使用するスタック
 */
UW int_stack[INT_STACK_SIZE];


/*
 *  CPU例外ハンドラの設定
 */
void
define_exc(EXCNO excno, FP exchdr)
{
    arm_install_handler(excno,exchdr);
}


/*
 *  未定義の例外が入った場合の処理
 */
void
undef_exception(){
  syslog(LOG_EMERG, "Unregistered Exception occurs. UNDEF");
    while(1);
}

void
swi_exception(){
  syslog(LOG_EMERG, "Unregistered Exception occurs. SWI");
  while(1);
}

void
prefetch_exception(){
  syslog(LOG_EMERG, "Unregistered Exception occurs. PREFETCH");
  while(1);
}

void
data_abort_exception(){
  syslog(LOG_EMERG, "Unregistered Exception occurs. DATA ABORT");
  while(1);
}


void
irq_exception(){
  syslog(LOG_EMERG, "Unregistered Exception occurs. IRQ");
  while(1);
}

void
fiq_exception(){
  syslog(LOG_EMERG, "Unregistered Exception occurs. FIQ");
  while(1);
}



/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize()
{

    UW i,vector_value;

	interrupt_count = 1;
	
    /*
     * 例外ベクタに登録されている命令から参照されるアドレスと，
     *そのアドレスの内容(ハンドラの実行先)を保存する．
     */
    for(i = 0; i <=7; i++){
        vector_value = *(volatile UW *)(i*4);
        vector_value &= 0x00000fff;
        arm_vector_add[i] =  (UW *)(vector_value + 8) + i;
        arm_handler_add[i] = *(arm_vector_add[i]);
    }
}


/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate()
{
    UW i;
    /*  ベクタテーブルを元に戻す．*/
    for(i = 0; i <=7; i++)
        *arm_vector_add[i] = arm_handler_add[i];
}
