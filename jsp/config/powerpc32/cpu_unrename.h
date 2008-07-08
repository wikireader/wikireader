/* This file is generated from cpu_rename.def by genrename. */

#ifdef _CPU_UNRENAME_H_
#undef _CPU_UNRENAME_H_

/*
 * cpu_support.S
 */
#undef activate_r
#undef ret_int
#undef ret_exc
#undef Exception_Entry
#undef External_interrupt_1

/*
 * cpu_config.c
 */
#undef int_table
#undef exc_table
#undef exe_list
#undef no_reg_exception
#undef syslog_data_on_stack

/*
 * cpu_config.h
 */
#undef define_inh
#undef define_exc

#ifdef LABEL_ASM

/*
 * cpu_support.S
 */
#undef _activate_r
#undef _ret_int
#undef _ret_exc
#undef _Exception_Entry
#undef _External_interrupt_1

/*
 * cpu_config.c
 */
#undef _int_table
#undef _exc_table
#undef _exe_list
#undef _no_reg_exception
#undef _syslog_data_on_stack

/*
 * cpu_config.h
 */
#undef _define_inh
#undef _define_exc

#endif /* LABEL_ASM */
#endif /* _CPU_UNRENAME_H_ */
