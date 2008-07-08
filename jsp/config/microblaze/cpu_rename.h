#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#ifndef OMIT_RENAME

#define activate_r		_kernel_activate_r
#define ret_int			_kernel_ret_int
#define ret_exc			_kernel_ret_exc
#define interrupt_count		_kernel_interrupt_count
#define int_handler_table	_kernel_int_handler_table
#define int_bit_table		_kernel_int_bit_table
#define exception_entry		_kernel_exception_entry
#define interrupt_entry		_kernel_interrupt_entry


#ifdef LABEL_ASM

#define _activate_r		__kernel_activate_r
#define _ret_int		__kernel_ret_int
#define _ret_exc		__kernel_ret_exc
#define _interrupt_count	__kernel_interrupt_count
#define _int_handler_table	__kernel_int_handler_table
#define _int_bit_table		__kernel_int_bit_table
#define _exception_entry	__kernel_exception_entry
#define _interrupt_entry	__kernel_interrupt_entry


#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _CPU_RENAME_H_ */
