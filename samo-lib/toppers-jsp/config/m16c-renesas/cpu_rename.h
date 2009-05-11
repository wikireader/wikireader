/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#define activate_r		_kernel_activate_r
#define ret_int			_kernel_ret_int
#define interrupt		_kernel_interrupt
#define intnest			_kernel_intnest
#define break_wait		_kernel_break_wait

#ifdef LABEL_ASM

#define _activate_r		__kernel_activate_r
#define _ret_int		__kernel_ret_int
#define _interrupt		__kernel_interrupt
#define _intnest		__kernel_intnest
#define _break_wait		__kernel_break_wait

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
