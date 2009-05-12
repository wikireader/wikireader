.file	"eforth.S"

/*===============================================================
	eForth 1.0 by Bill Muench and C. H. Ting, 1990
	Much of the code is derived from the following sources:
		8086 figForth by Thomas Newman, 1981 and Joe smith, 1983
		aFORTH by John Rible
		bFORTH by Bill Muench

	The goal of this implementation is to provide a simple eForth Model
	which can be ported easily to many 8, 16, 24 and 32 bit CPU's.
	The following attributes make it suitable for CPU's of the '90:

		small machine dependent kernel and portable high level code
		source code in the MASM format
		direct threaded code
		separated code and name dictionaries
		simple vectored terminal and file interface to host computer
		aligned with the proposed ANS Forth Standard
		easy upgrade path to optimize for specific CPU

	You are invited to implement this Model on your favorite CPU and
	contribute it to the eForth Library for public use. You may use
	a portable implementation to advertise more sophisticated and
	optimized version for commercial purposes. However, you are
	expected to implement the Model faithfully. The eForth Working
	Group reserves the right to reject implementation which deviates
	significantly from this Model.

	As the ANS Forth Standard is still evolving, this Model will
	change accordingly. Implementations must state clearly the
	version number of the Model being tracked.

	Representing the eForth Working Group
	in the Silicon Valley FIG Chapter.

	Send contributions to:

		Dr. C. H. Ting
		156 14th Avenue
		San Mateo, CA 94402
		(415) 571-7639

===============================================================*/

/* for each of these, #define enables the feature, #undef disables it */
#undef	__DEBUG__	/* debugging verbosity */
#undef	__TRACE__	/* tracing through external debugger */
#undef	__TRACE_EXIT__	/* tracing through external debugger */
#define	BUFFER_TOS	/* trying to buffer TOS in %ebx */
#define	__SYSCALL__	/* defining the syscall interface for Linux */
#define __USE_ENTER__

/* object file format dependent stuff: a.out or ELF */
#if defined(__AOUT__)
#  define CELL_ALIGN 2
#  define BASE_OFFSET 0
#elif defined(__ELF__)
#  define CELL_ALIGN 4
#  define BASE_OFFSET 0x74
#else
#  error "You must define either AOUT or ELF"
#endif

# Version control

VER	=	1		# Major release version
EXT	=	0		# Minor extension

# Constants

TRUEE	=	-1		# True flag
FALSEE	=	0		# False flag

COMPO	=	0x40		# Lexicon compile-only bit
IMEDD	=	0x80		#  ...immediate
MASKK	=	0x7F7F7F1F	# Lexicon mask

CELLL	=	4		# Size of a cell
CELLMASK =	-CELLL		# Mask offset within cell
NBPW	=	32		# bits in a word (cell)
BASEE	=	0xA		# Default radix
VOCSS	=	8		# Depth of vocabulary stack

BKSPP	=	8		# Backspace
DELETE	=	127		# Delete
LF	=	0xA		# Line feed
_CR_	=	0xD		# Carriage return
ERR	=	0x1B		# Error escape
TIC	=	0x27		/* Tick ' */

CALLL	=	0xE8909090	# CALL opcode, padded with NOPs to word
				# should be padded with a 1 cycle 3-byte NOP.
				# like leal 0(%ebp),%ebp

/* Memory allocation
 * [Faré]	I made it larger so no overflow should ever occur
 *		May be resized according to your needs
 */

EM	=	0x100000
/* 1 MB: Total __virtual__ memory available
 * only actually used pages are allocated
 * so let's be large
 */

RS_SZ	=	65536			# combined Return stack/TIB size
US_SZ	=	65536			# User area size
NAME_SZ	=	65536			# space for lots of symbols

NAMEE	=	__BASE__ + EM			# space for new names
RSTOP	=	NAMEE - NAME_SZ		# Top of return stack
TIBB	=	RSTOP - RS_SZ		# Terminal input buffer (TIB)
UPP	=	TIBB - US_SZ		# Start of user area (UP0)
DSTOP	=	UPP - US_SZ		# Top of return stack

RPP	=	RSTOP - 2 * CELLL	# orig. return stack pointer (RP0)
SPP	=	DSTOP - 2 * CELLL	# orig. data stack pointer (SP0)


/* Initialize assembly variables */

_USER	=	4*CELLL			/* first user variable offset */

/*  Define assembly macros */

#define __aligndata	.align CELL_ALIGN,0
		/* align to CELLL boundary, zero pad */
#define __aligncode	.align CELL_ALIGN,0x90
			/* align to CELLL boundary, NOP-pad */
#define __align		__aligndata

/*	Compile a code definition header. */

PREVCODE=0

#ifdef	__TRACE__
# define __SST__ .byte 0x2E,0x2E,0x2E,0xCC ;
/* Here are the GDB macros I used to debug eforth
set $dsd=2
set $rsd=2
display/a {long}$esp@$dsd
display/a $ebx
display/a $esp
display $dsd=(unsigned)(0xFC0000-(long)$esp)/4
display/a {long}$ebp@$rsd
display/a $ebp
display $rsd=(unsigned)(0xFF0000-(long)$ebp)/4
display/a {long}$esi@4
display/a $esi
# display/5i $eip
# display/a $eip
define c
  continue
end
define xtib
x/32c 0xfe0000
end
enable display 1
enable display 4
define regs
info registers
end
# END of GDB macros
To debug more, I should replace __NEXT by a call to some
debugging routine that verifies some conditions to decide if we trace.
No more need of GDB even !
*/
#else
# define __SST__
#endif

#define __INITSEG__	.text 0 /* initialization code segment */
#define __STRSEG__	.text 1 /* constant strings segment */
#define __DATASEG__	.data 0 /* data segment; must be before code */
#define __CODESEG__	.data 1 /* code segment; must be after data */

#define __CODE(lex, name, label) \
	__CODESEG__ ; \
	__aligncode ; \
label: ; \
	__SST__ ; \
	__DATASEG__ ; \
	__aligndata ; \
	.long label, PREVCODE ; \
	PREVCODE=. ; \
_NAME=. ; \
	.byte lex ; \
	.ascii name ; \
	__aligndata ; \
	__CODESEG__ ;

/*	Compile a colon definition header. */

/*
 * Note that the call to DOLST must be padded out to a CELLL boundary;
 * a call takes 5 bytes (just about worst-case...), so we add in segment
 * override prefixes (useless ones, at that) to pad out the size.
 */
#define __COLON(lex, name, label) \
	__CODE(lex, name, label) \
	.byte	0x2E,0x2E,0x2E ; \
	call DOLST

/*	Compile a user variable header. */

#define __USER(lex, name, label) \
	__CODE(lex, name, label) \
	.byte	0x2E,0x2E,0x2E ; \
	call DOLST ; \
	.long DOUSE, _USER ; \
	_USER = _USER+CELLL

/*	Assemble inline direct threaded code ending. */

#ifdef __USE_ENTER__
#  define __ENTER \
	add $CELLL,%esi
#  define __NEXT \
	jmp *(%esi)
#else
#  define __ENTER
#  define __NEXT \
	add $CELLL,%esi ; jmp -4(%esi)
#	lodsl ; jmp %eax
#endif


/* [Faré]	macros to debug eForth -- *USEFUL* -- currently disabled */
#ifdef __DEBUG__
#define DBG(x) \
	pushl $ x ; call prerr_string
#define PRNUM(x) \
	pushl x ; call prerr_num
#else
#define DBG(x)
#define PRNUM(x)
#endif
#define _DBG(x)		/* dummies; always disabled */
#define _PRNUM(x)	/* just remove the _ in sources to reenable ! */



/*****************  Main entry points and COLD start data *******************/

	__INITSEG__
__BASE__ = . - BASE_OFFSET
	__aligncode
	.globl	__init__
__init__:
	DBG(HMSG)
	/* First locate the start of the environment variables */
	popl %ecx			/* argc in %ecx, argv is %esp */
	leal 4(%esp,%ecx,4),%eax	/* environ */
	movl %ecx,argc
	movl %esp,argv
	movl %eax,environ

	movl	$45,%eax		/* 45: sys_brk: Move data end out */
	movl	$NAMEE,%ebx
	int	$0x80			/* Linux system call */
	movl	%eax,brkaddr

	/*
	 * Thread our dictionary.  Amazing, but I just can't get gas
	 * to do it right.  Uses of '.' all just reloc to the last
	 * value set for _LINK, and the div operator for length
	 * of lex doesn't work in = expressions.  So we just walk
	 * it forward, and patch it up during startup.	vandys
	 */
	movl	$BASEN,%ebx		/* First entry */
	movl	$0,%ecx			/* back-link pointer */

1:
	cmpl	$0,(%ebx)		/* 0 code ptr -> end */
	jz	2f
	movl	%ecx,4(%ebx)		/* write link */
	leal	8(%ebx),%ecx		/* advance link */
	movl	%ecx,%ebx		/* move to entry name */
	movb	(%ebx),%al		/* load count */
	andl	$0x1F,%eax		/* Mask to length only (5 bit) */
	leal	CELLL(%eax,%ebx),%ebx	/* include count byte (1)
					   and add CELLL-1 to round upwards */
	andl	$CELLMASK,%ebx		/* round down to beginning of cell */
	jmp	1b
2:
#ifdef BUFFER_TOS
	movl	$0xDEADBEEF, %ebx	/* initialize TOS */
	movl	$SPP + CELLL,%esp	/* initialize SP */
	pushl	%ebx
#else
	movl	$SPP,%esp		/* initialize SP */
#endif
	movl	$RPP,%ebp		/* initialize RP */
	cld				/* SI gets incremented */
	movl	$COLD1,%esi
	PRNUM($0)
	__NEXT				/* to high level cold start */



/* COLD start moves the following to USER variables. */
/* MUST BE IN SAME ORDER AS USER VARIABLES. */
	__DATASEG__
	__aligndata

UZERO:	.long	0,0,0,0		/* reserved space in user area */
argc:	.long	0
argv:	.long	0
environ:.long	0
brkaddr:.long	0
	.long	SPP		/* SP0 */
	.long	RPP		/* RP0 */
	.long	QRX		/* '?KEY */
	.long	TXSTO		/* 'EMIT */
	.long	ACCEP		/* 'EXPECT */
	.long	KTAP		/* 'TAP */
	.long	TXSTO		/* 'ECHO */
	.long	DOTOK		/* 'PROMPT */
	.long	BASEE		/* BASE */
	.long	0		/* tmp */
	.long	0		/* SPAN */
	.long	0		/* >IN */
	.long	0		/* #TIB */
	.long	TIBB		/* TIB */
	.long	0		/* CSP */
	.long	INTER		/* 'EVAL */
	.long	NUMBQ		/* 'NUMBER */
	.long	0		/* HLD */
	.long	0		/* HANDLER */
	.long	0		/* CONTEXT pointer */
	.long	0,0,0,0,0,0,0,0 /* vocabulary stack--see VOCSS */
	.long	0		/* CURRENT pointer */
	.long	0		/* vocabulary link pointer */
	.long	CTOP		/* CP */
	.long	NTOP		/* NP */
	.long	LASTN		/* LAST */
ULAST:

	__DATASEG__
BASEN:
	__CODESEG__


/************************* OS dependent stuff *******************************/


#ifdef	__SYSCALL__
/* Linux system calls with 0 ... 5 parameters */
/* on success, TRUEE is pushed with result; */
/* on error, FALSEE is pushed with error number */



