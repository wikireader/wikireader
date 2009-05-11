/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

/*
 * cpu_support.S
 */
#define activate_r		_kernel_activate_r
#define interrupt_entry		_kernel_interrupt_entry
#define cpu_exception_entry	_kernel_cpu_exception_entry
#define no_reg_exception	_kernel_no_reg_exception

/*
 * cpu_config.c
 */
#define task_intmask		_kernel_task_intmask
#define int_intmask		_kernel_int_intmask
#define intnest			_kernel_intnest
#define vector_table		_kernel_vector_table
#define cpu_experr		_kernel_cpu_experr
#define define_exc		_kernel_define_exc
#define check_cpu_exc_entry	_kernel_check_cpu_exc_entry

/*
 * start.S
 */
#define start			_kernel_start

#ifdef LABEL_ASM

/*
 * cpu_support.S
 */
#define _activate_r		__kernel_activate_r
#define _interrupt_entry	__kernel_interrupt_entry
#define _cpu_exception_entry	__kernel_cpu_exception_entry
#define _no_reg_exception	__kernel_no_reg_exception

/*
 * cpu_config.c
 */
#define _task_intmask		__kernel_task_intmask
#define _int_intmask		__kernel_int_intmask
#define _intnest		__kernel_intnest
#define _vector_table		__kernel_vector_table
#define _cpu_experr		__kernel_cpu_experr
#define _define_exc		__kernel_define_exc
#define _check_cpu_exc_entry	__kernel_check_cpu_exc_entry

/*
 * start.S
 */
#define _start			__kernel_start

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
