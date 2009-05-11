/* This file is generated from sys_rename.def by genrename. */

#ifndef _SYS_RENAME_H_
#define _SYS_RENAME_H_

/*
 * sys_config.h
 */
#define sys_initialize		_kernel_sys_initialize
#define sys_exit		_kernel_sys_exit
#define get_Offset		_kernel_get_Offset
#define get_BitFlag		_kernel_get_BitFlag

#ifdef LABEL_ASM

/*
 * sys_config.h
 */
#define _sys_initialize	__kernel_sys_initialize
#define _sys_exit		__kernel_sys_exit
#define _get_Offset		__kernel_get_Offset
#define _get_BitFlag		__kernel_get_BitFlag

#endif /* LABEL_ASM */
#endif /* _SYS_RENAME_H_ */
