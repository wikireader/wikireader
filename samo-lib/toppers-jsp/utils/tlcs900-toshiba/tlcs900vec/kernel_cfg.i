INHINIB 






































 



 






 





































 










 






 

 






 
#pragma define iniflg _kernel_iniflg



 
#pragma define print_banner _kernel_print_banner



 
#pragma define dispatch _kernel_dispatch
#pragma define exit_and_dispatch _kernel_exit_and_dispatch
#pragma define cpu_initialize _kernel_cpu_initialize
#pragma define cpu_terminate _kernel_cpu_terminate
#pragma define sys_initialize _kernel_sys_initialize
#pragma define sys_exit _kernel_sys_exit
#pragma define sys_putc _kernel_sys_putc



 
#pragma define runtsk _kernel_runtsk
#pragma define schedtsk _kernel_schedtsk
#pragma define reqflg _kernel_reqflg
#pragma define enadsp _kernel_enadsp
#pragma define ready_queue _kernel_ready_queue
#pragma define ready_primap _kernel_ready_primap
#pragma define task_initialize _kernel_task_initialize
#pragma define search_schedtsk _kernel_search_schedtsk
#pragma define make_runnable _kernel_make_runnable
#pragma define make_non_runnable _kernel_make_non_runnable
#pragma define make_dormant _kernel_make_dormant
#pragma define make_active _kernel_make_active
#pragma define exit_task _kernel_exit_task
#pragma define change_priority _kernel_change_priority
#pragma define rotate_ready_queue _kernel_rotate_ready_queue
#pragma define call_texrtn _kernel_call_texrtn
#pragma define calltex _kernel_calltex



 
#pragma define make_wait_tmout _kernel_make_wait_tmout
#pragma define wait_complete _kernel_wait_complete
#pragma define wait_tmout _kernel_wait_tmout
#pragma define wait_tmout_ok _kernel_wait_tmout_ok
#pragma define wait_cancel _kernel_wait_cancel
#pragma define wait_release _kernel_wait_release
#pragma define wobj_make_wait _kernel_wobj_make_wait
#pragma define wobj_make_wait_tmout _kernel_wobj_make_wait_tmout
#pragma define wobj_change_priority _kernel_wobj_change_priority



 
#pragma define systim_offset _kernel_systim_offset
#pragma define current_time _kernel_current_time
#pragma define next_time _kernel_next_time
#pragma define next_subtime _kernel_next_subtime
#pragma define last_index _kernel_last_index
#pragma define tmevt_initialize _kernel_tmevt_initialize
#pragma define tmevt_up _kernel_tmevt_up
#pragma define tmevt_down _kernel_tmevt_down
#pragma define tmevtb_insert _kernel_tmevtb_insert
#pragma define tmevtb_delete _kernel_tmevtb_delete



 
#pragma define syslog_buffer _kernel_syslog_buffer
#pragma define syslog_count _kernel_syslog_count
#pragma define syslog_head _kernel_syslog_head
#pragma define syslog_tail _kernel_syslog_tail
#pragma define syslog_lost _kernel_syslog_lost
#pragma define syslog_logmask _kernel_syslog_logmask
#pragma define syslog_lowmask _kernel_syslog_lowmask
#pragma define syslog_initialize _kernel_syslog_initialize
#pragma define syslog_terminate _kernel_syslog_terminate



 
#pragma define semaphore_initialize _kernel_semaphore_initialize



 
#pragma define eventflag_initialize _kernel_eventflag_initialize
#pragma define eventflag_cond _kernel_eventflag_cond



 
#pragma define dataqueue_initialize _kernel_dataqueue_initialize
#pragma define enqueue_data _kernel_enqueue_data
#pragma define force_enqueue_data _kernel_force_enqueue_data
#pragma define dequeue_data _kernel_dequeue_data
#pragma define send_data_rwait _kernel_send_data_rwait
#pragma define receive_data_swait _kernel_receive_data_swait



 
#pragma define mailbox_initialize _kernel_mailbox_initialize



 
#pragma define mempfix_initialize _kernel_mempfix_initialize
#pragma define mempfix_get_block _kernel_mempfix_get_block



 
#pragma define cyclic_initialize _kernel_cyclic_initialize
#pragma define tmevtb_enqueue_cyc _kernel_tmevtb_enqueue_cyc
#pragma define call_cychdr _kernel_call_cychdr



 
#pragma define interrupt_initialize _kernel_interrupt_initialize



 
#pragma define exception_initialize _kernel_exception_initialize



 
#pragma define object_initialize _kernel_object_initialize
#pragma define call_inirtn _kernel_call_inirtn
#pragma define call_terrtn _kernel_call_terrtn
#pragma define tmax_tskid _kernel_tmax_tskid
#pragma define tinib_table _kernel_tinib_table
#pragma define torder_table _kernel_torder_table
#pragma define tcb_table _kernel_tcb_table
#pragma define tmax_semid _kernel_tmax_semid
#pragma define seminib_table _kernel_seminib_table
#pragma define semcb_table _kernel_semcb_table
#pragma define tmax_flgid _kernel_tmax_flgid
#pragma define flginib_table _kernel_flginib_table
#pragma define flgcb_table _kernel_flgcb_table
#pragma define tmax_dtqid _kernel_tmax_dtqid
#pragma define dtqcb_table _kernel_dtqcb_table
#pragma define dtqinib_table _kernel_dtqinib_table
#pragma define tmax_mbxid _kernel_tmax_mbxid
#pragma define mbxcb_table _kernel_mbxcb_table
#pragma define mbxinib_table _kernel_mbxinib_table
#pragma define tmax_mpfid _kernel_tmax_mpfid
#pragma define mpfinib_table _kernel_mpfinib_table
#pragma define mpfcb_table _kernel_mpfcb_table
#pragma define tmax_cycid _kernel_tmax_cycid
#pragma define cycinib_table _kernel_cycinib_table
#pragma define cyccb_table _kernel_cyccb_table
#pragma define tnum_inhno _kernel_tnum_inhno
#pragma define inhinib_table _kernel_inhinib_table
#pragma define tnum_excno _kernel_tnum_excno
#pragma define excinib_table _kernel_excinib_table
#pragma define tmevt_heap _kernel_tmevt_heap





 




 




 










 




















 












 













 












 




 





 









 




 





 






 




 




 




































 







































 




















 










 







































 









 






 




































 








 






 
#pragma define _int8_ char
#pragma define _int16_ int
#pragma define _int32_ long



 





































 









































































































































 





































 








 










 
 
#pragma inline sil_reb_mem

#pragma inline sil_wrb_mem

#pragma inline sil_reh_mem

#pragma inline sil_wrh_mem

#pragma inline sil_reh_lem

#pragma inline sil_wrh_lem

#pragma inline sil_reh_bem

#pragma inline sil_wrh_bem

#pragma inline sil_rew_mem

#pragma inline sil_wrw_mem

#pragma inline sil_rew_lem

#pragma inline sil_wrw_lem

#pragma inline sil_rew_bem

#pragma inline sil_wrw_bem


 
#pragma inline _t_perror

#pragma inline _t_panic


 
#pragma inline _syslog_0

#pragma inline _syslog_1

#pragma inline _syslog_2

#pragma inline _syslog_3

#pragma inline _syslog_4

#pragma inline _syslog_5

#pragma inline _syslog_6


 
#pragma inline enqueue_msg_pri


 
#pragma inline bitmap_search


 
#pragma inline tmevtb_delete_top


 
#pragma inline make_non_wait

#pragma inline queue_insert_tpri

#pragma inline wobj_queue_insert


 
#pragma inline queue_initialize

#pragma inline queue_insert_prev

#pragma inline queue_delete

#pragma inline queue_delete_next

#pragma inline queue_empty


 
#pragma inline tmevtb_enqueue

#pragma inline tmevtb_enqueue_evttim

#pragma inline tmevtb_dequeue


 
#pragma inline make_wait


 
#pragma inline serial_snd_chr










#pragma define asm __asm







 
#pragma define Asm __ASM








 


 
 
 
 




#pragma define CHAR_BIT 8
#pragma define SCHAR_MAX 127
#pragma define SCHAR_MIN (-127)
#pragma define SHRT_MAX 32767
#pragma define SHRT_MIN (-32767)
#pragma define LONG_MAX (2147483647)
#pragma define LONG_MIN (-2147483647)
#pragma define UCHAR_MAX 255U
#pragma define UCHAR_MIN 0
#pragma define USHRT_MAX 65535U
#pragma define ULONG_MAX 4294967295U
#pragma define USHRT_MIN 0
#pragma define ULONG_MIN 0
#pragma define MB_LEN_MAX 4





