/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: h8_3048f.h,v 1.7 2007/03/23 07:58:33 honda Exp $
 */

#ifndef _H8_3048F_H_
#define _H8_3048F_H_

/*
 *  H8/3048F用定義
 *     H8/3048F-ONEと共用である。
 *     両者の違いは、フラッシュROM関連の制御レジスタ群の扱いであるが、
 *     このファイルにはフラッシュROM関連の制御レジスタ群は定義していない
 *     （実行中にフラッシュROMを書き換える使い方は想定していない）
 *
 *     int型が16ビットのため、アドレス値にはすべてLを付加している。
 */

/* Interrupt numbers */

#define IRQ_NMI                 7u               /* NMI */

#define IRQ_EXT0                12u              /* IRQ0 */
#define IRQ_EXT1                13u              /* IRQ1 */
#define IRQ_EXT2                14u              /* IRQ2 */
#define IRQ_EXT3                15u              /* IRQ3 */
#define IRQ_EXT4                16u              /* IRQ4 */
#define IRQ_EXT5                17u              /* IRQ5 */

#define IRQ_WOVI                20u              /* Watch Doc Timer */

#define IRQ_CMI                 21u              /* Compare Match */

#define IRQ_IMIA0               24u              /* ITU0 IMIA0 */
#define IRQ_IMIB0               25u              /* ITU0 IMIB0 */
#define IRQ_OVI0                26u              /* ITU0 OVI0  */

#define IRQ_IMIA1               28u              /* ITU1 IMIA1 */
#define IRQ_IMIB1               29u              /* ITU1 IMIB1 */
#define IRQ_OVI1                30u              /* ITU1 OVI1  */

#define IRQ_IMIA2               32u              /* ITU2 IMIA2 */
#define IRQ_IMIB2               33u              /* ITU2 IMIB2 */
#define IRQ_OVI2                34u              /* ITU2 OVI2  */

#define IRQ_IMIA3               36u              /* ITU3 IMIA3 */
#define IRQ_IMIB3               37u              /* ITU3 IMIB3 */
#define IRQ_OVI3                38u              /* ITU3 OVI3  */

#define IRQ_IMIA4               40u              /* ITU4 IMIA4 */
#define IRQ_IMIB4               41u              /* ITU4 IMIB4 */
#define IRQ_OVI4                42u              /* ITU4 OVI4  */

#define IRQ_DEND0A              44u              /* DMAC */
#define IRQ_DEND0B              45u              /* DMAC */
#define IRQ_DEND1A              46u              /* DMAC */
#define IRQ_DEND1B              47u              /* DMAC */

#define IRQ_ERI0                52u              /* SCI0 ERI */
#define IRQ_RXI0                53u              /* SCI0 RXI */
#define IRQ_TXI0                54u              /* SCI0 TXI */
#define IRQ_TEI0                55u              /* SCI0 TEI */

#define IRQ_ERI1                56u              /* SCI1 ERI */
#define IRQ_RXI1                57u              /* SCI1 RXI */
#define IRQ_TXI1                58u              /* SCI1 TXI */
#define IRQ_TEI1                59u              /* SCI1 TEI */

#define IRQ_ADI                 60u              /* A/D */

/* register address */

/* I/O ports */

/* port1: A0  - A7  */

#define H8P1DDR                 0xffffc0ul
#define H8P1DR                  0xffffc2ul

/* port2: A8  - A15 */

#define H8P2DDR                 0xffffc1ul
#define H8P2DR                  0xffffc3ul
#define H8P2PCR                 0xffffd8ul

/* port3: D8  - D15 */

#define H8P3DDR                 0xffffc4ul
#define H8P3DR                  0xffffc6ul

/* port4: D0  - D7  */

#define H8P4DDR                 0xffffc5ul
#define H8P4DR                  0xffffc7ul
#define H8P4PCR                 0xffffdaul

/* port5: A16 - A19 */

#define H8P5DDR                 0xffffc8ul
#define H8P5DR                  0xffffcaul
#define H8P5PCR                 0xffffdbul

#define H8P5DDR_A19_BIT         3u
#define H8P5DDR_A18_BIT         2u
#define H8P5DDR_A17_BIT         1u
#define H8P5DDR_A16_BIT         0u

