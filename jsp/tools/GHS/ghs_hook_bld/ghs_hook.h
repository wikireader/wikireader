/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *                2003      by Advanced Data Controls, Corp
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
 *  @(#) $Id: ghs_hook.h,v 1.2 2003/12/19 11:48:37 honda Exp $
 */

#ifdef GHS_HOOK
#include "hook.h"

#define TRACE_TNUM_TSKID 16
#define TRACE_TNUM_EVT 16

#ifndef BUFF_SIZE
#define BUFF_SIZE   0x4000	/* 0x400 | 0x800 | 0x1000 | 0x2000 | 0x4000 */
#endif

/* tskstat */

#define S_DMT       0x00    /* DORMANT */
#define S_RDY       0x10    /* RUN,READY */
#define S_SLP       0x20    /* wait due to slp_tsk or tslp_tsk */
#define S_DLY       0x30    /* wait due to dly_tsk */
#define S_RDV       0x40    /* wait for rendezvous completion */
#define S_FLG       0x50    /* wait due to wai_flg or twai_flg */
#define S_SMB       0x60    /* wait due to snd_mbf or tsnd_mbf */
#define S_CAL       0x70    /* wait for rendezvous call */
#define S_ACP       0x80    /* wait for rendezvous accept */
#define S_SEM       0x90    /* wait due to wai_sem or twai_sem */
#define S_MBX       0xa0    /* wait due to rcv_msg or trcv_msg */
#define S_MBF       0xb0    /* wait due to rcv_mbf or trcv_mbf */
#define S_MPL       0xc0    /* wait due to get_blk or tget_blk */
#define S_MPF       0xd0    /* wait due to get_blf or tget_blf */

typedef struct
{
	short max_tid;
    unsigned short trc_grp;
    unsigned short sys_grp;
    short tnum_tskid;
    short tnum_evt;
    unsigned char task_id[TRACE_TNUM_TSKID];
    unsigned char evtcode[TRACE_TNUM_EVT];
} T_HOOK;

typedef struct
{   char bigendian;
    char dummy;
    unsigned short size;
    unsigned short putp;
    unsigned short getp;
} T_TRACE;

typedef struct
{
    T_TRACE trace;
    T_HOOK hook;
    long syscall[BUFF_SIZE];
} T_ROS;

extern T_ROS Ros_buf;

void ghs_hook4(int sts, int p1, int p2);
void ghs_hook5(int sts, int p1, int p2, int p3);
void ghs_hook6(int sts, int p1, int p2, int p3, int p4);
void ghs_hook7(int sts, int p1, int p2, int p3, int p4, int p5);
void ghs_hook8(int sts, int p1, int p2, int p3, int p4, int p5, int p6);
void ghs_hook9(int sts, int p1, int p2, int p3, int p4, int p5, int p6, int p7);
#endif