#pragma define CHAR_MAX SCHAR_MAX
#pragma define CHAR_MIN SCHAR_MIN



#pragma define INT_MAX SHRT_MAX
#pragma define INT_MIN SHRT_MIN
#pragma define UINT_MAX USHRT_MAX
#pragma define UINT_MIN USHRT_MIN







 






 






 















 







































 



















 










 




































 








 






 






 









 
















 









 


 
 
 
 




typedef	long		ptrdiff_t;







typedef unsigned int	size_t;




typedef int		wchar_t;



#pragma define NULL ((void *)0)









 

#pragma define _bool_ int




 



typedef	signed char		B;	 
typedef	unsigned char		UB;	 
typedef	char			VB;	 



typedef	signed int		H;	 
typedef	unsigned int 	UH;	 
typedef	int			VH;	 


typedef	signed long		W;	 
typedef	unsigned long	UW;	 
typedef	long			VW;	 







typedef	void		*VP;		 
typedef	void		(*FP)();	 

typedef signed int	INT;		 
typedef unsigned int	UINT;		 

typedef int		BOOL;		 

typedef INT		FN;		 
typedef	INT		ER;		 
typedef	INT		ID;		 
typedef	UINT		ATR;		 
typedef	UINT		STAT;		 
typedef	UINT		MODE;		 
typedef	INT		PRI;		 
typedef	size_t		SIZE;		 

typedef	INT		TMO;		 
typedef	UINT		RELTIM;		 
typedef	UW		SYSTIM;		 




typedef	VP		VP_INT;		 


typedef	INT		ER_BOOL;	 
typedef	INT		ER_ID;		 
typedef	INT		ER_UINT;	 





 









 






#pragma define TRUE 1
#pragma define FALSE 0
#pragma define E_OK 0



 
#pragma define E_SYS (-5)
#pragma define E_NOSPT (-9)
#pragma define E_RSFN (-10)
#pragma define E_RSATR (-11)
#pragma define E_PAR (-17)
#pragma define E_ID (-18)
#pragma define E_CTX (-25)
#pragma define E_MACV (-26)
#pragma define E_OACV (-27)
#pragma define E_ILUSE (-28)
#pragma define E_NOMEM (-33)
#pragma define E_NOID (-34)
#pragma define E_OBJ (-41)
#pragma define E_NOEXS (-42)
#pragma define E_QOVR (-43)
#pragma define E_RLWAI (-49)
#pragma define E_TMOUT (-50)
#pragma define E_DLT (-51)
#pragma define E_CLS (-52)
#pragma define E_WBLK (-57)
#pragma define E_BOVR (-58)



 
#pragma define TA_NULL 0u



 
#pragma define TMO_POL 0
#pragma define TMO_FEVR (-1)
#pragma define TMO_NBLK (-2)



 








 



















 




































 








 







 






 
typedef	unsigned long SYSUTIM;	 







 
#pragma define TIC_NUME 1u
#pragma define TIC_DENO 1u



extern void start(void);


 
#pragma inline kernel_abort

void
kernel_abort()
{
	start();
	 
}







































 








 










 
typedef	UINT	INHNO;		 
typedef	UINT	EXCNO;		 



 
typedef	UINT	IPM;		 

extern ER	chg_ipm(IPM ipm) ;
extern ER	get_ipm(IPM *p_ipm) ;





 
#pragma define SIL_ENDIAN SIL_ENDIAN_LITTLE





 







































 























 









 
#pragma define LOG_TYPE_INH 0x01u
#pragma define LOG_TYPE_ISR 0x02u
#pragma define LOG_TYPE_CYC 0x03u
#pragma define LOG_TYPE_EXC 0x04u
#pragma define LOG_TYPE_TEX 0x05u
#pragma define LOG_TYPE_TSKSTAT 0x06u
#pragma define LOG_TYPE_DSP 0x07u
#pragma define LOG_TYPE_SVC 0x08u
#pragma define LOG_TYPE_COMMENT 0x09u
#pragma define LOG_TYPE_ASSERT 0x0au

#pragma define LOG_ENTER 0x00u
#pragma define LOG_LEAVE 0x80u



 
#pragma define LOG_EMERG 0u
#pragma define LOG_ALERT 1u
#pragma define LOG_CRIT 2u
#pragma define LOG_ERROR 3u
#pragma define LOG_WARNING 4u
#pragma define LOG_NOTICE 5u
#pragma define LOG_INFO 6u
#pragma define LOG_DEBUG 7u





 

#pragma define TMAX_LOGINFO 6

typedef struct {
		UINT	logtype;		 
		SYSTIM	logtim;			 
		VP_INT	loginfo[6];	 
	} SYSLOG;



 







 
extern ER	vwri_log(UINT prio, SYSLOG *p_log) ;



 
extern ER_UINT	vrea_log(SYSLOG *p_log) ;



 
extern ER	vmsk_log(UINT logmask, UINT lowmask) ;



 

 ER
_syslog_0(UINT prio, UINT type)
{
	SYSLOG	log;

	log.logtype = type;
	return(vwri_log(prio, &log));
}

 ER
_syslog_1(UINT prio, UINT type, VP_INT arg1)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	return(vwri_log(prio, &log));
}

 ER
_syslog_2(UINT prio, UINT type, VP_INT arg1, VP_INT arg2)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	return(vwri_log(prio, &log));
}

 ER
_syslog_3(UINT prio, UINT type, VP_INT arg1, VP_INT arg2, VP_INT arg3)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	log.loginfo[2] = arg3;
	return(vwri_log(prio, &log));
}

 ER
_syslog_4(UINT prio, UINT type, VP_INT arg1, VP_INT arg2,
				VP_INT arg3, VP_INT arg4)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	log.loginfo[2] = arg3;
	log.loginfo[3] = arg4;
	return(vwri_log(prio, &log));
}

 ER
_syslog_5(UINT prio, UINT type, VP_INT arg1, VP_INT arg2,
				VP_INT arg3, VP_INT arg4, VP_INT arg5)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	log.loginfo[2] = arg3;
	log.loginfo[3] = arg4;
	log.loginfo[4] = arg5;
	return(vwri_log(prio, &log));
}

 ER
_syslog_6(UINT prio, UINT type, VP_INT arg1, VP_INT arg2, VP_INT arg3,
				VP_INT arg4, VP_INT arg5, VP_INT arg6)
{
	SYSLOG	log;

	log.logtype = type;
	log.loginfo[0] = arg1;
	log.loginfo[1] = arg2;
	log.loginfo[2] = arg3;
	log.loginfo[3] = arg4;
	log.loginfo[4] = arg5;
	log.loginfo[5] = arg6;
	return(vwri_log(prio, &log));
}























 




























 
extern ER	syslog(UINT prio, const char *format, ...) ;



 
extern void	syslog_printf(const char *format, VP_INT *args,
						void (*putc)(char)) ;
extern void	syslog_print(SYSLOG *p_log, void (*putc)(char)) ;
extern void	syslog_output(void (*putc)(char)) ;






 





 


typedef	UINT		TEXPTN;		 
typedef	UINT		FLGPTN;		 

typedef	struct t_msg {			 
	struct t_msg	*next;
} T_MSG;

typedef	struct t_msg_pri {		 
	T_MSG		msgque;		 
	PRI		msgpri;		 
} T_MSG_PRI;





 



 
extern ER	act_tsk(ID tskid) ;
extern ER	iact_tsk(ID tskid) ;
extern ER_UINT	can_act(ID tskid) ;
extern void	ext_tsk(void) ;
extern ER	ter_tsk(ID tskid) ;
extern ER	chg_pri(ID tskid, PRI tskpri) ;
extern ER	get_pri(ID tskid, PRI *p_tskpri) ;


 
extern ER	slp_tsk(void) ;
extern ER	tslp_tsk(TMO tmout) ;
extern ER	wup_tsk(ID tskid) ;
extern ER	iwup_tsk(ID tskid) ;
extern ER_UINT	can_wup(ID tskid) ;
extern ER	rel_wai(ID tskid) ;
extern ER	irel_wai(ID tskid) ;
extern ER	sus_tsk(ID tskid) ;
extern ER	rsm_tsk(ID tskid) ;
extern ER	frsm_tsk(ID tskid) ;
extern ER	dly_tsk(RELTIM dlytim) ;


 
extern ER	ras_tex(ID tskid, TEXPTN rasptn) ;
extern ER	iras_tex(ID tskid, TEXPTN rasptn) ;
extern ER	dis_tex(void) ;
extern ER	ena_tex(void) ;
extern BOOL	sns_tex(void) ;


 
extern ER	sig_sem(ID semid) ;
extern ER	isig_sem(ID semid) ;
extern ER	wai_sem(ID semid) ;
extern ER	pol_sem(ID semid) ;
extern ER	twai_sem(ID semid, TMO tmout) ;

