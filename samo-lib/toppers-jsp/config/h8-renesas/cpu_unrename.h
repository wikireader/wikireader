/* This file is generated from cpu_rename.def by genrename. */

#ifdef _CPU_UNRENAME_H_
#undef _CPU_UNRENAME_H_

/*
 * cpu_config.c
 */
#undef intnest
#undef iscpulocked
#undef task_intmask
#undef int_intmask
#undef cpu_experr
#undef cpu_putc
#undef TIMER_ICU

/*
 * cpu_support.src
 */
#undef activate_r
#undef no_reg_exception

/*
 * start.src
 */
#undef start
#undef start_1

/*
 * hw_serial.c
 */
#undef siopcb_table
#undef SCI_cls_por
#undef SCI_ierdy_rcv
#undef SCI_ierdy_snd
#undef SCI_in_handler
#undef SCI_initialize
#undef SCI_out_handler

#ifdef LABEL_ASM

/*
 * cpu_config.c
 */
#undef _intnest
#undef _iscpulocked
#undef _task_intmask
#undef _int_intmask
#undef _cpu_experr
#undef _cpu_putc
#undef _TIMER_ICU

/*
 * cpu_support.src
 */
#undef _activate_r
#undef _no_reg_exception

/*
 * start.src
 */
#undef _start
#undef _start_1

/*
 * hw_serial.c
 */
#undef _siopcb_table
#undef _SCI_cls_por
#undef _SCI_ierdy_rcv
#undef _SCI_ierdy_snd
#undef _SCI_in_handler
#undef _SCI_initialize
#undef _SCI_out_handler

#endif /* LABEL_ASM */
#endif /* _CPU_UNRENAME_H_ */