#ifdef BUFFER_TOS
#  define	BEGIN_syscall \
		xchgl	%esp,%ebp ; \
		pushl	%edi ; \
		pushl	%esi ; \
		xchgl	%esp,%ebp ; \
		movl	%ebx,%eax		/* sys_call number */
#else
#  define	BEGIN_syscall \
		xchgl	%esp,%ebp ; \
		pushl	%edi ; \
		pushl	%esi ; \
		xchgl	%esp,%ebp ; \
		popl	%eax			/* sys_call number */
#endif


/*   syscall5	( w5 ... w1 n -- w T | n F ) */
		__CODE(8, "syscall5", syscall5 )
		BEGIN_syscall
		popl	%ebx			/* first argument */
		popl	%ecx			/* second argument */
		popl	%edx			/* third argument */
		popl	%esi			/* fourth argument */
		popl	%edi			/* fifth argument */
		jmp	__syscall

/*   syscall4	( w4 ... w1 n -- w T | n F ) */
		__CODE(8, "syscall4", syscall4 )
		BEGIN_syscall
		popl	%ebx			/* first argument */
		popl	%ecx			/* second argument */
		popl	%edx			/* third argument */
		popl	%esi			/* fourth argument */
		jmp	__syscall

/*   syscall3	( w3 w2 w1 n -- w T | n F ) */
		__CODE(8, "syscall3", syscall3 )
		BEGIN_syscall
		popl	%ebx			/* first argument */
		popl	%ecx			/* second argument */
		popl	%edx			/* third argument */
		jmp	__syscall


/*   syscall2	( w2 w1 n -- w T | n F ) */
		__CODE(8, "syscall2", syscall2 )
		BEGIN_syscall
		popl	%ebx			/* first argument */
		popl	%ecx			/* second argument */
		jmp	__syscall


/*   syscall1	( w1 n -- w T | n F ) */
		__CODE(8, "syscall1", syscall1 )
		BEGIN_syscall
		popl	%ebx			/* first argument */
		jmp	__syscall


/*   syscall0	( n -- w T | n F ) */
		__CODE(8, "syscall0", syscall0 )
		BEGIN_syscall

__syscall:	int	$0x80			/* call Linux */
		cld
		movl	$TRUEE,%ebx
		orl	%eax,%eax		/* test for error condition */
		jns	1f
		neg	%eax
		incl	%ebx			/* !!! FALSE = TRUEE + 1 = 0 */
1:		pushl	%eax
#ifndef BUFFER_TOS
		pushl	%ebx
#endif
		PRNUM($0)
		xchgl	%esp,%ebp
		popl	%esi
		popl	%edi
		xchgl	%esp,%ebp
		__ENTER
		__NEXT

#endif	/* __SYSCALL__ */


/*  Device dependent I/O */

/*   BYE		( -- ) */
/*		Exit eForth. */

		__CODE(3, "BYE", BYE)
#		__ENTER
		mov	$0,%ebx		/* exit with code 0 */
lxEXIT:
		mov	$1,%eax		/* sys_call 1 : exit */
		int	$0x80
		hlt
		jmp	lxEXIT

		__align
IOBuf:		.long	0


/*   ?RX		( -- c T | F ) */
/*		Return input character and true, or a false if no input. */

		__CODE(3, "?RX", QRX)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
#endif
		movl	$3,%eax		# SYS_read
		movl	$0,%ebx		# from stdin
		pushl	%ebx		# only low byte may be non-zero
		movl	%esp,%ecx	# into Input Buffer
		movl	$1,%edx		# 1 char
		int	$0x80		# call Linux
		cld
		negl	%eax		# !!! TRUEE is -1
		jnz	1f
		popl	%ebx		# pop the input buffer
#ifdef __DEBUG__
		jmp	2f
1:
		DBG(IMSG)
		PRNUM((%esp))
		DBG(CRLF)
2:
#else
1:
#endif
#ifdef BUFFER_TOS
		movl	%eax,%ebx
#else
		pushl	%eax
#endif
		__NEXT


/*   TX!		( c -- ) */
/*		Send character c to the output device. */

		__CODE(3, "TX!", TXSTO)
		__ENTER
#ifndef BUFFER_TOS
		popl	%ebx
#endif
		movzbl	%bl,%ebx
		pushl	%ebx
		DBG(OMSG)
		PRNUM((%esp))
		movl	$4,%eax		# SYS_write
		movl	$1,%ebx		# to stdout
		leal	(%esp),%ecx	# from output buffer; !!!
		movl	%ebx,%edx	# 1 char; !!! %ebx must contain 1
		int	$0x80
		cld
		popl	%eax		# pop the output buffer
#ifdef BUFFER_TOS
		popl	%ebx
#endif
		DBG(CRLF)
		__NEXT


#ifdef __DEBUG__
/* [Faré]	debugging functions */
/*		enabled only if debugging is enabled using __DEBUG__ */

/* prerr_num:	prints num in %edx to stderr */
prerr_num:	pushal
		movl	36(%esp),%edx
		movl	$numbuf+8,%ebx
		movl	$8,%ecx
1:		movb	%dl,%al
		andb	$0x0F,%al
		shrl	$4,%edx
		orb	$0x30,%al
		cmpb	$0x3A,%al
		jb	2f
		addb	$0x41-0x3A,%al
2:		decl	%ebx
		movb	%al,(%ebx)
		loop	1b
		pushl	%ebx
		call	prerr_string
		popal
		ret	$4
/* prerr_string: prints string in %ecx to stderr */
prerr_string:	pushal
		movl	36(%esp),%ecx
		call	count_string
		movl	$4,%eax
		movl	$2,%ebx
		int	$0x80
		cld
		popal
		ret	$4
/* compute size of zero-terminated string in %ecx; puts size in %edx */
count_string:	mov	$-1,%edx
		mov	%ecx,%edi
1:		inc	%edx
		testb	$-1,0(%edi,%edx)
		jnz	1b
		ret
		__STRSEG__
numbuf:		.ascii	"12345678\0"
HMSG:		.ascii	"Hello, World !\n\0"
CRLF:		.ascii	"\n\0"
EOFMSG: 	.ascii	"<EOF>\n\0"
IMSG:		.ascii	"Inputing: \0"
OMSG:		.ascii	"Echoing: \0"
BMSG0:		.ascii	"BRK. \0"
BMSG1:		.ascii	": SP=\0"
BMSG2:		.ascii	" RP=\0"
BMSG3:		.ascii	" TOS=\0"
BMSG4:		.ascii	" \0"
BMSG5:		.ascii	" TORS=\0"
nextmsg:	.ascii	"next\n\0"
doLITmsg:	.ascii	"doLIT\n\0"
EXITmsg:	.ascii	"EXIT\n\0"
doLISTmsg:	.ascii	"doLIST\n\0"
BRANmsg:	.ascii	"BRAN\n\0"
QBRANmsg:	.ascii	"QBRAN\n\0"

		__CODE(4,"BRK.",BRKPOINT)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
#endif
		DBG(BMSG0)
		lodsl
		PRNUM(%eax)
		DBG(BMSG1)
		mov %esp,%ebx
		PRNUM(%ebx)
		DBG(BMSG2)
		PRNUM(%ebp)
		DBG(BMSG3)
		PRNUM((%ebx))
		DBG(BMSG4)
		PRNUM(4(%ebx))
		DBG(BMSG5)
		PRNUM((%ebp))
		DBG(CRLF)
#ifdef BUFFER_TOS
		popl	%ebx
#endif
		__NEXT

#define BRKPNT(x)	.long BRKPOINT,x

#else			/* no __DEBUG__ing */
#define BRKPNT(x)
#endif

#define _BRKPNT(x)	/* always disabled, */
			/* so you only need remove _ in sources to reenable */

#ifdef __TRACE
#define	BRK	.long __BRK__
__BRK__:
		__ENTER
		__SST__
		__NEXT
#else
#define	BRK
#endif

/*   !IO		( -- ) */
/*		Initialize the serial I/O devices. */

		__CODE(3, "!IO", STOIO)
		__ENTER
		__NEXT


/*   argc		( -- n ) */
/*		C main() argument argc */

		__USER(4, "argc", CARGC)

/*   argv		( -- a ) */
/*		C main() argument argv */

		__USER(4, "argv", CARGV)

/*   environ		( -- a ) */
/*		C environment strings */

		__USER(7, "environ", CENVIRON)

/*   brk		( -- a ) */
/*		C brk() address */

		__USER(3, "brk", CBRK)



/*********************** The  OS-independent kernel **************************/

/*   doLIT	( -- w ) */
/*		Push an inline literal. */

		__CODE(COMPO+5, "doLIT", DOLIT)
		DBG(doLITmsg)
#ifdef __USE_ENTER__
#  ifdef BUFFER_TOS
		pushl	%ebx
		mov	4(%esi),%ebx
#  else
		pushl	4(%esi)
#  endif
		addl	$8,%esi
#else
		__ENTER
#  ifdef BUFFER_TOS
		pushl	%ebx
		mov	(%esi),%ebx
		addl	$4,%esi
#  else
		lodsl
		pushl	%eax
#  endif
#endif
		__NEXT

/*   doLIST	( a -- ) */
/*		Process colon list. */

		__CODE(COMPO+6, "doLIST", DOLST)
		__ENTER
		DBG(doLISTmsg)
		subl	$CELLL,%ebp	# push current PC on forth return stack
		movl	%esi,(%ebp)
		popl	%esi		# new list address from RET address !
/* Beware !
 * new address list is generated by a call instruction, and thus
 * not buffered in %ebx !!!
 * By systematically transforming all commands, I did a mistake here.
 */
		__NEXT

/*   next	( -- ) */
/*		Run time code for the single index loop. */
/*		: next ( -- ) \ hilevel model */
/*		  r> r> dup if 1 - >r @ >r exit then drop cell+ >r */

		__CODE(COMPO+4, "next", DONXT)
		__ENTER
		DBG(nextmsg)
		subl	$1,(%ebp)
		jc	1f		/* ?decrement below 0 */
		movl	(%esi),%esi	/* no, branch back again */
		__NEXT
1:		addl	$CELLL,%ebp	 /* yes, pop the index */
		addl	$CELLL,%esi	 /* continue past the branch offset */
		__NEXT

/*   ?branch	( f -- ) */
/*		Branch if flag is zero. */

		__CODE(COMPO+7, "?branch", QBRAN)
		DBG(QBRANmsg)
#ifdef BUFFER_TOS
		orl	%ebx,%ebx	/* ?flag = 0 */
		popl	%ebx		/* pop TOS */
#else
		popl	%ebx		/* Pop flag */
		orl	%ebx,%ebx	/* ?flag = 0 */
#endif
		jz	BRAN		/* Yes, branch */
#ifdef __USE_ENTER__
		addl	$2*CELLL,%esi	 /* Point IP to next cell */
#else
		addl	$CELLL,%esi	 /* Point IP to next cell */
#endif
		__NEXT


/*   branch	( -- ) */
/*		Branch to an inline address. */

		__CODE(COMPO+6, "branch", BRAN)
		__ENTER
		DBG(BRANmsg)
		movl	(%esi),%esi
		__NEXT

/*   EXECUTE	( ca -- ) */
/*		Execute the word at ca. */

		__CODE(7, "EXECUTE", EXECU)
