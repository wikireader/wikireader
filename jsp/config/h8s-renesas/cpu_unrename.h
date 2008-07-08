/* This file is generated from cpu_rename.def by genrename. */

#ifdef _CPU_UNRENAME_H_
#undef _CPU_UNRENAME_H_

/*
 * cpu_config.c
 */
#undef intnest
#undef iscpulocked
#undef task_intmask
#undef int_intmask
#undef cpu_experr
#undef cpu_putc

/*
 * cpu_support.src
 */
#undef activate_r
#undef no_reg_exception

/*
 * start.src
 */
#undef start

/*
 * h8s_sci.c
 */
#undef h8s_sci_initialize
#undef h8s_sci_openflag
#undef h8s_sci_opn_por
#undef h8s_sci_cls_por
#undef h8s_sci_snd_chr
#undef h8s_sci_rcv_chr
#undef h8s_sci_ena_cbr
#undef h8s_sci_dis_cbr
#undef h8s_sci_ierdy_snd
#undef h8s_sci_ierdy_rcv
#undef h8s_sci_init
#undef h8s_sci_putchar_pol


#ifdef LABEL_ASM

/*
 * cpu_config.c
 */
#undef _intnest
#undef _iscpulocked
#undef _task_intmask
#undef _int_intmask
#undef _cpu_experr
#undef _cpu_putc

/*
 * cpu_support.src
 */
#undef _activate_r
#undef _no_reg_exception

/*
 * start.src
 */
#undef _start

/*
 * h8s_sci.c
 */
#undef _h8s_sci_initialize
#undef _h8s_sci_openflag
#undef _h8s_sci_opn_por
#undef _h8s_sci_cls_por
#undef _h8s_sci_snd_chr
#undef _h8s_sci_rcv_chr
#undef _h8s_sci_ena_cbr
#undef _h8s_sci_dis_cbr
#undef _h8s_sci_ierdy_snd
#undef _h8s_sci_ierdy_rcv
#undef _h8s_sci_init
#undef _h8s_sci_putchar_pol


#endif /* LABEL_ASM */
#endif /* _CPU_UNRENAME_H_ */
