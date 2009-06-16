;;; c33regs.inc - Epson S1C33E07 soc register definitions

;;;      Copyright 2009 Christopher Hall <hsw@openmoko.com>
;;;
;;; Redistribution and use in source and binary forms, with or without
;;; modification, are permitted provided that the following conditions are
;;; met:
;;;
;;;  1. Redistributions of source code must retain the above copyright
;;;     notice, this list of conditions and the following disclaimer.
;;;
;;;  2. Redistributions in binary form must reproduce the above copyright
;;;     notice, this list of conditions and the following disclaimer in
;;;     the documentation and/or other materials provided with the
;;;     distribution.
;;;
;;; THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY
;;; EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;;; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;;; PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
;;; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;;; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;;; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
;;; BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
;;; WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
;;; OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
;;; IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


;;; this is derived from the Epson datasheet entitled:
;;;     CMOS 32-BIT SINGLE CHIP MICROCOMPUTER
;;;     S1C33E07 Technical Manual

;;; The symbols in the appendix are of the form: pUPPERCASE_NAME
;;; and the USB symbols are mixed case with no usb prefix
;;; the 'p' was deleted and USB_ added to USB symbols

;;; create two macros before including this file
;;; here is an example that simple defines symbols
;;; for each item:
;;;
;;;     .macro  REGDEF, address, bits, name
;;;     R\bits\()_\name = \address
;;;     .endm
;;;
;;;     .macro  REGBIT, name, value
;;;     \name = \value
;;;     .endm

        REGDEF  0x00300010   8  MISC_RTCWT
        REGDEF  0x00300012   8  MISC_USBWT
        REGDEF  0x00300014   8  MISC_PMUX
        REGDEF  0x00300016   8  MISC_PAC
        REGDEF  0x00300018   8  MISC_BOOT
        REGDEF  0x0030001A   8  MISC_COROM
        REGDEF  0x00300020   8  MISC_PROT
        REGDEF  0x00300260   8  INT_PP01L
        REGDEF  0x00300261   8  INT_PP23L
        REGDEF  0x00300262   8  INT_PK01L
        REGDEF  0x00300263   8  INT_PHSD01L
        REGDEF  0x00300264   8  INT_PHSD23L
        REGDEF  0x00300265   8  INT_PDM
        REGDEF  0x00300266   8  INT_P16T01
        REGDEF  0x00300267   8  INT_P16T23
        REGDEF  0x00300268   8  INT_P16T45
        REGDEF  0x00300269   8  INT_PLCDC_PSIO0
        REGDEF  0x0030026A   8  INT_PSIO1_PAD
        REGDEF  0x0030026B   8  INT_PRTC
        REGDEF  0x0030026C   8  INT_PP45L
        REGDEF  0x0030026D   8  INT_PP67L
        REGDEF  0x0030026E   8  INT_PSIO2_PSPI
        REGDEF  0x00300270   8  INT_EK01_EP03
        REGDEF  0x00300271   8  INT_EDMA
        REGDEF  0x00300272   8  INT_E16T01
        REGDEF  0x00300273   8  INT_E16T23
        REGDEF  0x00300274   8  INT_E16T45
        REGDEF  0x00300276   8  INT_ESIF01
        REGDEF  0x00300277   8  INT_EP47_ERTC_EAD
        REGDEF  0x00300278   8  INT_ELCDC
        REGDEF  0x00300279   8  INT_ESIF2_ESPI
        REGDEF  0x00300280   8  INT_FK01_FP03
        REGDEF  0x00300281   8  INT_FDMA
        REGDEF  0x00300282   8  INT_F16T01
        REGDEF  0x00300283   8  INT_F16T23
        REGDEF  0x00300284   8  INT_F16T45
        REGDEF  0x00300286   8  INT_FSIF01
        REGDEF  0x00300287   8  INT_FP47_FRTC_FAD
        REGDEF  0x00300288   8  INT_FLCDC
        REGDEF  0x00300289   8  INT_FSIF2_FSPI
        REGDEF  0x00300290   8  IDMAREQ_RP03_RHS_R16T0
        REGDEF  0x00300291   8  IDMAREQ_R16T14
        REGDEF  0x00300292   8  IDMAREQ_R16T5_RSIF0
        REGDEF  0x00300293   8  IDMAREQ_RSIF1_RAD_RP47
        REGDEF  0x00300294   8  IDMAEN_DEP03_DEHS_DE16T0
        REGDEF  0x00300295   8  IDMAEN_DE16T14
        REGDEF  0x00300296   8  IDMAEN_DE16T5_DESIF0
        REGDEF  0x00300297   8  IDMAEN_DESIF1_DEAD_DEP47
        REGDEF  0x00300298   8  HSDMA_HTGR1
        REGDEF  0x00300299   8  HSDMA_HTGR2
        REGDEF  0x0030029A   8  HSDMA_HSOFTTGR
        REGDEF  0x0030029B   8  IDMAREQ_RLCDC_RSIF2_RSPI
        REGDEF  0x0030029C   8  IDMAEN_DELCDC_DESIF2_DESPI
        REGDEF  0x0030029F   8  RST_RESET
        REGDEF  0x003002A0   8  INT_PP89L
        REGDEF  0x003002A1   8  INT_PP1011L
        REGDEF  0x003002A2   8  INT_PP1213L
        REGDEF  0x003002A3   8  INT_PP1415L
        REGDEF  0x003002A4   8  INT_PI2S
        REGDEF  0x003002A6   8  INT_EP815
        REGDEF  0x003002A7   8  INT_EI2S
        REGDEF  0x003002A9   8  INT_FP815
        REGDEF  0x003002AA   8  INT_FI2S
        REGDEF  0x003002AC   8  IDMAREQ_RP815
        REGDEF  0x003002AD   8  IDMAREQ_RI2S
        REGDEF  0x003002AE   8  IDMAEN_DEP815
        REGDEF  0x003002AF   8  IDMAEN_DEI2S
        REGDEF  0x00300300   8  CARDSETUP
        REGDEF  0x00300302   8  CARDFUNCSEL05
        REGDEF  0x00300310   8  ECCTRIGSEL
        REGDEF  0x00300311   8  ECCRSTRDY
        REGDEF  0x00300312   8  ECCENA
        REGDEF  0x00300313   8  ECCMD
        REGDEF  0x00300314   8  ECC0CP
        REGDEF  0x00300316   8  ECC0LPL
        REGDEF  0x00300317   8  ECC0LPH
        REGDEF  0x00300318   8  ECC1CP
        REGDEF  0x0030031A   8  ECC1LPL
        REGDEF  0x0030031B   8  ECC1LPH
        REGDEF  0x00300380   8  P0_P0D
        REGDEF  0x00300381   8  P0_IOC0
        REGDEF  0x00300382   8  P1_P1D
        REGDEF  0x00300383   8  P1_IOC1
        REGDEF  0x00300384   8  P2_P2D
        REGDEF  0x00300385   8  P2_IOC2
        REGDEF  0x00300386   8  P3_P3D
        REGDEF  0x00300387   8  P3_IOC3
        REGDEF  0x00300388   8  P4_P4D
        REGDEF  0x00300389   8  P4_IOC4
        REGDEF  0x0030038A   8  P5_P5D
        REGDEF  0x0030038B   8  P5_IOC5
        REGDEF  0x0030038C   8  P6_P6D
        REGDEF  0x0030038D   8  P6_IOC6
        REGDEF  0x0030038E   8  P7_P7D
        REGDEF  0x00300390   8  P8_P8D
        REGDEF  0x00300391   8  P8_IOC8
        REGDEF  0x00300392   8  P9_P9D
        REGDEF  0x00300393   8  P9_IOC9
        REGDEF  0x003003A0   8  P0_03_CFP
        REGDEF  0x003003A1   8  P0_47_CFP
        REGDEF  0x003003A2   8  P1_03_CFP
        REGDEF  0x003003A3   8  P1_47_CFP
        REGDEF  0x003003A4   8  P2_03_CFP
        REGDEF  0x003003A5   8  P2_47_CFP
        REGDEF  0x003003A6   8  P3_03_CFP
        REGDEF  0x003003A7   8  P3_46_CFP
        REGDEF  0x003003A8   8  P4_03_CFP
        REGDEF  0x003003A9   8  P4_47_CFP
        REGDEF  0x003003AA   8  P5_03_CFP
        REGDEF  0x003003AB   8  P5_47_CFP
        REGDEF  0x003003AC   8  P6_03_CFP
        REGDEF  0x003003AD   8  P6_47_CFP
        REGDEF  0x003003AE   8  P7_03_CFP
        REGDEF  0x003003AF   8  P7_4_CFP
        REGDEF  0x003003B0   8  P8_03_CFP
        REGDEF  0x003003B1   8  P8_45_CFP
        REGDEF  0x003003B2   8  P9_03_CFP
        REGDEF  0x003003B3   8  P9_47_CFP
        REGDEF  0x003003C0   8  PINTSEL_SPT03
        REGDEF  0x003003C1   8  PINTSEL_SPT47
        REGDEF  0x003003C2   8  PINTPOL_SPP07
        REGDEF  0x003003C3   8  PINTEL_SEPT07
        REGDEF  0x003003C4   8  PINTSEL_SPT811
        REGDEF  0x003003C5   8  PINTSEL_SPT1215
        REGDEF  0x003003C6   8  PINTPOL_SPP815
        REGDEF  0x003003C7   8  PINTEL_SEPT815
        REGDEF  0x003003D0   8  KINTSEL_SPPK01
        REGDEF  0x003003D2   8  KINTCOMP_SCPK0
        REGDEF  0x003003D3   8  KINTCOMP_SCPK1
        REGDEF  0x003003D4   8  KINTCOMP_SMPK0
        REGDEF  0x003003D5   8  KINTCOMP_SMPK1
        REGDEF  0x00300520  16  AD_CLKCTL
        REGDEF  0x00300540  16  AD_ADD
        REGDEF  0x00300542  16  AD_TRIG_CHNL
        REGDEF  0x00300544  16  AD_EN_SMPL_STAT
        REGDEF  0x00300546  16  AD_END
        REGDEF  0x00300548  16  AD_CH0_BUF
        REGDEF  0x0030054A  16  AD_CH1_BUF
        REGDEF  0x0030054C  16  AD_CH2_BUF
        REGDEF  0x0030054E  16  AD_CH3_BUF
        REGDEF  0x00300550  16  AD_CH4_BUF
        REGDEF  0x00300558  16  AD_UPPER
        REGDEF  0x0030055A  16  AD_LOWER
        REGDEF  0x0030055C  16  AD_CH04_INTMASK
        REGDEF  0x0030055E  16  AD_ADVMODE
        REGDEF  0x00300660  16  WD_WP
        REGDEF  0x00300662  16  WD_EN
        REGDEF  0x00300664  16  WD_COMP_LOW
        REGDEF  0x00300666  16  WD_COMP_HIGH
        REGDEF  0x00300668  16  WD_CNT_LOW
        REGDEF  0x0030066A  16  WD_CNT_HIGH
        REGDEF  0x0030066C  16  WD_CNTL
        REGDEF  0x00300780  16  T16_CR0A
        REGDEF  0x00300782  16  T16_CR0B
        REGDEF  0x00300784  16  T16_TC0
        REGDEF  0x00300786  16  T16_CTL0
        REGDEF  0x00300788  16  T16_CR1A
        REGDEF  0x0030078A  16  T16_CR1B
        REGDEF  0x0030078C  16  T16_TC1
        REGDEF  0x0030078E  16  T16_CTL1
        REGDEF  0x00300790  16  T16_CR2A
        REGDEF  0x00300792  16  T16_CR2B
        REGDEF  0x00300794  16  T16_TC2
        REGDEF  0x00300796  16  T16_CTL2
        REGDEF  0x00300798  16  T16_CR3A
        REGDEF  0x0030079A  16  T16_CR3B
        REGDEF  0x0030079C  16  T16_TC3
        REGDEF  0x0030079E  16  T16_CTL3
        REGDEF  0x003007A0  16  T16_CR4A
        REGDEF  0x003007A2  16  T16_CR4B
        REGDEF  0x003007A4  16  T16_TC4
        REGDEF  0x003007A6  16  T16_CTL4
        REGDEF  0x003007A8  16  T16_CR5A
        REGDEF  0x003007AA  16  T16_CR5B
        REGDEF  0x003007AC  16  T16_TC5
        REGDEF  0x003007AE  16  T16_CTL5
        REGDEF  0x003007D0  16  DA16_CR0A
        REGDEF  0x003007D2  16  DA16_CR1A
        REGDEF  0x003007D4  16  DA16_CR2A
        REGDEF  0x003007DC  16  T16_CNT_PAUSE
        REGDEF  0x003007DE  16  T16_ADVMODE
        REGDEF  0x003007E0  16  T16_CLKCTL_0
        REGDEF  0x003007E2  16  T16_CLKCTL_1
        REGDEF  0x003007E4  16  T16_CLKCTL_2
        REGDEF  0x003007E6  16  T16_CLKCTL_3
        REGDEF  0x003007E8  16  T16_CLKCTL_4
        REGDEF  0x003007EA  16  T16_CLKCTL_5
        REGDEF  0x00300900  16  USB_MainIntStat
        REGDEF  0x00300901   8  USB_SIE_IntStat
        REGDEF  0x00300902   8  USB_EPrIntStat
        REGDEF  0x00300903   8  USB_DMA_IntStat
        REGDEF  0x00300904   8  USB_FIFO_IntStat
        REGDEF  0x00300907   8  USB_EP0IntStat
        REGDEF  0x00300908   8  USB_EPaIntStat
        REGDEF  0x00300909   8  USB_EPbIntStat
        REGDEF  0x0030090A   8  USB_EPcIntStat
        REGDEF  0x0030090B   8  USB_EPdIntStat
        REGDEF  0x00300910   8  USB_MainIntEnb
        REGDEF  0x00300911   8  USB_SIE_IntEnb
        REGDEF  0x00300912   8  USB_EPrIntEnb
        REGDEF  0x00300913   8  USB_DMA_IntEnb
        REGDEF  0x00300914   8  USB_FIFO_IntEnb
        REGDEF  0x00300917   8  USB_EP0IntEnb
        REGDEF  0x00300918   8  USB_EPaIntEnb
        REGDEF  0x00300919   8  USB_EPbIntEnb
        REGDEF  0x0030091A   8  USB_EPcIntEnb
        REGDEF  0x0030091B   8  USB_EPdIntEnb
        REGDEF  0x00300920   8  USB_RevisionNum
        REGDEF  0x00300921   8  USB_USB_Control
        REGDEF  0x00300922   8  USB_USB_Status
        REGDEF  0x00300923   8  USB_XcvrControl
        REGDEF  0x00300924   8  USB_USB_Test
        REGDEF  0x00300925   8  USB_EPnControl
        REGDEF  0x00300926   8  USB_EPrFIFO_Clr
        REGDEF  0x0030092E   8  USB_FrameNumber_H
        REGDEF  0x0030092F   8  USB_FrameNumber_L
        REGDEF  0x00300930   8  USB_EP0Setup_0
        REGDEF  0x00300931   8  USB_EP0Setup_1
        REGDEF  0x00300932   8  USB_EP0Setup_2
        REGDEF  0x00300933   8  USB_EP0Setup_3
        REGDEF  0x00300934   8  USB_EP0Setup_4
        REGDEF  0x00300935   8  USB_EP0Setup_5
        REGDEF  0x00300936   8  USB_EP0Setup_6
        REGDEF  0x00300937   8  USB_EP0Setup_7
        REGDEF  0x00300938   8  USB_USB_Address
        REGDEF  0x00300939   8  USB_EP0Control
        REGDEF  0x0030093A   8  USB_EP0ControlIN
        REGDEF  0x0030093B   8  USB_EP0ControlOUT
        REGDEF  0x0030093F   8  USB_EP0MaxSize
        REGDEF  0x00300940   8  USB_EPaControl
        REGDEF  0x00300941   8  USB_EPbControl
        REGDEF  0x00300942   8  USB_EPcControl
        REGDEF  0x00300943   8  USB_EPdControl
        REGDEF  0x00300950   8  USB_EPaMaxSize_H
        REGDEF  0x00300951   8  USB_EPaMaxSize_L
        REGDEF  0x00300952   8  USB_EPaConfig_0
        REGDEF  0x00300953   8  USB_EPaConfig_1
        REGDEF  0x00300954   8  USB_EPbMaxSize_H
        REGDEF  0x00300955   8  USB_EPbMaxSize_L
        REGDEF  0x00300956   8  USB_EPbConfig_0
        REGDEF  0x00300957   8  USB_EPbConfig_1
        REGDEF  0x00300958   8  USB_EPcMaxSize_H
        REGDEF  0x00300959   8  USB_EPcMaxSize_L
        REGDEF  0x0030095A   8  USB_EPcConfig_0
        REGDEF  0x0030095B   8  USB_EPcConfig_1
        REGDEF  0x0030095C   8  USB_EPdMaxSize_H
        REGDEF  0x0030095D   8  USB_EPdMaxSize_L
        REGDEF  0x0030095E   8  USB_EPdConfig_0
        REGDEF  0x0030095F   8  USB_EPdConfig_1
        REGDEF  0x00300970   8  USB_EPaStartAdrs_H
        REGDEF  0x00300971   8  USB_EPaStartAdrs_L
        REGDEF  0x00300972   8  USB_EPbStartAdrs_H
        REGDEF  0x00300973   8  USB_EPbStartAdrs_L
        REGDEF  0x00300974   8  USB_EPcStartAdrs_H
        REGDEF  0x00300975   8  USB_EPcStartAdrs_L
        REGDEF  0x00300976   8  USB_EPdStartAdrs_H
        REGDEF  0x00300977   8  USB_EPdStartAdrs_L
        REGDEF  0x00300980   8  USB_CPU_JoinRd
        REGDEF  0x00300981   8  USB_CPU_JoinWr
        REGDEF  0x00300982   8  USB_EnEPnFIFO
        REGDEF  0x00300983   8  USB_EPnFIFOforCPU
        REGDEF  0x00300984   8  USB_EPnRdRemain_H
        REGDEF  0x00300985   8  USB_EPnRdRemain_L
        REGDEF  0x00300986   8  USB_EPnWrRemain_H
        REGDEF  0x00300987   8  USB_EPnWrRemain_L
        REGDEF  0x00300988   8  USB_DescAdrs_H
        REGDEF  0x00300989   8  USB_DescAdrs_L
        REGDEF  0x0030098A   8  USB_DescSize_H
        REGDEF  0x0030098B   8  USB_DescSize_L
        REGDEF  0x0030098F   8  USB_DescDoor
        REGDEF  0x00300990   8  USB_DMA_FIFO_Control
        REGDEF  0x00300991   8  USB_DMA_Join
        REGDEF  0x00300992   8  USB_DMA_Control
        REGDEF  0x00300994   8  USB_DMA_Config_0
        REGDEF  0x00300995   8  USB_DMA_Config_1
        REGDEF  0x00300997   8  USB_DMA_Latency
        REGDEF  0x00300998   8  USB_DMA_Remain_H
        REGDEF  0x00300999   8  USB_DMA_Remain_L
        REGDEF  0x0030099C   8  USB_DMA_Count_HH
        REGDEF  0x0030099D   8  USB_DMA_Count_HL
        REGDEF  0x0030099E   8  USB_DMA_Count_LH
        REGDEF  0x0030099F   8  USB_DMA_Count_LL
        REGDEF  0x00300B00   8  EFSIF0_TXD
        REGDEF  0x00300B01   8  EFSIF0_RXD
        REGDEF  0x00300B02   8  EFSIF0_STATUS
        REGDEF  0x00300B03   8  EFSIF0_CTL
        REGDEF  0x00300B04   8  EFSIF0_IRDA
        REGDEF  0x00300B05   8  EFSIF0_BRTRUN
        REGDEF  0x00300B06   8  EFSIF0_BRTRDL
        REGDEF  0x00300B07   8  EFSIF0_BRTRDM
        REGDEF  0x00300B08   8  EFSIF0_BRTCDL
        REGDEF  0x00300B09   8  EFSIF0_BRTCDM
        REGDEF  0x00300B10   8  EFSIF1_TXD
        REGDEF  0x00300B11   8  EFSIF1_RXD
        REGDEF  0x00300B12   8  EFSIF1_STATUS
        REGDEF  0x00300B13   8  EFSIF1_CTL
        REGDEF  0x00300B14   8  EFSIF1_IRDA
        REGDEF  0x00300B15   8  EFSIF1_BRTRUN
        REGDEF  0x00300B16   8  EFSIF1_BRTRDL
        REGDEF  0x00300B17   8  EFSIF1_BRTRDM
        REGDEF  0x00300B18   8  EFSIF1_BRTCDL
        REGDEF  0x00300B19   8  EFSIF1_BRTCDM
        REGDEF  0x00300B1A   8  EFSIF1_7816CTL
        REGDEF  0x00300B1B   8  EFSIF1_7816STA
        REGDEF  0x00300B1C   8  EFSIF1_FIDIL
        REGDEF  0x00300B1D   8  EFSIF1_FIDIM
        REGDEF  0x00300B1E   8  EFSIF1_TTGR
        REGDEF  0x00300B1F   8  EFSIF1_CLKNUM
        REGDEF  0x00300B20   8  EFSIF2_TXD
        REGDEF  0x00300B21   8  EFSIF2_RXD
        REGDEF  0x00300B22   8  EFSIF2_STATUS
        REGDEF  0x00300B23   8  EFSIF2_CTL
        REGDEF  0x00300B24   8  EFSIF2_IRDA
        REGDEF  0x00300B25   8  EFSIF2_BRTRUN
        REGDEF  0x00300B26   8  EFSIF2_BRTRDL
        REGDEF  0x00300B27   8  EFSIF2_BRTRDM
        REGDEF  0x00300B28   8  EFSIF2_BRTCDL
        REGDEF  0x00300B29   8  EFSIF2_BRTCDM
        REGDEF  0x00300B4F   8  EFSIF_ADV
        REGDEF  0x00300C00   8  PA_IOC
        REGDEF  0x00300C01   8  PA_DATA
        REGDEF  0x00300C02   8  PB_IOC
        REGDEF  0x00300C03   8  PB_DATA
        REGDEF  0x00300C04   8  PC_IOC
        REGDEF  0x00300C05   8  PC_DATA
        REGDEF  0x00300C20   8  PA_03_CFP
        REGDEF  0x00300C21   8  PA_4_CFP
        REGDEF  0x00300C22   8  PB_03_CFP
        REGDEF  0x00300C24   8  PC_03_CFP
        REGDEF  0x00300C25   8  PC_47_CFP
        REGDEF  0x00300C40   8  MISC_BUSPUP
        REGDEF  0x00300C41   8  MISC_BUSLOW
        REGDEF  0x00300C42   8  MISC_PUP0
        REGDEF  0x00300C43   8  MISC_PUP1
        REGDEF  0x00300C44   8  MISC_PUP2
        REGDEF  0x00300C45   8  MISC_PUP3
        REGDEF  0x00300C46   8  MISC_PUP4
        REGDEF  0x00300C47   8  MISC_PUP5
        REGDEF  0x00300C48   8  MISC_PUP6
        REGDEF  0x00300C49   8  MISC_PUP7
        REGDEF  0x00300C4A   8  MISC_PUP8
        REGDEF  0x00300C4B   8  MISC_PUP9
        REGDEF  0x00300C4C   8  MISC_PUPA
        REGDEF  0x00300C4D   8  MISC_PUPB
        REGDEF  0x00301100  16  IDMABASE
        REGDEF  0x00301102  16  IDMABASE_H
        REGDEF  0x00301104   8  IDMA_START
        REGDEF  0x00301105   8  IDMA_EN
        REGDEF  0x00301120  16  HS0_CNT
        REGDEF  0x00301122  16  HS0_CNT_H
        REGDEF  0x00301124  16  HS0_SADR
        REGDEF  0x00301126  16  HS0_SADR_h
        REGDEF  0x00301128  16  HS0_DADR
        REGDEF  0x0030112A  16  HS0_DADR_H
        REGDEF  0x0030112C  16  HS0_EN
        REGDEF  0x0030112E  16  HS0_TF
        REGDEF  0x00301130  16  HS1_CNT
        REGDEF  0x00301132  16  HS1_CNT_H
        REGDEF  0x00301134  16  HS1_SADR
        REGDEF  0x00301136  16  HS1_SADR_H
        REGDEF  0x00301138  16  HS1_DADR
        REGDEF  0x0030113A  16  HS1_DADR_H
        REGDEF  0x0030113C  16  HS1_EN
        REGDEF  0x0030113E  16  HS1_TF
        REGDEF  0x00301140  16  HS2_CNT
        REGDEF  0x00301142  16  HS2_CNT_H
        REGDEF  0x00301144  16  HS2_SADR
        REGDEF  0x00301146  16  HS2_SADR_H
        REGDEF  0x00301148  16  HS2_DADR
        REGDEF  0x0030114A  16  HS2_DADR_H
        REGDEF  0x0030114C  16  HS2_EN
        REGDEF  0x0030114E  16  HS2_TF
        REGDEF  0x00301150  16  HS3_CNT
        REGDEF  0x00301152  16  HS3_CNT_H
        REGDEF  0x00301154  16  HS3_SADR
        REGDEF  0x00301156  16  HS3_SADR_H
        REGDEF  0x00301158  16  HS3_DADR
        REGDEF  0x0030115A  16  HS3_DADR_H
        REGDEF  0x0030115C  16  HS3_EN
        REGDEF  0x0030115E  16  HS3_TF
        REGDEF  0x00301162  16  HS0_ADVMODE
        REGDEF  0x00301164  16  HS0_ADV_SADR
        REGDEF  0x00301166  16  HS0_ADV_SADR_H
        REGDEF  0x00301168  16  HS0_ADV_DADR
        REGDEF  0x0030116A  16  HS0_ADV_DADR_H
        REGDEF  0x00301172  16  HS1_ADVMODE
        REGDEF  0x00301174  16  HS1_ADV_SADR
        REGDEF  0x00301176  16  HS1_ADV_SADR_H
        REGDEF  0x00301178  16  HS1_ADV_DADR
        REGDEF  0x0030117A  16  HS1_ADV_DADR_H
        REGDEF  0x00301182  16  HS2_ADVMODE
        REGDEF  0x00301184  16  HS2_ADV_SADR
        REGDEF  0x00301186  16  HS2_ADV_SADR_H
        REGDEF  0x00301188  16  HS2_ADV_DADR
        REGDEF  0x0030118A  16  HS2_ADV_DADR_H
        REGDEF  0x00301192  16  HS3_ADVMODE
        REGDEF  0x00301194  16  HS3_ADV_SADR
        REGDEF  0x00301196  16  HS3_ADV_SADR_H
        REGDEF  0x00301198  16  HS3_ADV_DADR
        REGDEF  0x0030119A  16  HS3_ADV_DADR_H
        REGDEF  0x0030119C  16  HS_CNTLMODE
        REGDEF  0x0030119E  16  HS_ACCTIME
        REGDEF  0x00301500  32  SRAMC_BCLK_SETUP
        REGDEF  0x00301504  32  SRAMC_SWAIT
        REGDEF  0x00301508  32  SRAMC_SLV_SIZE
        REGDEF  0x0030150C  32  SRAMC_A0_BSL
        REGDEF  0x00301510  32  SRAMC_ALS
        REGDEF  0x00301600  32  SDRAMC_INI
        REGDEF  0x00301604  32  SDRAMC_CTL
        REGDEF  0x00301608  32  SDRAMC_REF
        REGDEF  0x00301610  32  SDRAMC_APP
        REGDEF  0x00301700  32  SPI_RXD
        REGDEF  0x00301704  32  SPI_TXD
        REGDEF  0x00301708  32  SPI_CTL1
        REGDEF  0x0030170C  32  SPI_CTL2
        REGDEF  0x00301710  32  SPI_WAIT
        REGDEF  0x00301714  32  SPI_STAT
        REGDEF  0x00301718  32  SPI_INT
        REGDEF  0x0030171C  32  SPI_RXMK
        REGDEF  0x00301800  32  DCSIO_CTL
        REGDEF  0x00301804  32  DCSIO_LOAD
        REGDEF  0x00301808  32  DCSIO_RCV
        REGDEF  0x00301814  32  DCSIO_INT
        REGDEF  0x00301818  32  DCSIO_STAT
        REGDEF  0x0030181C  32  DCSIO_DIR
        REGDEF  0x00301900  32  RTCINTSTAT
        REGDEF  0x00301904  32  RTCINTMODE
        REGDEF  0x00301908  32  RTC_CNTL0
        REGDEF  0x0030190C  32  RTC_CNTL1
        REGDEF  0x00301910  32  RTCSEC
        REGDEF  0x00301914  32  RTCMIN
        REGDEF  0x00301918  32  RTCHOUR
        REGDEF  0x0030191C  32  RTCDAY
        REGDEF  0x00301920  32  RTCMONTH
        REGDEF  0x00301924  32  RTCYEAR
        REGDEF  0x00301928  32  RTCDAYWEEK
        REGDEF  0x00301A00  32  LCDC_INT
        REGDEF  0x00301A04  32  LCDC_PS
        REGDEF  0x00301A10  32  LCDC_HD
        REGDEF  0x00301A14  32  LCDC_VD
        REGDEF  0x00301A18  32  LCDC_MR
        REGDEF  0x00301A20  32  LCDC_HDPS
        REGDEF  0x00301A24  32  LCDC_VDPS
        REGDEF  0x00301A28  32  LCDC_L
        REGDEF  0x00301A2C  32  LCDC_F
        REGDEF  0x00301A30  32  LCDC_FO
        REGDEF  0x00301A40  32  LCDC_TSO
        REGDEF  0x00301A44  32  LCDC_TC1
        REGDEF  0x00301A48  32  LCDC_TC0
        REGDEF  0x00301A4C  32  LCDC_TC2
        REGDEF  0x00301A60  32  LCDC_DMD
        REGDEF  0x00301A64  32  LCDC_IRAM
        REGDEF  0x00301A70  32  LCDC_MADD
        REGDEF  0x00301A74  32  LCDC_MLADD
        REGDEF  0x00301A80  32  LCDC_SADD
        REGDEF  0x00301A88  32  LCDC_SSP
        REGDEF  0x00301A8C  32  LCDC_SEP
        REGDEF  0x00301AA0  32  LCDC_LUT_03
        REGDEF  0x00301AA4  32  LCDC_LUT_47
        REGDEF  0x00301AA8  32  LCDC_LUT_8B
        REGDEF  0x00301AAC  32  LCDC_LUT_CF
        REGDEF  0x00301B00  32  CMU_GATEDCLK0
        REGDEF  0x00301B04  32  CMU_GATEDCLK1
        REGDEF  0x00301B08  32  CMU_CLKCNTL
        REGDEF  0x00301B0C  32  CMU_PLL
        REGDEF  0x00301B10  32  CMU_SSCG
        REGDEF  0x00301B14  32  CMU_OPT
        REGDEF  0x00301B24  32  CMU_PROTECT
        REGDEF  0x00301C00  32  I2S_CONTRL
        REGDEF  0x00301C04  32  I2S_DV_MCLK
        REGDEF  0x00301C08  32  I2S_DV_LRCLK
        REGDEF  0x00301C0C  32  I2S_START
        REGDEF  0x00301C10  32  I2S_HSDMAMD
        REGDEF  0x00301C14  32  I2S_FIFO_EMPTY
        REGDEF  0x00301C20  32  I2S_FIFO



