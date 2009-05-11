/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#define activate_r		_kernel_activate_r
#define ret_int			_kernel_ret_int
#define int_table		_kernel_int_table
#define int_mask_table		_kernel_int_mask_table
#define nest			_kernel_nest

#ifdef LABEL_ASM

#define _activate_r		__kernel_activate_r
#define _ret_int		__kernel_ret_int
#define _int_table		__kernel_int_table
#define _int_mask_table		__kernel_int_mask_table
#define _nest			__kernel_nest

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
