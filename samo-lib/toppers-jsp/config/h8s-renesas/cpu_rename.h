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

/*
 * cpu_support.src
 */
#define activate_r		_kernel_activate_r
#define no_reg_exception	_kernel_no_reg_exception

/*
 * start.src
 */
#define start			_kernel_start

/*
 * h8s_sci.c
 */
#define h8s_sci_initialize	_kernel_h8s_sci_initialize
#define h8s_sci_openflag	_kernel_h8s_sci_openflag
#define h8s_sci_opn_por		_kernel_h8s_sci_opn_por
#define h8s_sci_cls_por		_kernel_h8s_sci_cls_por
#define h8s_sci_snd_chr		_kernel_h8s_sci_snd_chr
#define h8s_sci_rcv_chr		_kernel_h8s_sci_rcv_chr
#define h8s_sci_ena_cbr		_kernel_h8s_sci_ena_cbr
#define h8s_sci_dis_cbr		_kernel_h8s_sci_dis_cbr
#define h8s_sci_ierdy_snd	_kernel_h8s_sci_ierdy_snd
#define h8s_sci_ierdy_rcv	_kernel_h8s_sci_ierdy_rcv
#define h8s_sci_init		_kernel_h8s_sci_init
#define h8s_sci_putchar_pol	_kernel_h8s_sci_putchar_pol


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

/*
 * cpu_support.src
 */
#define _activate_r		__kernel_activate_r
#define _no_reg_exception	__kernel_no_reg_exception

/*
 * start.src
 */
#define _start			__kernel_start

/*
 * h8s_sci.c
 */
#define _h8s_sci_initialize	__kernel_h8s_sci_initialize
#define _h8s_sci_openflag	__kernel_h8s_sci_openflag
#define _h8s_sci_opn_por	__kernel_h8s_sci_opn_por
#define _h8s_sci_cls_por	__kernel_h8s_sci_cls_por
#define _h8s_sci_snd_chr	__kernel_h8s_sci_snd_chr
#define _h8s_sci_rcv_chr	__kernel_h8s_sci_rcv_chr
#define _h8s_sci_ena_cbr	__kernel_h8s_sci_ena_cbr
#define _h8s_sci_dis_cbr	__kernel_h8s_sci_dis_cbr
#define _h8s_sci_ierdy_snd	__kernel_h8s_sci_ierdy_snd
#define _h8s_sci_ierdy_rcv	__kernel_h8s_sci_ierdy_rcv
#define _h8s_sci_init		__kernel_h8s_sci_init
#define _h8s_sci_putchar_pol	__kernel_h8s_sci_putchar_pol


#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