#define H8P5DDR_A19             (1u<<H8P5DDR_A19_BIT)
#define H8P5DDR_A18             (1u<<H8P5DDR_A18_BIT)
#define H8P5DDR_A17             (1u<<H8P5DDR_A17_BIT)
#define H8P5DDR_A16             (1u<<H8P5DDR_A16_BIT)

/* port6 */

#define H8P6DDR                 0xffffc9ul
#define H8P6DR                  0xffffcbul

#define H8P6DDR_HWR_BIT         6u
#define H8P6DDR_LWR_BIT         5u
#define H8P6DDR_RD_BIT          4u
#define H8P6DDR_AS_BIT          3u
#define H8P6DDR_BACK_BIT        2u
#define H8P6DDR_BREQ_BIT        1u
#define H8P6DDR_WAIT_BIT        0u

#define H8P6DDR_HWR             (1u<<H8P6DDR_HWR_BIT)
#define H8P6DDR_LWR             (1u<<H8P6DDR_LWR_BIT)
#define H8P6DDR_RD              (1u<<H8P6DDR_RD_BIT)
#define H8P6DDR_AS              (1u<<H8P6DDR_AS_BIT)
#define H8P6DDR_BACK            (1u<<H8P6DDR_BACK_BIT)
#define H8P6DDR_BREQ            (1u<<H8P6DDR_BREQ_BIT)
#define H8P6DDR_WAIT            (1u<<H8P6DDR_WAIT_BIT)

/* port7 */

#define H8P7DR                  0xffffceul

/* port8 */

#define H8P8DDR                 0xffffcdul
#define H8P8DR                  0xffffcful

#define H8P8DDR_CS0_BIT         4u
#define H8P8DDR_CS1_BIT         3u
#define H8P8DDR_CS2_BIT         2u
#define H8P8DDR_CS3_BIT         1u
#define H8P8DDR_RFSH_BIT        0u

#define H8P8DDR_CS0             (1u<<H8P8DDR_CS0_BIT)
#define H8P8DDR_CS1             (1u<<H8P8DDR_CS1_BIT)
#define H8P8DDR_CS2             (1u<<H8P8DDR_CS2_BIT)
#define H8P8DDR_CS3             (1u<<H8P8DDR_CS3_BIT)
#define H8P8DDR_RFSH            (1u<<H8P8DDR_RFSH_BIT)

#define H8P8DDR_IRQ3_BIT        3u
#define H8P8DDR_IRQ2_BIT        2u
#define H8P8DDR_IRQ1_BIT        1u
#define H8P8DDR_IRQ0_BIT        0u

#define H8P8DDR_IRQ3            (1u<<H8P8DDR_IRQ3_BIT)
#define H8P8DDR_IRQ2            (1u<<H8P8DDR_IRQ2_BIT)
#define H8P8DDR_IRQ1            (1u<<H8P8DDR_IRQ1_BIT)
#define H8P8DDR_IRQ0            (1u<<H8P8DDR_IRQ0_BIT)

/* port9 (SCI) */

#define H8P9DDR                 0xffffd0ul
#define H8P9DR                  0xffffd2ul

#define H8P9DDR_SCK1_BIT        5u
#define H8P9DDR_SCK0_BIT        4u
#define H8P9DDR_RXD1_BIT        3u
#define H8P9DDR_RXD0_BIT        2u
#define H8P9DDR_TXD1_BIT        1u
#define H8P9DDR_TXD0_BIT        0u

#define H8P9DDR_SCK1            (1u<<H8P9DDR_SCK1_BIT)
#define H8P9DDR_SCK0            (1u<<H8P9DDR_SCK0_BIT)
#define H8P9DDR_RXD1            (1u<<H8P9DDR_RXD1_BIT)
#define H8P9DDR_RXD0            (1u<<H8P9DDR_RXD0_BIT)
#define H8P9DDR_TXD1            (1u<<H8P9DDR_TXD1_BIT)
#define H8P9DDR_TXD0            (1u<<H8P9DDR_TXD0_BIT)

#define H8P9DDR_IRQ5_BIT        5u
#define H8P9DDR_IRQ4_BIT        4u

