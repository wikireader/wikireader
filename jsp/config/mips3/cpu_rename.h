/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#define activate_r		_kernel_activate_r
#define ret_int			_kernel_ret_int
#define ret_exc			_kernel_ret_exc
#define int_intmask		_kernel_int_intmask
#define general_exception	_kernel_general_exception
#define cpu_experr		_kernel_cpu_experr
#define join_interrupt_and_exception	_kernel_join_interrupt_and_exception
#define exc_table		_kernel_exc_table
#define int_table		_kernel_int_table

#ifdef LABEL_ASM

#define _activate_r		__kernel_activate_r
#define _ret_int		__kernel_ret_int
#define _ret_exc		__kernel_ret_exc
#define _int_intmask		__kernel_int_intmask
#define _general_exception	__kernel_general_exception
#define _cpu_experr		__kernel_cpu_experr
#define _join_interrupt_and_exception	__kernel_join_interrupt_and_exception
#define _exc_table		__kernel_exc_table
#define _int_table		__kernel_int_table

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