extern ER	set_flg(ID flgid, FLGPTN setptn) ;
extern ER	iset_flg(ID flgid, FLGPTN setptn) ;
extern ER	clr_flg(ID flgid, FLGPTN clrptn) ;
extern ER	wai_flg(ID flgid, FLGPTN waiptn,
			MODE wfmode, FLGPTN *p_flgptn) ;
extern ER	pol_flg(ID flgid, FLGPTN waiptn,
			MODE wfmode, FLGPTN *p_flgptn) ;
extern ER	twai_flg(ID flgid, FLGPTN waiptn,
			MODE wfmode, FLGPTN *p_flgptn, TMO tmout) ;

extern ER	snd_dtq(ID dtqid, VP_INT data) ;
extern ER	psnd_dtq(ID dtqid, VP_INT data) ;
extern ER	ipsnd_dtq(ID dtqid, VP_INT data) ;
extern ER	tsnd_dtq(ID dtqid, VP_INT data, TMO tmout) ;
extern ER	fsnd_dtq(ID dtqid, VP_INT data) ;
extern ER	ifsnd_dtq(ID dtqid, VP_INT data) ;
extern ER	rcv_dtq(ID dtqid, VP_INT *p_data) ;
extern ER	prcv_dtq(ID dtqid, VP_INT *p_data) ;
extern ER	trcv_dtq(ID dtqid, VP_INT *p_data, TMO tmout) ;

extern ER	snd_mbx(ID mbxid, T_MSG *pk_msg) ;
extern ER	rcv_mbx(ID mbxid, T_MSG **ppk_msg) ;
extern ER	prcv_mbx(ID mbxid, T_MSG **ppk_msg) ;
extern ER	trcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout) ;


 
extern ER	get_mpf(ID mpfid, VP *p_blk) ;
extern ER	pget_mpf(ID mpfid, VP *p_blk) ;
extern ER	tget_mpf(ID mpfid, VP *p_blk, TMO tmout) ;
extern ER	rel_mpf(ID mpfid, VP blk) ;


 
extern ER	set_tim(const SYSTIM *p_systim) ;
extern ER	get_tim(SYSTIM *p_systim) ;
extern ER	isig_tim(void) ;

extern ER	sta_cyc(ID cycid) ;
extern ER	stp_cyc(ID cycid) ;


 
extern ER	rot_rdq(PRI tskpri) ;
extern ER	irot_rdq(PRI tskpri) ;
extern ER	get_tid(ID *p_tskid) ;
extern ER	iget_tid(ID *p_tskid) ;
extern ER	loc_cpu(void) ;
extern ER	iloc_cpu(void) ;
extern ER	unl_cpu(void) ;
extern ER	iunl_cpu(void) ;
extern ER	dis_dsp(void) ;
extern ER	ena_dsp(void) ;
extern BOOL	sns_ctx(void) ;
extern BOOL	sns_loc(void) ;
extern BOOL	sns_dsp(void) ;
extern BOOL	sns_dpn(void) ;



 
extern BOOL	vxsns_ctx(VP p_excinf) ;
extern BOOL	vxsns_loc(VP p_excinf) ;
extern BOOL	vxsns_dsp(VP p_excinf) ;
extern BOOL	vxsns_dpn(VP p_excinf) ;
extern BOOL	vxsns_tex(VP p_excinf) ;
extern BOOL	vsns_ini(void) ;





 
#pragma define TA_HLNG 0x00u
#pragma define TA_ASM 0x01u

#pragma define TA_TFIFO 0x00u
#pragma define TA_TPRI 0x01u

#pragma define TA_MFIFO 0x00u
#pragma define TA_MPRI 0x02u

#pragma define TA_ACT 0x02u

#pragma define TA_WSGL 0x00u
#pragma define TA_CLR 0x04u

#pragma define TA_STA 0x02u



 
#pragma define TWF_ANDW 0x00u
#pragma define TWF_ORW 0x01u



 
#pragma define TSK_SELF 0
#pragma define TSK_NONE 0
#pragma define TPRI_SELF 0
#pragma define TPRI_INI 0



 



 
#pragma define TMIN_TPRI 1
#pragma define TMAX_TPRI 16
#pragma define TMIN_MPRI 1
#pragma define TMAX_MPRI 16



 
#pragma define TKERNEL_MAKER 0x0118u
#pragma define TKERNEL_PRID 0x0001u
#pragma define TKERNEL_SPVER 0x5402u
#pragma define TKERNEL_PRVER 0x1041u



 
#pragma define TMAX_ACTCNT 1
#pragma define TMAX_WUPCNT 1
#pragma define TMAX_SUSCNT 1



 
#pragma define TBIT_TEXPTN (sizeof(TEXPTN) * CHAR_BIT)
					 
#pragma define TBIT_FLGPTN (sizeof(FLGPTN) * CHAR_BIT)
					 









 





































 
















 










 




































 






 






 

 
















 




 
 
 



 
#pragma define TNUM_PORT 1
#pragma define TNUM_SIOP 1



 
#pragma define LOGTASK_PORTID 1
#pragma define CONSOLE_PORTID 1




 
extern void	_kernel_sys_initialize(void);






 
extern void	_kernel_sys_exit(void);






 
extern void	_kernel_sys_putc(char c);







 




































 






 






 

 






#pragma define activate_r _kernel_activate_r
#pragma define ret_int _kernel_ret_int
#pragma define interrupt _kernel_interrupt
#pragma define task_intmask _kernel_task_intmask
#pragma define int_intmask _kernel_int_intmask















 





































 



 






 
#pragma inline current_sr

UH
current_sr()
{
	__ASM("	push	sr");
	__ASM("	pop		hl");
	return(__HL);
}



 
#pragma inline set_sr

void
set_sr(UH sr)
{
	__HL = sr;
	__ASM("	push	hl");
	__ASM("	pop		sr");
}



 



 
#pragma inline disint

void
disint()
{
	__ASM("	ei	7	");
}



 
#pragma inline enaint

void
enaint()
{
	__ASM("	ei	0	");
}






 


#pragma inline bitmap_search

UINT
bitmap_search(UINT bitmap)
{
	


 
	__HL = bitmap;
	__ASM("	bs1f	a, hl");
	return (UINT)__A;
}






 







 
#pragma define TBIT_TCB_TSTAT 8
#pragma define TBIT_TCB_PRIORITY 4




 
typedef struct task_context_block {
	VP	xsp;		 
	FP	pc;		 
} CTXB;



 
extern UINT	intcnt;






 



 
#pragma inline current_intmask

UH
current_intmask()
{
	return(current_sr() & 0x7000);
}



 
#pragma inline set_intmask

void
set_intmask(UH intmask)
{
	set_sr((current_sr() & ~0x7000) | intmask);
}



 

#pragma inline sense_context

BOOL
sense_context()
{
	__ASM("	ldc	hl, intnest");
	return __HL;
}

#pragma inline sense_lock

BOOL
sense_lock()
{
	return(current_intmask() == 0x7000);
}

#pragma define t_sense_lock sense_lock
#pragma define i_sense_lock sense_lock






 


extern UH	_kernel_task_intmask;	 


#pragma inline t_lock_cpu

void
t_lock_cpu()
{
	disint();
}

#pragma inline t_unlock_cpu

void
t_unlock_cpu()
{

	


 
	set_intmask(_kernel_task_intmask);

	

}



 

extern UH	_kernel_int_intmask;	 

#pragma inline i_lock_cpu

void
i_lock_cpu()
{
	UH	intmask;

	



 
	intmask = current_intmask();
	disint();
	_kernel_int_intmask = intmask;
}