;;; bit definitions for various registers


;;;
;;; Clock Management Unit
;;;

;;; codes for: REG_CMU_PROTECT
        REGBIT  CMU_PROTECT_OFF 0x96
        REGBIT  CMU_PROTECT_ON  0x00

;;; bits for: REG_CMU_CLKCNTL
        REGBIT  CMU_CLK_SEL_OSC3_DIV_32     "(10 << 24)"
        REGBIT  CMU_CLK_SEL_OSC3_DIV_16     "(9 << 24)"
        REGBIT  CMU_CLK_SEL_OSC3_DIV_8      "(8 << 24)"
        REGBIT  CMU_CLK_SEL_OSC3_DIV_4      "(7 << 24)"
        REGBIT  CMU_CLK_SEL_OSC3_DIV_2      "(6 << 24)"
        REGBIT  CMU_CLK_SEL_OSC3_DIV_1      "(5 << 24)"
        REGBIT  CMU_CLK_SEL_LCDC_CLK        "(4 << 24)"
        REGBIT  CMU_CLK_SEL_MCLK            "(3 << 24)"
        REGBIT  CMU_CLK_SEL_PLL             "(2 << 24)"
        REGBIT  CMU_CLK_SEL_OSC1            "(1 << 24)"
        REGBIT  CMU_CLK_SEL_OSC3            "(0 << 24)"

        REGBIT  PLLINDIV_MASK "(15 << 20)"
        REGBIT  PLLINDIV_10   "(9 << 20)"
        REGBIT  PLLINDIV_9    "(8 << 20)"
        REGBIT  PLLINDIV_8    "(7 << 20)"
        REGBIT  PLLINDIV_7    "(6 << 20)"
        REGBIT  PLLINDIV_6    "(5 << 20)"
        REGBIT  PLLINDIV_5    "(4 << 20)"
        REGBIT  PLLINDIV_4    "(3 << 20)"
        REGBIT  PLLINDIV_3    "(2 << 20)"
        REGBIT  PLLINDIV_2    "(1 << 20)"
        REGBIT  PLLINDIV_1    "(0 << 20)"

        REGBIT  LCDCDIV_MASK  "(15 << 16)"
        REGBIT  LCDCDIV_16    "(15 << 16)"
        REGBIT  LCDCDIV_15    "(14 << 16)"
        REGBIT  LCDCDIV_14    "(13 << 16)"
        REGBIT  LCDCDIV_13    "(12 << 16)"
        REGBIT  LCDCDIV_12    "(11 << 16)"
        REGBIT  LCDCDIV_11    "(10 << 16)"
        REGBIT  LCDCDIV_10    "(9 << 16)"
        REGBIT  LCDCDIV_9     "(8 << 16)"
        REGBIT  LCDCDIV_8     "(7 << 16)"
        REGBIT  LCDCDIV_7     "(6 << 16)"
        REGBIT  LCDCDIV_6     "(5 << 16)"
        REGBIT  LCDCDIV_5     "(4 << 16)"
        REGBIT  LCDCDIV_4     "(3 << 16)"
        REGBIT  LCDCDIV_3     "(2 << 16)"
        REGBIT  LCDCDIV_2     "(1 << 16)"
        REGBIT  LCDCDIV_1     "(0 << 16)"

        REGBIT  MCLKDIV       "(1 << 12)"

        REGBIT  OSC3DIV_MASK  "(7 << 8)"
        REGBIT  OSC3DIV_32    "(5 << 8)"
        REGBIT  OSC3DIV_16    "(4 << 8)"
        REGBIT  OSC3DIV_8     "(3 << 8)"
        REGBIT  OSC3DIV_4     "(2 << 8)"
        REGBIT  OSC3DIV_2     "(1 << 8)"
        REGBIT  OSC3DIV_1     "(0 << 8)"

        REGBIT  OSCSEL_MASK   "(3 << 2)"
        REGBIT  OSCSEL_PLL    "(3 << 2)"
        REGBIT  OSCSEL_OSC3x  "(2 << 2)"
        REGBIT  OSCSEL_OSC1   "(1 << 2)"
        REGBIT  OSCSEL_OSC3   "(0 << 2)"

        REGBIT  SOSC3  "(1 << 1)"
        REGBIT  SOSC1  "(1 << 0)"

