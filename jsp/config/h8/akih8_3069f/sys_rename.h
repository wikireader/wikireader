/* This file is generated from sys_rename.def by genrename. */

#ifndef _SYS_RENAME_H_
#define _SYS_RENAME_H_

/*
 * sys_config.c
 */
#define load_vector		_kernel_load_vector
#define save_vector		_kernel_save_vector

/*
 * sys_config.c(cpu_config.c)
 */
#define cpu_putc		_kernel_cpu_putc


#ifdef LABEL_ASM

/*
 * sys_config.c
 */
#define _load_vector		__kernel_load_vector
#define _save_vector		__kernel_save_vector

/*
 * sys_config.c(cpu_config.c)
 */
#define _cpu_putc		__kernel_cpu_putc


#endif /* LABEL_ASM */
#endif /* _SYS_RENAME_H_ */
