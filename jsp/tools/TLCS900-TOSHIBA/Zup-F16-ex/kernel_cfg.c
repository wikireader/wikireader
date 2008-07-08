/* Configured with [-cpu tlcs900-toshiba -system zup_f16_ex -s sample1.cfg.i ] */

#include "kernel_cfg.h"
#include "kernel_id.h"

#if TKERNEL_PRVER >= 0x1040
#define CFG_INTHDR_ENTRY(inthdr) INTHDR_ENTRY(inthdr)
#define CFG_EXCHDR_ENTRY(exchdr) EXCHDR_ENTRY(exchdr)
#define CFG_INT_ENTRY(inthdr) INT_ENTRY(inthdr)
#define CFG_EXC_ENTRY(exchdr) EXC_ENTRY(exchdr)
#else
#error "This configuration file has no compatibility with TOPPERS/JSP rel 1.3 or earlier."
#endif

#ifndef __EMPTY_LABEL
#define __EMPTY_LABEL(x,y) x y[0]
#endif

#if TKERNEL_PRID != 0x0001u /* TOPPERS/JSP */
#error "You can not use this configuration file without TOPPERS/JSP"
#endif

	/* User specified include files*/
#include "sample1.h"
#include "hw_timer.h"
#include "timer.h"
#include <hw_serial.h>
#include "serial.h"
#include "logtask.h"


	/* Object initializer [task] */

#define TNUM_TSKID 5

const ID _kernel_tmax_tskid = (TMIN_TSKID + TNUM_TSKID - 1);

static __STK_UNIT __stack_TASK1[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT __stack_TASK2[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT __stack_TASK3[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT __stack_MAIN_TASK[__TCOUNT_STK_UNIT(512)];
static __STK_UNIT __stack_LOGTASK[__TCOUNT_STK_UNIT(LOGTASK_STACK_SIZE)];

const TINIB _kernel_tinib_table[TNUM_TSKID] = {
	{0, (VP_INT)(( VP_INT ) 1), (FP)(task), INT_PRIORITY(10), __TROUND_STK_UNIT(512), __stack_TASK1, 0, (FP)(tex_routine)},
	{0, (VP_INT)(( VP_INT ) 2), (FP)(task), INT_PRIORITY(10), __TROUND_STK_UNIT(512), __stack_TASK2, 0, (FP)(tex_routine)},
	{0, (VP_INT)(( VP_INT ) 3), (FP)(task), INT_PRIORITY(10), __TROUND_STK_UNIT(512), __stack_TASK3, 0, (FP)(tex_routine)},
	{0x00u | 0x02u, (VP_INT)(0), (FP)(main_task), INT_PRIORITY(5), __TROUND_STK_UNIT(512), __stack_MAIN_TASK, TA_NULL, (FP)(NULL)},
	{0x00u | 0x02u, (VP_INT)(( VP_INT ) 1), (FP)(logtask), INT_PRIORITY(LOGTASK_PRIORITY), __TROUND_STK_UNIT(LOGTASK_STACK_SIZE), __stack_LOGTASK, TA_NULL, (FP)(NULL)}
};

const ID _kernel_torder_table[TNUM_TSKID] = {1,2,3,4,5};

TCB _kernel_tcb_table[TNUM_TSKID];


	/* Object initializer [semaphore] */

#define TNUM_SEMID 2

const ID _kernel_tmax_semid = (TMIN_SEMID + TNUM_SEMID - 1);

const SEMINIB _kernel_seminib_table[TNUM_SEMID] = {
	{1, 0, 1},
	{1, 1, 1}
};

SEMCB _kernel_semcb_table[TNUM_SEMID];


	/* Object initializer [eventflag] */

#define TNUM_FLGID 0

const ID _kernel_tmax_flgid = (TMIN_FLGID + TNUM_FLGID - 1);

__EMPTY_LABEL(const FLGINIB, _kernel_flginib_table);
__EMPTY_LABEL(FLGCB, _kernel_flgcb_table);


	/* Object initializer [dataqueue] */

#define TNUM_DTQID 0

const ID _kernel_tmax_dtqid = (TMIN_DTQID + TNUM_DTQID - 1);

__EMPTY_LABEL(const DTQINIB, _kernel_dtqinib_table);
__EMPTY_LABEL(DTQCB, _kernel_dtqcb_table);


	/* Object initializer [mailbox] */

#define TNUM_MBXID 0

const ID _kernel_tmax_mbxid = (TMIN_MBXID + TNUM_MBXID - 1);

__EMPTY_LABEL(const MBXINIB, _kernel_mbxinib_table);
__EMPTY_LABEL(MBXCB, _kernel_mbxcb_table);


	/* Object initializer [mempfix] */

#define TNUM_MPFID 0

const ID _kernel_tmax_mpfid = (TMIN_MPFID + TNUM_MPFID - 1);

__EMPTY_LABEL(const MPFINIB, _kernel_mpfinib_table);
__EMPTY_LABEL(MPFCB, _kernel_mpfcb_table);


	/* Object initializer [cyclic] */

#define TNUM_CYCID 1

const ID _kernel_tmax_cycid = (TMIN_CYCID + TNUM_CYCID - 1);

const CYCINIB _kernel_cycinib_table[TNUM_CYCID] = {
	{0,0,(FP)(cyclic_handler),2000,0}
};

CYCCB _kernel_cyccb_table[TNUM_CYCID];


	/* Object initializer [interrupt] */

#define TNUM_INHNO 3

const UINT _kernel_tnum_inhno = TNUM_INHNO;

CFG_INTHDR_ENTRY(timer_handler);
CFG_INTHDR_ENTRY(serial_in_handler1);
CFG_INTHDR_ENTRY(serial_out_handler1);

const INHINIB _kernel_inhinib_table[TNUM_INHNO] = {
	{20,0,(FP)CFG_INT_ENTRY(timer_handler)},
	{36,0,(FP)CFG_INT_ENTRY(serial_in_handler1)},
	{37,0,(FP)CFG_INT_ENTRY(serial_out_handler1)}
};


	/* Object initializer [exception] */

#define TNUM_EXCNO 1

const UINT _kernel_tnum_excno = TNUM_EXCNO;

CFG_EXCHDR_ENTRY(cpuexc_handler);

const EXCINIB _kernel_excinib_table[TNUM_EXCNO] = {
	{3,0,(FP)CFG_EXC_ENTRY(cpuexc_handler)}
};

	/* Initialization handler */

void
_kernel_call_inirtn(void)
{
	timer_initialize( (VP_INT)(0) );
	serial_initialize( (VP_INT)(0) );
}

void
_kernel_call_terrtn(void)
{
	timer_terminate( (VP_INT)(0) );
}

	/* Object initialization routine */

void
_kernel_object_initialize(void)
{
	_kernel_task_initialize();
	_kernel_semaphore_initialize();
	_kernel_cyclic_initialize();
	_kernel_interrupt_initialize();
	_kernel_exception_initialize();
}

TMEVTN   _kernel_tmevt_heap[TNUM_TSKID + TNUM_CYCID];