;;; Bits for: REG_CMU_GATEDCLK0
        REGBIT  USBSAPB_CKE    "(1 << 9)"
        REGBIT  USB_CKE        "(1 << 8)"
        REGBIT  SDAPCPU_HCKE   "(1 << 7)"
        REGBIT  SDAPCPU_CKE    "(1 << 6)"
        REGBIT  SDAPLCDC_CKE   "(1 << 5)"
        REGBIT  SDSAPB_CKE     "(1 << 4)"
        REGBIT  DSTRAM_CKE     "(1 << 3)"
        REGBIT  LCDCAHBIF_CKE  "(1 << 2)"
        REGBIT  LCDCSAPB_CKE   "(1 << 1)"
        REGBIT  LCDC_CKE       "(1 << 0)"

;;; Bits for: REG_CMU_GATEDCLK1
        REGBIT  CPUAHB_HCKE    "(1 << 29)"
        REGBIT  LCDCAHB_HCKE   "(1 << 28)"
        REGBIT  GPIONSTP_HCKE  "(1 << 27)"
        REGBIT  SRAMC_HCKE     "(1 << 26)"
        REGBIT  EFSIOBR_HCKE   "(1 << 25)"
        REGBIT  MISC_HCKE      "(1 << 24)"
        REGBIT  IVRAMARB_CKE   "(1 << 19)"
        REGBIT  TM5_CKE        "(1 << 18)"
        REGBIT  TM4_CKE        "(1 << 17)"
        REGBIT  TM3_CKE        "(1 << 16)"
        REGBIT  TM2_CKE        "(1 << 15)"
        REGBIT  TM1_CKE        "(1 << 14)"
        REGBIT  TM0_CKE        "(1 << 13)"
        REGBIT  EGPIO_MISC_CK  "(1 << 12)"
        REGBIT  I2S_CKE        "(1 << 11)"
        REGBIT  DCSIO_CKE      "(1 << 10)"
        REGBIT  WDT_CKE        "(1 << 9)"
        REGBIT  GPIO_CKE       "(1 << 8)"
        REGBIT  SRAMSAPB_CKE   "(1 << 7)"
        REGBIT  SPI_CKE        "(1 << 6)"
        REGBIT  EFSIOSAPB_CKE  "(1 << 5)"
        REGBIT  CARD_CKE       "(1 << 4)"
        REGBIT  ADC_CKE        "(1 << 3)"
        REGBIT  ITC_CKE        "(1 << 2)"
        REGBIT  DMA_CKE        "(1 << 1)"
        REGBIT  RTCSAPB_CKE    "(1 << 0)"

