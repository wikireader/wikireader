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
 *  @(#) $Id: vea_oea.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*  
 *   PowerPCアーキテクチャVEA,OEA依存の定義
 *   　オリジナルのPowerPCアーキテクチャの場合
 *   　　モトローラMPCシリーズ、IPM PowerPC6xx/7xxシリーズは
 *   　　こちらに該当する。
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

#ifndef _VEA_OEA_H_
#define _VEA_OEA_H_

/*  
 *  レジスタ番号の定義
 */   

/*  
 *  Configuration Register
 */   
#define PVR	287	/*  プロセッサ・バージョン・レジスタ  */

/*  
 *  Memory Management Registers
 */   
/*  Instruction BAT Registers  */
#define IBAT0U	528
#define IBAT0L	529
#define IBAT1U	530
#define IBAT1L	531
#define IBAT2U	532
#define IBAT2L	533
#define IBAT3U	534
#define IBAT3L	535

/*  Instruction BAT Registers  */
#define DBAT0U	536
#define DBAT0L	537
#define DBAT1U	538
#define DBAT1L	539
#define DBAT2U	540
#define DBAT2L	541
#define DBAT3U	542
#define DBAT3L	543

#define SDR1	 25
#define ASR	280

/*  
 *  Exception Handling Registers
 */   
#define DAR	 19	/*  Data Address Register  */

#define SPRG0   272     /*  特殊レジスタ：OS用  */
#define SPRG1   273     /*  特殊レジスタ：OS用  */
#define SPRG2   274     /*  特殊レジスタ：OS用  */
#define SPRG3   275     /*  特殊レジスタ：OS用  */

#define DSISR    18     /*  DSIステータス・レジスタ(*)  */
                        /*  　DSI：データ・ストア割込み  */
#define SRR0     26     /*  待避／復帰レジスタ0  */
#define SRR1     27     /*  待避／復帰レジスタ1  */

#define FPECR	1022	/*  Floating-Point Exception Cause Register  */


/*  
 *  Miscellaneous Registers
 */   
/*  Time Base Facility(OEA:for Writing)  */
#define TBLw	284
#define TBUw	285

/*  Time Base Facility(VEA:for Reading)  */
#define TBL	268
#define TBU	269

#define DEC      22     /*  ディクリメント・レジスタ(*)  */

#define PIR	1023	/*  Processor Identification Register  */

#define DABR	1013	/*  DATA Address Breakpint Register(option)  */
#define EAR	 282	/*  Exception Access Register(option)  */



/*  
 *   ビット番号の定義
 *   　注意：PowerPCのビット番号は通常と逆になっている
 *   　　　　また、32ビットレジスタと16ビットレジスタでは
 *   　　　　最下位ビットのビット番号が異なる点にも注意
 */   

/*  マシンステータスレジスタの各ビットの定義  */
/*  　注意：32ビット・インプリメント専用  */
#define MSR_POW BIT13_32    /*  パワーマネージメント・イネーブル  */
#define MSR_ILE BIT15_32    /*  例外処理時のエンディアンモード  */
                            /*  　例外受付時にMSR.LE←MSR.ILE  */
#define MSR_EE  BIT16_32    /*  外部割込みイネーブル  */
#define MSR_PR  BIT17_32    /*  特権レベル  */
                            /*  　0：スーパバイザ・レベル  */
                            /*  　1：ユーザ・レベル  */
#define MSR_FP  BIT18_32    /*  浮動小数点イネーブル  */
#define MSR_ME  BIT19_32    /*  マシンチェック・イネーブル  */
#define MSR_FE0 BIT20_32    /*  浮動小数点例外モード0  */
#define MSR_FE1 BIT23_32    /*  浮動小数点例外モード1  */
                            /*  　注意：番号が不連続  */
#define MSR_SE  BIT21_32    /*  シングルステップトレース・イネーブル  */
#define MSR_BE  BIT22_32    /*  分岐トレース・イネーブル  */
#define MSR_IP  BIT25_32    /*  例外プリフィックス  */
                            /*  　0：0x000n,nnnn  */
                            /*  　1：0xfffn,nnnn  */
#define MSR_IR  BIT26_32    /*  命令アドレス変換イネーブル  */
#define MSR_DR  BIT27_32    /*  データアドレス変換イネーブル  */
#define MSR_RI  BIT30_32    /*  回復可能な例外  */
                            /*  　0:回復不可能  */
                            /*  　1:回復可能  */
#define MSR_LE  BIT31_32    /*  リトルエンディアン・モード  */
                            /*  　0：ビッグエンディアン  */
                            /*  　1：リトルエンディアン  */
/*  0で予約されているビットのためのマスク  */
#define MSR_MASK    ~(BIT0_32 | BIT1_32 | BIT2_32 | BIT3_32 | BIT4_32 \
                    | BIT5_32 | BIT6_32 | BIT7_32 | BIT8_32 | BIT9_32 \
                    | BIT10_32 | BIT11_32 | BIT12_32 | BIT24_32 \
                    | BIT28_32 | BIT29_32)


/*  
 *   CPU例外要因の定義
 *   　番号が不連続なのでCPU例外擬似ベクタテーブルに一部無駄が入るが、
 *   　PowerPCアーキテクチャの定義に合わせる方を優先した。
 */   
#define EXC_NO_SYSTEM_RESET             0x1   /*  システムリセット例外  */
#define EXC_NO_MACHINE_CHECK            0x2   /*  マシン・チェック例外  */
#define EXC_NO_DSI                      0x3   /*  DSI例外  */
                                        /*  （データ・メモリ・アクセス）  */
#define EXC_NO_ISI                      0x4   /*  ISI例外（命令フェッチ）  */
#define EXC_NO_EXTERNAL_INTERRUPT       0x5   /*  外部割込み  */
#define EXC_NO_ALIGNMENT                0x6   /*  アライメント例外  */
#define EXC_NO_PROGRAM                  0x7   /*  プログラム例外  */
#define EXC_NO_FLOATING_POINT_UNAVAILABLE 0x8   /*  浮動小数点使用不可  */
#define EXC_NO_DECREMENTER              0x9   /*  デクリメンタ例外  */
                            /*  インプリメンテーション固有の例外0x00a00  */
#define EXC_NO_IMPLEMENT_EXCEPTION_00A00 0xa 
#define EXC_NO_SYSTEM_CALL              0xc /*  システムコール  */
#define EXC_NO_TRACE                    0xd /*  トレース（オプション）  */
#define EXC_NO_FLOATING_POINT_ASSIST    0xe /*  浮動小数点補助  */
                            /*  インプリメンテーション固有の例外0x01000  */
#define EXC_NO_IMPLEMENT_EXCEPTION_01000 0x10    


/*  
 *   例外の種別数
 *   　外部割込みも１つと数える
 *   　　0番は未使用
 *   　　　・例外ベクタのオフセットと対応
 *   　　　・外部割り込みも１つと数える
 *   　　　・配列宣言のため、+1している
 */   

#ifdef IMPLEMENT_EXCEPTION_01000    /*  例外ベクタ0x1000を使用する場合  */
#define TMAX_EXCNO       ( 0x10 + 1 )

#else /* IMPLEMENT_EXCEPTION_01000 */
#define TMAX_EXCNO       ( 0xe + 1 )

#endif /* IMPLEMENT_EXCEPTION_01000 */




#endif /* _VEA_OEA_H_ */
/*  end of file  */