#pragma inline i_unlock_cpu

void
i_unlock_cpu()
{
	set_intmask(_kernel_int_intmask);
}



 






 
extern void	_kernel_dispatch(void);





 
extern void	_kernel_exit_and_dispatch(void);



 







 

#pragma inline define_inh

void
define_inh(INHNO inhno, FP inthdr)
{
}







 
#pragma inline define_exc

void
define_exc(EXCNO excno, FP exchdr)
{
}




 

















 
















 














 

















 






 



 
#pragma inline exc_sense_context

BOOL
exc_sense_context(VP p_excinf)
{
	__ASM("	ldc	hl, intnest");
	return ( __HL > 1 );
}



 
#pragma inline exc_sense_lock

BOOL
exc_sense_lock(VP p_excinf)
{
	return ((*((UH *) p_excinf) & 0x7000) == 0x7000);
}



 
extern void	_kernel_cpu_initialize(void);



 
extern void	_kernel_cpu_terminate(void);






 




































 






 






 




 




 


#pragma inline call_atexit

 void
call_atexit()
{





}





 






























































































































































































 
#pragma define TMIN_TSKID 1
#pragma define TMIN_SEMID 1
#pragma define TMIN_FLGID 1
#pragma define TMIN_DTQID 1
#pragma define TMIN_MBXID 1
#pragma define TMIN_MPFID 1
#pragma define TMIN_CYCID 1



 
#pragma define TNUM_TPRI (TMAX_TPRI - TMIN_TPRI + 1)
#pragma define TNUM_MPRI (TMAX_MPRI - TMIN_MPRI + 1)



 




 




 
extern void	_kernel_object_initialize(void);



 
extern void	_kernel_call_inirtn(void);



 
extern void	_kernel_call_terrtn(void);



 
extern void	_kernel_print_banner(void);



 
extern void	kernel_exit(void);



 
extern BOOL	_kernel_iniflg;






 





































 



 









































 










 






 
typedef struct queue {
	struct queue *next;		 
	struct queue *prev;		 
} QUEUE;





 
 void
queue_initialize(QUEUE *queue)
{
	queue->prev = queue->next = queue;
}






 
 void
queue_insert_prev(QUEUE *queue, QUEUE *entry)
{
	entry->prev = queue->prev;
	entry->next = queue;
	queue->prev->next = entry;
	queue->prev = entry;
}





 
 void
queue_delete(QUEUE *entry)
{
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
}







 
 QUEUE *
queue_delete_next(QUEUE *queue)
{
	QUEUE	*entry;

	((void)(!(queue->next != queue) ? (_syslog_3(0u, 0x0au, (VP_INT)("D:\\jsp141_win\\tools\\TLCS900L1-TOSHIBA\\../../kernel\\queue.h"), (VP_INT)(111), (VP_INT)("queue->next != queue")), kernel_abort(), 0) : 0));
	entry = queue->next;
	queue->next = entry->next;
	entry->next->prev = queue;
	return(entry);
}





 
 BOOL
queue_empty(QUEUE *queue)
{
	if (queue->next == queue) {
		((void)(!(queue->prev == queue) ? (_syslog_3(0u, 0x0au, (VP_INT)("D:\\jsp141_win\\tools\\TLCS900L1-TOSHIBA\\../../kernel\\queue.h"), (VP_INT)(127), (VP_INT)("queue->prev == queue")), kernel_abort(), 0) : 0));
		return(1);
	}
	return(0);
}







































 



 











 
typedef UW	EVTTIM;



 
#pragma define TMAX_RELTIM ((((EVTTIM) 1) << (sizeof(EVTTIM) * CHAR_BIT - 1)) - 1)



 
typedef void	(*CBACK)(VP);	 

typedef struct time_event_block {
	UINT	index;		 
	CBACK	callback;	 
	VP	arg;		 
} TMEVTB;



 
typedef struct time_event_node {
	EVTTIM	time;		 
	TMEVTB	*tmevtb;	 
} TMEVTN;



 
extern TMEVTN	_kernel_tmevt_heap[];



 
extern SYSTIM	_kernel_systim_offset;






 
extern SYSTIM	_kernel_current_time;



 
extern SYSTIM	_kernel_next_time;






 









 

#pragma define base_time ((EVTTIM) next_time)








 
extern UINT	_kernel_last_index;



 
extern void	_kernel_tmevt_initialize(void);



 
extern UINT	_kernel_tmevt_up(UINT index, EVTTIM time);
extern UINT	_kernel_tmevt_down(UINT index, EVTTIM time);



 
extern void	_kernel_tmevtb_insert(TMEVTB *tmevtb, EVTTIM time);
extern void	_kernel_tmevtb_delete(TMEVTB *tmevtb);






 
 void
tmevtb_enqueue(TMEVTB *tmevtb, RELTIM time, CBACK callback, VP arg)
{
	((void)(!(time <= (RELTIM)((((EVTTIM) 1) << (sizeof(EVTTIM) * 8 - 1)) - 1)) ? (_syslog_3(0u, 0x0au, (VP_INT)("D:\\jsp141_win\\tools\\TLCS900L1-TOSHIBA\\../../kernel\\time_event.h"), (VP_INT)(158), (VP_INT)("time <= (RELTIM)TMAX_RELTIM")), kernel_abort(), 0) : 0));

	tmevtb->callback = callback;
	tmevtb->arg = arg;
	_kernel_tmevtb_insert(tmevtb, ((EVTTIM) _kernel_next_time) + time);
}






 
 void
tmevtb_enqueue_evttim(TMEVTB *tmevtb, EVTTIM time, CBACK callback, VP arg)
{
	tmevtb->callback = callback;
	tmevtb->arg = arg;
	_kernel_tmevtb_insert(tmevtb, time);
}



 
 void
tmevtb_dequeue(TMEVTB *tmevtb)
{
	_kernel_tmevtb_delete(tmevtb);
}





 










 
#pragma define TS_DORMANT 0x00u
#pragma define TS_RUNNABLE 0x01u
#pragma define TS_WAITING 0x02u
#pragma define TS_SUSPENDED 0x04u

#pragma define TS_WAIT_SLEEP 0x08u
#pragma define TS_WAIT_WOBJ 0x10u
#pragma define TS_WAIT_WOBJCB 0x20u








 














































 
typedef union waiting_information {
	ER	wercd;		 
	TMEVTB	*tmevtb;	 
} WINFO;













 
typedef struct task_initialization_block {
	ATR	tskatr;		 
	VP_INT	exinf;		 
	FP	task;		 
	UINT	ipriority;	 
	SIZE	stksz;		 
	VP	stk;		 

	ATR	texatr;		 
	FP	texrtn;		 
} TINIB;






 
















 
typedef struct task_control_block {
	QUEUE	task_queue;	 
	const TINIB *tinib;	 

	UINT	tstat : 8;		 
	UINT	priority : 4;	 
	unsigned int	actcnt : 1;		 
	unsigned int	wupcnt : 1;		 
	unsigned int	enatex : 1;		 

	TEXPTN	texptn;		 
	WINFO	*winfo;		 
	CTXB	tskctxb;	 
} TCB;









 
extern TCB	*_kernel_runtsk;








 
extern TCB	*_kernel_schedtsk;






 
extern BOOL	_kernel_reqflg;






 
extern BOOL	_kernel_enadsp;







 
extern QUEUE	_kernel_ready_queue[(16 - 1 + 1)];










 
extern UINT	_kernel_ready_primap;



 
extern const ID	_kernel_tmax_tskid;



 
extern const TINIB	_kernel_tinib_table[];



 
extern const ID	_kernel_torder_table[];



 
extern TCB	_kernel_tcb_table[];



 
#pragma define TNUM_TSK ((UINT)(tmax_tskid - TMIN_TSKID + 1))



 






 




 
extern void	_kernel_task_initialize(void);






 
extern TCB	*_kernel_search_schedtsk(void);








 
extern BOOL	_kernel_make_runnable(TCB *tcb);








 
extern BOOL	_kernel_make_non_runnable(TCB *tcb);







 
extern void	_kernel_make_dormant(TCB *tcb);







 
extern BOOL	_kernel_make_active(TCB *tcb);










 
extern void	_kernel_exit_task(void);








 
extern BOOL	_kernel_change_priority(TCB *tcb, UINT newpri);







 
extern BOOL	_kernel_rotate_ready_queue(UINT pri);















 
extern void	_kernel_call_texrtn(void);











 
extern void	_kernel_calltex(void);







































 


 









































 










 






 

	
	






 



	







 



	
	
	
	






 



	
	








 



	

	
	
	
	
	






 



	
		
		
	
	






 
typedef struct semaphore_initialization_block {
	ATR	sematr;		 
	UINT	isemcnt;	 
	UINT	maxsem;		 
} SEMINIB;



 
typedef struct semaphore_control_block {
	QUEUE	wait_queue;	 
	const SEMINIB *seminib;	 
	UINT	semcnt;		 
} SEMCB;



 
extern void	_kernel_semaphore_initialize(void);







































 


 









































 










 






 

	
	






 



	







 



	
	
	
	






 



	
	








 



	

	
	
	
	
	






 



	
		
		
	
	






 
typedef struct eventflag_initialization_block {
	ATR	flgatr;		 
	FLGPTN	iflgptn;	 
} FLGINIB;



 
typedef struct eventflag_control_block {
	QUEUE	wait_queue;	 
	const FLGINIB *flginib;	 
	FLGPTN	flgptn;		 
} FLGCB;



 
extern void	_kernel_eventflag_initialize(void);



 
extern BOOL	_kernel_eventflag_cond(FLGCB *flgcb, FLGPTN waiptn,
					MODE wfmode, FLGPTN *p_flgptn);







































 


 









































 










 






 

	
	






 



	







 



	
	
	
	






 



	
	








 



	

	
	
	
	
	






 



	
		
		
	
	






 