;;; Bits for: REG_CMU_OPT
        REGBIT  OSCTM_SHIFT 8
        REGBIT  OSC3OFF     "(1 << 3)"
        REGBIT  TMHSP       "(1 << 2)"
        REGBIT  WAKEUPWT    "(1 << 0)"


;;;
;;; Serial Controller
;;;

;;; Bits for: REG_EFSIFx_STATUS
        REGBIT  RXDxNUM1  "(1 << 7)"
        REGBIT  RXDxNUM0  "(1 << 6)"
        REGBIT  TENDx	  "(1 << 5)"
        REGBIT  FERx	  "(1 << 4)"
        REGBIT  PERx	  "(1 << 3)"
        REGBIT  OERx	  "(1 << 2)"
        REGBIT  TDBEx	  "(1 << 1)"
        REGBIT  RDBFx	  "(1 << 0)"

;;; Bits for: REG_EFSIFx_CTL
        REGBIT  TXENx 	  "(0x1 << 7)"
        REGBIT  TX_DISENx "(0x0 << 7)"
        REGBIT  RXENx 	  "(0x1 << 6)"
        REGBIT  RX_DISENx "(0x0 << 6)"
        REGBIT  PARx 	  "(0x1 << 5)"
        REGBIT  NO_PARx   "(0x0 << 5)"
        REGBIT  ODDx 	  "(0x1 << 4)"
        REGBIT  EVENx 	  "(0x0 << 4)"
        REGBIT  ONE_STPBx "(0x1 << 3)"
        REGBIT  TWO_STPBx "(0x0 << 3)"
        REGBIT  SCLKx 	  "(0x1 << 2)"
        REGBIT  INT_CLKx  "(0x0 << 2)"
        REGBIT  EIGHT_BIT_ASYNx   0x3
        REGBIT  SEVEN_BIT_ASYNx   0x2
        REGBIT  CLK_SLAVEx 	  0x1
        REGBIT  CLK_MASTERx 	  0x0