/*		__ENTER
 *	this one made the program eat all vm !
 */
#ifdef BUFFER_TOS
		xchgl	%ebx,(%esp)
		ret
#else
		popl	%eax
		jmp	%eax	# Jump to code address
#endif

/*   EXIT	( -- ) */
/*		Terminate a colon definition. */

		__CODE(4, "EXIT", EXIT)
		__ENTER
		DBG(EXITmsg)
		movl	(%ebp),%esi	/* Pop return address */
		addl	$CELLL,%ebp	 /* Adjust RP */
#ifdef __TRACE_EXIT__
		int	$3
#endif
		__NEXT

/*   !		( w a -- ) */
/*		Pop the data stack to memory. */

		__CODE(1, "!", STORE)
		__ENTER
#ifdef BUFFER_TOS
		popl	(%ebx)
		popl	%ebx
#else
		popl	%ebx
		popl	(%ebx)
#endif
		__NEXT

/*   @		( a -- w ) */
/*		Push memory location to the data stack. */

		__CODE(1, "@", AT)
		__ENTER
#ifdef BUFFER_TOS
		movl	(%ebx),%ebx
#else
		popl	%ebx
		pushl	(%ebx)
#endif
		__NEXT

/*   C!		( c b -- ) */
/*		Pop the data stack to byte memory. */

		__CODE(2, "C!", CSTOR)
		__ENTER
#ifdef BUFFER_TOS
		popl	%eax
		movb	%al,(%ebx)
		popl	%ebx
#else
		popl	%ebx
		popl	%eax
		movb	%al,(%ebx)
#endif
		__NEXT

/*   C@		( b -- c ) */
/*		Push byte memory location to the data stack. */

		__CODE(2, "C@", CAT)
		__ENTER
#ifdef BUFFER_TOS
		movzbl	(%ebx),%ebx
#else
		popl	%ebx
		xorl	%eax,%eax
		movb	(%ebx),%al
		pushl	%eax
#endif
		__NEXT

/*   RP@		( -- a ) */
/*		Push the current RP to the data stack. */

		__CODE(3, "RP@", RPAT)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
		movl	%ebp,%ebx
#else
		pushl	%ebp
#endif
		__NEXT

/*   RP!		( a -- ) */
/*		Set the return stack pointer. */

		__CODE(COMPO+3, "RP!", RPSTO)
		__ENTER
#ifdef BUFFER_TOS
		movl	%ebx,%ebp
		popl	%ebx
#else
		popl	%ebp
#endif
		__NEXT

/*   R>		( -- w ) */
/*		Pop the return stack to the data stack. */

		__CODE(COMPO+2, "R>", RFROM)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
		movl	(%ebp),%ebx
#else
		pushl	(%ebp)
#endif
		addl	$CELLL,%ebp
		__NEXT

/*   R@		( -- w ) */
/*		Copy top of return stack to the data stack. */

		__CODE(2, "R@", RAT)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
		movl	(%ebp),%ebx
#else
		pushl	(%ebp)
#endif
		__NEXT

/*   >R		( w -- ) */
/*		Push the data stack to the return stack. */

		__CODE(COMPO+2, ">R", TOR)
		__ENTER
		subl	$CELLL,%ebp
#ifdef BUFFER_TOS
		movl	%ebx,(%ebp)
		popl	%ebx
#else
		popl	(%ebp)
#endif
		__NEXT

/*   SP@		( -- a ) */
/*		Push the current data stack pointer. */

		__CODE(3, "SP@", SPAT)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
		movl	%esp,%ebx
#else
		pushl	%esp
#endif
		__NEXT

/*   SP!		( a -- ) */
/*		Set the data stack pointer. */

		__CODE(3, "SP!", SPSTO)
		__ENTER
#ifdef BUFFER_TOS
		movl	%ebx,%esp
		popl	%ebx
#else
		popl	%esp
#endif
		__NEXT

/*   DROP	( w -- ) */
/*		Discard top stack item. */

		__CODE(4, "DROP", DROP)
		__ENTER
#ifdef BUFFER_TOS
		popl	%ebx
#else
		addl	$CELLL,%esp
#endif
		__NEXT

/*   DUP		( w -- w w ) */
/*		Duplicate the top stack item. */

		__CODE(3, "DUP", DUPP)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
#else
		pushl	(%esp)
#endif
		__NEXT

/*   SWAP	( w1 w2 -- w2 w1 ) */
/*		Exchange top two stack items. */

		__CODE(4, "SWAP", SWAP)
		__ENTER
#ifdef BUFFER_TOS
		xchg	%ebx,(%esp)
#else
		popl	%ebx
		popl	%eax
		pushl	%ebx
		pushl	%eax
#endif
		__NEXT

/*   OVER	( w1 w2 -- w1 w2 w1 ) */
/*		Copy second stack item to top. */

		__CODE(4, "OVER", OVER)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
		movl	CELLL(%esp),%ebx
#else
		pushl	CELLL(%esp)
#endif
		__NEXT

/*   0<		( n -- t ) */
/*		Return true if n is negative. */

		__CODE(2, "0<", ZLESS)
		__ENTER
#ifdef BUFFER_TOS
		mov	%ebx,%eax
		cdq
		mov	%edx,%ebx
#else
		popl	%eax
		cdq
		pushl	%edx
#endif
		__NEXT

/*   AND		( w w -- w ) */
/*		Bitwise AND. */

		__CODE(3, "AND", ANDD)
		__ENTER
		popl	%eax
#ifdef BUFFER_TOS
		andl	%eax,%ebx
#else
		andl	%eax,(%esp)
#endif
		__NEXT

/*   OR		( w w -- w ) */
/*		Bitwise inclusive OR. */

		__CODE(2, "OR", ORR)
		__ENTER
		popl	%eax
#ifdef BUFFER_TOS
		orl	%eax,%ebx
#else
		orl	%eax,(%esp)
#endif
		__NEXT

/*   XOR		( w w -- w ) */
/*		Bitwise exclusive OR. */

		__CODE(3, "XOR", XORR)
		__ENTER
		popl	%eax
#ifdef BUFFER_TOS
		xorl	%eax,%ebx
#else
		xorl	%eax,(%esp)
#endif
		__NEXT

/*   UM+		( u u -- udsum ) */
/*		Add two unsigned single numbers and return a double sum. */

		__CODE(3, "UM+", UPLUS)
		__ENTER
#ifdef BUFFER_TOS
		addl	%ebx,(%esp)
		leal	0,%ebx
		adcl	%ebx,%ebx
#else
		popl	%eax
		leal	0,%ebx
		addl	%eax,(%esp)
		adcl	%ebx,%ebx
		pushl	%ebx
#endif
		__NEXT

/*  System and user variables */

/*   doVAR	( -- a ) */
/*		Run time routine for VARIABLE and CREATE. */

		__CODE(COMPO+5,"doVAR",DOVAR)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
		movl	%esi,%ebx
#else
		pushl	%esi
#endif
		movl	(%ebp),%esi
		addl	$CELLL,%ebp
		__NEXT

/*   UP		( -- a ) */
/*		Pointer to the user area. */

		__CODE(2,"UP",UP)
		__ENTER
#ifdef BUFFER_TOS
		pushl	%ebx
		movl	$up2,%ebx
#else
		pushl	$up2
#endif
		__NEXT

up2:		.long	UPP

/*   doUSER	( -- a ) */
/*		Run time routine for user variables. */

		__CODE(COMPO+6,"doUSER",DOUSE)
		__ENTER
		movl	(%esi),%eax
		addl	up2,%eax
#ifdef BUFFER_TOS
		pushl	%ebx
		movl	%eax,%ebx
#else
		pushl	%eax
#endif
		movl	(%ebp),%esi	/* Pop return address */
		addl	$CELLL,%ebp	/* Adjust RP */
		__NEXT
		/* .long	RFROM,AT,UP,AT,PLUS,EXIT */

/*   SP0		( -- a ) */
/*		Pointer to bottom of the data stack. */

		__USER(3, "SP0", SZERO)

/*   RP0		( -- a ) */
/*		Pointer to bottom of the return stack. */

		__USER(3, "RP0", RZERO)

/*   '?KEY	( -- a ) */
/*		Execution vector of ?KEY. */

		__USER(5,"'?KEY",TQKEY)

/*   'EMIT	( -- a ) */
/*		Execution vector of EMIT. */

		__USER(5,"'EMIT",TEMIT)

/*   'EXPECT	( -- a ) */
/*		Execution vector of EXPECT. */

		__USER(7,"'EXPECT",TEXPE)

/*   'TAP	( -- a ) */
/*		Execution vector of TAP. */

		__USER(4,"'TAP",TTAP)

/*   'ECHO	( -- a ) */
/*		Execution vector of ECHO. */

		__USER(5,"'ECHO",TECHO)

/*   'PROMPT	( -- a ) */
/*		Execution vector of PROMPT. */

		__USER(7,"'PROMPT",TPROM)

/*   BASE	( -- a ) */
/*		Storage of the radix base for numeric I/O. */

		__USER(4, "BASE", BASE)

/*   tmp		( -- a ) */
/*		A temporary storage location used in parse and find. */

		__USER(COMPO+3, "tmp", TEMP)

/*   SPAN	( -- a ) */
/*		Hold character count received by EXPECT. */

		__USER(4, "SPAN", SPAN)

/*   >IN		( -- a ) */
/*		Hold the character pointer while parsing input stream. */

		__USER(3, ">IN", INN)

/*   #TIB	( -- a ) */
/*		Hold the current count in and address of the terminal input buffer. */

		__USER(4, "#TIB", NTIB)
		_USER = _USER+CELLL		/* hold the base address of the terminal input buffer */

/*   CSP		( -- a ) */
/*		Hold the stack pointer for error checking. */

		__USER(3, "CSP", CSP)

/*   'EVAL	( -- a ) */
/*		Execution vector of EVAL. */

		__USER(5,"'EVAL",TEVAL)

/*   'NUMBER	( -- a ) */
/*		Execution vector of NUMBER?. */

		__USER(7,"'NUMBER",TNUMB)

/*   HLD		( -- a ) */
/*		Hold a pointer in building a numeric output string. */

		__USER(3, "HLD", HLD)

/*   HANDLER	( -- a ) */
/*		Hold the return stack pointer for error handling. */

		__USER(7, "HANDLER", HANDL)

/*   CONTEXT	( -- a ) */
/*		A area to specify vocabulary search order. */

		__USER(7, "CONTEXT", CNTXT)
		_USER = _USER+VOCSS*CELLL	/* vocabulary stack */

/*   CURRENT	( -- a ) */
/*		Point to the vocabulary to be extended. */

		__USER(7, "CURRENT", CRRNT)
		_USER = _USER+CELLL		/* vocabulary link pointer */

/*   CP		( -- a ) */
/*		Point to the top of the code dictionary. */

		__USER(2, "CP", CP)

/*   NP		( -- a ) */
/*		Point to the bottom of the name dictionary. */

		__USER(2, "NP", NP)

/*   LAST	( -- a ) */
/*		Point to the last name in the name dictionary. */

		__USER(4, "LAST", LAST)

/*  Common functions  */

/*   doVOC	( -- ) */
/*		Run time action of VOCABULARY's. */

		__COLON(COMPO+5,"doVOC",DOVOC)
		.long	RFROM,CNTXT,STORE,EXIT

