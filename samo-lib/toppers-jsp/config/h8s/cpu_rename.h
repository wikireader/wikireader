/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#define activate_r		_kernel_activate_r
#define ret_main			_kernel_ret_main
#define ret_int			_kernel_ret_int
#define ret_exc			_kernel_ret_exc
#define task_intmask		_kernel_task_intmask
#define int_intmask		_kernel_int_intmask
#define intnest			_kernel_intnest

#define no_reg_exception	_kernel_no_reg_exception
#define cpu_putc		_kernel_cpu_putc

#ifdef LABEL_ASM

#define _activate_r		__kernel_activate_r
#define _ret_main		__kernel_ret_main
#define _ret_int		__kernel_ret_int
#define _ret_exc		__kernel_ret_exc
#define _task_intmask		__kernel_task_intmask
#define _int_intmask		__kernel_int_intmask
#define _intnest		__kernel_intnest

#define _no_reg_exception	__kernel_no_reg_exception
#define _cpu_putc		__kernel_cpu_putc

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