;;; Bits for: REG_EFSIFx_IrDA
        REGBIT  DIVMD_8x	  "(0x1 << 4)"
        REGBIT  DIVMD_16x	  "(0x0 << 4)"
        REGBIT  IRMD_IRDAx		  0x10
        REGBIT  IRMD_GEN_IFx 	  0x00

;;; Bits for: REG_EFSIFx_BRTCTL
        REGBIT  BRTRUN_STARx 	0x1
        REGBIT  BRTRUN_STOPx 	0x0


;;;
;;; Interrupt Controller
;;;

;;; Bits for: REG_INT_ESIF01
        REGBIT  ESTX1  "(1 << 5)"
        REGBIT  ESRX1  "(1 << 4)"
        REGBIT  ESERR1 "(1 << 3)"
        REGBIT  ESTX0  "(1 << 2)"
        REGBIT  ESRX0  "(1 << 1)"
        REGBIT  ESERR0 "(1 << 0)"

;;; Bits for: REG_INT_FSIF01
        REGBIT  FSTX1  "(1 << 5)"
        REGBIT  FSRX1  "(1 << 4)"
        REGBIT  FSERR1 "(1 << 3)"
        REGBIT  FSTX0  "(1 << 2)"
        REGBIT  FSRX0  "(1 << 1)"
        REGBIT  FSERR0 "(1 << 0)"

