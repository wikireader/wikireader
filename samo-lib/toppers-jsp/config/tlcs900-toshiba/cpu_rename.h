/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#ifndef OMIT_RENAME

#define intcnt			_kernel_intcnt
#define task_intmask	_kernel_task_intmask
#define int_intmask		_kernel_int_intmask
#define activate_r		_kernel_activate_r
#define interrupt		_kernel_interrupt
#define task_intmask	_kernel_task_intmask
#define int_intmask		_kernel_int_intmask

#ifdef LABEL_ASM

#define _intcnt			__kernel_intcnt
#define _task_intmask	__kernel_task_intmask
#define _int_intmask	__kernel_int_intmask
#define _activate_r		__kernel_activate_r
#define _interrupt		__kernel_interrupt
#define _task_intmask	__kernel_task_intmask
#define _int_intmask	__kernel_int_intmask

#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _CPU_RENAME_H_ */
