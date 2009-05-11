/* This file is generated from jsp_rename.def by genrename. */

#ifdef _JSP_UNRENAME_H_
#undef _JSP_UNRENAME_H_

/*
 *  startup.c
 */
#undef iniflg

/*
 *  banner.c
 */
#undef print_banner

/*
 *  cpu_config.c, cpu_support.S, sys_config.c, sys_support.S
 */
#undef dispatch
#undef exit_and_dispatch
#undef cpu_initialize
#undef cpu_terminate
#undef sys_initialize
#undef sys_exit
#undef sys_putc

/*
 *  task.c
 */
#undef runtsk
#undef schedtsk
#undef reqflg
#undef enadsp
#undef ready_queue
#undef ready_primap
#undef task_initialize
#undef search_schedtsk
#undef make_runnable
#undef make_non_runnable
#undef make_dormant
#undef make_active
#undef exit_task
#undef change_priority
#undef rotate_ready_queue
#undef call_texrtn
#undef calltex

/*
 *  wait.c
 */
#undef make_wait_tmout
#undef wait_complete
#undef wait_tmout
#undef wait_tmout_ok
#undef wait_cancel
#undef wait_release
#undef wobj_make_wait
#undef wobj_make_wait_tmout
#undef wobj_change_priority

/*
 *  time_event.c
 */
#undef systim_offset
#undef current_time
#undef next_time
#undef next_subtime
#undef last_index
#undef tmevt_initialize
#undef tmevt_up
#undef tmevt_down
#undef tmevtb_insert
#undef tmevtb_delete

/*
 *  syslog.c
 */
#undef syslog_buffer
#undef syslog_count
#undef syslog_head
#undef syslog_tail
#undef syslog_lost
#undef syslog_logmask
#undef syslog_lowmask
#undef syslog_initialize
#undef syslog_terminate

/*
 *  semaphore.c
 */
#undef semaphore_initialize

/*
 *  eventflag.c
 */
#undef eventflag_initialize
#undef eventflag_cond

/*
 *  dataqueue.c
 */
#undef dataqueue_initialize
#undef enqueue_data
#undef force_enqueue_data
#undef dequeue_data
#undef send_data_rwait
#undef receive_data_swait

/*
 *  mailbox.c
 */
#undef mailbox_initialize

/*
 *  mempfix.c
 */
#undef mempfix_initialize
#undef mempfix_get_block

/*
 *  cyclic.c
 */
#undef cyclic_initialize
#undef tmevtb_enqueue_cyc
#undef call_cychdr

/*
 *  interrupt.c
 */
#undef interrupt_initialize

/*
 *  exception.c
 */
#undef exception_initialize

/*
 *  kernel_cfg.c
 */
#undef object_initialize
#undef call_inirtn
#undef call_terrtn
#undef tmax_tskid
#undef tinib_table
#undef torder_table
#undef tcb_table
#undef tmax_semid
#undef seminib_table
#undef semcb_table
#undef tmax_flgid
#undef flginib_table
#undef flgcb_table
#undef tmax_dtqid
#undef dtqcb_table
#undef dtqinib_table
#undef tmax_mbxid
#undef mbxcb_table
#undef mbxinib_table
#undef tmax_mpfid
#undef mpfinib_table
#undef mpfcb_table
#undef tmax_cycid
#undef cycinib_table
#undef cyccb_table
#undef tnum_inhno
#undef inhinib_table
#undef tnum_excno
#undef excinib_table
#undef tmevt_heap

#ifdef LABEL_ASM

/*
 *  startup.c
 */
#undef _iniflg

/*
 *  banner.c
 */
#undef _print_banner

/*
 *  cpu_config.c, cpu_support.S, sys_config.c, sys_support.S
 */
#undef _dispatch
#undef _exit_and_dispatch
#undef _cpu_initialize
#undef _cpu_terminate
#undef _sys_initialize
#undef _sys_exit
#undef _sys_putc

/*
 *  task.c
 */
#undef _runtsk
#undef _schedtsk
#undef _reqflg
#undef _enadsp
#undef _ready_queue
#undef _ready_primap
#undef _task_initialize
#undef _search_schedtsk
#undef _make_runnable
#undef _make_non_runnable
#undef _make_dormant
#undef _make_active
#undef _exit_task
#undef _change_priority
#undef _rotate_ready_queue
#undef _call_texrtn
#undef _calltex

/*
 *  wait.c
 */
#undef _make_wait_tmout
#undef _wait_complete
#undef _wait_tmout
#undef _wait_tmout_ok
#undef _wait_cancel
#undef _wait_release
#undef _wobj_make_wait
#undef _wobj_make_wait_tmout
#undef _wobj_change_priority

/*
 *  time_event.c
 */
#undef _systim_offset
#undef _current_time
#undef _next_time
#undef _next_subtime
#undef _last_index
#undef _tmevt_initialize
#undef _tmevt_up
#undef _tmevt_down
#undef _tmevtb_insert
#undef _tmevtb_delete

/*
 *  syslog.c
 */
#undef _syslog_buffer
#undef _syslog_count
#undef _syslog_head
#undef _syslog_tail
#undef _syslog_lost
#undef _syslog_logmask
#undef _syslog_lowmask
#undef _syslog_initialize
#undef _syslog_terminate

/*
 *  semaphore.c
 */
#undef _semaphore_initialize

/*
 *  eventflag.c
 */
#undef _eventflag_initialize
#undef _eventflag_cond

/*
 *  dataqueue.c
 */
#undef _dataqueue_initialize
#undef _enqueue_data
#undef _force_enqueue_data
#undef _dequeue_data
#undef _send_data_rwait
#undef _receive_data_swait

/*
 *  mailbox.c
 */
#undef _mailbox_initialize

/*
 *  mempfix.c
 */
#undef _mempfix_initialize
#undef _mempfix_get_block

/*
 *  cyclic.c
 */
#undef _cyclic_initialize
#undef _tmevtb_enqueue_cyc
#undef _call_cychdr

/*
 *  interrupt.c
 */
#undef _interrupt_initialize

/*
 *  exception.c
 */
#undef _exception_initialize

/*
 *  kernel_cfg.c
 */
#undef _object_initialize
#undef _call_inirtn
#undef _call_terrtn
#undef _tmax_tskid
#undef _tinib_table
#undef _torder_table
#undef _tcb_table
#undef _tmax_semid
#undef _seminib_table
#undef _semcb_table
#undef _tmax_flgid
#undef _flginib_table
#undef _flgcb_table
#undef _tmax_dtqid
#undef _dtqcb_table
#undef _dtqinib_table
#undef _tmax_mbxid
#undef _mbxcb_table
#undef _mbxinib_table
#undef _tmax_mpfid
#undef _mpfinib_table
#undef _mpfcb_table
#undef _tmax_cycid
#undef _cycinib_table
#undef _cyccb_table
#undef _tnum_inhno
#undef _inhinib_table
#undef _tnum_excno
#undef _excinib_table
#undef _tmevt_heap

#endif /* LABEL_ASM */
#endif /* _JSP_UNRENAME_H_ */