;;;  Bits for: REG_RST_RESET
        REGBIT  DENONLY  "(1 << 2)"
        REGBIT  IDMAONLY "(1 << 1)"
        REGBIT  RSTONLY  "(1 << 0)"


;;;
;;; SDRAM Controller
;;;

;;; bits for: REG_SDRAMC_CTL
        REGBIT  T24NS_SHIFT  12
        REGBIT  T60NS_SHIFT  8
        REGBIT  T80NS_SHIFT  4

        REGBIT  ADDRC_32M_x_16_bits_x_1 0x7
        REGBIT  ADDRC_16M_x__8_bits_x_2 0x6
        REGBIT  ADDRC__8M_x__8_bits_x_2 0x5
        REGBIT  ADDRC__2M_x__8_bits_x_2 0x4
        REGBIT  ADDRC_16M_x_16_bits_x_1 0x3
        REGBIT  ADDRC__8M_x_16_bits_x_1 0x2
        REGBIT  ADDRC__4M_x_16_bits_x_1 0x1
        REGBIT  ADDRC__1M_x_16_bits_x_1 0x0


;;; Bits for: REG_SDRAMC_REF
        REGBIT  SELDO "(1 << 25)"
        REGBIT  SCKON "(1 << 24)"
        REGBIT  SELEN "(1 << 23)"
        REGBIT  SELCO_SHIFT 16
        REGBIT  AURCO_SHIFT  0

