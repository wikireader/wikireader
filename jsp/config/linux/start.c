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
 *  @(#) $Id: start.c,v 1.6 2003/12/24 07:24:40 honda Exp $
 */

#include <signal.h>
#include "jsp_kernel.h"
#include "check.h"

extern void kernel_start();


int main()
{
    struct sigaltstack      ss;
    struct sigaction action;
    /*
     *  シグナルスタックを，プロセススタック上に取る．
     *  BSDのシグナルと異なりss.ss_flagsにSS_ONSTACKを
     *  書き込んでも反映されないため、タスク独立部を表す
     *  inSigStackを使用する。
     */
    

    ss.ss_sp = (void *)(((INT) &ss) - SIGSTACK_MERGIN - SIGSTKSZ);
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    sigaltstack(&ss, 0);

    /*
     *  カーネルスタートアップルーチン(kernel_start())
     *  をSIGUSR1で起動するように設定し、raise()で呼び出し、
     *  スタックを切り替えて動作を開始する。
     */
      action.sa_handler = kernel_start;
      action.sa_flags   =  SA_ONSTACK;
      sigfillset(&action.sa_mask);
      sigaction(SIGUSR1,&action,NULL);
      raise(SIGUSR1);
      
      
    /*
     * ここに戻ることはない。
     */
    return(0);
}