/*   FORTH	( -- ) */
/*		Make FORTH the context vocabulary. */

		__COLON(5,"FORTH",FORTH)
		.long	DOVOC
		.long	0			/* vocabulary head pointer */
		.long	0			/* vocabulary link pointer */

/*   ?DUP	( w -- w w | 0 ) */
/*		Dup tos if its is not zero. */

		__COLON(4,"?DUP",QDUP)
		.long	DUPP
		.long	QBRAN,QDUP1
		.long	DUPP
QDUP1:		.long	EXIT

/*   ROT		( w1 w2 w3 -- w2 w3 w1 ) */
/*		Rot 3rd item to top. */

		__COLON(3,"ROT",ROT)
		.long	TOR,SWAP,RFROM,SWAP,EXIT

/*   2DROP	( w w -- ) */
/*		Discard two items on stack. */

		__COLON(5,"2DROP",DDROP)
		.long	DROP,DROP,EXIT

/*   2DUP	( w1 w2 -- w1 w2 w1 w2 ) */
/*		Duplicate top two items. */

		__COLON(4,"2DUP",DDUP)
		.long	OVER,OVER,EXIT

/*   +		( w w -- sum ) */
/*		Add top two items. */

		__CODE(1,"+",PLUS)
		__ENTER
		popl	%eax
#ifdef BUFFER_TOS
		addl	%eax,%ebx
#else
		addl	%eax,(%esp)
#endif
		__NEXT

/*   NOT		( w -- w ) */
/*		One's complement of tos. */

		__COLON(3,"NOT",INVER)
		.long	DOLIT,-1,XORR,EXIT

/*   NEGATE	( n -- -n ) */
/*		Two's complement of tos. */

		__COLON(6,"NEGATE",NEGAT)
		.long	INVER,DOLIT,1,PLUS,EXIT

/*   DNEGATE	( d -- -d ) */
/*		Two's complement of top double. */

		__COLON(7,"DNEGATE",DNEGA)
		.long	INVER,TOR,INVER
		.long	DOLIT,1,UPLUS
		.long	RFROM,PLUS,EXIT

/*   -		( n1 n2 -- n1-n2 ) */
/*		Subtraction. */

		__COLON(1,"-",SUBB)
		.long	NEGAT,PLUS,EXIT

/*   ABS		( n -- n ) */
/*		Return the absolute value of n. */

		__COLON(3,"ABS",ABSS)
		.long	DUPP,ZLESS
		.long	QBRAN,ABS1
		.long	NEGAT
ABS1:		.long	EXIT

/*   =		( w w -- t ) */
/*		Return true if top two are equal. */

		__CODE(1,"=",EQUAL)
		__ENTER
		popl	%eax
#ifdef BUFFER_TOS
		cmpl	%eax,%ebx
		jne	sfalse
		movl	$TRUEE,%ebx
#else
		cmpl	%eax,(%esp)
		jne	sfalse
		movl	$TRUEE,(%esp)
#endif
		__NEXT

		/*
		.long	XORR
		.long	QBRAN,EQU1
		.long	DOLIT,0,EXIT
EQU1:		.long	DOLIT,TRUEE,EXIT
		*/

/*   U<		( u u -- t ) */
/*		Unsigned compare of top two items. */

		__COLON(2,"U<",ULESS)
		.long	DDUP,XORR,ZLESS
		.long	QBRAN,ULES1
		.long	SWAP,DROP,ZLESS,EXIT
ULES1:		.long	SUBB,ZLESS,EXIT

/*   <		( n1 n2 -- t ) */
/*		Signed compare of top two items. */

		__CODE(1,"<",LESS)
		__ENTER
		popl	%eax
#ifdef BUFFER_TOS
		cmpl	%ebx,%eax	# beware the inversion !!!
		jge	sfalse
		movl	$TRUEE,%ebx
#else
		cmpl	%eax,(%esp)	# in the order of operands
		jge	sfalse
		movl	$TRUEE,(%esp)
#endif
		__NEXT
sfalse:
#ifdef BUFFER_TOS
		movl	$0,%ebx
#else
		movl	$0,(%esp)
#endif
		__NEXT

/*   MAX		( n n -- n ) */
/*		Return the greater of two top stack items. */

		__COLON(3,"MAX",MAX)
		.long	DDUP,LESS
		.long	QBRAN,MAX1
		.long	SWAP
MAX1:		.long	DROP,EXIT

/*   MIN		( n n -- n ) */
/*		Return the smaller of top two stack items. */

		__COLON(3,"MIN",MIN)
		.long	DDUP,SWAP,LESS
		.long	QBRAN,MIN1
		.long	SWAP
MIN1:		.long	DROP,EXIT

/*   WITHIN	( u ul uh -- t ) */
/*		Return true if u is within the range of ul and uh. ( ul <= u < uh ) */

		__COLON(6,"WITHIN",WITHI)
		.long	OVER,SUBB,TOR
		.long	SUBB,RFROM,ULESS,EXIT

/*  Divide */

/*   UM/MOD	( udl udh un -- ur uq ) */
/*		Unsigned divide of a double by a single. Return mod and quotient. */

		__COLON(6,"UM/MOD",UMMOD)
		.long	DDUP,ULESS
		.long	QBRAN,UMM4
		.long	NEGAT,DOLIT,NBPW-1,TOR
UMM1:		.long	TOR,DUPP,UPLUS
		.long	TOR,TOR,DUPP,UPLUS
		.long	RFROM,PLUS,DUPP
		.long	RFROM,RAT,SWAP,TOR
		.long	UPLUS,RFROM,ORR
		.long	QBRAN,UMM2
		.long	TOR,DROP,DOLIT,1,PLUS,RFROM
		.long	BRAN,UMM3
UMM2:		.long	DROP
UMM3:		.long	RFROM
		.long	DONXT,UMM1
		.long	DROP,SWAP,EXIT
UMM4:		.long	DROP,DDROP
		.long	DOLIT,-1,DUPP,EXIT

/*   M/MOD	( d n -- r q ) */
/*		Signed floored divide of double by single. Return mod and quotient. */

		__COLON(5,"M/MOD",MSMOD)
		.long	DUPP,ZLESS,DUPP,TOR
		.long	QBRAN,MMOD1
		.long	NEGAT,TOR,DNEGA,RFROM
MMOD1:		.long	TOR,DUPP,ZLESS
		.long	QBRAN,MMOD2
		.long	RAT,PLUS
MMOD2:		.long	RFROM,UMMOD,RFROM
		.long	QBRAN,MMOD3
		.long	SWAP,NEGAT,SWAP
MMOD3:		.long	EXIT

/*   /MOD	( n n -- r q ) */
/*		Signed divide. Return mod and quotient. */

		__COLON(4,"/MOD",SLMOD)
		.long	OVER,ZLESS,SWAP,MSMOD,EXIT

/*   MOD		( n n -- r ) */
/*		Signed divide. Return mod only. */

		__COLON(3,"MOD",MODD)
		.long	SLMOD,DROP,EXIT

/*   /		( n n -- q ) */
/*		Signed divide. Return quotient only. */

		__COLON(1,"/",SLASH)
		.long	SLMOD,SWAP,DROP,EXIT

/*  Multiply */

/*   UM*		( u u -- ud ) */
/*		Unsigned multiply. Return double product. */

		__COLON(3,"UM*",UMSTA)
		.long	DOLIT,0,SWAP,DOLIT,NBPW-1,TOR
UMST1:		.long	DUPP,UPLUS,TOR,TOR
		.long	DUPP,UPLUS,RFROM,PLUS,RFROM
		.long	QBRAN,UMST2
		.long	TOR,OVER,UPLUS,RFROM,PLUS
UMST2:		.long	DONXT,UMST1
		.long	ROT,DROP,EXIT

/*   *		( n n -- n ) */
/*		Signed multiply. Return single product. */

		__COLON(1,"*",STAR)
		.long	UMSTA,DROP,EXIT

/*   M*		( n n -- d ) */
/*		Signed multiply. Return double product. */

		__COLON(2,"M*",MSTAR)
		.long	DDUP,XORR,ZLESS,TOR
		.long	ABSS,SWAP,ABSS,UMSTA
		.long	RFROM
		.long	QBRAN,MSTA1
		.long	DNEGA
MSTA1:		.long	EXIT

/*   *_/MOD	( n1 n2 n3 -- r q ) */
/*		Multiply n1 and n2, then divide by n3. Return mod and quotient. */

		__COLON(5,"*/MOD",SSMOD)
		.long	TOR,MSTAR,RFROM,MSMOD,EXIT

/*   *_/		( n1 n2 n3 -- q ) */
/*		Multiply n1 by n2, then divide by n3. Return quotient only. */

		__COLON(2,"*/",STASL)
		.long	SSMOD,SWAP,DROP,EXIT

/* Miscellaneous */

/*   CELL+	( a -- a ) */
/*		Add cell size in byte to address. */

		__CODE(5,"CELL+",CELLP)
		__ENTER
#ifdef BUFFER_TOS
		addl	$CELLL,%ebx
#else
		addl	$CELLL,(%esp)
#endif
		__NEXT

/*   CELL-	( a -- a ) */
/*		Subtract cell size in byte from address. */

		__CODE(5,"CELL-",CELLM)
		__ENTER
#ifdef BUFFER_TOS
		subl	$CELLL,%ebx
#else
		subl	$CELLL,(%esp)
#endif
		__NEXT

/*   CELLS	( n -- n ) */
/*		Multiply tos by cell size in bytes. */

		__CODE(5,"CELLS",CELLS)
		__ENTER
#ifdef BUFFER_TOS
		sall	$2,%ebx		# LOG(CELLL)
#else
		sall	$2,(%esp)	# 2 = LOG(CELLL)
#endif
		__NEXT

/*   ALIGNED	( b -- a ) */
/*		Align address to the cell boundary. */

		__COLON(7,"ALIGNED",ALGND)
		.long	DUPP,DOLIT,0,DOLIT,CELLL
		.long	UMMOD,DROP,DUPP
		.long	QBRAN,ALGN1
		.long	DOLIT,CELLL,SWAP,SUBB
ALGN1:		.long	PLUS,EXIT

/*   BL		( -- 32 ) */
/*		Return 32, the blank character. */

		__COLON(2,"BL",BLANK)
		.long	DOLIT,32,EXIT

/*   >CHAR	( c -- c ) */
/*		Filter non-printing characters. */

		__COLON(5,">CHAR",TCHAR)
		.long	DOLIT,0x7F,ANDD,DUPP	/* mask msb */
		.long	DOLIT,0x7F,BLANK,WITHI	/* check for printable */
		.long	QBRAN,TCHA1
		.long	DROP,DOLIT,95		/* replace non-printables */
TCHA1:		.long	EXIT

/*   DEPTH	( -- n ) */
/*		Return the depth of the data stack. */

		__COLON(5,"DEPTH",DEPTH)
		.long	SPAT,SZERO,AT,SWAP,SUBB
		.long	DOLIT,CELLL,SLASH,EXIT

/*   PICK	( ... +n -- ... w ) */
/*		Copy the nth stack item to tos. */

		__COLON(4,"PICK",PICK)
		.long	DOLIT,1,PLUS,CELLS
		.long	SPAT,PLUS,AT,EXIT

/* Memory access */

/*   +!		( n a -- ) */
/*		Add n to the contents at address a. */

		__COLON(2,"+!",PSTOR)
		.long	SWAP,OVER,AT,PLUS
		.long	SWAP,STORE,EXIT