typedef struct dataqueue_initialization_block {
	ATR	dtqatr;		 
	UINT	dtqcnt;		 
	VP	dtq;		 
} DTQINIB;



 
typedef struct dataqueue_control_block {
	QUEUE	swait_queue;	 
	const DTQINIB *dtqinib;	 
	QUEUE	rwait_queue;	 
	UINT	count;		 
	UINT	head;		 
	UINT	tail;		 
} DTQCB;



 
extern void	_kernel_dataqueue_initialize(void);



 
extern BOOL	_kernel_enqueue_data(DTQCB *dtqcb, VP_INT data);



 
extern void	_kernel_force_enqueue_data(DTQCB *dtqcb, VP_INT data);



 
extern BOOL	_kernel_dequeue_data(DTQCB *dtqcb, VP_INT *p_data);



 
extern TCB	*_kernel_send_data_rwait(DTQCB *dtqcb, VP_INT data);



 
extern TCB	*_kernel_receive_data_swait(DTQCB *dtqcb, VP_INT *p_data);







































 


 









































 










 






 

	
	






 



	







 



	
	
	
	






 



	
	








 



	

	
	
	
	
	






 



	
		
		
	
	






 
typedef struct mailbox_initialization_block {
	ATR	mbxatr;		 
	PRI	maxmpri;	 
} MBXINIB;





 
typedef struct mailbox_control_block {
	QUEUE	wait_queue;	 
	const MBXINIB *mbxinib;	 
	T_MSG	*head;		 
	T_MSG	*last;		 
} MBXCB;



 
extern void	_kernel_mailbox_initialize(void);







































 


 









































 










 






 

	
	






 



	







 



	
	
	
	






 



	
	








 



	

	
	
	
	
	






 



	
		
		
	
	






 
typedef struct fixed_memorypool_initialization_block {
	ATR	mpfatr;		 
	UINT	blksz;		 
	VP	mpf;		 
	VP	limit;		 
} MPFINIB;



 
typedef struct free_list {
	struct free_list *next;
} FREEL;



 
typedef struct fixed_memorypool_control_block {
	QUEUE	wait_queue;	 
	const MPFINIB *mpfinib;	 
	VP	unused;		 
	FREEL	*freelist;	 
} MPFCB;



 
extern void	_kernel_mempfix_initialize(void);



 
extern BOOL	_kernel_mempfix_get_block(MPFCB *mpfcb, VP *p_blk);







































 


 









































 










 






 

	
	






 



	







 



	
	
	
	






 



	
	








 



	

	
	
	
	
	






 



	
		
		
	
	








































 



 











 




 




 



	
	
	




 

	
	




 




 







 




 







 









 










 




 




 





 








 



	

	
	
	







 



	
	
	




 



	






 
typedef struct cyclic_handler_initialization_block {
	ATR	cycatr;		 
	VP_INT	exinf;		 
	FP	cychdr;		 
	RELTIM	cyctim;		 
	RELTIM	cycphs;		 
} CYCINIB;



 
typedef struct cyclic_handler_control_block {
	const CYCINIB *cycinib;	 
	BOOL	cycsta;		 
	EVTTIM	evttim;		 
	TMEVTB	tmevtb;		 
} CYCCB;



 
extern void	_kernel_cyclic_initialize(void);



 
extern void	_kernel_tmevtb_enqueue_cyc(CYCCB *cyccb, EVTTIM evttim);



 
extern void	_kernel_call_cychdr(CYCCB *cyccb);







































 


 






 
typedef struct interrupt_handler_initialization_block {
	INHNO	inhno;		 
	ATR	inhatr;		 
	FP	inthdr;		 
} INHINIB;



 
extern void	_kernel_interrupt_initialize(void);







































 


 






 
typedef struct cpu_exception_handler_initialization_block {
	EXCNO	excno;		 
	ATR	excatr;		 
	FP	exchdr;		 
} EXCINIB;



 
extern void	_kernel_exception_initialize(void);







































 



 











 




 




 



	
	
	




 

	
	




 




 







 




 







 









 










 




 




 





 








 



	

	
	
	







 



	
	
	




 



	













 


typedef VP	__STK_UNIT;



typedef VP	__MPF_UNIT;














 

 






 




 




 










 




















 












 













 












 




 





 









 




 





 






 




 




 



































 




 




 










 




















 












 













 












 




 





 









 




 





 






 




 




 


































 














 





























	 

#pragma define CYCHDR1 1
#pragma define LOGTASK 5
#pragma define MAIN_TASK 4
#pragma define SERIAL_RCV_SEM1 1
#pragma define SERIAL_SND_SEM1 2
#pragma define TASK1 1
#pragma define TASK2 2
#pragma define TASK3 3





















	 







































 



 






































 













 










 







































 




















 










 




 




 





 




 





 






	



	
	






 



 









 













 







 










			

			

			

















 






 








 

















 











 


















 





 







 



 







 







 






 

					 

					 











 





































 



 






 
typedef struct {
		UINT	reacnt;		 
		UINT	wricnt;		 
	} T_SERIAL_RPOR;



 
extern void	serial_initialize(VP_INT exinf) ;



 
extern ER	serial_opn_por(ID portid) ;
extern ER	serial_cls_por(ID portid) ;
extern ER_UINT	serial_rea_dat(ID portid, char *buf, UINT len) ;
extern ER_UINT	serial_wri_dat(ID portid, char *buf, UINT len) ;
extern ER	serial_ctl_por(ID portid, UINT ioctl) ;
extern ER	serial_ref_por(ID portid, T_SERIAL_RPOR *pk_rpor) ;





 
#pragma define IOCTL_NULL 0u
#pragma define IOCTL_ECHO 0x0001u
#pragma define IOCTL_CRLF 0x0010u
#pragma define IOCTL_FCSND 0x0100u
#pragma define IOCTL_FCANY 0x0200u
#pragma define IOCTL_FCRCV 0x0400u





 
extern void	kernel_exit(void) ;



 

extern void	t_perror(UINT prio, const char *file, int line,
					const char *expr, ER ercd) ;

 ER
_t_perror(const char *file, int line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(3u, file, line, expr, ercd);
	}
	return(ercd);
}

 ER
_t_panic(const char *file, int line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(0u, file, line, expr, ercd);
		kernel_abort();
	}
	return(ercd);
}














 

#pragma define MAIN_PRIORITY 5
					 

#pragma define HIGH_PRIORITY 9
#pragma define MID_PRIORITY 10
#pragma define LOW_PRIORITY 11



 






























































































#pragma define CPUEXC1 3

#pragma define STACK_SIZE 128
#pragma define TASK_PORTID 1






 










#pragma define LOOP_REF 1000000




 


