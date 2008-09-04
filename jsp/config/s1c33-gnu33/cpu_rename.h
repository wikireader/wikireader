/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

/*
 * cpu_config.c
 */
#define cpu_terminate		_kernel_cpu_terminate
#define activate_r		_kernel_activate_r
#define dispatch		_kernel_dispatch
#define exit_and_dispatch	_kernel_exit_and_dispatch
#define ret_int			_kernel_ret_int
#define ena_int			_kernel_ena_int
#define dis_int			_kernel_dis_int
#define clr_int			_kernel_clr_int
#define tps_IntNestCnt		_kernel_tps_IntNestCnt
#define tps_OrgIntLevel		_kernel_tps_OrgIntLevel
#define init_lib		_kernel_init_lib
#define init_sys		_kernel_init_sys

#ifdef LABEL_ASM

/*
 * cpu_config.c
 */
#define _cpu_terminate		__kernel_cpu_terminate
#define _activate_r		__kernel_activate_r
#define _dispatch		__kernel_dispatch
#define _exit_and_dispatch	__kernel_exit_and_dispatch
#define _ret_int		__kernel_ret_int
#define _ena_int		__kernel_ena_int
#define _dis_int		__kernel_dis_int
#define _clr_int		__kernel_clr_int
#define _tps_IntNestCnt		__kernel_tps_IntNestCnt
#define _tps_OrgIntLevel	__kernel_tps_OrgIntLevel
#define _init_lib		__kernel_init_lib
#define _init_sys		__kernel_init_sys

#endif /* LABEL_ASM */
#endif /* _CPU_RENAME_H_ */