/*   2!		( d a -- ) */
/*		Store the double integer to address a. */

		__COLON(2,"2!",DSTOR)
		.long	SWAP,OVER,STORE
		.long	CELLP,STORE,EXIT

/*   2@		( a -- d ) */
/*		Fetch double integer from address a. */

		__COLON(2,"2@",DAT)
		.long	DUPP,CELLP,AT
		.long	SWAP,AT,EXIT

/*   COUNT	( b -- b +n ) */
/*		Return count byte of a string and add 1 to byte address. */

		__COLON(5,"COUNT",COUNT)
		.long	DUPP,DOLIT,1,PLUS
		.long	SWAP,CAT,EXIT

/*   HERE	( -- a ) */
/*		Return the top of the code dictionary. */

		__COLON(4,"HERE",HERE)
		.long	CP,AT,EXIT

/*   PAD		( -- a ) */
/*		Return the address of the text buffer above the code dictionary. */

		__COLON(3,"PAD",PAD)
		.long	HERE,DOLIT,80,PLUS,EXIT

/*   TIB		( -- a ) */
/*		Return the address of the terminal input buffer. */

		__COLON(3,"TIB",TIB)
		.long	NTIB,CELLP,AT,EXIT

/*   @EXECUTE	( a -- ) */
/*		Execute vector stored in address a. */

		__COLON(8,"@EXECUTE",ATEXE)
		.long	AT,QDUP			/* ?address or zero */
		.long	QBRAN,EXE1
		.long	EXECU			/* execute if non-zero */
EXE1:		.long	EXIT			/* do nothing if zero */

/*   CMOVE	( b1 b2 u -- ) */
/*		Copy u bytes from b1 to b2. */

		__CODE(5,"CMOVE",CMOVE)
		__ENTER
#ifdef BUFFER_TOS
		mov	%ebx,%ecx
#else
		popl	%ecx
#endif
		popl	%edi
		popl	%eax
		pushl	%esi
		movl	%eax,%esi
		rep
		movsb
		popl	%esi
#ifdef BUFFER_TOS
		popl	%ebx
#endif
		__NEXT

/*   FILL	( b u c -- ) */
/*		Fill u bytes of character c to area beginning at b. */

		__CODE(4,"FILL",FILL)
		__ENTER
#ifdef BUFFER_TOS
		mov	%ebx,%eax
#else
		popl	%eax
#endif
		popl	%ecx
		popl	%edi
		rep
		stosb
#ifdef BUFFER_TOS
		popl	%ebx
#endif
		__NEXT
		/*
		.long	SWAP,TOR,SWAP
		.long	BRAN,FILL2
FILL1:		.long	DDUP,CSTOR,DOLIT,1,PLUS
FILL2:		.long	DONXT,FILL1
		.long	DDROP,EXIT
		*/

/*   -TRAILING	( b u -- b u ) */
/*		Adjust the count to eliminate trailing white space. */

		__COLON(9,"-TRAILING",DTRAI)
		.long	TOR
		.long	BRAN,DTRA2
DTRA1:		.long	BLANK,OVER,RAT,PLUS,CAT,LESS
		.long	QBRAN,DTRA2
		.long	RFROM,DOLIT,1,PLUS,EXIT
DTRA2:		.long	DONXT,DTRA1
		.long	DOLIT,0,EXIT

/*   PACK$	( b u a -- a ) */
/*		Build a counted string with u characters from b. Null fill. */

		__COLON(5,"PACK$",PACKS)
		.long	ALGND,DUPP,TOR		/* strings only on cell boundary */
		.long	OVER,DUPP,DOLIT,0
		.long	DOLIT,CELLL,UMMOD,DROP	/* count mod cell */
		.long	SUBB,OVER,PLUS
		.long	DOLIT,0,SWAP,STORE	/* null fill cell */
		.long	DDUP,CSTOR,DOLIT,1,PLUS /* save count */
		.long	SWAP,CMOVE,RFROM,EXIT	/* move string */

/* Numeric output, single precision */

/*   DIGIT	( u -- c ) */
/*		Convert digit u to a character. */

		__COLON(5,"DIGIT",DIGIT)
		.long	DOLIT,9,OVER,LESS
		.long	DOLIT,7,ANDD,PLUS
		.long	DOLIT,48,PLUS,EXIT

/*   EXTRACT	( n base -- n c ) */
/*		Extract the least significant digit from n. */

		__COLON(7,"EXTRACT",EXTRC)
		.long	DOLIT,0,SWAP,UMMOD
		.long	SWAP,DIGIT,EXIT

/*   <#		( -- ) */
/*		Initiate the numeric output process. */

		__COLON(2,"<#",BDIGS)
		.long	PAD,HLD,STORE,EXIT

/*   HOLD	( c -- ) */
/*		Insert a character into the numeric output string. */

		__COLON(4,"HOLD",HOLD)
		.long	HLD,AT,DOLIT,1,SUBB
		.long	DUPP,HLD,STORE,CSTOR,EXIT

/*   #		( u -- u ) */
/*		Extract one digit from u and append the digit to output string. */

		__COLON(1,"#",DIG)
		.long	BASE,AT,EXTRC,HOLD,EXIT

/*   #S		( u -- 0 ) */
/*		Convert u until all digits are added to the output string. */

		__COLON(2,"#S",DIGS)
DIGS1:		.long	DIG,DUPP
		.long	QBRAN,DIGS2
		.long	BRAN,DIGS1
DIGS2:		.long	EXIT

/*   SIGN	( n -- ) */
/*		Add a minus sign to the numeric output string. */

		__COLON(4,"SIGN",SIGN)
		.long	ZLESS
		.long	QBRAN,SIGN1
		.long	DOLIT,45,HOLD
SIGN1:		.long	EXIT

/*   #>		( w -- b u ) */
/*		Prepare the output string to be TYPE'd. */

		__COLON(2,"#>",EDIGS)
		.long	DROP,HLD,AT
		.long	PAD,OVER,SUBB,EXIT

/*   str		( w -- b u ) */
/*		Convert a signed integer to a numeric string. */

		__COLON(3,"str",STR)
		.long	DUPP,TOR,ABSS
		.long	BDIGS,DIGS,RFROM
		.long	SIGN,EDIGS,EXIT

/*   HEX		( -- ) */
/*		Use radix 16 as base for numeric conversions. */

		__COLON(3,"HEX",HEX)
		.long	DOLIT,16,BASE,STORE,EXIT

/*   DECIMAL	( -- ) */
/*		Use radix 10 as base for numeric conversions. */

		__COLON(7,"DECIMAL",DECIM)
		.long	DOLIT,10,BASE,STORE,EXIT

/*  Numeric input, single precision */

/*   DIGIT?	( c base -- u t ) */
/*		Convert a character to its numeric value. A flag indicates success. */

		__COLON(6,"DIGIT?",DIGTQ)
		.long	TOR,DOLIT,48,SUBB
		.long	DOLIT,9,OVER,LESS
		.long	QBRAN,DGTQ1
		.long	DOLIT,7,SUBB
		.long	DUPP,DOLIT,10,LESS,ORR
DGTQ1:		.long	DUPP,RFROM,ULESS,EXIT

/*   NUMBER?	( a -- n T | a F ) */
/*		Convert a number string to integer. Push a flag on tos. */

		__COLON(7,"NUMBER?",NUMBQ)
		.long	BASE,AT,TOR,DOLIT,0,OVER,COUNT
		.long	OVER,CAT,DOLIT,36,EQUAL
		.long	QBRAN,NUMQ1
		.long	HEX,SWAP,DOLIT,1,PLUS
		.long	SWAP,DOLIT,1,SUBB
NUMQ1:		.long	OVER,CAT,DOLIT,45,EQUAL,TOR
		.long	SWAP,RAT,SUBB,SWAP,RAT,PLUS,QDUP
		.long	QBRAN,NUMQ6
		.long	DOLIT,1,SUBB,TOR
NUMQ2:		.long	DUPP,TOR,CAT,BASE,AT,DIGTQ
		.long	QBRAN,NUMQ4
		.long	SWAP,BASE,AT,STAR,PLUS,RFROM
		.long	DOLIT,1,PLUS
		.long	DONXT,NUMQ2
		.long	RAT,SWAP,DROP
		.long	QBRAN,NUMQ3
		.long	NEGAT
NUMQ3:		.long	SWAP
		.long	BRAN,NUMQ5
NUMQ4:		.long	RFROM,RFROM,DDROP,DDROP,DOLIT,0
NUMQ5:		.long	DUPP
NUMQ6:		.long	RFROM,DDROP
		.long	RFROM,BASE,STORE,EXIT

/*  Basic I/O */

/*   ?KEY	( -- c T | F ) */
/*		Return input character and true, or a false if no input. */

		__COLON(4,"?KEY",QKEY)
		.long	TQKEY,ATEXE,EXIT

/*   KEY		( -- c ) */
/*		Wait for and return an input character. */

		__COLON(3,"KEY",KEY)
KEY1:		.long	QKEY
		.long	QBRAN,KEY1
		.long	EXIT

/*   EMIT	( c -- ) */
/*		Send a character to the output device. */

		__COLON(4,"EMIT",EMIT)
		.long	TEMIT,ATEXE,EXIT

/*   NUF?	( -- t ) */
/*		Return false if no input, else pause and if CR return true. */

		__COLON(4,"NUF?",NUFQ)
		.long	QKEY,DUPP
		.long	QBRAN,NUFQ1
		.long	DDROP,KEY
		.long	DUPP,DOLIT,LF,EQUAL,SWAP
		.long	DOLIT,_CR_,EQUAL,ORR
NUFQ1:		.long	EXIT

/*   PACE	( -- ) */
/*		Send a pace character for the file downloading process. */

		__COLON(4,"PACE",PACE)
		.long	DOLIT,11,EMIT,EXIT

/*   SPACE	( -- ) */
/*		Send the blank character to the output device. */

		__COLON(5,"SPACE",SPACE)
		.long	BLANK,EMIT,EXIT

/*   SPACES	( +n -- ) */
/*		Send n spaces to the output device. */

		__COLON(6,"SPACES",SPACS)
		.long	DOLIT,0,MAX,TOR
		.long	BRAN,CHAR2
CHAR1:		.long	SPACE
CHAR2:		.long	DONXT,CHAR1
		.long	EXIT

/*   TYPE	( b u -- ) */
/*		Output u characters from b. */

		__COLON(4,"TYPE",TYPES)
		.long	TOR
		.long	BRAN,TYPE2
TYPE1:		.long	DUPP,CAT,EMIT
		.long	DOLIT,1,PLUS
TYPE2:		.long	DONXT,TYPE1
		.long	DROP,EXIT

/*   CR		( -- ) */
/*		Output a carriage return and a line feed. */

		__COLON(2,"CR",CR)
/*		.long	DOLIT,_CR_,EMIT	# no need for this one under unix */
		.long	DOLIT,LF,EMIT
		.long	EXIT

/*   do$		( -- a ) */
/*		Return the address of a compiled string. */

		__COLON(COMPO+3,"do$",DOSTR)
		.long	RFROM,RAT,RFROM,COUNT,PLUS
		.long	ALGND,TOR,SWAP,TOR,EXIT

/*   $"|		( -- a ) */
/*		Run time routine compiled by $". Return address of a compiled string. */

		__COLON(COMPO+3,"$\"|",STRQP)
		.long	DOSTR,EXIT		/* force a call to do$ */

