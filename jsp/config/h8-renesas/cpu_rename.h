/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

/*
 * cpu_config.c
 */
#define intnest			_kernel_intnest
#define iscpulocked		_kernel_iscpulocked
#define task_intmask		_kernel_task_intmask
#define int_intmask		_kernel_int_intmask
#define cpu_experr		_kernel_cpu_experr
#define cpu_putc		_kernel_cpu_putc
#define TIMER_ICU		_kernel_TIMER_ICU

/*
 * cpu_support.src
 */
#define activate_r		_kernel_activate_r
#define no_reg_exception	_kernel_no_reg_exception

/*
 * start.src
 */
#define start			_kernel_start
#define start_1			_kernel_start_1

/*
 * hw_serial.c
 */
#define siopcb_table		_kernel_siopcb_table
#define SCI_cls_por		_kernel_SCI_cls_por
#define SCI_ierdy_rcv		_kernel_SCI_ierdy_rcv
#define SCI_ierdy_snd		_kernel_SCI_ierdy_snd
#define SCI_in_handler		_kernel_SCI_in_handler
#define SCI_initialize		_kernel_SCI_initialize
#define SCI_out_handler		_kernel_SCI_out_handler

#ifdef LABEL_ASM

/*
 * cpu_config.c
 */
#define _intnest		__kernel_intnest
#define _iscpulocked		__kernel_iscpulocked
#define _task_intmask		__kernel_task_intmask
#define _int_intmask		__kernel_int_intmask
#define _cpu_experr		__kernel_cpu_experr
#define _cpu_putc		__kernel_cpu_putc
#define _TIMER_ICU		__kernel_TIMER_ICU

/*
 * cpu_support.src
 */
#define _activate_r		__kernel_activate_r
#define _no_reg_exception	__kernel_no_reg_exception

/*
 * start.src
 */
#define _start			__kernel_start
#define _start_1		__kernel_start_1

/*
 * hw_serial.c
 */
#define _siopcb_table		__kernel_siopcb_table
#define _SCI_cls_por		__kernel_SCI_cls_por
#define _SCI_ierdy_rcv		__kernel_SCI_ierdy_rcv
#define _SCI_ierdy_snd		__kernel_SCI_ierdy_snd
#define _SCI_in_handler		__kernel_SCI_in_handler
#define _SCI_initialize		__kernel_SCI_initialize
#define _SCI_out_handler	__kernel_SCI_out_handler

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
