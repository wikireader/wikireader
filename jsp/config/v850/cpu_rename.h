/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#define task_intmask	_kernel_task_intmask

#define int_intmask		_kernel_int_intmask

#define intnest			_kernel_intnest

#define InterruptHandlerEntry	_kernel_InterruptHandlerEntry

#define cpu_experr		_kernel_cpu_experr


#ifdef LABEL_ASM

#define _task_intmask	__kernel_task_intmask

#define _int_intmask	__kernel_int_intmask

#define _intnest		__kernel_intnest

#define _InterruptHandlerEntry	__kernel_InterruptHandlerEntry

#define _cpu_experr		__kernel_cpu_experr


#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
