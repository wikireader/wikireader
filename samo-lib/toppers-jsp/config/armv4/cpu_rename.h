/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#ifndef OMIT_RENAME

#define activate_r		_kernel_activate_r
#define ret_int			_kernel_ret_int
#define ret_exc			_kernel_ret_exc
#define interrupt_count		_kernel_interrupt_count
#define arm_vector_add		_kernel_arm_vector_add
#define arm_handler_add		_kernel_arm_handler_add
#define int_stack		_kernel_int_stack

#ifdef LABEL_ASM

#define _activate_r		__kernel_activate_r
#define _ret_int		__kernel_ret_int
#define _ret_exc		__kernel_ret_exc
#define _interrupt_count	__kernel_interrupt_count
#define _arm_vector_add		__kernel_arm_vector_add
#define _arm_handler_add	__kernel_arm_handler_add
#define _int_stack		__kernel_int_stack

#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _CPU_RENAME_H_ */