;;; Bits for: REG_SDRAMC_INI
        REGBIT  SDON   "(1 << 4)"
        REGBIT  SDEN   "(1 << 3)"
        REGBIT  INIMRS "(1 << 2)"
        REGBIT  INIPRE "(1 << 1)"
        REGBIT  INIREF "(1 << 0)"

;;; SDRAM controller commands for REG_SDRAMC_INI
        REGBIT  SDRAM_CMD_FIRST "(SDON)"
        REGBIT  SDRAM_CMD_FINAL "(SDON)"
        REGBIT  SDRAM_CMD_REF   "(SDON | INIREF)"
        REGBIT  SDRAM_CMD_PALL  "(SDON | INIPRE)"
        REGBIT  SDRAM_CMD_MRS   "(SDON | INIMRS)"

;;; Bits for: REG_SDRAMC_APP
        REGBIT  ARBON  "(1 << 31)"
        REGBIT  DBF    "(1 << 5)"
        REGBIT  INCR   "(1 << 4)"
        REGBIT  CAS1   "(1 << 3)"
        REGBIT  CAS0   "(1 << 2)"
        REGBIT  APPON  "(1 << 1)"
        REGBIT  IQB    "(1 << 0)"

;;; Bits for: REG_CH1_INT_PRIORITY
        REGBIT  SERIAL_CH1_INT_PRI_7  	0x7
        REGBIT  SERIAL_CH1_INT_PRI_6  	0x6
        REGBIT  SERIAL_CH1_INT_PRI_5  	0x5
        REGBIT  SERIAL_CH1_INT_PRI_4  	0x4
        REGBIT  SERIAL_CH1_INT_PRI_3  	0x3
        REGBIT  SERIAL_CH1_INT_PRI_2  	0x2
        REGBIT  SERIAL_CH1_INT_PRI_1  	0x1
        REGBIT  SERIAL_CH1_INT_PRI_0  	0x0


;;;
;;; LCD controller
;;;

;;; Bits for: REG_LCDC_PS
        REGBIT  INTF              "(1 << 31)"
        REGBIT  VNDPF             "(1 << 7)"
        REGBIT  PSAVE_NORMAL      "(3 << 0)"
        REGBIT  PSAVE_DOZE        "(2 << 0)"
        REGBIT  PSAVE_POWER_SAVE  "(0 << 0)"

;;; Bits for: REG_LCDC_HD
        REGBIT  HTCNT_SHIFT    16
        REGBIT  HDPCNT_SHIFT   0

;;; Bits for: REG_LCDC_VD
        REGBIT  VTCNT_SHIFT    16
        REGBIT  VDPCNT_SHIFT   0

;;; Bits for: REG_LCDC_DMD
        REGBIT  TFTSEL         "(1 << 31)"
        REGBIT  COLOR          "(1 << 30)"
        REGBIT  FPSMASK        "(1 << 29)"

        REGBIT  DWD_8_BIT_2    "(3 << 26)"
        REGBIT  DWD_8_BIT_1    "(1 << 26)"
        REGBIT  DWD_4_BIT      "(0 << 26)"

        REGBIT  SWINV          "(1 << 25)"
        REGBIT  BLANK          "(1 << 24)"
        REGBIT  FRMRPT         "(1 << 7)"
        REGBIT  DITHEN         "(1 << 6)"
        REGBIT  LUTPASS        "(1 << 4)"

        REGBIT  BPP_16         "(5 << 0)"
        REGBIT  BPP_12         "(4 << 0)"
        REGBIT  BPP_8          "(3 << 0)"
        REGBIT  BPP_4          "(2 << 0)"
        REGBIT  BPP_2          "(1 << 0)"
        REGBIT  BPP_1          "(0 << 0)"


;;;
;;; Timers
;;;

;;; Bits for: REG_T16_ADVMODE
        REGBIT  T16ADV    "(1 << 0)"

;;; Bits for: REG_T16_CNT_PAUSE
        REGBIT  PAUSE5    "(1 << 5)"
        REGBIT  PAUSE4    "(1 << 4)"
        REGBIT  PAUSE3    "(1 << 3)"
        REGBIT  PAUSE2    "(1 << 2)"
        REGBIT  PAUSE1    "(1 << 1)"
        REGBIT  PAUSE0    "(1 << 0)"

;;; Bits for: REG_T16_CTLx
        REGBIT  INITOLx    "(1 << 8)"
;;;     REGBIT  (TMODEx)   "(1 << 7)" - reserved, do not set to 1
        REGBIT  SELFMx     "(1 << 6)"
        REGBIT  SELCRBx    "(1 << 5)"
        REGBIT  OUTINVx    "(1 << 4)"
        REGBIT  CKSLx      "(1 << 3)"
        REGBIT  PTMx       "(1 << 2)"
        REGBIT  PRESETx    "(1 << 1)"
        REGBIT  PRUNx      "(1 << 0)"

;;;Bits for: REG_T16_CLKCTL_x
        REGBIT  P16TONx               "(1 << 3)"
        REGBIT  P16TSx_MCLK_DIV_4096  "(7 << 0)"
        REGBIT  P16TSx_MCLK_DIV_1024  "(6 << 0)"
        REGBIT  P16TSx_MCLK_DIV_256   "(5 << 0)"
        REGBIT  P16TSx_MCLK_DIV_64    "(4 << 0)"
        REGBIT  P16TSx_MCLK_DIV_16    "(3 << 0)"
        REGBIT  P16TSx_MCLK_DIV_4     "(2 << 0)"
        REGBIT  P16TSx_MCLK_DIV_2     "(1 << 0)"
        REGBIT  P16TSx_MCLK_DIV_1     "(0 << 0)"


;;;
;;; SPI module
;;;