extern void	task(VP_INT tskno);
extern void	main_task(VP_INT exinf);
extern void	tex_routine(TEXPTN texptn, VP_INT tskno);
extern void	cyclic_handler(VP_INT exinf);
extern void	cpuexc_handler(VP p_excinf);






































 



 









































 


















 










 





































 
















 










 







































 









 






 







 






 






 






 















 







































 



















 










 







 






 






 



























































 









 












 
























 




 






 








 



















 




































 








 







 






 








 








 




	
	 








































 








 










 





 









 






 







































 























 









 
















 













 




		
		
		
	



 







 




 




 




 




	

	
	





	

	
	
	





	

	
	
	
	





	

	
	
	
	
	




				

	

	
	
	
	
	
	




				

	

	
	
	
	
	
	
	




				

	

	
	
	
	
	
	
	
	
























 




























 




 

						








 
#pragma define SIL_ENDIAN_LITTLE 0
#pragma define SIL_ENDIAN_BIG 1





 

#pragma define SIL_PRE_LOC BOOL _sil_loc_ = sns_loc()








 
extern void	sil_dly_nse(UINT dlytim) ;



 













 




 


 VB
sil_reb_mem(VP mem)
{
	return(*((volatile VB *) mem));
}

 void
sil_wrb_mem(VP mem, VB data)
{
	*((volatile VB *) mem) = data;
}





 


 VH
sil_reh_mem(VP mem)
{
	return(*((volatile VH *) mem));
}

 void
sil_wrh_mem(VP mem, VH data)
{
	*((volatile VH *) mem) = data;
}











	

	
	








	










 VH
sil_reh_bem(VP mem)
{
	VH	data;

	data = *((volatile VH *) mem);
	return(((VH)((((UH)(data) & 0xff) << 8) | (((UH)(data) >> 8) & 0xff))));
}




 void
sil_wrh_bem(VP mem, VH data)
{
	*((volatile VH *) mem) = ((VH)((((UH)(data) & 0xff) << 8) | (((UH)(data) >> 8) & 0xff)));
}







 

 VW
sil_rew_mem(VP mem)
{
	return(*((volatile VW *) mem));
}

 void
sil_wrw_mem(VP mem, VW data)
{
	*((volatile VW *) mem) = data;
}











	

	
	








	










 VW
sil_rew_bem(VP mem)
{
	VW	data;

	data = *((volatile VW *) mem);
	return(((VW)((((UW)(data) & 0xff) << 24) | (((UW)(data) & 0xff00) << 8) | (((UW)(data)>> 8) & 0xff00) | (((UW)(data) >> 24) & 0xff))));
}




 void
sil_wrw_bem(VP mem, VW data)
{
	*((volatile VW *) mem) = ((VW)((((UW)(data) & 0xff) << 24) | (((UW)(data) & 0xff00) << 8) | (((UW)(data)>> 8) & 0xff00) | (((UW)(data) >> 24) & 0xff)));
}















 





































 
















 










 




 




 


















































 
















 










 




 




 





 




 







 










 




 













 




 





	





	






 





	





	












	

	
	








	













	

	
	








	








 




	





	












	

	
	








	













	

	
	








	

















































 




 






 
#pragma define INT_NO_RST_SW0 1
#pragma define INT_NO_SW1 2
#pragma define INT_NO_UNDEF_SW2 3
#pragma define INT_NO_SW3 4
#pragma define INT_NO_SW4 5
#pragma define INT_NO_SW5 6
#pragma define INT_NO_SW6 7
#pragma define INT_NO_SW7 8
#pragma define INT_NO_NMI 9
#pragma define INT_NO_WD 10
#pragma define INT_NO_0 11
#pragma define INT_NO_1 12
#pragma define INT_NO_2 13
#pragma define INT_NO_3 14
#pragma define INT_NO_4 15
#pragma define INT_NO_5 16
#pragma define INT_NO_6 17
#pragma define INT_NO_7 18
#pragma define INT_NO_8 19
#pragma define INT_NO_TA0 20
#pragma define INT_NO_TA1 21
#pragma define INT_NO_TA2 22
#pragma define INT_NO_TA3 23
#pragma define INT_NO_TA4 24
#pragma define INT_NO_TA5 25
#pragma define INT_NO_TA6 26
#pragma define INT_NO_TA7 27
#pragma define INT_NO_TB00 28
#pragma define INT_NO_TB01 29
#pragma define INT_NO_TB10 30
#pragma define INT_NO_TB11 31
#pragma define INT_NO_TBOF0 32
#pragma define INT_NO_TBOF1 33
#pragma define INT_NO_RX0 34
#pragma define INT_NO_TX0 35
#pragma define INT_NO_RX1 36
#pragma define INT_NO_TX1 37
#pragma define INT_NO_SBI 38
#pragma define INT_NO_RTC 39
#pragma define INT_NO_AD 40
#pragma define INT_NO_TC0 41
#pragma define INT_NO_TC1 42
#pragma define INT_NO_TC2 43
#pragma define INT_NO_TC3 44



 


 
#pragma define TADR_SFR_DMA0V 0x0080
#pragma define TADR_SFR_DMA1V 0x0081
#pragma define TADR_SFR_DMA2V 0x0082
#pragma define TADR_SFR_DMA3V 0x0083
#pragma define TADR_SFR_INTCLR 0x0088
#pragma define TADR_SFR_DMAR 0x0089
#pragma define TADR_SFR_DMAB 0x008a
#pragma define TADR_SFR_IIMC 0x008c
#pragma define TADR_SFR_INTE0AD 0x0090
#pragma define TADR_SFR_INTE12 0x0091
#pragma define TADR_SFR_INTE34 0x0092
#pragma define TADR_SFR_INTE56 0x0093
#pragma define TADR_SFR_INTE78 0x0094
#pragma define TADR_SFR_INTETA01 0x0095
#pragma define TADR_SFR_INTETA23 0x0096
#pragma define TADR_SFR_INTETA45 0x0097
#pragma define TADR_SFR_INTETA67 0x0098
#pragma define TADR_SFR_INTETB0 0x0099
#pragma define TADR_SFR_INTETB1 0x009a
#pragma define TADR_SFR_INTETB01V 0x009b
#pragma define TADR_SFR_INTES0 0x009c
#pragma define TADR_SFR_INTES1 0x009d
#pragma define TADR_SFR_INTSBIRTC 0x009e
#pragma define TADR_SFR_INTETC01 0x00a0
#pragma define TADR_SFR_INTETC23 0x00a1



 
#pragma define TADR_SFR_TA01RUN 0x0100
#pragma define TADR_SFR_TA0REG 0x0102
#pragma define TADR_SFR_TA1REG 0x0103
#pragma define TADR_SFR_TA01MOD 0x0104
#pragma define TADR_SFR_TA1FFCR 0x0105
#pragma define TADR_SFR_TA23RUN 0x0108
#pragma define TADR_SFR_TA2REG 0x010a
#pragma define TADR_SFR_TA3REG 0x010b
#pragma define TADR_SFR_TA23MOD 0x010c
#pragma define TADR_SFR_TA3FFCR 0x010d
#pragma define TADR_SFR_TA45RUN 0x0110
#pragma define TADR_SFR_TA4REG 0x0112
#pragma define TADR_SFR_TA5REG 0x0113
#pragma define TADR_SFR_TA45MOD 0x0114
#pragma define TADR_SFR_TA5FFCR 0x0115
#pragma define TADR_SFR_TA67RUN 0x0118
#pragma define TADR_SFR_TA6REG 0x011a
#pragma define TADR_SFR_TA7REG 0x011b
#pragma define TADR_SFR_TA67MOD 0x011c
#pragma define TADR_SFR_TA7FFCR 0x011d



 
#pragma define TADR_SFR_SC0BUF 0x0200
#pragma define TADR_SFR_SC0CR 0x0201
#pragma define TADR_SFR_SC0MOD0 0x0202
#pragma define TADR_SFR_BR0CR 0x0203
#pragma define TADR_SFR_BR0ADD 0x0204
#pragma define TADR_SFR_SC0MOD1 0x0205
#pragma define TADR_SFR_SIRCR 0x0207
#pragma define TADR_SFR_SC1BUF 0x0208
#pragma define TADR_SFR_SC1CR 0x0209
#pragma define TADR_SFR_SC1MOD0 0x020a
#pragma define TADR_SFR_BR1CR 0x020b
#pragma define TADR_SFR_BR1ADD 0x020c
#pragma define TADR_SFR_SC1MOD1 0x020d
#pragma define TADR_SFR_SBI0CR1 0x0240
#pragma define TADR_SFR_SBI0DBR 0x0241
#pragma define TADR_SFR_I2C0AR 0x0242
#pragma define TADR_SFR_SBI0CR2 0x0243
#pragma define TADR_SFR_SBI0BR0 0x0244
#pragma define TADR_SFR_SBI0BR1 0x0245



 


 
 