#define H8P9DDR_IRQ5            (1u<<H8P9DDR_IRQ5_BIT)
#define H8P9DDR_IRQ4            (1u<<H8P9DDR_IRQ4_BIT)

/* portA (TPC/ITU/DMA) */

#define H8PADDR                 0xffffd1ul
#define H8PADR                  0xffffd3ul

#define H8PADDR_TP7_BIT         7u
#define H8PADDR_TP6_BIT         6u
#define H8PADDR_TP5_BIT         5u
#define H8PADDR_TP4_BIT         4u
#define H8PADDR_TP3_BIT         3u
#define H8PADDR_TP2_BIT         2u
#define H8PADDR_TP1_BIT         1u
#define H8PADDR_TP0_BIT         0u

#define H8PADDR_TP7             (1u<<H8PADDR_TP7_BIT)
#define H8PADDR_TP6             (1u<<H8PADDR_TP6_BIT)
#define H8PADDR_TP5             (1u<<H8PADDR_TP5_BIT)
#define H8PADDR_TP4             (1u<<H8PADDR_TP4_BIT)
#define H8PADDR_TP3             (1u<<H8PADDR_TP3_BIT)
#define H8PADDR_TP2             (1u<<H8PADDR_TP2_BIT)
#define H8PADDR_TP1             (1u<<H8PADDR_TP1_BIT)
#define H8PADDR_TP0             (1u<<H8PADDR_TP0_BIT)

#define H8PADDR_TIOCB2_BIT      7u
#define H8PADDR_TIOCA2_BIT      6u
#define H8PADDR_TIOCB1_BIT      5u
#define H8PADDR_TIOCA1_BIT      4u
#define H8PADDR_TIOCB0_BIT      3u
#define H8PADDR_TIOCA0_BIT      2u

#define H8PADDR_TIOCB2          (1u<<H8PADDR_TIOCB2_BIT)
#define H8PADDR_TIOCA2          (1u<<H8PADDR_TIOCA2_BIT)
#define H8PADDR_TIOCB1          (1u<<H8PADDR_TIOCB1_BIT)
#define H8PADDR_TIOCA1          (1u<<H8PADDR_TIOCA1_BIT)
#define H8PADDR_TIOCB0          (1u<<H8PADDR_TIOCB0_BIT)
#define H8PADDR_TIOCA0          (1u<<H8PADDR_TIOCA0_BIT)

#define H8PADDR_TEND1_BIT       1u
#define H8PADDR_TEND0_BIT       0u

#define H8PADDR_TEND1           (1u<<H8PADDR_TEND1_BIT)
#define H8PADDR_TEND0           (1u<<H8PADDR_TEND0_BIT)

#define H8PADDR_A20_BIT         7u
#define H8PADDR_A21_BIT         6u
#define H8PADDR_A22_BIT         5u
#define H8PADDR_A23_BIT         4u

#define H8PADDR_A20             (1u<<H8PADDR_A20_BIT)
#define H8PADDR_A21             (1u<<H8PADDR_A21_BIT)
#define H8PADDR_A22             (1u<<H8PADDR_A22_BIT)
#define H8PADDR_A23             (1u<<H8PADDR_A23_BIT)

#define H8PADDR_CS4_BIT         6u
#define H8PADDR_CS5_BIT         5u
#define H8PADDR_CS6_BIT         4u

#define H8PADDR_CS4             (1u<<H8PADDR_CS4_BIT)
#define H8PADDR_CS5             (1u<<H8PADDR_CS5_BIT)
#define H8PADDR_CS6             (1u<<H8PADDR_CS6_BIT)

#define H8PADDR_TCLKD_BIT       3u
#define H8PADDR_TCLKC_BIT       2u
#define H8PADDR_TCLKB_BIT       1u
#define H8PADDR_TCLKA_BIT       0u

#define H8PADDR_TCLKD           (1u<<H8PADDR_TCLKD_BIT)
#define H8PADDR_TCLKC           (1u<<H8PADDR_TCLKC_BIT)
#define H8PADDR_TCLKB           (1u<<H8PADDR_TCLKB_BIT)
#define H8PADDR_TCLKA           (1u<<H8PADDR_TCLKA_BIT)

/* portB (TP/ITU/DMA/AD) */