/*   ."|		( -- ) */
/*		Run time routine of ." . Output a compiled string. */

		__COLON(COMPO+3,".\"|",DOTQP)
		.long	DOSTR,COUNT,TYPES,EXIT

/*   .R		( n +n -- ) */
/*		Display an integer in a field of n columns, right justified. */

		__COLON(2,".R",DOTR)
		.long	TOR,STR,RFROM,OVER,SUBB
		.long	SPACS,TYPES,EXIT

/*   U.R		( u +n -- ) */
/*		Display an unsigned integer in n column, right justified. */

		__COLON(3,"U.R",UDOTR)
		.long	TOR,BDIGS,DIGS,EDIGS
		.long	RFROM,OVER,SUBB
		.long	SPACS,TYPES,EXIT

/*   U.		( u -- ) */
/*		Display an unsigned integer in free format. */

		__COLON(2,"U.",UDOT)
		.long	BDIGS,DIGS,EDIGS
		.long	SPACE,TYPES,EXIT

/*   .		( w -- ) */
/*		Display an integer in free format, preceeded by a space. */

		__COLON(1,".",DOT)
		.long	BASE,AT,DOLIT,10,XORR	/* ?decimal */
		.long	QBRAN,DOT1
		.long	UDOT,EXIT		/* no, display unsigned */
DOT1:		.long	STR,SPACE,TYPES,EXIT	/* yes, display signed */

/*   ?		( a -- ) */
/*		Display the contents in a memory cell. */

		__COLON(1,"?",QUEST)
		.long	AT,DOT,EXIT

/*  Parsing */

/*   parse	( b u c -- b u delta  <string> ) */
/*		Scan string delimited by c. Return found string and its offset.
 */

		__COLON(5,"parse",PARS)
		.long	TEMP,STORE,OVER,TOR,DUPP
		.long	QBRAN,PARS8
		.long	DOLIT,1,SUBB,TEMP,AT,BLANK,EQUAL
		.long	QBRAN,PARS3
		.long	TOR
PARS1:		.long	BLANK,OVER,CAT		/* skip leading blanks ONLY */
		.long	SUBB,ZLESS,INVER
		.long	QBRAN,PARS2
		.long	DOLIT,1,PLUS
		.long	DONXT,PARS1
		.long	RFROM,DROP,DOLIT,0,DUPP,EXIT
PARS2:		.long	RFROM
PARS3:		.long	OVER,SWAP
		.long	TOR
PARS4:		.long	TEMP,AT,OVER,CAT,SUBB	/* scan for delimiter */
		.long	TEMP,AT,BLANK,EQUAL
		.long	QBRAN,PARS5
		.long	ZLESS
PARS5:		.long	QBRAN,PARS6
		.long	DOLIT,1,PLUS
		.long	DONXT,PARS4
		.long	DUPP,TOR
		.long	BRAN,PARS7
PARS6:		.long	RFROM,DROP,DUPP
		.long	DOLIT,1,PLUS,TOR
PARS7:		.long	OVER,SUBB
		.long	RFROM,RFROM,SUBB,EXIT
PARS8:		.long	OVER,RFROM,SUBB,EXIT

/*   PARSE	( c -- b u   <string> ) */
/*		Scan input stream and return counted string delimited by c. */

		__COLON(5,"PARSE",PARSE)
		.long	TOR,TIB,INN,AT,PLUS	# current input buffer pointer
		.long	NTIB,AT,INN,AT,SUBB	# remaining count
		.long	RFROM,PARS
		.long	INN,PSTOR,EXIT

/*   .(		( -- ) */
/*		Output following string up to next ) . */

		__COLON(IMEDD+2,".(",DOTPR)
		.long	DOLIT,41,PARSE,TYPES,EXIT

/*   (		( -- ) */
/*		Ignore following string up to next ) . A comment. */

		__COLON(IMEDD+1,"(",PAREN)
		.long	DOLIT,41,PARSE,DDROP,EXIT

/*   \		( -- ) */
/*		Ignore following text till the end of line. */

		__COLON(IMEDD+1,"\\",BKSLA)
		.long	NTIB,AT,INN,STORE,EXIT

/*   CHAR	( -- c ) */
/*		Parse next word and return its first character. */

		__COLON(4,"CHAR",CHAR)
		.long	BLANK,PARSE,DROP,CAT,EXIT

/*   TOKEN	( -- a	 <string> ) */
/*		Parse a word from input stream and copy it to name dictionary. */

		__COLON(5,"TOKEN",TOKEN)
		.long	BLANK,PARSE
		.long	DOLIT,31,MIN
		.long	NP,AT,OVER,SUBB,CELLM
		.long	PACKS
		.long	EXIT

/*   WORD	( c -- a   <string> ) */
/*		Parse a word from input stream and copy it to code dictionary. */

		__COLON(4,"WORD",WORDD)
		.long	PARSE,HERE,PACKS,EXIT

/*  Dictionary search */

/*   NAME>	( na -- ca ) */
/*		Return a code address given a name address. */

		__COLON(5,"NAME>",NAMET)
		.long	CELLM,CELLM,AT,EXIT

/*   SAME?	( a a u -- a a f \ -0+ ) */
/*		Compare u cells in two strings. Return 0 if identical. */

		__COLON(5,"SAME?",SAMEQ)
		.long	TOR
		.long	BRAN,SAME2
SAME1:		.long	OVER,RAT,CELLS,PLUS,AT
		.long	OVER,RAT,CELLS,PLUS,AT
		.long	SUBB,QDUP
		.long	QBRAN,SAME2
		.long	RFROM,DROP,EXIT
SAME2:		.long	DONXT,SAME1
		.long	DOLIT,0,EXIT

/*   find	( a va -- ca na | a F ) */
/*		Search a vocabulary for a string. Return ca and na if succeeded. */

		__COLON(4,"find",FIND)
		.long	SWAP,DUPP,CAT
		.long	DOLIT,CELLL,SLASH,TEMP,STORE
		.long	DUPP,AT,TOR,CELLP,SWAP
FIND1:		.long	AT,DUPP
		.long	QBRAN,FIND6
		.long	DUPP,AT,DOLIT,MASKK,ANDD,RAT,XORR
		.long	QBRAN,FIND2
		.long	CELLP,DOLIT,-1
		.long	BRAN,FIND3
FIND2:		.long	CELLP,TEMP,AT,SAMEQ
FIND3:		.long	BRAN,FIND4
FIND6:		.long	RFROM,DROP
		.long	SWAP,CELLM,SWAP,EXIT
FIND4:		.long	QBRAN,FIND5
		.long	CELLM,CELLM
		.long	BRAN,FIND1
FIND5:		.long	RFROM,DROP,SWAP,DROP
		.long	CELLM
		.long	DUPP,NAMET,SWAP,EXIT

/*   NAME?	( a -- ca na | a F ) */
/*		Search all context vocabularies for a string. */

		__COLON(5,"NAME?",NAMEQ)
		.long	CNTXT,DUPP,DAT,XORR
		.long	QBRAN,NAMQ1
		.long	CELLM
NAMQ1:		.long	TOR
NAMQ2:		.long	RFROM,CELLP,DUPP,TOR
		.long	AT,QDUP
		.long	QBRAN,NAMQ3
		.long	FIND,QDUP
		.long	QBRAN,NAMQ2
		.long	RFROM,DROP,EXIT
NAMQ3:		.long	RFROM,DROP
		.long	DOLIT,0,EXIT

/*  Terminal response */

/*   ^H		( bot eot cur -- bot eot cur ) */
/*		Backup the cursor by one character. */

		__COLON(2,"^H",BKSP)
		.long	TOR,OVER,RFROM,SWAP,OVER,XORR
		.long	QBRAN,BACK1
		.long	DOLIT,BKSPP,TECHO,ATEXE,DOLIT,1,SUBB
		.long	BLANK,TECHO,ATEXE
		.long	DOLIT,BKSPP,TECHO,ATEXE
BACK1:		.long	EXIT

/*   TAP		( bot eot cur c -- bot eot cur ) */
/*		Accept and echo the key stroke and bump the cursor. */
/*   [Faré]	not echoing the key stroke anymore */

		__COLON(3,"TAP",TAP)
/*		  .long	  DUPP,TECHO,ATEXE */
		.long	OVER,CSTOR,DOLIT,1,PLUS,EXIT

/*   kTAP	( bot eot cur c -- bot eot cur ) */
/*		Process a key stroke, CR or backspace. */

		__COLON(4,"kTAP",KTAP)
		.long	DUPP,DOLIT,LF,XORR	/* was _CR_ instead of LF */
		.long	QBRAN,KTAP2
		.long	DOLIT,DELETE,XORR	/* was BKSPP, not DELETE */
		.long	QBRAN,KTAP1
		.long	BLANK,TAP,EXIT
KTAP1:		.long	BKSP,EXIT
KTAP2:		.long	DROP,SWAP,DROP,DUPP,EXIT

/*   accept	( b u -- b u ) */
/*		Accept characters to input buffer. Return with actual count. */

		__COLON(6,"accept",ACCEP)
		.long	OVER,PLUS,OVER
ACCP1:		.long	DDUP,XORR
		.long	QBRAN,ACCP4
		.long	KEY,DUPP
/*		.long	BLANK,SUBB,DOLIT,95,ULESS */
		.long	BLANK,DOLIT,127,WITHI
		.long	QBRAN,ACCP2
		.long	TAP
		.long	BRAN,ACCP3
ACCP2:		.long	TTAP,ATEXE
ACCP3:		.long	BRAN,ACCP1
ACCP4:		.long	DROP,OVER,SUBB,EXIT

/*   EXPECT	( b u -- ) */
/*		Accept input stream and store count in SPAN. */

		__COLON(6,"EXPECT",EXPEC)
		.long	TEXPE,ATEXE,SPAN,STORE,DROP,EXIT

/*   QUERY	( -- ) */
/*		Accept input stream to terminal input buffer. */

		__COLON(5,"QUERY",QUERY)
		.long	TIB,DOLIT,80,TEXPE,ATEXE,NTIB,STORE
		.long	DROP,DOLIT,0,INN,STORE,EXIT

/*  Error handling */

/*   CATCH	( ca -- 0 | err# ) */
/*		Execute word at ca and set up an error frame for it. */

		__COLON(5,"CATCH",CATCH)
		.long	SPAT,TOR,HANDL,AT,TOR	/* save error frame */
		.long	RPAT,HANDL,STORE,EXECU	/* execute */
		.long	RFROM,HANDL,STORE	/* restore error frame */
		.long	RFROM,DROP,DOLIT,0,EXIT /* no error */

/*   THROW	( err# -- err# ) */
/*		Reset system to current local error frame an update error flag. */

		__COLON(5,"THROW",THROW)
		.long	HANDL,AT,RPSTO		/* restore return stack */
		.long	RFROM,HANDL,STORE	/* restore handler frame */
		.long	RFROM,SWAP,TOR,SPSTO	/* restore data stack */
		.long	DROP,RFROM,EXIT

/*   NULL$	( -- a ) */
/*		Return address of a null string with zero count. */

		__COLON(5,"NULL$",NULLS)
		.long	DOVAR			/* emulate CREATE */
		.long	0
		.byte	99,111,121,111,116,101
		/* [Faré] why is it written "coyote" ??? */