#pragma define TBIT_INTM_L 0x07
#pragma define TBIT_INTC_L 0x08
#pragma define TBIT_INTM_H 0x70
#pragma define TBIT_INTC_H 0x80
 
 
#pragma define TBIT_INT0_CLR 0x0a
#pragma define TBIT_INT1_CLR 0x0b
#pragma define TBIT_INT2_CLR 0x0c
#pragma define TBIT_INT3_CLR 0x0d
#pragma define TBIT_INT4_CLR 0x0e
#pragma define TBIT_INT5_CLR 0x0f
#pragma define TBIT_INT6_CLR 0x10
#pragma define TBIT_INT7_CLR 0x11
#pragma define TBIT_INT8_CLR 0x12
#pragma define TBIT_TA0_CLR 0x13
#pragma define TBIT_TA1_CLR 0x14
#pragma define TBIT_TA2_CLR 0x15
#pragma define TBIT_TA3_CLR 0x16
#pragma define TBIT_TA4_CLR 0x17
#pragma define TBIT_TA5_CLR 0x18
#pragma define TBIT_TA6_CLR 0x19
#pragma define TBIT_TA7_CLR 0x1a
#pragma define TBIT_TB00_CLR 0x1b
#pragma define TBIT_TB01_CLR 0x1c
#pragma define TBIT_TB10_CLR 0x1d
#pragma define TBIT_TB11_CLR 0x1e
#pragma define TBIT_TBOF0_CLR 0x1f
#pragma define TBIT_TBOF1_CLR 0x20
#pragma define TBIT_RX0_CLR 0x21
#pragma define TBIT_TX0_CLR 0x22
#pragma define TBIT_RX1_CLR 0x23
#pragma define TBIT_TX1_CLR 0x24
#pragma define TBIT_SBI_CLR 0x25
#pragma define TBIT_RTC_CLR 0x26
#pragma define TBIT_AD_CLR 0x27



 
 
#pragma define TBIT_TA0RUN 0x01
#pragma define TBIT_TA1RUN 0x02
#pragma define TBIT_TA01PRUN 0x04
#pragma define TBIT_I2TA01 0x08
#pragma define TBIT_TA0RDE 0x80
 
#pragma define TBIT_TA0CLK 0x03
#pragma define TBIT_TA0CLK0 0x00
#pragma define TBIT_TA0CLK1 0x01
#pragma define TBIT_TA0CLK4 0x02
#pragma define TBIT_TA0CLK16 0x03
#pragma define TBIT_TA1CLK 0x0c
#pragma define TBIT_TA1CLK0 0x00
#pragma define TBIT_TA1CLK1 0x04
#pragma define TBIT_TA1CLK16 0x08
#pragma define TBIT_TA1CLK256 0x0c
#pragma define TBIT_PWM 0x30
#pragma define TBIT_PWM26 0x10
#pragma define TBIT_PWM27 0x20
#pragma define TBIT_PWM28 0x30
#pragma define TBIT_TA01M 0xc0
#pragma define TBIT_TA01M8 0x00
#pragma define TBIT_TA01M16 0x40
#pragma define TBIT_TA01MPPG 0x80
#pragma define TBIT_TA01MPWM 0xc0



 
 
#pragma define TBIT_SIOSC 0x03
#pragma define TBIT_SIOSCT 0x00
#pragma define TBIT_SIOSCBRG 0x01
#pragma define TBIT_SIOSCFSYS 0x02
#pragma define TBIT_SIOSCSCLK 0x03
#pragma define TBIT_SIOSM 0x0c
#pragma define TBIT_SIOSMIO 0x00
#pragma define TBIT_SIOSMU7 0x04
#pragma define TBIT_SIOSMU8 0x08
#pragma define TBIT_SIOSMU9 0x0c
#pragma define TBIT_SIOWU 0x10
#pragma define TBIT_SIORXE 0x20
#pragma define TBIT_SIOCTSE 0x40
#pragma define TBIT_SIOTB8 0x80
 
#pragma define TBIT_SIOIOC 0x01
#pragma define TBIT_SIOSCLKS 0x02
#pragma define TBIT_SIOFERR 0x04
#pragma define TBIT_SIOPERR 0x08
#pragma define TBIT_SIOOERR 0x10
#pragma define TBIT_SIOPE 0x20
#pragma define TBIT_SIOEVEN 0x40
#pragma define TBIT_SIORB8 0x80
 
#pragma define TBIT_SIOBRS 0x0f
#pragma define TBIT_SIOBRCK 0x30
#pragma define TBIT_SIOBRCK0 0x00
#pragma define TBIT_SIOBRCK2 0x10
#pragma define TBIT_SIOBRCK8 0x20
#pragma define TBIT_SIOBRCK32 0x30
#pragma define TBIT_SIOBRADD 0x40
#pragma define TBIT_SIOBRK 0x0f
 
#pragma define TBIT_SIOFDPX 0x40
#pragma define TBIT_SIOI2S 0x80






 
#pragma define INHNO_TIMER INT_NO_TA0



 
#pragma define INT_LEVEL_TIMER 0x05





 
typedef UH	CLOCK;





 
#pragma define TIMER_CLOCK 125



 
#pragma define MAX_CLOCK ((CLOCK) 0xff)



 
#pragma define GET_TOLERANCE 100





 
#pragma inline hw_timer_initialize

void
hw_timer_initialize()
{
	 
	sil_wrb_mem((VP)0x0100,
		(sil_reb_mem((VP)0x0100) & ~0x01));
	 
	sil_wrb_mem((VP)0x0100,
		(sil_reb_mem((VP)0x0100) | 0x08));
	sil_wrb_mem((VP)0x0104,
		(sil_reb_mem((VP)0x0104) | 0x03));
	sil_wrb_mem((VP)0x0100, 125 );
	 
	sil_wrb_mem((VP)0x0088, 0x13 );
	sil_wrb_mem((VP)0x0095,
		((sil_reb_mem((VP)0x0095) & ~0x07) | 0x05));
	 
	sil_wrb_mem((VP)0x0100,
		(sil_reb_mem((VP)0x0100) | (0x04 | 0x01)));
}



 
#pragma inline hw_timer_int_clear

void
hw_timer_int_clear()
{
	 
	sil_wrb_mem((VP)0x0088, 0x13 );
}





 
#pragma inline hw_timer_terminate

void
hw_timer_terminate()
{
	 
	sil_wrb_mem((VP)0x0100,
		(sil_reb_mem((VP)0x0100) & ~0x01));
}



 
#pragma inline hw_timer_get_current

CLOCK
hw_timer_get_current(void)
{
	return 0;
}



 
#pragma inline hw_timer_fetch_interrupt