#define H8PBDDR                 0xffffd4ul
#define H8PBDR                  0xffffd6ul

#define H8PBDDR_TP15_BIT        7u
#define H8PBDDR_TP14_BIT        6u
#define H8PBDDR_TP13_BIT        5u
#define H8PBDDR_TP12_BIT        4u
#define H8PBDDR_TP11_BIT        3u
#define H8PBDDR_TP10_BIT        2u
#define H8PBDDR_TP9_BIT         1u
#define H8PBDDR_TP8_BIT         0u

#define H8PBDDR_TP15            (1u<<H8PBDDR_TP15_BIT)
#define H8PBDDR_TP14            (1u<<H8PBDDR_TP14_BIT)
#define H8PBDDR_TP13            (1u<<H8PBDDR_TP13_BIT)
#define H8PBDDR_TP12            (1u<<H8PBDDR_TP12_BIT)
#define H8PBDDR_TP11            (1u<<H8PBDDR_TP11_BIT)
#define H8PBDDR_TP10            (1u<<H8PBDDR_TP10_BIT)
#define H8PBDDR_TP9             (1u<<H8PBDDR_TP9_BIT)
#define H8PBDDR_TP8             (1u<<H8PBDDR_TP8_BIT)

#define H8PBDDR_DREQ1_BIT       7u
#define H8PBDDR_DREQ0_BIT       6u

#define H8PBDDR_DREQ1           (1u<<H8PBDDR_DREQ1_BIT)
#define H8PBDDR_DREQ0           (1u<<H8PBDDR_DREQ0_BIT)

#define H8PBDDR_TOCXB4_BIT      5u
#define H8PBDDR_TOCXA4_BIT      4u
#define H8PBDDR_TIOCB4_BIT      3u
#define H8PBDDR_TIOCA4_BIT      2u
#define H8PBDDR_TIOCB3_BIT      1u
#define H8PBDDR_TIOCA3_BIT      0u

#define H8PBDDR_TOCXB4          (1u<<H8PBDDR_TOCXB4_BIT)
#define H8PBDDR_TOCXA4          (1u<<H8PBDDR_TOCXA4_BIT)
#define H8PBDDR_TIOCB4          (1u<<H8PBDDR_TIOCB4_BIT)
#define H8PBDDR_TIOCA4          (1u<<H8PBDDR_TIOCA4_BIT)
#define H8PBDDR_TIOCB3          (1u<<H8PBDDR_TIOCB3_BIT)
#define H8PBDDR_TIOCA3          (1u<<H8PBDDR_TIOCA3_BIT)

#define H8PBDDR_ADTRG_BIT       7u

#define H8PBDDR_ADTRG           (1u<<H8PBDDR_ADTRG_BIT)

#define H8PBDDR_CS7_BIT         6u

#define H8PBDDR_CS7             (1u<<H8PBDDR_CS7_BIT)

/* Interrupt Contolller */

#define H8SYSCR                 0xfffff2ul
#define H8IPRA                  0xfffff8ul
#define H8IPRB                  0xfffff9ul

/* System Control Register */

#define H8SYSCR_SSBY_BIT        7u
#define H8SYSCR_STS2_BIT        6u
#define H8SYSCR_STS1_BIT        5u
#define H8SYSCR_STS0_BIT        4u
#define H8SYSCR_UE_BIT          3u
#define H8SYSCR_NMIEG_BIT       2u
#define H8SYSCR_RAME_BIT        0u

#define H8SYSCR_SSBY            (1u<<(H8SYSCR_SSBY_BIT))
#define H8SYSCR_STS2            (1u<<(H8SYSCR_STS2_BIT))
#define H8SYSCR_STS1            (1u<<(H8SYSCR_STS1_BIT))
#define H8SYSCR_STS0            (1u<<(H8SYSCR_STS0_BIT))
#define H8SYSCR_UE              (1u<<(H8SYSCR_UE_BIT))
#define H8SYSCR_NMIEG           (1u<<(H8SYSCR_NMIEG_BIT))
#define H8SYSCR_RAME            (1u<<(H8SYSCR_RAME_BIT))

/* Interrupt Priority Register A */

