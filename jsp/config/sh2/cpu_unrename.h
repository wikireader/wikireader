/* This file is generated from cpu_rename.def by genrename. */

#ifdef _CPU_UNRENAME_H_
#undef _CPU_UNRENAME_H_

/*
 * cpu_support.S
 */
#undef activate_r
#undef interrupt_entry
#undef cpu_exception_entry
#undef no_reg_exception

/*
 * cpu_config.c
 */
#undef task_intmask
#undef int_intmask
#undef intnest
#undef vector_table
#undef cpu_experr
#undef define_exc
#undef check_cpu_exc_entry

/*
 * start.S
 */
#undef start

#ifdef LABEL_ASM

/*
 * cpu_support.S
 */
#undef _activate_r
#undef _interrupt_entry
#undef _cpu_exception_entry
#undef _no_reg_exception

/*
 * cpu_config.c
 */
#undef _task_intmask
#undef _int_intmask
#undef _intnest
#undef _vector_table
#undef _cpu_experr
#undef _define_exc
#undef _check_cpu_exc_entry

/*
 * start.S
 */
#undef _start

#endif /* LABEL_ASM */
#endif /* _CPU_UNRENAME_H_ */