BOOL
hw_timer_fetch_interrupt(void)
{
	return (sil_reb_mem((VP)0x0095) & 0x08);
}








































 






 








 
extern void	timer_initialize(VP_INT exinf);



 
extern void	timer_handler(void);





 
extern void	timer_terminate(VP_INT exinf);






































 



 









































 


















 










 




 












































 
















 










 




 




 





 




 







 










 




 













 




 





	





	






 





	





	












	

	
	








	













	

	
	








	








 




	





	












	

	
	








	













	

	
	








	

















































 




 






 















































 


 




























 























 






















 


 
 




 
 

































 
 





 






















 
 














 








 








 









































 




 






 
#pragma define BRS_19200 13
#pragma define BRK_19200 0
#pragma define BRS_38400 6
#pragma define BRK_38400 8
#pragma define INT_LEVEL_UART 6



 
#pragma define INHNO_SERIAL_IN1 INT_NO_RX0
#pragma define INHNO_SERIAL_OUT1 INT_NO_TX0
#pragma define INHNO_SERIAL_IN2 INT_NO_RX1
#pragma define INHNO_SERIAL_OUT2 INT_NO_TX1





 
typedef struct sio_port_control_block	SIOPCB;



 
#pragma define UART_ERDY_SND 1u
#pragma define UART_ERDY_RCV 2u



 
extern void	uart_initialize(void);



 
extern SIOPCB * uart_opn_por(ID siopid, VP_INT exinf);



 
extern void	uart_cls_por(SIOPCB *siopcb);



 
extern BOOL	uart_snd_chr(SIOPCB *siopcb, char c);



 
extern INT	uart_rcv_chr(SIOPCB *siopcb);



 
extern void	uart_ena_cbr(SIOPCB *siopcb, UINT cbrtn);



 
extern void	uart_dis_cbr(SIOPCB *siopcb, UINT cbrtn);



 
extern void	uart_ierdy_snd(VP_INT exinf);



 
extern void	uart_ierdy_rcv(VP_INT exinf);







 
#pragma define SIO_ERDY_SND UART_ERDY_SND
#pragma define SIO_ERDY_RCV UART_ERDY_RCV





 
#pragma define sio_initialize uart_initialize



 
#pragma define sio_opn_por uart_opn_por



 
#pragma define sio_cls_por uart_cls_por



 
#pragma define sio_snd_chr uart_snd_chr



 
#pragma define sio_rcv_chr uart_rcv_chr



 
#pragma define sio_ena_cbr uart_ena_cbr



 
#pragma define sio_dis_cbr uart_dis_cbr



 
#pragma define sio_ierdy_snd uart_ierdy_snd



 
#pragma define sio_ierdy_rcv uart_ierdy_rcv








































 



 






 

		
		
	



 




 











 













































 



 









































 
















 










 




 




 










  

#pragma define LOGTASK_PRIORITY 3



#pragma define LOGTASK_STACK_SIZE 1024



#pragma define LOGTASK_INTERVAL 10




 
extern void	logtask(VP_INT exinf);




	 

#pragma define TNUM_TSKID 5

const ID _kernel_tmax_tskid = (1 + 5 - 1);

static __STK_UNIT __stack_TASK1[(((128) + sizeof(__STK_UNIT) - 1) / sizeof(__STK_UNIT))];
static __STK_UNIT __stack_TASK2[(((128) + sizeof(__STK_UNIT) - 1) / sizeof(__STK_UNIT))];
static __STK_UNIT __stack_TASK3[(((128) + sizeof(__STK_UNIT) - 1) / sizeof(__STK_UNIT))];
static __STK_UNIT __stack_MAIN_TASK[(((128) + sizeof(__STK_UNIT) - 1) / sizeof(__STK_UNIT))];
static __STK_UNIT __stack_LOGTASK[(((1024) + sizeof(__STK_UNIT) - 1) / sizeof(__STK_UNIT))];

const TINIB _kernel_tinib_table[5] = {
	{0, (VP_INT)(( VP_INT ) 1), (FP)(task), ((UINT)((10) - 1)), (((128) + sizeof(__STK_UNIT) - 1) & ~((SIZE)sizeof(__STK_UNIT) - 1)), __stack_TASK1, 0, (FP)(tex_routine)},
	{0, (VP_INT)(( VP_INT ) 2), (FP)(task), ((UINT)((10) - 1)), (((128) + sizeof(__STK_UNIT) - 1) & ~((SIZE)sizeof(__STK_UNIT) - 1)), __stack_TASK2, 0, (FP)(tex_routine)},
	{0, (VP_INT)(( VP_INT ) 3), (FP)(task), ((UINT)((10) - 1)), (((128) + sizeof(__STK_UNIT) - 1) & ~((SIZE)sizeof(__STK_UNIT) - 1)), __stack_TASK3, 0, (FP)(tex_routine)},
	{0x00u | 0x02u, (VP_INT)(0), (FP)(main_task), ((UINT)((5) - 1)), (((128) + sizeof(__STK_UNIT) - 1) & ~((SIZE)sizeof(__STK_UNIT) - 1)), __stack_MAIN_TASK, 0u, (FP)(((void *)0))},
	{0x00u | 0x02u, (VP_INT)(( VP_INT ) 1), (FP)(logtask), ((UINT)((3) - 1)), (((1024) + sizeof(__STK_UNIT) - 1) & ~((SIZE)sizeof(__STK_UNIT) - 1)), __stack_LOGTASK, 0u, (FP)(((void *)0))}
};

const ID _kernel_torder_table[5] = {1,2,3,4,5};

TCB _kernel_tcb_table[5];


	 

#pragma define TNUM_SEMID 2

const ID _kernel_tmax_semid = (1 + 2 - 1);

const SEMINIB _kernel_seminib_table[2] = {
	{1, 0, 1},
	{1, 1, 1}
};

SEMCB _kernel_semcb_table[2];


	 

#pragma define TNUM_FLGID 0

const ID _kernel_tmax_flgid = (1 + 0 - 1);

const FLGINIB _kernel_flginib_table[];
FLGCB _kernel_flgcb_table[];


	 

#pragma define TNUM_DTQID 0

const ID _kernel_tmax_dtqid = (1 + 0 - 1);

const DTQINIB _kernel_dtqinib_table[];
DTQCB _kernel_dtqcb_table[];


	 

#pragma define TNUM_MBXID 0

const ID _kernel_tmax_mbxid = (1 + 0 - 1);

const MBXINIB _kernel_mbxinib_table[];
MBXCB _kernel_mbxcb_table[];


	 

#pragma define TNUM_MPFID 0

const ID _kernel_tmax_mpfid = (1 + 0 - 1);

const MPFINIB _kernel_mpfinib_table[];
MPFCB _kernel_mpfcb_table[];


	 

#pragma define TNUM_CYCID 1

const ID _kernel_tmax_cycid = (1 + 1 - 1);

const CYCINIB _kernel_cycinib_table[1] = {
	{0,0,(FP)(cyclic_handler),2000,0}
};

CYCCB _kernel_cyccb_table[1];


	 

#pragma define TNUM_INHNO 3

const UINT _kernel_tnum_inhno = 3;

extern void _kernel_interrupt(void); extern void timer_handler(void); void timer_handler_entry(void) { __ASM("	push	xwa	"); __ASM("	push	sr"); __ASM("	pop		wa"); __ASM("	ei		7"); __ASM("	push	xhl	"); __ASM("	ld		xhl, _" "timer_handler" ); __ASM("	jp	__kernel_interrupt" ); };
extern void _kernel_interrupt(void); extern void serial_in_handler1(void); void serial_in_handler1_entry(void) { __ASM("	push	xwa	"); __ASM("	push	sr"); __ASM("	pop		wa"); __ASM("	ei		7"); __ASM("	push	xhl	"); __ASM("	ld		xhl, _" "serial_in_handler1" ); __ASM("	jp	__kernel_interrupt" ); };
extern void _kernel_interrupt(void); extern void serial_out_handler1(void); void serial_out_handler1_entry(void) { __ASM("	push	xwa	"); __ASM("	push	sr"); __ASM("	pop		wa"); __ASM("	ei		7"); __ASM("	push	xhl	"); __ASM("	ld		xhl, _" "serial_out_handler1" ); __ASM("	jp	__kernel_interrupt" ); };

const INHINIB _kernel_inhinib_table[3] = {
	{20,0,(FP)timer_handler_entry},
	{34,0,(FP)serial_in_handler1_entry},
	{35,0,(FP)serial_out_handler1_entry}
};


	 

#pragma define TNUM_EXCNO 1

const UINT _kernel_tnum_excno = 1;

extern void _kernel_interrupt(void); extern void cpuexc_handler(VP p_excinf); void cpuexc_handler_entry(void) { __ASM("	push	xwa	"); __ASM("	push	sr"); __ASM("	pop		wa"); __ASM("	ei		7"); __ASM("	push	xhl	"); __ASM("	ld		xhl, _" "cpuexc_handler" ); __ASM("	jp	__kernel_interrupt" ); };

const EXCINIB _kernel_excinib_table[1] = {
	{3,0,(FP)cpuexc_handler_entry}
};

	 

void
_kernel_call_inirtn(void)
{
	timer_initialize( (VP_INT)(0) );
	serial_initialize( (VP_INT)(0) );
}

void
_kernel_call_terrtn(void)
{
	timer_terminate( (VP_INT)(0) );
}

	 

void
_kernel_object_initialize(void)
{
	_kernel_task_initialize();
	_kernel_semaphore_initialize();
	_kernel_cyclic_initialize();
	_kernel_interrupt_initialize();
	_kernel_exception_initialize();
}

TMEVTN   _kernel_tmevt_heap[5 + 1];