#define H8IPR_IRQ0_BIT          7u       /* IRQ0         */
#define H8IPR_IRQ1_BIT          6u       /* IRQ1         */
#define H8IPR_IRQ2_BIT          5u       /* IRQ2         */
#define H8IPR_IRQ3_BIT          5u       /* IRQ3         */
#define H8IPR_IRQ4_BIT          4u       /* IRQ4         */
#define H8IPR_IRQ5_BIT          4u       /* IRQ5         */
#define H8IPR_WDT_BIT           3u       /* WDT          */
#define H8IPR_CMI_BIT           3u       /* CMI          */
#define H8IPR_ITU0_BIT          2u       /* ITU0         */
#define H8IPR_ITU1_BIT          1u       /* ITU1         */
#define H8IPR_ITU2_BIT          0u       /* ITU2         */

#define H8IPR_IRQ0              (1u<<(H8IPR_IRQ0_BIT))
#define H8IPR_IRQ1              (1u<<(H8IPR_IRQ1_BIT))
#define H8IPR_IRQ2              (1u<<(H8IPR_IRQ2_BIT))
#define H8IPR_IRQ3              (1u<<(H8IPR_IRQ3_BIT))
#define H8IPR_IRQ4              (1u<<(H8IPR_IRQ4_BIT))
#define H8IPR_IRQ5              (1u<<(H8IPR_IRQ5_BIT))
#define H8IPR_WDT               (1u<<(H8IPR_WDT_BIT))
#define H8IPR_CMI               (1u<<(H8IPR_CMI_BIT))
#define H8IPR_ITU0              (1u<<(H8IPR_ITU0_BIT))
#define H8IPR_ITU1              (1u<<(H8IPR_ITU1_BIT))
#define H8IPR_ITU2              (1u<<(H8IPR_ITU2_BIT))

/* Interrupt Priority Register B */

#define H8IPR_ITU3_BIT          7u       /* ITU3         */
#define H8IPR_ITU4_BIT          6u       /* ITU4         */
#define H8IPR_DMAC_BIT          5u       /* DMAC (CH0,1) */
#define H8IPR_SCI0_BIT          3u       /* SCI0         */
#define H8IPR_SCI1_BIT          2u       /* SCI1         */
#define H8IPR_AD_BIT            1u       /* A/D          */

#define H8IPR_ITU3              (1u<<(H8IPR_ITU3_BIT))
#define H8IPR_ITU4              (1u<<(H8IPR_ITU4_BIT))
#define H8IPR_DMAC              (1u<<(H8IPR_DMAC_BIT))
#define H8IPR_SCI0              (1u<<(H8IPR_SCI0_BIT))
#define H8IPR_SCI1              (1u<<(H8IPR_SCI1_BIT))
#define H8IPR_AD                (1u<<(H8IPR_AD_BIT))

/*      Bus Release Control Regisger    */

#define H8BRCR                  0xfffff3ul

/*      Control bit in BRCR             */

#define H8BRCR_A23E_BIT         7u
#define H8BRCR_A22E_BIT         6u
#define H8BRCR_A21E_BIT         5u
#define H8BRCR_BRLE_BIT         1u

#define H8BRCR_A23E             (1u<<(H8BRCR_A23E_BIT))
#define H8BRCR_A22E             (1u<<(H8BRCR_A22E_BIT))
#define H8BRCR_A21E             (1u<<(H8BRCR_A21E_BIT))
#define H8BRCR_BRLE             (1u<<(H8BRCR_BRLE_BIT))

/* Integrated Timer Unit (ITU) */

#define H8ITU_TSTR              0xffff60ul
#define H8ITU_TSNC              0xffff61ul
#define H8ITU_TMDR              0xffff62ul
#define H8ITU_TFCR              0xffff63ul
#define H8ITU_TOER              0xffff90ul
#define H8ITU_TOCR              0xffff91ul

#define H8ITU0                  0xffff64ul       /* base address */
#define H8ITU1                  0xffff6eul
#define H8ITU2                  0xffff78ul
#define H8ITU3                  0xffff82ul
#define H8ITU4                  0xffff92ul

/* Integrated Timer Unit (ITU) */

/* address offset */