/*   ABORT	( -- ) */
/*		Reset data stack and jump to QUIT. */

		__COLON(5,"ABORT",ABORT)
		.long	NULLS,THROW

/*   abort"	( f -- ) */
/*		Run time routine of ABORT" . Abort with a message. */

		__COLON(COMPO+6,"abort\"",ABORQ)
		.long	QBRAN,ABOR1		/* text flag */
		.long	DOSTR,THROW		/* pass error string */
ABOR1:		.long	DOSTR,DROP,EXIT		/* drop error */

/*  The text interpreter */

/*   $INTERPRET ( a -- ) */
/*		Interpret a word. If failed, try to convert it to an integer. */

		__COLON(10,"$INTERPRET",INTER)
		.long	NAMEQ,QDUP		# ?defined
		.long	QBRAN,INTE1
		.long	AT,DOLIT,COMPO,ANDD	# ?compile only lexicon bits
		.long	ABORQ
		.byte	13
		.ascii	" compile only"
		__align
		.long	EXECU,EXIT		/* execute defined word */
INTE1:		.long	TNUMB,ATEXE		/* convert a number */
		.long	QBRAN,INTE2
		.long	EXIT
INTE2:		.long	THROW			/* error */

/*   [		( -- ) */
/*		Start the text interpreter. */

		__COLON(IMEDD+1,"[",LBRAC)
		.long	DOLIT,INTER,TEVAL,STORE,EXIT

/*   .OK		( -- ) */
/*		Display 'ok' only while interpreting. */

		__COLON(3,".OK",DOTOK)
		.long	DOLIT,INTER,TEVAL,AT,EQUAL
		.long	QBRAN,DOTO1
		.long	DOTQP
		.byte	3
		.ascii	" Ok"
		__align
DOTO1:		.long	CR,EXIT

/*   ?STACK	( -- ) */
/*		Abort if the data stack underflows. */

		__COLON(6,"?STACK",QSTAC)
		.long	DEPTH,ZLESS		/* check only for underflow */
		.long	ABORQ
		.byte	10
		.ascii	" underflow"
		__align
		.long	EXIT

/*   EVAL	( -- ) */
/*		Interpret the input stream. */

		__COLON(4,"EVAL",EVAL)
EVAL1:		.long	TOKEN,DUPP,CAT		# ?input stream empty
		.long	QBRAN,EVAL2
		.long	TEVAL,ATEXE,QSTAC	# evaluate input, check stack
		.long	BRAN,EVAL1
EVAL2:		.long	DROP,TPROM,ATEXE,EXIT	/* prompt */

/*  Shell */

/*   PRESET	( -- ) */
/*		Reset data stack pointer and the terminal input buffer. */

		__COLON(6,"PRESET",PRESE)
		.long	SZERO,AT,SPSTO
		.long	DOLIT,TIBB,NTIB,CELLP,STORE
		.long	EXIT

/*   xio		( a a a -- ) */
/*		Reset the I/O vectors 'EXPECT, 'TAP, 'ECHO and 'PROMPT. */

		__COLON(COMPO+3,"xio",XIO)
		.long	DOLIT,ACCEP,TEXPE,DSTOR
		.long	TECHO,DSTOR,EXIT

/*   FILE	( -- ) */
/*		Select I/O vectors for file download. */

		__COLON(4,"FILE",FILE)
		.long	DOLIT,PACE,DOLIT,DROP
		.long	DOLIT,KTAP,XIO,EXIT

/*   HAND	( -- ) */
/*		Select I/O vectors for terminal interface. */

		__COLON(4,"HAND",HAND)
		.long	DOLIT,DOTOK,DOLIT,EMIT
		.long	DOLIT,KTAP,XIO,EXIT

/*   I/O		( -- a ) */
/*		Array to store default I/O vectors. */

		__COLON(3,"I/O",ISLO)
		.long	DOVAR			/* emulate CREATE */
		.long	QRX,TXSTO		/* default I/O vectors */

/*   CONSOLE	( -- ) */
/*		Initiate terminal interface. */

		__COLON(7,"CONSOLE",CONSO)
		.long	ISLO,DAT,TQKEY,DSTOR	/* restore default I/O device */
		.long	HAND,EXIT		/* keyboard input */

/*   QUIT	( -- ) */
/*		Reset return stack pointer and start text interpreter. */

		__COLON(4,"QUIT",QUIT)
		.long	RZERO,AT,RPSTO		/* reset return stack pointer */
QUIT1:		.long	LBRAC			/* start interpretation */
QUIT2:		.long	QUERY			/* get input */
		.long	DOLIT,EVAL,CATCH,QDUP	/* evaluate input */
		.long	QBRAN,QUIT2		/* continue till error */
		.long	TPROM,AT,TOR		/* save input device */
		.long	CONSO,NULLS,OVER,XORR	/* ?display error message */
		.long	QBRAN,QUIT3
		.long	SPACE,COUNT,TYPES	/* error message */
		.long	DOTQP
		.byte	3
		.ascii	" ? "
		__align
QUIT3:		.long	RFROM,DOLIT,DOTOK,XORR	/* ?file input */
		.long	QBRAN,QUIT4
		.long	DOLIT,ERR,EMIT		/* file error, tell host */
QUIT4:		.long	PRESE			/* some cleanup */
		.long	BRAN,QUIT1

/* The compiler */

/*   '		( -- ca ) */
/*		Search context vocabularies for the next word in input stream. */

		__COLON(1,"'",TICK)
		.long	TOKEN,NAMEQ		/* ?defined */
		.long	QBRAN,TICK1
		.long	EXIT			/* yes, push code address */
TICK1:		.long	THROW			/* no, error */

/*   ALLOT	( n -- ) */
/*		Allocate n bytes to the code dictionary. */

		__COLON(5,"ALLOT",ALLOT)
		.long	CP,PSTOR,EXIT		/* adjust code pointer */

/*   ,		( w -- ) */
/*		Compile an integer into the code dictionary. */

		__COLON(1,",",COMMA)
		.long	HERE,DUPP,CELLP		/* cell boundary */
		.long	CP,STORE,STORE,EXIT	/* adjust code pointer and compile */

/*   [COMPILE]	( --  <string> ) */
/*		Compile the next immediate word into code dictionary. */

		__COLON(IMEDD+9,"[COMPILE]",BCOMP)
		.long	TICK,COMMA,EXIT

/*   COMPILE	( -- ) */
/*		Compile the next address in colon list to code dictionary. */

		__COLON(COMPO+7,"COMPILE",COMPI)
		.long	RFROM,DUPP,AT,COMMA	/* compile address */
		.long	CELLP,TOR,EXIT		/* adjust return address */

/*   LITERAL	( w -- ) */
/*		Compile tos to code dictionary as an integer literal. */

		__COLON(IMEDD+7,"LITERAL",LITER)
		.long	COMPI,DOLIT,COMMA,EXIT

/*   $,"		( -- ) */
/*		Compile a literal string up to next " . */

		__COLON(3,"$,\"",STRCQ)
		.long	DOLIT,34,WORDD		/* move string to code dictionary */
		.long	COUNT,PLUS,ALGND	/* calculate aligned end of string */
		.long	CP,STORE,EXIT		/* adjust the code pointer */

/*   RECURSE	( -- ) */
/*		Make the current word available for compilation. */

		__COLON(IMEDD+7,"RECURSE",RECUR)
		.long	LAST,AT,NAMET,COMMA,EXIT

/*  Structures */

/*   FOR		( -- a ) */
/*		Start a FOR-NEXT loop structure in a colon definition. */

		__COLON(IMEDD+3,"FOR",FOR)
		.long	COMPI,TOR,HERE,EXIT

/*   BEGIN	( -- a ) */
/*		Start an infinite or indefinite loop structure. */

		__COLON(IMEDD+5,"BEGIN",BEGIN)
		.long	HERE,EXIT

/*   NEXT	( a -- ) */
/*		Terminate a FOR-NEXT loop structure. */

		__COLON(IMEDD+4,"NEXT",NEXT)
		.long	COMPI,DONXT,COMMA,EXIT

/*   UNTIL	( a -- ) */
/*		Terminate a BEGIN-UNTIL indefinite loop structure. */

		__COLON(IMEDD+5,"UNTIL",UNTIL)
		.long	COMPI,QBRAN,COMMA,EXIT

/*   AGAIN	( a -- ) */
/*		Terminate a BEGIN-AGAIN infinite loop structure. */

		__COLON(IMEDD+5,"AGAIN",AGAIN)
		.long	COMPI,BRAN,COMMA,EXIT

/*   IF		( -- A ) */
/*		Begin a conditional branch structure. */

		__COLON(IMEDD+2,"IF",IFF)
		.long	COMPI,QBRAN,HERE
		.long	DOLIT,0,COMMA,EXIT

/*   AHEAD	( -- A ) */
/*		Compile a forward branch instruction. */

		__COLON(IMEDD+5,"AHEAD",AHEAD)
		.long	COMPI,BRAN,HERE,DOLIT,0,COMMA,EXIT

/*   REPEAT	( A a -- ) */
/*		Terminate a BEGIN-WHILE-REPEAT indefinite loop. */

		__COLON(IMEDD+6,"REPEAT",REPEA)
		.long	AGAIN,HERE,SWAP,STORE,EXIT

/*   THEN	( A -- ) */
/*		Terminate a conditional branch structure. */

		__COLON(IMEDD+4,"THEN",THENN)
		.long	HERE,SWAP,STORE,EXIT

/*   AFT		( a -- a A ) */
/*		Jump to THEN in a FOR-AFT-THEN-NEXT loop the first time through. */

		__COLON(IMEDD+3,"AFT",AFT)
		.long	DROP,AHEAD,BEGIN,SWAP,EXIT

/*   ELSE	( A -- A ) */
/*		Start the false clause in an IF-ELSE-THEN structure. */

		__COLON(IMEDD+4,"ELSE",ELSEE)
		.long	AHEAD,SWAP,THENN,EXIT

/*   WHILE	( a -- A a ) */
/*		Conditional branch out of a BEGIN-WHILE-REPEAT loop. */

		__COLON(IMEDD+5,"WHILE",WHILE)
		.long	IFF,SWAP,EXIT

/*   ABORT"	( --  <string> ) */
/*		Conditional abort with an error message. */

		__COLON(IMEDD+6,"ABORT\"",ABRTQ)
		.long	COMPI,ABORQ,STRCQ,EXIT

/*   $"		( --  <string> ) */
/*		Compile an inline string literal. */

		__COLON(IMEDD+2,"$\"",STRQ)
		.long	COMPI,STRQP,STRCQ,EXIT

/*   ."		( --   <string> ) */
/*		Compile an inline string literal to be typed out at run time. */

		__COLON(IMEDD+2,".\"",DOTQ)
		.long	COMPI,DOTQP,STRCQ,EXIT

/*  Name compiler */

/*   ?UNIQUE	( a -- a ) */
/*		Display a warning message if the word already exists. */

		__COLON(7,"?UNIQUE",UNIQU)
		.long	DUPP,NAMEQ		/* ?name exists */
		.long	QBRAN,UNIQ1
		.long	DOTQP			/* redefinitions are OK */
		.byte	7
		.ascii	" reDef "		/* but the user should be warned */
		__align
		.long	OVER,COUNT,TYPES	/* just in case its not planned */
UNIQ1:		.long	DROP,EXIT