;;; Bits for: REG_SPI_CTL1

        REGBIT  BPT_32_BITS        "(31 << 10)"
        REGBIT  BPT_16_BITS        "(15 << 10)"
        REGBIT  BPT_8_BITS         "( 7 << 10)"
        REGBIT  BPT_1_BITS         "( 0 << 10)"

        REGBIT  CPHA               "(1 << 9)"
        REGBIT  CPOL               "(1 << 8)"

        REGBIT  MCBR_MCLK_DIV_512  "(7 << 4)"
        REGBIT  MCBR_MCLK_DIV_256  "(6 << 4)"
        REGBIT  MCBR_MCLK_DIV_128  "(5 << 4)"
        REGBIT  MCBR_MCLK_DIV_64   "(4 << 4)"
        REGBIT  MCBR_MCLK_DIV_32   "(3 << 4)"
        REGBIT  MCBR_MCLK_DIV_16   "(2 << 4)"
        REGBIT  MCBR_MCLK_DIV_8    "(1 << 4)"
        REGBIT  MCBR_MCLK_DIV_4    "(0 << 4)"

        REGBIT  TXDE               "(1 << 3)"
        REGBIT  RXDE               "(1 << 2)"

        REGBIT  MODE_MASTER        "(1 << 1)"
        REGBIT  MODE_SLAVE         "(0 << 1)"

        REGBIT  ENA                "(1 << 0)"

;;; Bits for: REG_SPI_STAT
        REGBIT  BSYF   "(1 << 6)"
        REGBIT  MFEF   "(1 << 5)"
        REGBIT  TDEF   "(1 << 4)"
        REGBIT  RDOF   "(1 << 3)"
        REGBIT  RDFF   "(1 << 2)"

;;; Bits for REG_HS_CNTLMODE
        REGBIT  HSDMAADV "(1 << 0)"

;;; Bits for DMA advanced channels
        REGBIT  DMA_ENABLED  "(1 << 0)"
        REGBIT  DMA_DISABLED "(0 << 0)"



;;; Interrupt vectors

        VECTOR  Reset 0
        VECTOR  reserved_1 1
        VECTOR  ext_exception 2
        VECTOR  Undefined_instruction_exception 3
        VECTOR  reserved_4 4
        VECTOR  reserved_5 5
        VECTOR  Address_misaligned_exception 6
        VECTOR  NMI 7
        VECTOR  reserved_8 8
        VECTOR  reserved_9 9
        VECTOR  reserved_10 10
        VECTOR  Illegal_interrupt_exception 11
        VECTOR  Software_exception_0 12
        VECTOR  Software_exception_1 13
        VECTOR  Software_exception_2 14
        VECTOR  Software_exception_3 15
        VECTOR  Port_input_interrupt_0 16
        VECTOR  Port_input_interrupt_1 17
        VECTOR  Port_input_interrupt_2 18
        VECTOR  Port_input_interrupt_3 19
        VECTOR  Key_input_interrupt_0 20
        VECTOR  Key_input_interrupt_1 21
        VECTOR  High_speed_DMA_Ch_0 22
        VECTOR  High_speed_DMA_Ch_1 23
        VECTOR  High_speed_DMA_Ch_2 24
        VECTOR  High_speed_DMA_Ch_3 25
        VECTOR  Intelligent_DMA 26
        VECTOR  reserved_27 27
        VECTOR  reserved_28 28
        VECTOR  reserved_29 29
        VECTOR  16_bit_timer_0_compare_match_B 30
        VECTOR  16_bit_timer_0_compare_match_A 31
        VECTOR  reserved_32 32
        VECTOR  reserved_33 33
        VECTOR  16_bit_timer_1_compare_match_B 34
        VECTOR  16_bit_timer_1_compare_match_A 35
        VECTOR  reserved_36 36
        VECTOR  reserved_37 37
        VECTOR  16_bit_timer_2_compare_match_B 38
        VECTOR  16_bit_timer_2_compare_match_A 39
        VECTOR  reserved_40 40
        VECTOR  reserved_41 41
        VECTOR  16_bit_timer_3_compare_match_B 42
        VECTOR  16_bit_timer_3_compare_match_A 43
        VECTOR  reserved_44 44
        VECTOR  reserved_45 45
        VECTOR  16_bit_timer_4_compare_match_B 46
        VECTOR  16_bit_timer_4_compare_match_A 47
        VECTOR  reserved_48 48
        VECTOR  reserved_49 49
        VECTOR  16_bit_timer_5_compare_match_B 50
        VECTOR  16_bit_timer_5_compare_match_A 51
        VECTOR  reserved_52 52
        VECTOR  reserved_53 53
        VECTOR  reserved_54 54
        VECTOR  reserved_55 55
        VECTOR  Serial_interface_Ch_0_Receive_error 56
        VECTOR  Serial_interface_Ch_0_Receive_buffer_full 57
        VECTOR  Serial_interface_Ch_0_Transmit_buffer_empty 58
        VECTOR  reserved_59 59
        VECTOR  Serial_interface_Ch_1_Receive_error 60
        VECTOR  Serial_interface_Ch_1_Receive_buffer_full 61
        VECTOR  Serial_interface_Ch_1_Transmit_buffer_empty 62
        VECTOR  A_D_converter_Result_out_of_range 63
        VECTOR  A_D_converter_End_of_conversion 64
        VECTOR  RTC 65
        VECTOR  reserved_66 66
        VECTOR  reserved_67 67
        VECTOR  Port_input_interrupt_4 68
        VECTOR  Port_input_interrupt_5 69
        VECTOR  Port_input_interrupt_6 70
        VECTOR  Port_input_interrupt_7 71
        VECTOR  reserved_72 72
        VECTOR  LCDC 73
        VECTOR  reserved_74 74
        VECTOR  reserved_75 75
        VECTOR  Serial_interface_Ch_2_Receive_error 76
        VECTOR  Serial_interface_Ch_2_Receive_buffer_full 77
        VECTOR  Serial_interface_Ch_2_Transmit_buffer_empty 78
        VECTOR  reserved_79 79
        VECTOR  reserved_80 80
        VECTOR  SPI_Receive_DMA_request 81
        VECTOR  SPI_Transmit_DMA_request 82
        VECTOR  reserved_83 83
        VECTOR  Port_input_interrupt_8_SPI_SPI_interrupt 84
        VECTOR  Port_input_interrupt_9_USB_PDREQ_USB_DMA_request 85
        VECTOR  Port_input_interrupt_10_USB_USBinterrupt 86
        VECTOR  Port_input_interrupt_11_DCSIO_DCSIO_interrupt 87
        VECTOR  Port_input_interrupt_12 88
        VECTOR  Port_input_interrupt_13 89
        VECTOR  Port_input_interrupt_14 90
        VECTOR  Port_input_interrupt_15 91
        VECTOR  reserved_92 92
        VECTOR  reserved_93 93
        VECTOR  I2S_interface_I2S_FIFO_empty 94
        VECTOR  reserved_95 95
        VECTOR  reserved_96 96
        VECTOR  reserved_97 97
        VECTOR  reserved_98 98
        VECTOR  reserved_99 99
        VECTOR  reserved_100 100
        VECTOR  reserved_101 101
        VECTOR  reserved_102 102
        VECTOR  reserved_103 103
        VECTOR  reserved_104 104
        VECTOR  reserved_105 105
        VECTOR  reserved_106 106
        VECTOR  reserved_107 107
