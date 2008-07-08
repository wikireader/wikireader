/* This file is generated from sys_rename.def by genrename. */

#ifndef _SYS_RENAME_H_
#define _SYS_RENAME_H_

/*
 * sys_config.c
 */
#define ipm_table		_kernel_ipm_table
#define no_reg_CPM_interrupt	_kernel_no_reg_CPM_interrupt
#define no_reg_SIU_interrupt	_kernel_no_reg_SIU_interrupt
#define sil_debug_on		_kernel_sil_debug_on
#define sil_buffer		_kernel_sil_buffer
#define sil_pos			_kernel_sil_pos
#define sil_log_id		_kernel_sil_log_id

#ifdef LABEL_ASM

/*
 * sys_config.c
 */
#define _ipm_table		__kernel_ipm_table
#define _no_reg_CPM_interrupt	__kernel_no_reg_CPM_interrupt
#define _no_reg_SIU_interrupt	__kernel_no_reg_SIU_interrupt
#define _sil_debug_on		__kernel_sil_debug_on
#define _sil_buffer		__kernel_sil_buffer
#define _sil_pos		__kernel_sil_pos
#define _sil_log_id		__kernel_sil_log_id

#endif /* LABEL_ASM */
#endif /* _SYS_RENAME_H_ */
