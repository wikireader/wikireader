/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *                     2003 by Advanced Data Controls, Corp
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
 *  @(#) $Id: tool_config.h,v 1.5 2007/01/05 02:02:38 honda Exp $
 */

/*
 *	開発環境依存モジュール
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
 */

#ifndef _TOOL_CONFIG_H_
#define _TOOL_CONFIG_H_

/*
 *  ラベルの別名を定義するためのマクロ
 */
#define	_LABEL_ALIAS(new_label, defined_label) \
	asm(".globl " #new_label "\n" #new_label " = " #defined_label);
#define LABEL_ALIAS(x, y) _LABEL_ALIAS(x, y)

#define __EMPTY_LABEL(x,y) x y[]

/*
 *  atexit の処理とデストラクタの実行
 */
#ifndef _MACRO_ONLY
#pragma weak software_term_hook

Inline void
call_atexit()
{
	extern void	software_term_hook(void);
	volatile FP	fp = software_term_hook;

	/*
	 *  software_term_hookへのポインタを，一旦volatile FP型のfpに
	 *  代入してから使うのは，0との比較が最適化で削除されないよう
	 *  にするためである．
	 */
	if (fp != 0) {
		(*fp)();
	}
}

#endif /* _MACRO_ONLY */

/*
 *  GHS Event Analyzer用のトレースログ設定
 */
#ifdef GHS_HOOK
#include "hook.h"

#define hook_initialize() tool_initialize()

#define	LOG_INH_ENTER(inhno)		
#define	LOG_INH_LEAVE(inhno)		

#define	LOG_ISR_ENTER(intno)		
#define	LOG_ISR_LEAVE(intno)		

#define	LOG_CYC_ENTER(cyccb)
#define	LOG_CYC_LEAVE(cyccb)

#define	LOG_EXC_ENTER(excno)		
#define	LOG_EXC_LEAVE(excno)		

#define	LOG_TEX_ENTER(texptn)
#define	LOG_TEX_LEAVE(texptn)

#define	LOG_TSKSTAT(tcb)				sts_hook(TSKID(tcb));

#define	LOG_DSP_ENTER(tcb)		
#define	LOG_DSP_LEAVE(tcb)		

#define	LOG_ACT_TSK_ENTER(tskid)			sys_hook1(THF_TSK, TFN_ACT_TSK, (int)tskid);
#define	LOG_ACT_TSK_LEAVE(ercd)				trc_rtn_hook(THF_TSK, TFN_ACT_TSK, ercd);
#define	LOG_IACT_TSK_ENTER(tskid)			sys_hook1_i(THF_TSK, TFN_IACT_TSK, (int)tskid);			
#define	LOG_IACT_TSK_LEAVE(ercd)			trc_rtn_hook(THF_TSK, TFN_IACT_TSK, ercd);
#define	LOG_CAN_ACT_ENTER(tskid)			sys_hook1(THF_TSK, TFN_CAN_ACT, (int)tskid);			
#define	LOG_CAN_ACT_LEAVE(ercd)				trc_rtn_hook(THF_TSK, TFN_CAN_ACT, tcb->actcnt);
#define	LOG_EXT_TSK_ENTER()				sys_hook0(THF_TSK, TFN_EXT_TSK);
#define	LOG_TER_TSK_ENTER(tskid)			sys_hook1(THF_TSK, TFN_TER_TSK, (int)tskid);
#define	LOG_TER_TSK_LEAVE(ercd)				trc_rtn_hook(THF_TSK, TFN_TER_TSK, ercd);
#define	LOG_CHG_PRI_ENTER(tskid, tskpri)		sys_hook2(THF_TSK, TFN_CHG_PRI, (int)tskid, (int)tskpri);
#define	LOG_CHG_PRI_LEAVE(ercd)				trc_rtn_hook(THF_TSK, TFN_CHG_PRI, ercd);	
#define	LOG_GET_PRI_ENTER(tskid, p_tskpri)		
#define	LOG_GET_PRI_LEAVE(ercd, tskpri)
#define	LOG_SLP_TSK_ENTER()				sys_hook0(THF_TSKS, TFN_SLP_TSK);
#define	LOG_SLP_TSK_LEAVE(ercd)				trc_rtn_hook(THF_TSKS, TFN_SLP_TSK, ercd);
#define	LOG_TSLP_TSK_ENTER(tmout)			sys_hook1(THF_TSKS, TFN_TSLP_TSK, (int)tmout);
#define	LOG_TSLP_TSK_LEAVE(ercd)			trc_rtn_hook(THF_TSKS, TFN_TSLP_TSK, ercd);
#define	LOG_WUP_TSK_ENTER(tskid)			sys_hook1(THF_TSKS, TFN_WUP_TSK, (int)tskid);
#define	LOG_WUP_TSK_LEAVE(ercd)				trc_rtn_hook(THF_TSKS, TFN_WUP_TSK, ercd);				
#define	LOG_IWUP_TSK_ENTER(tskid)			sys_hook1_i(THF_TSKS, TFN_IWUP_TSK, (int)tskid);
#define	LOG_IWUP_TSK_LEAVE(ercd)			trc_rtn_hook(THF_TSKS, TFN_IWUP_TSK, ercd);
#define	LOG_CAN_WUP_ENTER(tskid)			sys_hook1(THF_TSKS, TFN_CAN_WUP, (int)tskid);
#define	LOG_CAN_WUP_LEAVE(ercd)				trc_rtn_hook(THF_TSKS, TFN_CAN_WUP, tcb->wupcnt);
#define	LOG_REL_WAI_ENTER(tskid)			sys_hook1(THF_TSKS, TFN_REL_WAI, (int)tskid);
#define	LOG_REL_WAI_LEAVE(ercd)				trc_rtn_hook(THF_TSKS, TFN_REL_WAI, ercd);	
#define	LOG_IREL_WAI_ENTER(tskid)			sys_hook1_i(THF_TSKS, TFN_IREL_WAI, (int)tskid);
#define	LOG_IREL_WAI_LEAVE(ercd)			trc_rtn_hook(THF_TSKS, TFN_IREL_WAI, ercd);			
#define	LOG_SUS_TSK_ENTER(tskid)			sys_hook1(THF_TSKS, TFN_SUS_TSK, (int)tskid);
#define	LOG_SUS_TSK_LEAVE(ercd)				trc_rtn_hook(THF_TSKS, TFN_SUS_TSK, ercd);
#define	LOG_RSM_TSK_ENTER(tskid)			sys_hook1(THF_TSKS, TFN_RSM_TSK, (int)tskid);
#define	LOG_RSM_TSK_LEAVE(ercd)				trc_rtn_hook(THF_TSKS, TFN_RSM_TSK, ercd);
#define	LOG_FRSM_TSK_ENTER(tskid)
#define	LOG_FRSM_TSK_LEAVE(ercd)
#define	LOG_DLY_TSK_ENTER(dlytim)			sys_hook1(THF_TSKS, TFN_DLY_TSK, (int)dlytim);
#define	LOG_DLY_TSK_LEAVE(ercd)				trc_rtn_hook(THF_TSKS, TFN_DLY_TSK, winfo.wercd);
#define	LOG_RAS_TEX_ENTER(tskid, rasptn)		sys_hook2(THF_TEX, TFN_RAS_TEX, (int)tskid, (int)rasptn);
#define	LOG_RAS_TEX_LEAVE(ercd)				trc_rtn_hook(THF_TEX, TFN_RAS_TEX, ercd);
#define	LOG_IRAS_TEX_ENTER(tskid, rasptn)		sys_hook2_i(THF_TEX, TFN_IRAS_TEX, (int)tskid, (int)rasptn);
#define	LOG_IRAS_TEX_LEAVE(ercd)			trc_rtn_hook(THF_TEX, TFN_IRAS_TEX, ercd);	
#define	LOG_DIS_TEX_ENTER()				sys_hook0(THF_TEX, TFN_DIS_TEX);
#define	LOG_DIS_TEX_LEAVE(ercd)				trc_rtn_hook(THF_TEX, TFN_DIS_TEX, ercd);
#define	LOG_ENA_TEX_ENTER()				sys_hook0(THF_TEX, TFN_ENA_TEX);
#define	LOG_ENA_TEX_LEAVE(ercd)				trc_rtn_hook(THF_TEX, TFN_ENA_TEX, ercd);		
#define	LOG_SNS_TEX_ENTER()				
#define	LOG_SNS_TEX_LEAVE(state)
#define	LOG_SIG_SEM_ENTER(semid)			sys_hook1(THF_SEM, TFN_SIG_SEM, (int)semid);
#define	LOG_SIG_SEM_LEAVE(ercd)				trc_rtn_hook(THF_SEM, TFN_SIG_SEM, ercd);
#define	LOG_ISIG_SEM_ENTER(semid)			sys_hook1_i(THF_SEM, TFN_ISIG_SEM, (int)semid);
#define	LOG_ISIG_SEM_LEAVE(ercd)			trc_rtn_hook(THF_SEM, TFN_ISIG_SEM, ercd);
#define	LOG_WAI_SEM_ENTER(semid)			sys_hook1(THF_SEM, TFN_WAI_SEM, (int)semid);
#define	LOG_WAI_SEM_LEAVE(ercd)				trc_rtn_hook(THF_SEM, TFN_WAI_SEM, ercd);
#define	LOG_POL_SEM_ENTER(semid)			sys_hook1(THF_SEM, TFN_POL_SEM, (int)semid);
#define	LOG_POL_SEM_LEAVE(ercd)				trc_rtn_hook(THF_SEM, TFN_POL_SEM, ercd);
#define	LOG_TWAI_SEM_ENTER(semid, tmout)		sys_hook2(THF_SEM, TFN_TWAI_SEM, (int)semid, (int)tmout);
#define	LOG_TWAI_SEM_LEAVE(ercd)			trc_rtn_hook(THF_SEM, TFN_TWAI_SEM, ercd);
#define	LOG_SET_FLG_ENTER(flgid, setptn)				sys_hook2(THF_FLG, TFN_SET_FLG, (int)flgid, (int)setptn);
#define	LOG_SET_FLG_LEAVE(ercd)						trc_rtn_hook(THF_FLG, TFN_SET_FLG);
#define	LOG_ISET_FLG_ENTER(flgid, setptn)				sys_hook2_i(THF_FLG, TFN_ISET_FLG, (int)flgid, (int)setptn);
#define	LOG_ISET_FLG_LEAVE(ercd)					trc_rtn_hook(THF_FLG, TFN_ISET_FLG);
#define	LOG_CLR_FLG_ENTER(flgid, clrptn)				sys_hook2(THF_FLG, TFN_CLR_FLG, (int)flgid, (int)clrptn);
#define	LOG_CLR_FLG_LEAVE(ercd)						trc_rtn_hook(THF_FLG, TFN_CLR_FLG);
#define	LOG_WAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn)		sys_hook3(THF_FLG, TFN_WAI_FLG, (int)flgid, (int)waiptn, (int)wfmode);
#define	LOG_WAI_FLG_LEAVE(ercd, flgptn)					trc_rtn_hook(THF_FLG, TFN_WAI_FLG, ercd);
#define	LOG_POL_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn)		sys_hook3(THF_FLG, TFN_POL_FLG, (int)flgid, (int)waiptn, (int)wfmode);
#define	LOG_POL_FLG_LEAVE(ercd, flgptn)					trc_rtn_hook(THF_FLG, TFN_POL_FLG, ercd);
#define	LOG_TWAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn, tmout)	sys_hook4(THF_FLG, TFN_TWAI_FLG, (int)flgid, (int)waiptn, (int)wfmode, (int)tmout);
#define	LOG_TWAI_FLG_LEAVE(ercd, flgptn)				trc_rtn_hook(THF_FLG, TFN_TWAI_FLG, ercd);
#define	LOG_SND_DTQ_ENTER(dtqid, data)					sys_hook2(THF_DTQ, TFN_SND_DTQ, (int)dtqid, (int)data);
#define	LOG_SND_DTQ_LEAVE(ercd)						trc_rtn_hook(THF_DTQ, TFN_SND_DTQ, ercd);
#define	LOG_PSND_DTQ_ENTER(dtqid, data)					sys_hook2(THF_DTQ, TFN_PSND_DTQ, (int)dtqid, (int)data);
#define	LOG_PSND_DTQ_LEAVE(ercd)					trc_rtn_hook(THF_DTQ, TFN_PSND_DTQ, ercd);
#define	LOG_IPSND_DTQ_ENTER(dtqid, data)				sys_hook2_i(THF_DTQ, TFN_IPSND_DTQ, (int)dtqid, (int)data);
#define	LOG_IPSND_DTQ_LEAVE(ercd)					trc_rtn_hook(THF_DTQ, TFN_IPSND_DTQ, ercd);
#define	LOG_TSND_DTQ_ENTER(dtqid, data, tmout)				sys_hook3(THF_DTQ, TFN_TSND_DTQ, (int)dtqid, (int)data, (int)tmout);
#define	LOG_TSND_DTQ_LEAVE(ercd)					trc_rtn_hook(THF_DTQ, TFN_TSND_DTQ, ercd);
#define	LOG_FSND_DTQ_ENTER(dtqid, data)					sys_hook2(THF_DTQ, TFN_FSND_DTQ, (int)dtqid, (int)data);
#define	LOG_FSND_DTQ_LEAVE(ercd)					trc_rtn_hook(THF_DTQ, TFN_FSND_DTQ);
#define	LOG_IFSND_DTQ_ENTER(dtqid, data)				sys_hook2_i(THF_DTQ, TFN_IFSND_DTQ, (int)dtqid, (int)data);
#define	LOG_IFSND_DTQ_LEAVE(ercd)					trc_rtn_hook(THF_DTQ, TFN_IFSND_DTQ);
#define	LOG_RCV_DTQ_ENTER(dtqid, p_data)				sys_hook1(THF_DTQ, TFN_RCV_DTQ, (int)dtqid);
#define	LOG_RCV_DTQ_LEAVE(ercd, data)					trc_rtn_hook(THF_DTQ, TFN_RCV_DTQ, ercd);
#define	LOG_PRCV_DTQ_ENTER(dtqid, p_data)				sys_hook1(THF_DTQ, TFN_PRCV_DTQ, (int)dtqid);
#define	LOG_PRCV_DTQ_LEAVE(ercd, data)					trc_rtn_hook(THF_DTQ, TFN_PRCV_DTQ, ercd);
#define	LOG_TRCV_DTQ_ENTER(dtqid, p_data, tmout)			sys_hook1(THF_DTQ, TFN_TRCV_DTQ, (int)dtqid);
#define	LOG_TRCV_DTQ_LEAVE(ercd, data)					trc_rtn_hook(THF_DTQ, TFN_TRCV_DTQ, ercd);
#define	LOG_SND_MBX_ENTER(mbxid, pk_msg)				sys_hook2(THF_MBX, TFN_SND_MBX, (int)mbxid, (int)pk_msg);
#define	LOG_SND_MBX_LEAVE(ercd)						trc_rtn_hook(THF_MBX, TFN_SND_MBX, ercd);
#define	LOG_RCV_MBX_ENTER(mbxid, ppk_msg)		sys_hook1(THF_MBX, TFN_RCV_MBX, (int)mbxid);
#define	LOG_RCV_MBX_LEAVE(ercd, pk_msg)			trc_rtn_hook(THF_MBX, TFN_RCV_MBX, ercd);
#define	LOG_PRCV_MBX_ENTER(mbxid, ppk_msg)		sys_hook1(THF_MBX, TFN_PRCV_MBX, (int)mbxid);
#define	LOG_PRCV_MBX_LEAVE(ercd, pk_msg)		trc_rtn_hook(THF_MBX, TFN_PRCV_MBX, ercd);
#define	LOG_TRCV_MBX_ENTER(mbxid, ppk_msg, tmout)	sys_hook2(THF_MBX, TFN_TRCV_MBX, (int)mbxid, (int)tmout);
#define	LOG_TRCV_MBX_LEAVE(ercd, pk_msg)		trc_rtn_hook(THF_MBX, TFN_TRCV_MBX, ercd);
#define	LOG_GET_MPF_ENTER(mpfid, p_blk)			sys_hook1(THF_MPF, TFN_GET_MPF, (int)mpfid);
#define	LOG_GET_MPF_LEAVE(ercd, blk)			trc_rtn_hook(THF_MPF, TFN_GET_MPF, ercd);
#define	LOG_PGET_MPF_ENTER(mpfid, p_blk)		sys_hook1(THF_MPF, TFN_PGET_MPF, (int)mpfid);		
#define	LOG_PGET_MPF_LEAVE(ercd, blk)			trc_rtn_hook(THF_MPF, TFN_PGET_MPF, ercd);
#define	LOG_TGET_MPF_ENTER(mpfid, p_blk, tmout)		sys_hook2(THF_MPF, TFN_TGET_MPF, (int)mpfid, (int)tmout);
#define	LOG_TGET_MPF_LEAVE(ercd, blk)			trc_rtn_hook(THF_MPF, TFN_TGET_MPF, ercd);
#define	LOG_REL_MPF_ENTER(mpfid, blk)			sys_hook2(THF_MPF, TFN_REL_MPF, (int)mpfid, (int)blk);
#define	LOG_REL_MPF_LEAVE(ercd)				trc_rtn_hook(THF_MPF, TFN_REL_MPF, ercd);
#define	LOG_SET_TIM_ENTER(p_systim)			sys_hook1(THF_TIM, TFN_SET_TIM, (int)p_systim);
#define	LOG_SET_TIM_LEAVE(ercd)				trc_rtn_hook(THF_TIM, TFN_SET_TIM);
#define	LOG_GET_TIM_ENTER(p_systim)			
#define	LOG_GET_TIM_LEAVE(ercd, systim)
#define	LOG_ISIG_TIM_ENTER()
#define	LOG_ISIG_TIM_LEAVE(ercd)
#define	LOG_STA_CYC_ENTER(cycid)			sys_hook1(THF_TIM, TFN_STA_CYC, (int)cycid);
#define	LOG_STA_CYC_LEAVE(ercd)				trc_rtn_hook(THF_TIM, TFN_STA_CYC);
#define	LOG_STP_CYC_ENTER(cycid)			sys_hook1(THF_TIM, TFN_STP_CYC, (int)cycid);
#define	LOG_STP_CYC_LEAVE(ercd)				trc_rtn_hook(THF_TIM, TFN_STP_CYC);
#define	LOG_ROT_RDQ_ENTER(tskpri)			sys_hook1(THF_SYS, TFN_ROT_RDQ, (int)tskpri);
#define	LOG_ROT_RDQ_LEAVE(ercd)				trc_rtn_hook(THF_SYS, TFN_ROT_RDQ);
#define	LOG_IROT_RDQ_ENTER(tskpri)			sys_hook1_i(THF_SYS, TFN_IROT_RDQ, (int)tskpri);
#define	LOG_IROT_RDQ_LEAVE(ercd)			trc_rtn_hook(THF_SYS, TFN_IROT_RDQ);
#define	LOG_GET_TID_ENTER(p_tskid)			
#define	LOG_GET_TID_LEAVE(ercd, tskid)
#define	LOG_IGET_TID_ENTER(p_tskid)
#define	LOG_IGET_TID_LEAVE(ercd, tskid)
#define	LOG_LOC_CPU_ENTER()				sys_hook0(THF_SYS, TFN_LOC_CPU);			
#define	LOG_LOC_CPU_LEAVE(ercd)				trc_rtn_hook(THF_SYS, TFN_LOC_CPU);			
#define	LOG_ILOC_CPU_ENTER()				sys_hook0_i(THF_SYS, TFN_ILOC_CPU);		
#define	LOG_ILOC_CPU_LEAVE(ercd)			trc_rtn_hook(THF_SYS, TFN_ILOC_CPU);
#define	LOG_UNL_CPU_ENTER()				sys_hook0(THF_SYS, TFN_UNL_CPU);		
#define	LOG_UNL_CPU_LEAVE(ercd)				trc_rtn_hook(THF_SYS, TFN_UNL_CPU);		
#define	LOG_IUNL_CPU_ENTER()				sys_hook0_i(THF_SYS, TFN_IUNL_CPU);		
#define	LOG_IUNL_CPU_LEAVE(ercd)			trc_rtn_hook(THF_SYS, TFN_IUNL_CPU);
#define	LOG_DIS_DSP_ENTER()				sys_hook0(THF_SYS, TFN_DIS_DSP);		
#define	LOG_DIS_DSP_LEAVE(ercd)				trc_rtn_hook(THF_SYS, TFN_DIS_DSP);
#define	LOG_ENA_DSP_ENTER()				sys_hook0(THF_SYS, TFN_ENA_DSP);
#define	LOG_ENA_DSP_LEAVE(ercd)				trc_rtn_hook(THF_SYS, TFN_ENA_DSP);
#define	LOG_SNS_CTX_ENTER()			
#define	LOG_SNS_CTX_LEAVE(state)
#define	LOG_SNS_LOC_ENTER()
#define	LOG_SNS_LOC_LEAVE(state)
#define	LOG_SNS_DSP_ENTER()
#define	LOG_SNS_DSP_LEAVE(state)
#define	LOG_SNS_DPN_ENTER()
#define	LOG_SNS_DPN_LEAVE(state)
#define	LOG_VSNS_INI_ENTER()
#define	LOG_VSNS_INI_LEAVE(state)
#define	LOG_VXSNS_CTX_ENTER(p_excinf)
#define	LOG_VXSNS_CTX_LEAVE(state)
#define	LOG_VXSNS_LOC_ENTER(p_excinf)
#define	LOG_VXSNS_LOC_LEAVE(state)
#define	LOG_VXSNS_DSP_ENTER(p_excinf)
#define	LOG_VXSNS_DSP_LEAVE(state)
#define	LOG_VXSNS_DPN_ENTER(p_excinf)
#define	LOG_VXSNS_DPN_LEAVE(state)
#define	LOG_VXSNS_TEX_ENTER(p_excinf)
#define	LOG_VXSNS_TEX_LEAVE(state)
#define	LOG_VXGET_TIM_ENTER(p_sysutim)
#define	LOG_VXGET_TIM_LEAVE(ercd, sysutim)

#define	LOG_CHG_IPM_ENTER(ipm)
#define	LOG_CHG_IPM_LEAVE(ercd)
#define	LOG_GET_IPM_ENTER(p_ipm)
#define	LOG_GET_IPM_LEAVE(ercd, ipm)

#else

#define	LOG_INH_LEAVE(inhno)		

#define	LOG_ISR_ENTER(intno)		
#define	LOG_ISR_LEAVE(intno)		

#define	LOG_CYC_ENTER(cyccb)
#define	LOG_CYC_LEAVE(cyccb)

#define	LOG_EXC_ENTER(excno)		
#define	LOG_EXC_LEAVE(excno)		

#define	LOG_TEX_ENTER(texptn)
#define	LOG_TEX_LEAVE(texptn)

#define	LOG_TSKSTAT(tcb)

#define	LOG_DSP_ENTER(tcb)		
#define	LOG_DSP_LEAVE(tcb)		

#define	LOG_ACT_TSK_ENTER(tskid)
#define	LOG_ACT_TSK_LEAVE(ercd)
#define	LOG_IACT_TSK_ENTER(tskid)
#define	LOG_IACT_TSK_LEAVE(ercd)
#define	LOG_CAN_ACT_ENTER(tskid)
#define	LOG_CAN_ACT_LEAVE(ercd)
#define	LOG_EXT_TSK_ENTER()
#define	LOG_TER_TSK_ENTER(tskid)
#define	LOG_TER_TSK_LEAVE(ercd)
#define	LOG_CHG_PRI_ENTER(tskid, tskpri)
#define	LOG_CHG_PRI_LEAVE(ercd)
#define	LOG_GET_PRI_ENTER(tskid, p_tskpri)
#define	LOG_GET_PRI_LEAVE(ercd, tskpri)
#define	LOG_SLP_TSK_ENTER()
#define	LOG_SLP_TSK_LEAVE(ercd)
#define	LOG_TSLP_TSK_ENTER(tmout)
#define	LOG_TSLP_TSK_LEAVE(ercd)
#define	LOG_WUP_TSK_ENTER(tskid)
#define	LOG_WUP_TSK_LEAVE(ercd)
#define	LOG_IWUP_TSK_ENTER(tskid)
#define	LOG_IWUP_TSK_LEAVE(ercd)
#define	LOG_CAN_WUP_ENTER(tskid)
#define	LOG_CAN_WUP_LEAVE(ercd)
#define	LOG_REL_WAI_ENTER(tskid)
#define	LOG_REL_WAI_LEAVE(ercd)
#define	LOG_IREL_WAI_ENTER(tskid)
#define	LOG_IREL_WAI_LEAVE(ercd)
#define	LOG_SUS_TSK_ENTER(tskid)
#define	LOG_SUS_TSK_LEAVE(ercd)
#define	LOG_RSM_TSK_ENTER(tskid)
#define	LOG_RSM_TSK_LEAVE(ercd)
#define	LOG_FRSM_TSK_ENTER(tskid)
#define	LOG_FRSM_TSK_LEAVE(ercd)
#define	LOG_DLY_TSK_ENTER(dlytim)
#define	LOG_DLY_TSK_LEAVE(ercd)
#define	LOG_RAS_TEX_ENTER(tskid, rasptn)
#define	LOG_RAS_TEX_LEAVE(ercd)
#define	LOG_IRAS_TEX_ENTER(tskid, rasptn)
#define	LOG_IRAS_TEX_LEAVE(ercd)
#define	LOG_DIS_TEX_ENTER()
#define	LOG_DIS_TEX_LEAVE(ercd)
#define	LOG_ENA_TEX_ENTER()
#define	LOG_ENA_TEX_LEAVE(ercd)
#define	LOG_SNS_TEX_ENTER()
#define	LOG_SNS_TEX_LEAVE(state)
#define	LOG_SIG_SEM_ENTER(semid)
#define	LOG_SIG_SEM_LEAVE(ercd)
#define	LOG_ISIG_SEM_ENTER(semid)
#define	LOG_ISIG_SEM_LEAVE(ercd)
#define	LOG_WAI_SEM_ENTER(semid)
#define	LOG_WAI_SEM_LEAVE(ercd)
#define	LOG_POL_SEM_ENTER(semid)
#define	LOG_POL_SEM_LEAVE(ercd)
#define	LOG_TWAI_SEM_ENTER(semid, tmout)
#define	LOG_TWAI_SEM_LEAVE(ercd)
#define	LOG_SET_FLG_ENTER(flgid, setptn)
#define	LOG_SET_FLG_LEAVE(ercd)
#define	LOG_ISET_FLG_ENTER(flgid, setptn)
#define	LOG_ISET_FLG_LEAVE(ercd)
#define	LOG_CLR_FLG_ENTER(flgid, clrptn)
#define	LOG_CLR_FLG_LEAVE(ercd)
#define	LOG_WAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn)
#define	LOG_WAI_FLG_LEAVE(ercd, flgptn)
#define	LOG_POL_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn)
#define	LOG_POL_FLG_LEAVE(ercd, flgptn)
#define	LOG_TWAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn, tmout)
#define	LOG_TWAI_FLG_LEAVE(ercd, flgptn)
#define	LOG_SND_DTQ_ENTER(dtqid, data)
#define	LOG_SND_DTQ_LEAVE(ercd)
#define	LOG_PSND_DTQ_ENTER(dtqid, data)
#define	LOG_PSND_DTQ_LEAVE(ercd)
#define	LOG_IPSND_DTQ_ENTER(dtqid, data)
#define	LOG_IPSND_DTQ_LEAVE(ercd)
#define	LOG_TSND_DTQ_ENTER(dtqid, data, tmout)
#define	LOG_TSND_DTQ_LEAVE(ercd)
#define	LOG_FSND_DTQ_ENTER(dtqid, data)
#define	LOG_FSND_DTQ_LEAVE(ercd)
#define	LOG_IFSND_DTQ_ENTER(dtqid, data)
#define	LOG_IFSND_DTQ_LEAVE(ercd)
#define	LOG_RCV_DTQ_ENTER(dtqid, p_data)
#define	LOG_RCV_DTQ_LEAVE(ercd, data)
#define	LOG_PRCV_DTQ_ENTER(dtqid, p_data)
#define	LOG_PRCV_DTQ_LEAVE(ercd, data)
#define	LOG_TRCV_DTQ_ENTER(dtqid, p_data, tmout)
#define	LOG_TRCV_DTQ_LEAVE(ercd, data)
#define	LOG_SND_MBX_ENTER(mbxid, pk_msg)
#define	LOG_SND_MBX_LEAVE(ercd)
#define	LOG_RCV_MBX_ENTER(mbxid, ppk_msg)
#define	LOG_RCV_MBX_LEAVE(ercd, pk_msg)
#define	LOG_PRCV_MBX_ENTER(mbxid, ppk_msg)
#define	LOG_PRCV_MBX_LEAVE(ercd, pk_msg)
#define	LOG_TRCV_MBX_ENTER(mbxid, ppk_msg, tmout)
#define	LOG_TRCV_MBX_LEAVE(ercd, pk_msg)
#define	LOG_GET_MPF_ENTER(mpfid, p_blk)
#define	LOG_GET_MPF_LEAVE(ercd, blk)
#define	LOG_PGET_MPF_ENTER(mpfid, p_blk)
#define	LOG_PGET_MPF_LEAVE(ercd, blk)
#define	LOG_TGET_MPF_ENTER(mpfid, p_blk, tmout)
#define	LOG_TGET_MPF_LEAVE(ercd, blk)
#define	LOG_REL_MPF_ENTER(mpfid, blk)
#define	LOG_REL_MPF_LEAVE(ercd)
#define	LOG_SET_TIM_ENTER(p_systim)
#define	LOG_SET_TIM_LEAVE(ercd)
#define	LOG_GET_TIM_ENTER(p_systim)
#define	LOG_GET_TIM_LEAVE(ercd, systim)
#define	LOG_ISIG_TIM_ENTER()
#define	LOG_ISIG_TIM_LEAVE(ercd)
#define	LOG_STA_CYC_ENTER(cycid)
#define	LOG_STA_CYC_LEAVE(ercd)
#define	LOG_STP_CYC_ENTER(cycid)
#define	LOG_STP_CYC_LEAVE(ercd)
#define	LOG_ROT_RDQ_ENTER(tskpri)
#define	LOG_ROT_RDQ_LEAVE(ercd)
#define	LOG_IROT_RDQ_ENTER(tskpri)
#define	LOG_IROT_RDQ_LEAVE(ercd)
#define	LOG_GET_TID_ENTER(p_tskid)
#define	LOG_GET_TID_LEAVE(ercd, tskid)
#define	LOG_IGET_TID_ENTER(p_tskid)
#define	LOG_IGET_TID_LEAVE(ercd, tskid)
#define	LOG_LOC_CPU_ENTER()
#define	LOG_LOC_CPU_LEAVE(ercd)
#define	LOG_ILOC_CPU_ENTER()
#define	LOG_ILOC_CPU_LEAVE(ercd)
#define	LOG_UNL_CPU_ENTER()
#define	LOG_UNL_CPU_LEAVE(ercd)
#define	LOG_IUNL_CPU_ENTER()
#define	LOG_IUNL_CPU_LEAVE(ercd)
#define	LOG_DIS_DSP_ENTER()
#define	LOG_DIS_DSP_LEAVE(ercd)
#define	LOG_ENA_DSP_ENTER()
#define	LOG_ENA_DSP_LEAVE(ercd)
#define	LOG_SNS_CTX_ENTER()
#define	LOG_SNS_CTX_LEAVE(state)
#define	LOG_SNS_LOC_ENTER()
#define	LOG_SNS_LOC_LEAVE(state)
#define	LOG_SNS_DSP_ENTER()
#define	LOG_SNS_DSP_LEAVE(state)
#define	LOG_SNS_DPN_ENTER()
#define	LOG_SNS_DPN_LEAVE(state)
#define	LOG_VSNS_INI_ENTER()
#define	LOG_VSNS_INI_LEAVE(state)
#define	LOG_VXSNS_CTX_ENTER(p_excinf)
#define	LOG_VXSNS_CTX_LEAVE(state)
#define	LOG_VXSNS_LOC_ENTER(p_excinf)
#define	LOG_VXSNS_LOC_LEAVE(state)
#define	LOG_VXSNS_DSP_ENTER(p_excinf)
#define	LOG_VXSNS_DSP_LEAVE(state)
#define	LOG_VXSNS_DPN_ENTER(p_excinf)
#define	LOG_VXSNS_DPN_LEAVE(state)
#define	LOG_VXSNS_TEX_ENTER(p_excinf)
#define	LOG_VXSNS_TEX_LEAVE(state)
#define	LOG_VXGET_TIM_ENTER(p_sysutim)
#define	LOG_VXGET_TIM_LEAVE(ercd, sysutim)

#define	LOG_CHG_IPM_ENTER(ipm)
#define	LOG_CHG_IPM_LEAVE(ercd)
#define	LOG_GET_IPM_ENTER(p_ipm)
#define	LOG_GET_IPM_LEAVE(ercd, ipm)

#endif


#endif /* _TOOL_CONFIG_H_ */

