/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#define activate_r		_kernel_activate_r
#define ret_int			_kernel_ret_int
#define ret_exc			_kernel_ret_exc
#define interrupt_count		_kernel_interrupt_count
#define int_handler_table	_kernel_int_handler_table
#define exc_handler_table	_kernel_exc_handler_table
#define _irq_entry		_kernel__irq_entry

#ifdef LABEL_ASM

#define _activate_r		__kernel_activate_r
#define _ret_int		__kernel_ret_int
#define _ret_exc		__kernel_ret_exc
#define _interrupt_count	__kernel_interrupt_count
#define _int_handler_table	__kernel_int_handler_table
#define _exc_handler_table	__kernel_exc_handler_table
#define __irq_entry		__kernel__irq_entry

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
