/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#define activate_r  		_kernel_activate_r  
#define ret_int     		_kernel_ret_int     
#define ret_exc     		_kernel_ret_exc     
#define task_intmask		_kernel_task_intmask
#define int_intmask 		_kernel_int_intmask 
#define int_table		_kernel_int_table
#define int_plevel_table	_kernel_int_plevel_table
#define exc_table		_kernel_exc_table
#define BASE_VBR		_kernel_BASE_VBR
#define general_exception	_kernel_general_exception
#define no_reg_interrupt	_kernel_no_reg_interrupt
#define cpu_expevt		_kernel_cpu_expevt
#define cpu_interrupt		_kernel_cpu_interrupt


#ifdef LABEL_ASM

#define _activate_r  		__kernel_activate_r  
#define _ret_int     		__kernel_ret_int     
#define _ret_exc     		__kernel_ret_exc     
#define _task_intmask		__kernel_task_intmask
#define _int_intmask 		__kernel_int_intmask 
#define _int_table		__kernel_int_table
#define _int_plevel_table	__kernel_int_plevel_table
#define _exc_table		__kernel_exc_table
#define _BASE_VBR		__kernel_BASE_VBR
#define _general_exception	__kernel_general_exception
#define _no_reg_interrupt	__kernel_no_reg_interrupt
#define _cpu_expevt		__kernel_cpu_expevt
#define _cpu_interrupt		__kernel_cpu_interrupt


#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
