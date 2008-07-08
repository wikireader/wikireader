/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 * 
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: vea_oea_emb.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*  
 *   PowerPCアーキテクチャVEA,OEA依存の定義
 *   　The IBM PowerPC Embedded Environmentの場合
 *   　　IBM系PowerPC40xファミリこちらに該当する。
 *   　
 *   　PowerPCアーキテクチャの定義は以下の３つのレベルから成る
 *   　・USIA:User Instruction Set Architecture
 *   　・VEA: Virtual Environment Architecture
 *   　・OEA: Operating Environment Architecture
 *   　
 *   　USIAについては全機種共通であるが、VEAとOEAについては
 *   　オリジナルのPowerPCアーキテクチャとThe IBM PowerPC 
 *   　Embedded Environmentそれぞれ別に定義されているため、
 *   　VEAとOEAの定義はファイルを分けて、それをインクルード
 *   　している。
 */   


#ifndef _VEA_OEA_EMB_H_
#define _VEA_OEA_EMB_H_

/*  
 *  レジスタ番号の定義
 */   

/*
 *  Special Purpose Registerのレジスタ番号
 *  　　(*)印：元のPowerPCアーキテクチャでは定義されていないレジスタ
 */

/*
 *  例外処理に関するレジスタ
 */
#define SRR0	  26	/* Save/Restore Register0  */
#define SRR1	  27	/* Save/Restore Register1  */
#define SRR2	 990	/* Save/Restore Register2(*)  */
#define SRR3	 991	/* Save/Restore Register3(*)  */
#define ESR	 980	/* Exception Syndrome Register(*)  */
#define DEAR	 981	/* Data Exception Address Register(*)  */
#define EVPR	 982	/* Exception Vector Prefix Register(*)  */

#define SPRG0	 272	/* SPR General 0  */
#define SPRG1	 273	/* SPR General 1  */
#define SPRG2	 274	/* SPR General 2  */
#define SPRG3	 275	/* SPR General 3  */

/*
 *  その他のレジスタ
 */
#define DAC	1014	/* Data Address Compare(*) */
#define DBCR	1010	/* Debug Control Register(*) */
#define DBSR	1008	/* Debug Status Register(*) */
#define DCCR	1018	/* Data Cache Cacheability Register(*) */
#define DCWR	 954	/* Data Cache Write-thru Register(*) */

#define IAC	1012	/* Instruction Address Compare(*) */
#define ICCR	1019	/* Instruction Cache Cacheability Register(*) */

#define PID	 945	/* Process ID Register  */
#define PIT	 987	/* Programmable Interval Timer(*) */
#define PVR	 287	/* Processor Version Register  */

#define SGR	 953	/* Storage Guarded Register(*) */
#define SLER	 955	/* Storage Little-Endian Register(*) */
#define SMR	 952	/* Storage Memory-Coherent Register(*) */

/*  VEA  */
#define TBL	 268	/* Time Base Lower(for read)  */
#define TBU	 269	/* Time Base Upper(for read)  */
/*  OEA  */
#define TBLw	 284	/* Time Base Lower(for write)  */
#define TBUw	 285	/* Time Base Upper(for write)  */

#define TCR	 986	/* Timer Control Registe(*) */
#define TSR	 984	/* Timer Status Register(*) */
#define ZPR	 944	/* Zone Protection Register(*) */

/*
 *  MSRのビット配列
 */
/*  0-10:Reserved  */
#define MSR_APE	BIT11_32	/* Auxiliary Processor Exception Enable */
#define MSR_APA	BIT12_32	/* Auxiliary Processor Available */
#define MSR_WE	BIT13_32	/* Wait State Enable */
#define MSR_CE	BIT14_32	/* Critical Enable */
#define MSR_ILE	BIT15_32	/* Interrupt Little Endian */
#define MSR_EE	BIT16_32	/* External Enable */
#define MSR_PR	BIT17_32	/* Problem State */
#define MSR_FP	BIT18_32	/* Floating Point Available */
#define MSR_ME	BIT19_32	/* Machine Check Enable */
#define MSR_FE0	BIT20_32	/* Floating Point Exception Mode 0 */
/*  21:Reserved  */
#define MSR_DE	BIT22_32	/* Debug Interrupts Enable */
#define MSR_FE1	BIT23_32	/* Floating Point Exception Mode 1 */
/*  23-25:Reserved  */
#define MSR_IR	BIT26_32	/* Instruction Relocate */
#define MSR_DR	BIT27_32	/* Data Relocate */
/*  28-30:Reserved  */
#define MSR_LE	BIT31_32	/* Little Endian */


