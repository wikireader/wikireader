#
#  TOPPERS/JSP Kernel
#      Toyohashi Open Platform for Embedded Real-Time Systems/
#      Just Standard Profile Kernel
#
#  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
#                              Toyohashi Univ. of Technology, JAPAN
#
#  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
#
#  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
#  によって公表されている GNU General Public License の Version 2 に記
#  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
#  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
#  利用と呼ぶ）することを無償で許諾する．
#  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
#      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
#      スコード中に含まれていること．
#  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
#      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
#      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
#      の無保証規定を掲載すること．
#  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
#      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
#      と．
#    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
#        作権表示，この利用条件および下記の無保証規定を掲載すること．
#    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
#        報告すること．
#  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
#      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
# 
#  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
#  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
#  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
#  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
#

# make file made by GWB33
#
# Numbered sections can be customized by user.
# Not numbered sections don't need to customize usually.


### 1. Specify target file name(except expantion), CPU, SYSTEM and development tool
TARGET= sample1
CPU= s1c33
SYS= dmt33e07
TOOL= gnu33

# Defin target file and necessary path
all: $(TARGET).elf

CFG=       $(SRCDIR)/cfg/cfg
SRCDIR=    ..
KER_DIR=   $(SRCDIR)/kernel
INC_DIR=   $(SRCDIR)/include
CPU_DIR=   $(SRCDIR)/config/$(CPU)
SYS_DIR=   $(SRCDIR)/config/$(CPU)/$(SYS)
GUILIB_DIR= ../../gui-lib/
WIKILIB_DIR_GENERIC= $(SRCDIR)/../wiki-lib/generic
WIKILIB_DIR_SPECIFIC= $(SRCDIR)/../wiki-lib/specific
FATLIB_DIR= $(SRCDIR)/../fatfs/src


### 2. If there are source files and so on in other directory, add Definitions
STASK_DIR= $(SRCDIR)/systask
UTASK_DIR= $(SRCDIR)/library
vpath %.c  $(CPU_DIR):$(SYS_DIR):$(KER_DIR):$(STASK_DIR):$(UTASK_DIR):\
	$(GUILIB_DIR):$(WIKILIB_DIR_GENERIC):$(WIKILIB_DIR_SPECIFIC):\
	$(FATLIB_DIR)

# Include CPU / target dependence definitions.
# following definitions are used later section.
#  KERNEL_LCOBJS   KERNEL_COBJS   KERNEL_CFLAGS   START_OBJS
#
include $(SRCDIR)/config/$(CPU)-$(TOOL)/$(SYS)/Makefile.config
include $(SRCDIR)/config/$(CPU)-$(TOOL)/Makefile.config
include $(SRCDIR)/kernel/Makefile.kernel


### 3. Regist object files of user program and necessary libraries.
KERNEL_COBJS:= $(KERNEL_COBJS) startup.o banner.o 

APPLI_COBJS= kernel_cfg.o sample1.o keyboard.o gui.o input.o 
STASK_COBJS= logtask.o serial.o timer.o
UTASK_COBJS= log_output.o strerror.o t_perror.o vasyslog.o
DBG_COBJS=   dbg_mon.o
DBG_ASMOBJS= dbg_mon_sub.o
GUILIB_COBJS= guilib.o glyph.o
WIKILIB_COBJS= msg-toppers.o file-io-libfat.o file-io.o time-toppers.o wikilib.o \
		malloc-simple.o search-test.o
FATLIB_COBJS= tff.o mmc.o fattime.o cache.o

ALL_COBJS=   $(START_OBJS) $(APPLI_COBJS) $(STASK_COBJS) $(UTASK_COBJS) \
	     $(DBG_COBJS) $(DBG_ASMOBJS) $(KERNEL_COBJS) $(WIKILIB_COBJS) \
	     $(GUILIB_COBJS) $(FATLIB_COBJS) $(KERNEL_LCOBJS)

OBJLDS= 

C_LIBDIR= $(SRCDIR)/../mini-libc/lib
LIBS= $(C_LIBDIR)/libc.a `$(CC) -print-libgcc-file-name`


### 4. Define compiler options, compilation rules and so on.
### Do not change the name about following 5 macros.
INCLUDES:= $(INCLUDES) -I. -I$(INC_DIR) 	\
	-I$(SRCDIR)/../mini-libc/include	\
	-I$(SRCDIR)/../fatfs/src 		\
	-I$(SRCDIR)/../fatfs/config/c33		\
	-I$(SRCDIR)/../gui-lib			\
	-I$(SRCDIR)/../wiki-lib/include		\
	-I$(SRCDIR)/../bootloader
CDEFS:= $(CDEFS) -DTPS_ENA_LOGGING
CFLAGS=  $(COPTS) $(CDEFS) $(INCLUDES)
#ASFLAGS= -B$(TOPDIR)/ -c -xassembler-with-cpp -Wa,--gstabs -mc33pe
ASFLAGS= -mc33pe
LDFLAGS= -T $(SRCDIR)/config/$(LDSCRIPT) -Map $(TARGET).map -N 
#--strip-all
KERNEL_CFLAGS:= $(KERNEL_CFLAGS) -I$(KER_DIR)

$(STASK_COBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $<

$(UTASK_COBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $<

$(KERNEL_COBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(KERNEL_CFLAGS) $<

# Generate kernel_cfg.c, kernel_id.h, kernel_chk.c(configurator generates)
kernel_cfg.c kernel_id.h kernel_chk.c: $(TARGET).cfg
	$(CC) -E $(CFLAGS) -x c-header $(TARGET).cfg > tmpfile1
	$(CFG) -s tmpfile1 -c -obj -cpu $(CPU) -system $(SYS)
	$(RM) -f tmpfile1

# Link all object files and libraries.
$(TARGET).elf : $(ALL_COBJS)
	$(LD) $(LDFLAGS) -o $@ $(ALL_COBJS) $(OBJLDS) $(LIBS)

clean:
	$(RM) -f *.o
	$(RM) -f kernel_*.c kernel_id.h kernel_obj.dat
	$(RM) -f $(TARGET).elf $(TARGET).map

