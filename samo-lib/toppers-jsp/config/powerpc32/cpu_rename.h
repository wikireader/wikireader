/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

/*
 * cpu_support.S
 */
#define activate_r		_kernel_activate_r
#define ret_int			_kernel_ret_int
#define ret_exc			_kernel_ret_exc
#define Exception_Entry		_kernel_Exception_Entry
#define External_interrupt_1	_kernel_External_interrupt_1

/*
 * cpu_config.c
 */
#define int_table		_kernel_int_table
#define exc_table		_kernel_exc_table
#define exe_list		_kernel_exe_list
#define no_reg_exception	_kernel_no_reg_exception
#define syslog_data_on_stack	_kernel_syslog_data_on_stack

/*
 * cpu_config.h
 */
#define define_inh		_kernel_define_inh
#define define_exc		_kernel_define_exc

#ifdef LABEL_ASM

/*
 * cpu_support.S
 */
#define _activate_r		__kernel_activate_r
#define _ret_int		__kernel_ret_int
#define _ret_exc		__kernel_ret_exc
#define _Exception_Entry	__kernel_Exception_Entry
#define _External_interrupt_1	__kernel_External_interrupt_1

/*
 * cpu_config.c
 */
#define _int_table		__kernel_int_table
#define _exc_table		__kernel_exc_table
#define _exe_list		__kernel_exe_list
#define _no_reg_exception	__kernel_no_reg_exception
#define _syslog_data_on_stack	__kernel_syslog_data_on_stack

/*
 * cpu_config.h
 */
#define _define_inh		__kernel_define_inh
#define _define_exc		__kernel_define_exc

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