/*   $,n		( na -- ) */
/*		Build a new dictionary name using the string at na. */

		__COLON(3,"$,n",SNAME)
		.long	DUPP,CAT		/* ?null input */
		.long	QBRAN,PNAM1
		.long	UNIQU			/* ?redefinition */
		.long	DUPP,LAST,STORE		/* save na for vocabulary link */
		.long	HERE,ALGND,SWAP		/* align code address */
		.long	CELLM			/* link address */
		.long	CRRNT,AT,AT,OVER,STORE
		.long	CELLM,DUPP,NP,STORE	/* adjust name pointer */
		.long	STORE,EXIT		/* save code pointer */
PNAM1:		.long	STRQP
		.byte	5
		.ascii	" name"			/* null input */
		__align
		.long	THROW

/*  FORTH compiler */

/*   $COMPILE	( a -- ) */
/*		Compile next word to code dictionary as a token or literal. */

		__COLON(8,"$COMPILE",SCOMP)
		.long	NAMEQ,QDUP		/* ?defined */
		.long	QBRAN,SCOM2
		.long	AT,DOLIT,IMEDD,ANDD	/* ?immediate */
		.long	QBRAN,SCOM1
		.long	EXECU,EXIT		/* its immediate, execute */
SCOM1:		.long	COMMA,EXIT		/* its not immediate, compile */
SCOM2:		.long	TNUMB,ATEXE		/* try to convert to number */
		.long	QBRAN,SCOM3
		.long	LITER,EXIT		/* compile number as integer */
SCOM3:		.long	THROW			/* error */

/*   OVERT	( -- ) */
/*		Link a new word into the current vocabulary. */

		__COLON(5,"OVERT",OVERT)
		.long	LAST,AT,CRRNT,AT,STORE,EXIT

/*		( -- ) */
/*		Terminate a colon definition. */

		__COLON(IMEDD+COMPO+1,";",SEMIS)
		.long	COMPI,EXIT,LBRAC,OVERT,EXIT

/*   ]		( -- ) */
/*		Start compiling the words in the input stream. */

		__COLON(1,"]",RBRAC)
		.long	DOLIT,SCOMP,TEVAL,STORE,EXIT

/*   call,	( ca -- ) */
/*		Assemble a call instruction to ca. */

		__COLON(5,"call,",CALLC)
		.long	DOLIT,CALLL,COMMA,HERE	/* Direct Threaded Code */
		.long	CELLP,SUBB,COMMA,EXIT	/* DTC 8086 relative call */

/*   :		( --   <string> ) */
/*		Start a new colon definition using next word as its name. */

		__COLON(1,":",COLON)
		.long	TOKEN,SNAME,DOLIT,DOLST
		.long	CALLC,RBRAC,EXIT

/*   IMMEDIATE	( -- ) */
/*		Make the last compiled word an immediate word. */

		__COLON(9,"IMMEDIATE",IMMED)
		.long	DOLIT,IMEDD,LAST,AT,AT,ORR
		.long	LAST,AT,STORE,EXIT

/* Defining words */

/*   USER	( u --	 <string> ) */
/*		Compile a new user variable. */

		__COLON(4,"USER",USER)
		.long	TOKEN,SNAME,OVERT
		.long	DOLIT,DOLST,CALLC
		.long	DOLIT,DOUSE,COMMA
		.long	COMMA,EXIT

/*   CREATE	( --   <string> ) */
/*		Compile a new array entry without allocating code space. */

		__COLON(6,"CREATE",CREAT)
		.long	TOKEN,SNAME,OVERT
		.long	DOLIT,DOLST,CALLC
		.long	DOLIT,DOVAR,COMMA,EXIT

/*   VARIABLE	( --   <string> ) */
/*		Compile a new variable initialized to 0. */

		__COLON(8,"VARIABLE",VARIA)
		.long	CREAT,DOLIT,0,COMMA,EXIT

/* Tools */

/*   _TYPE	( b u -- ) */
/*		Display a string. Filter non-printing characters. */

		__COLON(5,"_TYPE",UTYPE)
		.long	TOR			/* start count down loop */
		.long	BRAN,UTYP2		/* skip first pass */
UTYP1:		.long	DUPP,CAT,TCHAR,EMIT	/* display only printable */
		.long	DOLIT,1,PLUS		/* increment address */
UTYP2:		.long	DONXT,UTYP1		/* loop till done */
		.long	DROP,EXIT

/*   dm+		( a u -- a ) */
/*		Dump u bytes from , leaving a+u on the stack. */

		__COLON(3,"dm+",DUMPP)
		.long	OVER,DOLIT,4,UDOTR	/* display address */
		.long	SPACE,TOR		/* start count down loop */
		.long	BRAN,PDUM2		/* skip first pass */
PDUM1:		.long	DUPP,CAT,DOLIT,3,UDOTR	/* display numeric data */
		.long	DOLIT,1,PLUS		/* increment address */
PDUM2:		.long	DONXT,PDUM1		/* loop till done */
		.long	EXIT

/*   DUMP	( a u -- ) */
/*		Dump u bytes from a, in a formatted manner. */

		__COLON(4,"DUMP",DUMP)
		.long	BASE,AT,TOR,HEX		/* save radix, set hex */
		.long	DOLIT,16,SLASH		/* change count to lines */
		.long	TOR			/* start count down loop */
DUMP1:		.long	CR,DOLIT,16,DDUP,DUMPP	/* display numeric */
		.long	ROT,ROT
		.long	DOLIT,2,SPACS,UTYPE	/* display printable characters */
		.long	NUFQ,INVER		/* user control */
		.long	QBRAN,DUMP2
		.long	DONXT,DUMP1		/* loop till done */
		.long	BRAN,DUMP3
DUMP2:		.long	RFROM,DROP		/* cleanup loop stack, early exit */
DUMP3:		.long	DROP,RFROM,BASE,STORE	/* restore radix */
		.long	EXIT

/*   .S		( ... -- ... ) */
/*		Display the contents of the data stack. */

		__COLON(2,".S",DOTS)
		.long	CR,DEPTH		/* stack depth */
		.long	TOR			/* start count down loop */
		.long	BRAN,DOTS2		/* skip first pass */
DOTS1:		.long	RAT,PICK,DOT		/* index stack, display contents */
DOTS2:		.long	DONXT,DOTS1		/* loop till done */
		.long	DOTQP
		.byte	4
		.ascii	" <sp"
		__align
		.long	EXIT

/*   !CSP	( -- ) */
/*		Save stack pointer in CSP for error checking. */

		__COLON(4,"!CSP",STCSP)
		.long	SPAT,CSP,STORE,EXIT	/* save pointer */

/*   ?CSP	( -- ) */
/*		Abort if stack pointer differs from that saved in CSP. */

		__COLON(4,"?CSP",QCSP)
		.long	SPAT,CSP,AT,XORR	/* compare pointers */
		.long	ABORQ			/* abort if different */
		.byte	6
		.ascii	"stacks"
		__align
		.long	EXIT

/*   >NAME	( ca -- na | F ) */
/*		Convert code address to a name address. */

		__COLON(5,">NAME",TNAME)
		.long	CRRNT			/* vocabulary link */
TNAM1:		.long	CELLP,AT,QDUP		/* check all vocabularies */
		.long	QBRAN,TNAM4
		.long	DDUP
TNAM2:		.long	AT,DUPP			/* ?last word in a vocabulary */
		.long	QBRAN,TNAM3
		.long	DDUP,NAMET,XORR		/* compare */
		.long	QBRAN,TNAM3
		.long	CELLM			/* continue with next word */
		.long	BRAN,TNAM2
TNAM3:		.long	SWAP,DROP,QDUP
		.long	QBRAN,TNAM1
		.long	SWAP,DROP,SWAP,DROP,EXIT
TNAM4:		.long	DROP,DOLIT,0,EXIT

/*   .ID		( na -- ) */
/*		Display the name at address. */

		__COLON(3,".ID",DOTID)
		.long	QDUP			/* if zero no name */
		.long	QBRAN,DOTI1
		.long	COUNT,DOLIT,0x1F,ANDD	/* mask lexicon bits */
		.long	UTYPE,EXIT		/* display name string */
DOTI1:		.long	DOTQP
		.byte	9
		.ascii	" {noName}"
		__align
		.long	EXIT

/*   SEE		( --   <string> ) */
/*		A simple decompiler. */

		__COLON(3,"SEE",SEE)
		.long	TICK			/* starting address */
		.long	CR,CELLP
SEE1:		.long	CELLP,DUPP,AT,DUPP	/* ?does it contain a zero */
		.long	QBRAN,SEE2
		.long	TNAME			/* ?is it a name */
SEE2:		.long	QDUP			/* name address or zero */
		.long	QBRAN,SEE3
		.long	SPACE,DOTID		/* display name */
		.long	BRAN,SEE4
SEE3:		.long	DUPP,AT,UDOT		/* display number */
SEE4:		.long	NUFQ			/* user control */
		.long	QBRAN,SEE1
		.long	DROP,EXIT

/*   WORDS	( -- ) */
/*		Display the names in the context vocabulary. */

		__COLON(5,"WORDS",WORDS)
		.long	CR,CNTXT,AT		/* only in context */
WORS1:		.long	AT,QDUP			/* ?at end of list */
		.long	QBRAN,WORS2
		.long	DUPP,SPACE,DOTID	/* display a name */
		.long	CELLM
		.long	NUFQ		  /* user control */
		.long	QBRAN,WORS1
		.long	DROP
WORS2:		.long	EXIT

/*  Hardware reset */

/*   VER		( -- n ) */
/*		Return the version number of this implementation. */

		__COLON(3,"VER",VERSN)
		.long	DOLIT,VER*256+EXT,EXIT


/*   hi		( -- ) */
/*		Initialize I/O; print system version. */

		__COLON(2,"hi",HI)
		.long	STOIO			/* initialize I/O */
		.long	CR,DOTQP		/* Show version */
		.byte	11
		.ascii	"eForth v"
		.byte	VER+48			/* model */
		.ascii	"."
		.byte	EXT+48			/* version */
		__align
		.long	CR
		.long	EXIT


/*   'BOOT	( -- a ) */
/*		The application startup vector. */

		__COLON(5,"'BOOT",TBOOT)
		.long	DOVAR
		.long	STOIO			/* application to boot */
/* [Faré]
 * was "hi" instead of "STOIO", to show a welcome message.
 * hi must now be called explicitly.
 * Thus, we can have eForth used as filter or script,
 * that won't print a nasty "eForth v1.0" everytimes
 */

/*   COLD	( -- ) */
/*		The hilevel cold start sequence. */

		__COLON(4,"COLD",COLD)
COLD1:		.long	DOLIT,UZERO,DOLIT,UPP
		.long	DOLIT,ULAST-UZERO,CMOVE /* initialize user area */
		.long	PRESE			/* initialize data stack and TIB */
		.long	TBOOT,ATEXE		/* application boot */
		.long	FORTH,CNTXT,AT,DUPP	/* initialize search order */
		.long	CRRNT,DSTOR,OVERT
		.long	QUIT			/* start interpretation */
		.long	BRAN,COLD1		/* just in case */

/*===============================================================*/
	__DATASEG__
DICTENDMARK:
	.long	0,0	/* mark end of dictionary */

LASTN	=	_NAME	/* last name address in name dictionary */

NTOP	=	NAMEE	/* next available memory in name dictionary */

	__CODESEG__

	__aligncode
CTOP	=	.	/* next available memory in code dictionary */
