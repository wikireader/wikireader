/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: api.h,v 1.2 2003/06/30 15:44:11 takayuki Exp $
 */

#ifndef API_H
#define API_H

#define __cycini
#define __cycenq
#define __sta_cyc
#define __stp_cyc
#define __cyccal
#define __dtqini
#define __dtqenq
#define __dtqfenq
#define __dtqdeq
#define __dtqsnd
#define __dtqrcv
#define __snd_dtq
#define __psnd_dtq
#define __ipsnd_dtq
#define __tsnd_dtq
#define __fsnd_dtq
#define __ifsnd_dtq
#define __rcv_dtq
#define __prcv_dtq
#define __trcv_dtq
#define __flgini
#define __flgcnd
#define __set_flg
#define __iset_flg
#define __clr_flg
#define __wai_flg
#define __pol_flg
#define __twai_flg
#define __excini
#define __vxsns_ctx
#define __vxsns_loc
#define __vxsns_dsp
#define __vxsns_dpn
#define __vxsns_tex
#define __inhini
#define __mbxini
#define __snd_mbx
#define __rcv_mbx
#define __prcv_mbx
#define __trcv_mbx
#define __mpfini
#define __mpfget
#define __get_mpf
#define __pget_mpf
#define __tget_mpf
#define __rel_mpf
#define __semini
#define __sig_sem
#define __isig_sem
#define __wai_sem
#define __pol_sem
#define __twai_sem
#define __rot_rdq
#define __irot_rdq
#define __get_tid
#define __iget_tid
#define __loc_cpu
#define __iloc_cpu
#define __unl_cpu
#define __iunl_cpu
#define __dis_dsp
#define __ena_dsp
#define __sns_ctx
#define __sns_loc
#define __sns_dsp
#define __sns_dpn
#define __logini
#define __vwri_log
#define __vrea_log
#define __vmsk_log
#define __logter
#define __tskini
#define __tsksched
#define __tskrun
#define __tsknrun
#define __tskdmt
#define __tskact
#define __tskext
#define __tskpri
#define __tskrot
#define __tsktex
#define __ras_tex
#define __iras_tex
#define __dis_tex
#define __ena_tex
#define __sns_tex
#define __act_tsk
#define __iact_tsk
#define __can_act
#define __ext_tsk
#define __ter_tsk
#define __chg_pri
#define __get_pri
#define __slp_tsk
#define __tslp_tsk
#define __wup_tsk
#define __iwup_tsk
#define __can_wup
#define __rel_wai
#define __irel_wai
#define __sus_tsk
#define __rsm_tsk
#define __frsm_tsk
#define __dly_tsk
#define __tmeini
#define __tmeup
#define __tmedown
#define __tmeins
#define __tmedel
#define __isig_tim
#define __set_tim
#define __get_tim
#define __vxget_tim
#define __waimake
#define __waicmp
#define __waitmo
#define __waitmook
#define __waican
#define __wairel
#define __wobjwai
#define __wobjwaitmo
#define __wobjpri


#endif