#define H8TCR                   0ul
#define H8TIOR                  1ul
#define H8TIER                  2ul
#define H8TSR                   3ul
#define H8TCNT                  4ul
#define H8TCNTH                 4ul
#define H8TCNTL                 5ul
#define H8GRA                   6ul
#define H8GRAH                  6ul
#define H8GRAL                  7ul
#define H8GRB                   8ul
#define H8GRBH                  8ul
#define H8GRBL                  9ul
#define H8BRA                   10ul
#define H8BRAH                  10ul
#define H8BRAL                  11ul
#define H8BRB                   12ul
#define H8BRBH                  12ul

/* ITU Timer Start Register (TSTR) */

#define H8TSTR_STR4_BIT         4u
#define H8TSTR_STR3_BIT         3u
#define H8TSTR_STR2_BIT         2u
#define H8TSTR_STR1_BIT         1u
#define H8TSTR_STR0_BIT         0u

#define H8TSTR_STR4             (1u<<H8TSTR_STR4_BIT)
#define H8TSTR_STR3             (1u<<H8TSTR_STR3_BIT)
#define H8TSTR_STR2             (1u<<H8TSTR_STR2_BIT)
#define H8TSTR_STR1             (1u<<H8TSTR_STR1_BIT)
#define H8TSTR_STR0             (1u<<H8TSTR_STR0_BIT)

/* ITU Timer Control Register (TCR) */

#define H8TCR_CCLR1_BIT         6u
#define H8TCR_CCLR0_BIT         5u
#define H8TCR_CKEG1_BIT         4u
#define H8TCR_CKEG0_BIT         3u
#define H8TCR_TPSC2_BIT         2u
#define H8TCR_TPSC1_BIT         1u
#define H8TCR_TPSC0_BIT         0u

#define H8TCR_CCLR1             (1u<<H8TCR_CCLR1_BIT)
#define H8TCR_CCLR0             (1u<<H8TCR_CCLR0_BIT)
#define H8TCR_CKEG1             (1u<<H8TCR_CKEG1_BIT)
#define H8TCR_CKEG0             (1u<<H8TCR_CKEG0_BIT)
#define H8TCR_TPSC2             (1u<<H8TCR_TPSC2_BIT)
#define H8TCR_TPSC1             (1u<<H8TCR_TPSC1_BIT)
#define H8TCR_TPSC0             (1u<<H8TCR_TPSC0_BIT)

/* ITU Timer Status Register (TSR) */

#define H8TSR_OVIF_BIT          2u
#define H8TSR_IMIFB_BIT         1u
#define H8TSR_IMIFA_BIT         0u

#define H8TSR_OVIF              (1u<<H8TSR_OVIF_BIT)
#define H8TSR_IMIFB             (1u<<H8TSR_IMIFB_BIT)
#define H8TSR_IMIFA             (1u<<H8TSR_IMIFA_BIT)

/* ITU Timer Intrrupt Enable Register (TIER) */

#define H8TIER_OVIE_BIT         2u
#define H8TIER_IMIEB_BIT        1u
#define H8TIER_IMIEA_BIT        0u

#define H8TIER_OVIE             (1u<<H8TIER_OVIE_BIT)
#define H8TIER_IMIEB            (1u<<H8TIER_IMIEB_BIT)
#define H8TIER_IMIEA            (1u<<H8TIER_IMIEA_BIT)

/* ITU Timer I/O Control Register (TIOR) */

#define H8TIOR_IOB2_BIT         6u
#define H8TIOR_IOB1_BIT         5u
#define H8TIOR_IOB0_BIT         4u
#define H8TIOR_IOA2_BIT         2u
#define H8TIOR_IOA1_BIT         1u
#define H8TIOR_IOA0_BIT         0u

#define H8TIOR_IOB2             (1u<<H8TIOR_IOB2_BIT)
#define H8TIOR_IOB1             (1u<<H8TIOR_IOB1_BIT)
#define H8TIOR_IOB0             (1u<<H8TIOR_IOB0_BIT)
#define H8TIOR_IOA2             (1u<<H8TIOR_IOA2_BIT)
#define H8TIOR_IOA1             (1u<<H8TIOR_IOA1_BIT)
#define H8TIOR_IOA0             (1u<<H8TIOR_IOA0_BIT)

/* Serial Communication Interface (SCI) */