/*
 *  ESR(Exception Syndrome Register)のビット配列
 */
#define ESR_PIL	BIT4_32	 /* Program - Illegal Instruction exception */
#define ESR_PPR	BIT5_32	 /* Program - Privileged Instruction exception */
#define ESR_PTR	BIT6_32	 /* Program - Trap exception */
#define ESR_PFE	BIT7_32	 /* Program - Floating Point Enabled exception */
#define ESR_DST	BIT8_32	 /* Data Storage / Data TLB Miss - Store Operations */
#define ESR_DIZ	BIT9_32	 /* Data / Instruction Storage - Zone exception */
	/* Program - Auxiliary Processor Unavailable exception */
#define ESR_PAU	BIT12_32
	/* Program - Floating Point Enabled but Unimplemented exception */
#define ESR_PFEU BIT13_32
	/* Program - Auxiliary Processor Enabled exception */
#define ESR_PAE	BIT14_32


/*  
 *   CPU例外要因の定義
 *   　番号が不連続なのでCPU例外擬似ベクタテーブルに一部無駄が入るが、
 *   　PowerPCアーキテクチャの定義に合わせる方を優先した。
 */   
#define EXC_NO_CRITICAL_INPUT           	0x1
#define EXC_NO_MACHINE_CHECK            	0x2
#define EXC_NO_DATA_STORAGE             	0x3
#define EXC_NO_INSTRUCTION_STORAGE      	0x4
#define EXC_NO_EXTERNAL_INTERRUPT       	0x5   /*  外部割込み  */
#define EXC_NO_ALIGNMENT                	0x6
			/*  プログラム例外（要因７種）*/
#define EXC_NO_PROGRAM                  	0x7
#define EXC_NO_FLOATING_POINT_UNAVAILABLE 	0x8
/*  （APUを持つPowerPC405、440のみ）  */

/*  0x900：Reserved  */
/*  0xa00：Reserved  */
/*  0xb00：Reserved  */
#define EXC_NO_SYSTEM_CALL              	0xc	/*  システムコール  */
/*  0xd00：Reserved  */
/*  0xe00：Reserved  */
/*  0xe10-0xff0：Reserved  */
/* 0x1000：Programmable Interval Timer  */
/* 0x1010：Fixed Interval Timer  */
/* 0x1020：Watchdog Timer  */
/* 0x1030-0x10f0：Reserved  */
/* 0x1100：Data TLB miss  */
/* 0x1110-0x11f0：Reserved  */
/* 0x1200：Instruction TLB miss  */
/* 0x1210-0x1ff0：Reserved  */
/* 0x2000：Debug（要因７種）   */
/* 0x2010-0x2ff0：Implementation Specific  */

/*  
 *   例外の種別数
 *   　外部割込みも１つと数える
 *   　　0番は未使用
 *   　　　・例外ベクタのオフセットと対応
 *   　　　・外部割り込みも１つと数える
 *   　　　・配列宣言のため、+1している
 */   

#define TMAX_EXCNO       ( 0x20 + 1 + NUM_IMPLEMENT_EXCEPTION )


/*
 *  DCRへのアクセス
 *     DCR:Device Control Register
 *     以下のルーチンはデバイスドライバ向けに作成している。
 *     DCRの具体的な名称や機能はデバイス依存である。
 *     
 *     備考
 *     　mfdcr,mtdcr命令はDCRの番号を汎用レジスタではなく、
 *     　即値で指定するため、インライン関数にできない。
 */

/*
 *  DCRの現在値の読出し
 *     reg:レジスタ番号（整数定数）
 *     val:読み出した値を格納する変数（UW型）
 */
#define _sil_rew_dcr(reg, val)	Asm("mfdcr %0," #reg : "=r"(val))
#define sil_rew_dcr(reg, val)	_sil_rew_dcr(reg, val)

/*
 *  DCRの現在値の変更
 *     reg:レジスタ番号（整数定数）
 *     val:設定する値が格納された変数（UW型）
 */
#define _sil_wrw_dcr(reg, val)	Asm("mtdcr "#reg",%0" : : "r"(val) )
#define sil_wrw_dcr(reg, val)	_sil_wrw_dcr(reg, val)


#endif /* _VEA_OEA_EMB_H_ */
/*  end of file  */