#define H8SCI0                  0xffffb0ul       /* base address */
#define H8SCI1                  0xffffb8ul

/* address offset */

#define H8SMR                   0ul
#define H8BRR                   1ul
#define H8SCR                   2ul
#define H8TDR                   3ul
#define H8SSR                   4ul
#define H8RDR                   5ul

/* SCI Serial Mode Register (SMR) */

#define H8SMR_CA_BIT            7u
#define H8SMR_CHR_BIT           6u
#define H8SMR_PE_BIT            5u
#define H8SMR_OE_BIT            4u
#define H8SMR_STOP_BIT          3u
#define H8SMR_MP_BIT            2u
#define H8SMR_CKS1_BIT          1u
#define H8SMR_CKS0_BIT          0u

#define H8SMR_CA                (1u<<H8SMR_CA_BIT)
#define H8SMR_CHR               (1u<<H8SMR_CHR_BIT)
#define H8SMR_PE                (1u<<H8SMR_PE_BIT)
#define H8SMR_OE                (1u<<H8SMR_OE_BIT)
#define H8SMR_STOP              (1u<<H8SMR_STOP_BIT)
#define H8SMR_MP                (1u<<H8SMR_MP_BIT)
#define H8SMR_CKS1              (1u<<H8SMR_CKS1_BIT)
#define H8SMR_CKS0              (1u<<H8SMR_CKS0_BIT)
#define H8SMR_CKS_MASK          (H8SMR_CKS1|H8SMR_CKS0)

/* SCI Serial Control Register (SCR) */

#define H8SCR_TIE_BIT           7u
#define H8SCR_RIE_BIT           6u
#define H8SCR_TE_BIT            5u
#define H8SCR_RE_BIT            4u
#define H8SCR_MPIE_BIT          3u
#define H8SCR_TEIE_BIT          2u
#define H8SCR_CKE1_BIT          1u
#define H8SCR_CKE0_BIT          0u

#define H8SCR_TIE               (1u<<H8SCR_TIE_BIT)
#define H8SCR_RIE               (1u<<H8SCR_RIE_BIT)
#define H8SCR_TE                (1u<<H8SCR_TE_BIT)
#define H8SCR_RE                (1u<<H8SCR_RE_BIT)
#define H8SCR_MPIE              (1u<<H8SCR_MPIE_BIT)
#define H8SCR_TEIE              (1u<<H8SCR_TEIE_BIT)
#define H8SCR_CKE1              (1u<<H8SCR_CKE1_BIT)
#define H8SCR_CKE0              (1u<<H8SCR_CKE0_BIT)
#define H8SCR_CKE_MASK          (H8SCR_CKE1|H8SCR_CKE0)
#define H8SCR_IE                (H8SCR_TIE|H8SCR_RIE|H8SCR_MPIE|H8SCR_TEIE)

/* SCI Serial Status Register (SSR) */

#define H8SSR_TDRE_BIT          7u
#define H8SSR_RDRF_BIT          6u
#define H8SSR_ORER_BIT          5u
#define H8SSR_FER_BIT           4u
#define H8SSR_PER_BIT           3u
#define H8SSR_TEND_BIT          2u
#define H8SSR_MPB_BIT           1u
#define H8SSR_MPBT_BIT          0u

#define H8SSR_TDRE              (1u<<H8SSR_TDRE_BIT)
#define H8SSR_RDRF              (1u<<H8SSR_RDRF_BIT)
#define H8SSR_ORER              (1u<<H8SSR_ORER_BIT)
#define H8SSR_FER               (1u<<H8SSR_FER_BIT)
#define H8SSR_PER               (1u<<H8SSR_PER_BIT)
#define H8SSR_TEND              (1u<<H8SSR_TEND_BIT)
#define H8SSR_MPB               (1u<<H8SSR_MPB_BIT)
#define H8SSR_MPBT              (1u<<H8SSR_MPBT_BIT)

/*
 *  内蔵メモリの定義
 */

#define H8IN_ROM_BASE           0x000000ul
#define H8IN_ROM_SIZE           0x020000ul
#define H8IN_RAM_BASE           0xffef10ul
#define H8IN_RAM_SIZE           0x001000ul

#endif /* _H8_3048F_H_ */
