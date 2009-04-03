;;; regs.inc

;;; start of internal register area
REG_BASE = 0x300000

;;; Misc Register #1
REG_MISC_RTCWT = (REG_BASE + 0x10)              ; 8
REG_MISC_USBWT = (REG_BASE + 0x12)              ; 8
REG_MISC_PMUX = (REG_BASE + 0x14)               ; 8
REG_MISC_PAC = (REG_BASE + 0x16)                ; 8
REG_MISC_BOOT = (REG_BASE + 0x18)               ; 8
REG_MISC_COROM = (REG_BASE + 0x1a)              ; 8
REG_MISC_PROT = (REG_BASE + 0x20)               ; 8

;;; Interrupt Controller
REG_INT_PP01L = (REG_BASE + 0x260)              ; 8
REG_INT_PP23L = (REG_BASE + 0x261)              ; 8
REG_INT_PK01L = (REG_BASE + 0x262)              ; 8
REG_INT_PHSD01L = (REG_BASE + 0x263)            ; 8
REG_INT_PHSD23L = (REG_BASE + 0x264)            ; 8
REG_INT_PDM = (REG_BASE + 0x265)                ; 8
REG_INT_P16T01 = (REG_BASE + 0x266)             ; 8
REG_INT_P16T23 = (REG_BASE + 0x267)             ; 8
REG_INT_P16T45 = (REG_BASE + 0x268)             ; 8
REG_INT_PLCDC_PSIO0 = (REG_BASE + 0x269)        ; 8
REG_INT_PSI01_PAD = (REG_BASE + 0x26a)          ; 8
REG_INT_PRTC = (REG_BASE + 0x26b)               ; 8
REG_INT_PP45L = (REG_BASE + 0x26c)              ; 8
REG_INT_PP67L = (REG_BASE + 0x26d)              ; 8
REG_INT_PSI02_PSPI = (REG_BASE + 0x26e)         ; 8
REG_INT_EK01_EP0_3 = (REG_BASE + 0x270)         ; 8
REG_INT_EDMA = (REG_BASE + 0x271)               ; 8
REG_INT_E16T01 = (REG_BASE + 0x272)             ; 8
REG_INT_E16T23 = (REG_BASE + 0x273)             ; 8
REG_INT_E16T45 = (REG_BASE + 0x274)             ; 8
REG_INT_ESIF01 = (REG_BASE + 0x276)             ; 8
REG_INT_EP47_ERTC_EAD = (REG_BASE + 0x277)      ; 8
REG_INT_ELCDC = (REG_BASE + 0x278)              ; 8
REG_INT_ESIF2_ESPI = (REG_BASE + 0x279)         ; 8
REG_INT_FK01_FP03 = (REG_BASE + 0x280)          ; 8
REG_INT_FDMA = (REG_BASE + 0x281)               ; 8
REG_INT_F16T01 = (REG_BASE + 0x282)             ; 8
REG_INT_F16T23 = (REG_BASE + 0x283)             ; 8
REG_INT_F16T45 = (REG_BASE + 0x284)             ; 8
REG_INT_FSIF01 = (REG_BASE + 0x286)             ; 8
REG_INT_FP47_FRTC_FAD = (REG_BASE + 0x287)      ; 8
REG_INT_FLCDC = (REG_BASE + 0x288)              ; 8
REG_INT_FSIF2_FSPI = (REG_BASE + 0x289)         ; 8
REG_IDMAREQ_RP03_RHS_R16T0 = (REG_BASE + 0x290) ; 8
REG_IDMAREQ_R16T14 = (REG_BASE + 0x291)         ; 8
REG_IDMAREQ_R16T5_RSIF0 = (REG_BASE + 0x292)    ; 8
REG_IDMAREQ_RSIF1_RAD_RP47 = (REG_BASE + 0x293) ; 8
REG_IDMAEN_DEP03_DEHS_DE16T0 = (REG_BASE + 0x294) ; 8
REG_IDMAEN_DE16T14 = (REG_BASE + 0x295)           ; 8
REG_IDMAEN_DE16T5_DESIF0 = (REG_BASE + 0x296)     ; 8
REG_IDMAEN_DESIF1_DEAD_DEP47 = (REG_BASE + 0x297) ; 8
REG_HSDMA_HTGR1 = (REG_BASE + 0x298)              ; 8
REG_HSDMA_HTGR2 = (REG_BASE + 0x299)              ; 8
REG_HSDMA_HSOFTTGR = (REG_BASE + 0x29a)           ; 8
REG_IDMAREQ_RLCDC_RSIF2_RSPI = (REG_BASE + 0x29b) ; 8
REG_IDMAEN_DELCDC_DESIF2_DESPI = (REG_BASE + 0x29c) ; 8
REG_RST_RESET = (REG_BASE + 0x29f)                  ; 8
REG_INT_PP89L = (REG_BASE + 0x2a0)                  ; 8
REG_INT_PP1011L = (REG_BASE + 0x2a1)                ; 8
REG_INT_PP1213L = (REG_BASE + 0x2a2)                ; 8
REG_INT_PP1415L = (REG_BASE + 0x2a3)                ; 8
REG_INT_PI2S = (REG_BASE + 0x2a4)                   ; 8
REG_INT_EP815 = (REG_BASE + 0x2a6)                  ; 8
REG_INT_EI2S = (REG_BASE + 0x2a7)                   ; 8
REG_INT_FP815 = (REG_BASE + 0x2a9)                  ; 8
REG_INT_FI2S = (REG_BASE + 0x2aa)                   ; 8
REG_IDMAREQ_RP815 = (REG_BASE + 0x2ac)              ; 8
REG_IDMAREQ_RI2S = (REG_BASE + 0x2ad)               ; 8
REG_IDMAEN_DEP815 = (REG_BASE + 0x2ae)              ; 8
REG_IDMAEN_DEI2S = (REG_BASE + 0x2af)               ; 8

;;; Card Interface
REG_CARDSETUP = (REG_BASE + 0x300)              ; 8
REG_CARDFUNCSEL05 = (REG_BASE + 0x302)          ; 8
REG_ECCTRIGSEL = (REG_BASE + 0x310)             ; 8
REG_ECCRSTRDY = (REG_BASE + 0x311)              ; 8
REG_ECCENA = (REG_BASE + 0x312)                 ; 8
REG_ECCMD = (REG_BASE + 0x313)                  ; 8
REG_ECC0CP = (REG_BASE + 0x314)                 ; 8
REG_ECC0LPL = (REG_BASE + 0x316)                ; 8
REG_ECC0LPH = (REG_BASE + 0x317)                ; 8
REG_ECC1CP = (REG_BASE + 0x318)                 ; 8
REG_ECC1LPL = (REG_BASE + 0x31a)                ; 8
REG_ECC1LPH = (REG_BASE + 0x31b)                ; 8

;;; I/O Ports
REG_P0_P0D = (REG_BASE + 0x380)                 ; 8
REG_P0_IOC0 = (REG_BASE + 0x381)                ; 8
REG_P1_P1D = (REG_BASE + 0x382)                 ; 8
REG_P1_IOC1 = (REG_BASE + 0x383)                ; 8
REG_P2_P2D = (REG_BASE + 0x384)                 ; 8
REG_P2_IOC2 = (REG_BASE + 0x385)                ; 8
REG_P3_P3D = (REG_BASE + 0x386)                 ; 8
REG_P3_IOC3 = (REG_BASE + 0x387)                ; 8
REG_P4_P4D = (REG_BASE + 0x388)                 ; 8
REG_P4_IOC4 = (REG_BASE + 0x389)                ; 8
REG_P5_P5D = (REG_BASE + 0x38a)                 ; 8
REG_P5_IOC5 = (REG_BASE + 0x38b)                ; 8
REG_P6_P6D = (REG_BASE + 0x38c)                 ; 8
REG_P6_IOC6 = (REG_BASE + 0x38d)                ; 8
REG_P7_P7D = (REG_BASE + 0x38e)                 ; 8
REG_P8_P8D = (REG_BASE + 0x390)                 ; 8
REG_P8_IOC8 = (REG_BASE + 0x391)                ; 8
REG_P9_P9D = (REG_BASE + 0x392)                 ; 8
REG_P9_IOC9 = (REG_BASE + 0x393)                ; 8
REG_P0_03_CFP = (REG_BASE + 0x3a0)              ; 8
REG_P0_47_CFP = (REG_BASE + 0x3a1)              ; 8
REG_P1_03_CFP = (REG_BASE + 0x3a2)              ; 8
REG_P1_47_CFP = (REG_BASE + 0x3a3)              ; 8
REG_P2_03_CFP = (REG_BASE + 0x3a4)              ; 8
REG_P2_47_CFP = (REG_BASE + 0x3a5)              ; 8
REG_P3_03_CFP = (REG_BASE + 0x3a6)              ; 8
REG_P3_46_CFP = (REG_BASE + 0x3a7)              ; 8
REG_P4_03_CFP = (REG_BASE + 0x3a8)              ; 8
REG_P4_47_CFP = (REG_BASE + 0x3a9)              ; 8
REG_P5_03_CFP = (REG_BASE + 0x3aa)              ; 8
REG_P5_47_CFP = (REG_BASE + 0x3ab)              ; 8
REG_P6_03_CFP = (REG_BASE + 0x3ac)              ; 8
REG_P6_47_CFP = (REG_BASE + 0x3ad)              ; 8
REG_P7_03_CFP = (REG_BASE + 0x3ae)              ; 8
REG_P7_4_CFP = (REG_BASE + 0x3af)               ; 8
REG_P8_03_CFP = (REG_BASE + 0x3b0)              ; 8
REG_P8_45_CFP = (REG_BASE + 0x3b1)              ; 8
REG_P9_03_CFP = (REG_BASE + 0x3b2)              ; 8
REG_P9_47_CFP = (REG_BASE + 0x3b3)              ; 8
REG_PINTSEL_SPT03 = (REG_BASE + 0x3c0)          ; 8
REG_PINTSEL_SPT47 = (REG_BASE + 0x3c1)          ; 8
REG_PINTPOL_SPP07 = (REG_BASE + 0x3c2)          ; 8
REG_PINTEL_SEPT07 = (REG_BASE + 0x3c3)          ; 8
REG_PINTSEL_SPT811 = (REG_BASE + 0x3c4)         ; 8
REG_PINTSEL_SPT1215 = (REG_BASE + 0x3c5)        ; 8
REG_PINTPOL_SPP815 = (REG_BASE + 0x3c6)         ; 8
REG_PINTEL_SEPT815 = (REG_BASE + 0x3c7)         ; 8
REG_KINTSEL_SPPK01 = (REG_BASE + 0x3d0)         ; 8
REG_KINTCOMP_SCPK0 = (REG_BASE + 0x3d2)         ; 8
REG_KINTCOMP_SCPK1 = (REG_BASE + 0x3d3)         ; 8
REG_KINTCOMP_SMPK0 = (REG_BASE + 0x3d4)         ; 8
REG_KINTCOMP_SMPK1 = (REG_BASE + 0x3d5)         ; 8

;;; A/D Converter
REG_AD_CLKCTL = (REG_BASE + 0x520)              ; 16
REG_AD_ADD = (REG_BASE + 0x540)                 ; 16
REG_AD_TRIG_CHNL = (REG_BASE + 0x542)           ; 16
REG_AD_EN_SMPL_STAT = (REG_BASE + 0x544)        ; 16
REG_AD_END = (REG_BASE + 0x546)                 ; 16
REG_AD_CH0_BUF = (REG_BASE + 0x548)             ; 16
REG_AD_CH1_BUF = (REG_BASE + 0x54a)             ; 16
REG_AD_CH2_BUF = (REG_BASE + 0x54c)             ; 16
REG_AD_CH3_BUF = (REG_BASE + 0x54e)             ; 16
REG_AD_CH4_BUF = (REG_BASE + 0x550)             ; 16
REG_AD_UPPER = (REG_BASE + 0x558)               ; 16
REG_AD_LOWER = (REG_BASE + 0x55a)               ; 16
REG_AD_CH04_INTMASK = (REG_BASE + 0x55c)        ; 16
REG_AD_ADVMODE = (REG_BASE + 0x55e)             ; 16

;;; Watchdog Timer
REG_WD_WP = (REG_BASE + 0x660)                  ; 16
REG_WD_EN = (REG_BASE + 0x662)                  ; 16
REG_WD_COMP_LOW = (REG_BASE + 0x664)            ; 16
REG_WD_COMP_HIGH = (REG_BASE + 0x666)           ; 16
REG_WD_CNT_LOW = (REG_BASE + 0x668)             ; 16
REG_WD_CNT_HIGH = (REG_BASE + 0x66a)            ; 16
REG_WD_CNTL = (REG_BASE + 0x66c)                ; 16

;;; 16-bit Timer
REG_T16_CR0A = (REG_BASE + 0x780)               ; 16
REG_T16_CR0B = (REG_BASE + 0x782)               ; 16
REG_T16_TC0 = (REG_BASE + 0x784)                ; 16
REG_T16_CTL0 = (REG_BASE + 0x786)               ; 16
REG_T16_CR1A = (REG_BASE + 0x788)               ; 16
REG_T16_CR1B = (REG_BASE + 0x78a)               ; 16
REG_T16_TC1 = (REG_BASE + 0x78c)                ; 16
REG_T16_CTL1 = (REG_BASE + 0x78e)               ; 16
REG_T16_CR2A = (REG_BASE + 0x790)               ; 16
REG_T16_CR2B = (REG_BASE + 0x792)               ; 16
REG_T16_TC2 = (REG_BASE + 0x794)                ; 16
REG_T16_CTL2 = (REG_BASE + 0x796)               ; 16
REG_T16_CR3A = (REG_BASE + 0x798)               ; 16
REG_T16_CR3B = (REG_BASE + 0x79a)               ; 16
REG_T16_TC3 = (REG_BASE + 0x79c)                ; 16
REG_T16_CTL3 = (REG_BASE + 0x79e)               ; 16
REG_T16_CR4A = (REG_BASE + 0x7a0)               ; 16
REG_T16_CR4B = (REG_BASE + 0x7a2)               ; 16
REG_T16_TC4 = (REG_BASE + 0x7a4)                ; 16
REG_T16_CTL4 = (REG_BASE + 0x7a6)               ; 16
REG_T16_CR5A = (REG_BASE + 0x7a8)               ; 16
REG_T16_CR5B = (REG_BASE + 0x7aa)               ; 16
REG_T16_TC5 = (REG_BASE + 0x7ac)                ; 16
REG_T16_CTL5 = (REG_BASE + 0x7ae)               ; 16
REG_DA16_CR0A = (REG_BASE + 0x7d0)              ; 16
REG_DA16_CR1A = (REG_BASE + 0x7d2)              ; 16
REG_DA16_CR2A = (REG_BASE + 0x7d4)              ; 16
REG_T16_CNT_PAUSE = (REG_BASE + 0x7dc)          ; 16
REG_T16_ADVMODE = (REG_BASE + 0x7de)            ; 16
REG_T16_CLKCTL_0 = (REG_BASE + 0x7e0)           ; 16
REG_T16_CLKCTL_1 = (REG_BASE + 0x7e2)           ; 16
REG_T16_CLKCTL_2 = (REG_BASE + 0x7e4)           ; 16
REG_T16_CLKCTL_3 = (REG_BASE + 0x7e6)           ; 16
REG_T16_CLKCTL_4 = (REG_BASE + 0x7e8)           ; 16
REG_T16_CLKCTL_5 = (REG_BASE + 0x7ea)           ; 16

;;; USB Function Controller
REG_USB_MAIN_INT_STAT = (REG_BASE + 0x900)      ; 8
REG_USB_SIE_INT_STAT = (REG_BASE + 0x901)       ; 8
REG_USB_EPR_INT_STAT = (REG_BASE + 0x902)       ; 8
REG_USB_DMA_INT_STAT = (REG_BASE + 0x903)       ; 8
REG_USB_FIFO_INT_STAT = (REG_BASE + 0x904)      ; 8
REG_USB_EP0_INT_STAT = (REG_BASE + 0x907)       ; 8
REG_USB_EPA_INT_STAT = (REG_BASE + 0x908)       ; 8
REG_USB_EPB_INT_STAT = (REG_BASE + 0x909)       ; 8
REG_USB_EPC_INT_STAT = (REG_BASE + 0x90a)       ; 8
REG_USB_EPD_INT_STAT = (REG_BASE + 0x90b)       ; 8
REG_USB_MAIN_INT_EN = (REG_BASE + 0x910)        ; 8
REG_USB_SIE_INT_EN = (REG_BASE + 0x911)         ; 8
REG_USB_EPR_INT_EN = (REG_BASE + 0x912)         ; 8
REG_USB_DMA_INT_EN = (REG_BASE + 0x913)         ; 8
REG_USB_FIFO_INT_EN = (REG_BASE + 0x914)        ; 8
REG_USB_EP0_INT_EN = (REG_BASE + 0x917)         ; 8
REG_USB_EPA_INT_EN = (REG_BASE + 0x918)         ; 8
REG_USB_EPB_INT_EN = (REG_BASE + 0x919)         ; 8
REG_USB_EPC_INT_EN = (REG_BASE + 0x91a)         ; 8
REG_USB_EPD_INT_EN = (REG_BASE + 0x91b)         ; 8
REG_USB_REVNUM = (REG_BASE + 0x920)             ; 8
REG_USB_USB_CTRL = (REG_BASE + 0x921)           ; 8
REG_USB_STATUS = (REG_BASE + 0x922)             ; 8
REG_USB_XCVR_CTRL = (REG_BASE + 0x923)          ; 8
REG_USB_TEST = (REG_BASE + 0x924)               ; 8
REG_USB_EPn_CTRL = (REG_BASE + 0x925)           ; 8
REG_USB_EPrFIFO_Clr = (REG_BASE + 0x926)        ; 8
REG_USB_FRAME_NUM_H = (REG_BASE + 0x92e)        ; 8
REG_USB_FRAME_NUM_L = (REG_BASE + 0x92f)        ; 8
REG_USB_EP0_SETUP_0 = (REG_BASE + 0x930)        ; 8
REG_USB_EP0_SETUP_1 = (REG_BASE + 0x931)        ; 8
REG_USB_EP0_SETUP_2 = (REG_BASE + 0x932)        ; 8
REG_USB_EP0_SETUP_3 = (REG_BASE + 0x933)        ; 8
REG_USB_EP0_SETUP_4 = (REG_BASE + 0x934)        ; 8
REG_USB_EP0_SETUP_5 = (REG_BASE + 0x935)        ; 8
REG_USB_EP0_SETUP_6 = (REG_BASE + 0x936)        ; 8
REG_USB_EP0_SETUP_7 = (REG_BASE + 0x937)        ; 8
REG_USB_ADDRESS = (REG_BASE + 0x938)            ; 8
REG_USB_EP0_CTRL = (REG_BASE + 0x939)           ; 8
REG_USB_EP0_CTRL_IN = (REG_BASE + 0x93a)        ; 8
REG_USB_EP0_CTRL_OUT = (REG_BASE + 0x93b)       ; 8
REG_USB_EP0_MAXSIZE = (REG_BASE + 0x93f)        ; 8
REG_USB_EPA_CTRL = (REG_BASE + 0x940)           ; 8
REG_USB_EPB_CTRL = (REG_BASE + 0x941)           ; 8
REG_USB_EPC_CTRL = (REG_BASE + 0x942)           ; 8
REG_USB_EPD_CTRL = (REG_BASE + 0x943)           ; 8
REG_USB_EPA_MAXSIZE_H = (REG_BASE + 0x950)      ; 8
REG_USB_EPA_MAXSIZE_L = (REG_BASE + 0x951)      ; 8
REG_USB_EPA_CONFIG_0 = (REG_BASE + 0x952)       ; 8
REG_USB_EPA_CONFIG_1 = (REG_BASE + 0x953)       ; 8
REG_USB_EPB_MAXSIZE_H = (REG_BASE + 0x954)      ; 8
REG_USB_EPB_MAXSIZE_L = (REG_BASE + 0x955)      ; 8
REG_USB_EPB_CONFIG_0 = (REG_BASE + 0x956)       ; 8
REG_USB_EPB_CONFIG_1 = (REG_BASE + 0x957)       ; 8
REG_USB_EPC_MAXSIZE_H = (REG_BASE + 0x958)      ; 8
REG_USB_EPC_MAXSIZE_L = (REG_BASE + 0x959)      ; 8
REG_USB_EPC_CONFIG_0 = (REG_BASE + 0x95a)       ; 8
REG_USB_EPC_CONFIG_1 = (REG_BASE + 0x95b)       ; 8
REG_USB_EPD_MAXSIZE_H = (REG_BASE + 0x95c)      ; 8
REG_USB_EPD_MAXSIZE_L = (REG_BASE + 0x95d)      ; 8
REG_USB_EPD_CONFIG_0 = (REG_BASE + 0x95e)       ; 8
REG_USB_EPD_CONFIG_1 = (REG_BASE + 0x95f)       ; 8
REG_USB_EPA_START_ADDR_H = (REG_BASE + 0x970)   ; 8
REG_USB_EPA_START_ADDR_L = (REG_BASE + 0x971)   ; 8
REG_USB_EPB_START_ADDR_H = (REG_BASE + 0x972)   ; 8
REG_USB_EPB_START_ADDR_L = (REG_BASE + 0x973)   ; 8
REG_USB_EPC_START_ADDR_H = (REG_BASE + 0x974)   ; 8
REG_USB_EPC_START_ADDR_L = (REG_BASE + 0x975)   ; 8
REG_USB_EPD_START_ADDR_H = (REG_BASE + 0x976)   ; 8
REG_USB_EPD_START_ADDR_L = (REG_BASE + 0x977)   ; 8
REG_USB_CPU_JOIN_RD = (REG_BASE + 0x980)        ; 8
REG_USB_CPU_JOIN_WR = (REG_BASE + 0x981)        ; 8
REG_USB_EN_EPN_FIFO = (REG_BASE + 0x982)        ; 8
REG_USB_EPN_FIFO_FOR_CPU = (REG_BASE + 0x983)   ; 8
REG_USB_EPN_RD_REMAIN_H = (REG_BASE + 0x984)    ; 8
REG_USB_EPN_RD_REMAIN_L = (REG_BASE + 0x985)    ; 8
REG_USB_EPN_WR_REMAIN_H = (REG_BASE + 0x986)    ; 8
REG_USB_EPN_WR_REMAIN_L = (REG_BASE + 0x987)    ; 8
REG_USB_DESC_ADDR_H = (REG_BASE + 0x988)        ; 8
REG_USB_DESC_ADDR_L = (REG_BASE + 0x989)        ; 8
REG_USB_DESC_SIZE_H = (REG_BASE + 0x98a)        ; 8
REG_USB_DESC_SIZE_L = (REG_BASE + 0x98b)        ; 8
REG_USB_DESC_DOOR = (REG_BASE + 0x98f)          ; 8
REG_USB_DMA_FIFO_CTRL = (REG_BASE + 0x990)      ; 8
REG_USB_DMA_JOIN = (REG_BASE + 0x991)           ; 8
REG_USB_DMA_CTRL = (REG_BASE + 0x992)           ; 8
REG_USB_DMA_CONFIG_0 = (REG_BASE + 0x994)       ; 8
REG_USB_DMA_CONFIG_1 = (REG_BASE + 0x995)       ; 8
REG_USB_DMA_Latency = (REG_BASE + 0x997)        ; 8
REG_USB_DMA_REMAIN_H = (REG_BASE + 0x998)       ; 8
REG_USB_DMA_REMAIN_L = (REG_BASE + 0x999)       ; 8
REG_USB_DMA_Count_HH = (REG_BASE + 0x99c)       ; 8
REG_USB_DMA_Count_HL = (REG_BASE + 0x99d)       ; 8
REG_USB_DMA_Count_LH = (REG_BASE + 0x99e)       ; 8
REG_USB_DMA_Count_LL = (REG_BASE + 0x99f)       ; 8

;;; Serial Interface
REG_EFSIF0_TXD = (REG_BASE + 0xb00)             ; 8
REG_EFSIF0_RXD = (REG_BASE + 0xb01)             ; 8
REG_EFSIF0_STATUS = (REG_BASE + 0xb02)          ; 8
REG_EFSIF0_CTL = (REG_BASE + 0xb03)             ; 8
REG_EFSIF0_IRDA = (REG_BASE + 0xb04)            ; 8
REG_EFSIF0_BRTRUN = (REG_BASE + 0xb05)          ; 8
REG_EFSIF0_BRTRDL = (REG_BASE + 0xb06)          ; 8
REG_EFSIF0_BRTRDM = (REG_BASE + 0xb07)          ; 8
REG_EFSIF0_BRTCDL = (REG_BASE + 0xb08)          ; 8
REG_EFSIF0_BRTCDM = (REG_BASE + 0xb09)          ; 8
REG_EFSIF1_TXD = (REG_BASE + 0xb10)             ; 8
REG_EFSIF1_RXD = (REG_BASE + 0xb11)             ; 8
REG_EFSIF1_STATUS = (REG_BASE + 0xb12)          ; 8
REG_EFSIF1_CTL = (REG_BASE + 0xb13)             ; 8
REG_EFSIF1_IRDA = (REG_BASE + 0xb14)            ; 8
REG_EFSIF1_BRTRUN = (REG_BASE + 0xb15)          ; 8
REG_EFSIF1_BRTRDL = (REG_BASE + 0xb16)          ; 8
REG_EFSIF1_BRTRDM = (REG_BASE + 0xb17)          ; 8
REG_EFSIF1_BRTCDL = (REG_BASE + 0xb18)          ; 8
REG_EFSIF1_BRTCDM = (REG_BASE + 0xb19)          ; 8
REG_EFSIF1_7816CTL = (REG_BASE + 0xb1a)         ; 8
REG_EFSIF1_7816STA = (REG_BASE + 0xb1b)         ; 8
REG_EFSIF1_FIDIL = (REG_BASE + 0xb1c)           ; 8
REG_EFSIF1_FIDIM = (REG_BASE + 0xb1d)           ; 8
REG_EFSIF1_TTGR = (REG_BASE + 0xb1e)            ; 8
REG_EFSIF1_CLKNUM = (REG_BASE + 0xb1f)          ; 8
REG_EFSIF2_TXD = (REG_BASE + 0xb20)             ; 8
REG_EFSIF2_RXD = (REG_BASE + 0xb21)             ; 8
REG_EFSIF2_STATUS = (REG_BASE + 0xb22)          ; 8
REG_EFSIF2_CTL = (REG_BASE + 0xb23)             ; 8
REG_EFSIF2_IRDA = (REG_BASE + 0xb24)            ; 8
REG_EFSIF2_BRTRUN = (REG_BASE + 0xb25)          ; 8
REG_EFSIF2_BRTRDL = (REG_BASE + 0xb26)          ; 8
REG_EFSIF2_BRTRDM = (REG_BASE + 0xb27)          ; 8
REG_EFSIF2_BRTCDL = (REG_BASE + 0xb28)          ; 8
REG_EFSIF2_BRTCDM = (REG_BASE + 0xb29)          ; 8
REG_EFSIF_ADV = (REG_BASE + 0xb4f)              ; 8

;;; Extended Ports
REG_PA_IOC = (REG_BASE + 0xc00)                 ; 8
REG_PA_DATA = (REG_BASE + 0xc01)                ; 8
REG_PB_IOC = (REG_BASE + 0xc02)                 ; 8
REG_PB_DATA = (REG_BASE + 0xc03)                ; 8
REG_PC_IOC = (REG_BASE + 0xc04)                 ; 8
REG_PC_DATA = (REG_BASE + 0xc05)                ; 8
REG_PA_03_CFP = (REG_BASE + 0xc20)              ; 8
REG_PA_4_CFP = (REG_BASE + 0xc21)               ; 8
REG_PB_03_CFP = (REG_BASE + 0xc22)              ; 8
REG_PC_03_CFP = (REG_BASE + 0xc24)              ; 8
REG_PC_47_CFP = (REG_BASE + 0xc25)              ; 8

;;; Misc registers (2)
REG_MISC_BUSPUP = (REG_BASE + 0xc40)            ; 8
REG_MISC_BUSLOW = (REG_BASE + 0xc41)            ; 8
REG_MISC_PUP0 = (REG_BASE + 0xc42)              ; 8
REG_MISC_PUP1 = (REG_BASE + 0xc43)              ; 8
REG_MISC_PUP2 = (REG_BASE + 0xc44)              ; 8
REG_MISC_PUP3 = (REG_BASE + 0xc45)              ; 8
REG_MISC_PUP4 = (REG_BASE + 0xc46)              ; 8
REG_MISC_PUP5 = (REG_BASE + 0xc47)              ; 8
REG_MISC_PUP6 = (REG_BASE + 0xc48)              ; 8
REG_MISC_PUP7 = (REG_BASE + 0xc49)              ; 8
REG_MISC_PUP8 = (REG_BASE + 0xc4a)              ; 8
REG_MISC_PUP9 = (REG_BASE + 0xc4b)              ; 8
REG_MISC_PUPA = (REG_BASE + 0xc4c)              ; 8
REG_MISC_PUPB = (REG_BASE + 0xc4d)              ; 8

;;; Intelligent DMA
REG_IDMABASE0 = (REG_BASE + 0x1100)             ; 16
REG_IDMABASE1 = (REG_BASE + 0x1102)             ; 16
REG_IDMA_START = (REG_BASE + 0x1104)            ; 16
REG_IDMA_EN = (REG_BASE + 0x1105)               ; 16

REG_HS0_CNT = (REG_BASE + 0x1120)               ; 16
REG_HS0_CTRL = (REG_BASE + 0x1122)              ; 16
REG_HS0_SADR_L = (REG_BASE + 0x1124)            ; 16
REG_HS0_SADR_H = (REG_BASE + 0x1126)            ; 16
REG_HS0_DADR_L = (REG_BASE + 0x1128)            ; 16
REG_HS0_DADR_H = (REG_BASE + 0x112a)            ; 16
REG_HS0_EN = (REG_BASE + 0x112c)                ; 16
REG_HS0_TF = (REG_BASE + 0x112e)                ; 16

REG_HS1_CNT = (REG_BASE + 0x1130)               ; 16
REG_HS1_CTRL = (REG_BASE + 0x1132)              ; 16
REG_HS1_SADR_L = (REG_BASE + 0x1134)            ; 16
REG_HS1_SADR_H = (REG_BASE + 0x1136)            ; 16
REG_HS1_DADR_L = (REG_BASE + 0x1138)            ; 16
REG_HS1_DADR_H = (REG_BASE + 0x113a)            ; 16
REG_HS1_EN = (REG_BASE + 0x113c)                ; 16
REG_HS1_TF = (REG_BASE + 0x113e)                ; 16

REG_HS2_CNT = (REG_BASE + 0x1140)               ; 16
REG_HS2_CTRL = (REG_BASE + 0x1142)              ; 16
REG_HS2_SADR_L = (REG_BASE + 0x1144)            ; 16
REG_HS2_SADR_H = (REG_BASE + 0x1146)            ; 16
REG_HS2_DADR_L = (REG_BASE + 0x1148)            ; 16
REG_HS2_DADR_H = (REG_BASE + 0x114a)            ; 16
REG_HS2_EN = (REG_BASE + 0x114c)                ; 16
REG_HS2_TF = (REG_BASE + 0x114e)                ; 16

REG_HS3_CNT = (REG_BASE + 0x1150)               ; 16
REG_HS3_CTRL = (REG_BASE + 0x1152)              ; 16
REG_HS3_SADR_L = (REG_BASE + 0x1154)            ; 16
REG_HS3_SADR_H = (REG_BASE + 0x1156)            ; 16
REG_HS3_DADR_L = (REG_BASE + 0x1158)            ; 16
REG_HS3_DADR_H = (REG_BASE + 0x115a)            ; 16
REG_HS3_EN = (REG_BASE + 0x115c)                ; 16
REG_HS3_TF = (REG_BASE + 0x115e)                ; 16

REG_HS0_ADVMODE = (REG_BASE + 0x1162)           ; 16
REG_HS0_ADV_SADR_L = (REG_BASE + 0x1164)        ; 16
REG_HS0_ADV_SADR_H = (REG_BASE + 0x1166)        ; 16
REG_HS0_ADV_DADR_L = (REG_BASE + 0x1168)        ; 16
REG_HS0_ADV_DADR_H = (REG_BASE + 0x116a)        ; 16

REG_HS1_ADVMODE = (REG_BASE + 0x1172)           ; 16
REG_HS1_ADV_SADR_L = (REG_BASE + 0x1174)        ; 16
REG_HS1_ADV_SADR_H = (REG_BASE + 0x1176)        ; 16
REG_HS1_ADV_DADR_L = (REG_BASE + 0x1178)        ; 16
REG_HS1_ADV_DADR_H = (REG_BASE + 0x117a)        ; 16

REG_HS2_ADVMODE = (REG_BASE + 0x1182)           ; 16
REG_HS2_ADV_SADR_L = (REG_BASE + 0x1184)        ; 16
REG_HS2_ADV_SADR_H = (REG_BASE + 0x1186)        ; 16
REG_HS2_ADV_DADR_L = (REG_BASE + 0x1188)        ; 16
REG_HS2_ADV_DADR_H = (REG_BASE + 0x118a)        ; 16

REG_HS3_ADVMODE = (REG_BASE + 0x1192)           ; 16
REG_HS3_ADV_SADR_L = (REG_BASE + 0x1194)        ; 16
REG_HS3_ADV_SADR_H = (REG_BASE + 0x1196)        ; 16
REG_HS3_ADV_DADR_L = (REG_BASE + 0x1198)        ; 16
REG_HS3_ADV_DADR_H = (REG_BASE + 0x119a)        ; 16

REG_HS_CNTLMODE = (REG_BASE + 0x119c)           ; 16
REG_HS_ACCTIME = (REG_BASE + 0x119e)            ; 16

;;; SRAM Controller
REG_SRAMC_BCLK_SETUP = (REG_BASE + 0x1500)      ; 32
REG_SRAMC_SWAIT = (REG_BASE + 0x1504)           ; 32
REG_SRAMC_SLV_SIZE = (REG_BASE + 0x1508)        ; 32
REG_SRAMC_A0_BSL = (REG_BASE + 0x150c)          ; 32
REG_SRAMC_ALS = (REG_BASE + 0x1510)             ; 32

;;; SDRAM controller
REG_SDRAMC_INI = (REG_BASE + 0x1600)            ; 32
REG_SDRAMC_CTL = (REG_BASE + 0x1604)            ; 32
REG_SDRAMC_REF = (REG_BASE + 0x1608)            ; 32
REG_SDRAMC_APP = (REG_BASE + 0x1610)            ; 32

;;; SPI
REG_SPI_RXD = (REG_BASE + 0x1700)               ; 32
REG_SPI_TXD = (REG_BASE + 0x1704)               ; 32
REG_SPI_CTL1 = (REG_BASE + 0x1708)              ; 32
REG_SPI_CTL2 = (REG_BASE + 0x170c)              ; 32
REG_SPI_WAIT = (REG_BASE + 0x1710)              ; 32
REG_SPI_STAT = (REG_BASE + 0x1714)              ; 32
REG_SPI_INT = (REG_BASE + 0x1718)               ; 32
REG_SPI_RXMK = (REG_BASE + 0x171c)              ; 32

;;; DCSIO
REG_DCSIO_CTL = (REG_BASE + 0x1800)             ; 32
REG_DCSIO_LOAD = (REG_BASE + 0x1804)            ; 32
REG_DCSIO_RCV = (REG_BASE + 0x1808)             ; 32
REG_DCSIO_INT = (REG_BASE + 0x1814)             ; 32
REG_DCSIO_STAT = (REG_BASE + 0x1818)            ; 32
REG_DCSIO_DIR = (REG_BASE + 0x181c)             ; 32

;;; Real Time Clock
REG_RTCINTSTAT = (REG_BASE + 0x1900)            ; 32
REG_RTCINTMODE = (REG_BASE + 0x1904)            ; 32
REG_RTC_CNTL0 = (REG_BASE + 0x1908)             ; 32
REG_RTC_CNTL1 = (REG_BASE + 0x190c)             ; 32
REG_RTCSEC = (REG_BASE + 0x1910)                ; 32
REG_RTCMIN = (REG_BASE + 0x1914)                ; 32
REG_RTCHOUR = (REG_BASE + 0x1918)               ; 32
REG_RTCDAY = (REG_BASE + 0x191c)                ; 32
REG_RTCMONTH = (REG_BASE + 0x1920)              ; 32
REG_RTCYEAR = (REG_BASE + 0x1924)               ; 32
REG_RTCDAYWEEK = (REG_BASE + 0x1928)            ; 32

;;; LCD Controller
REG_LCDC_INT = (REG_BASE + 0x1a00)              ; 32
REG_LCDC_PS = (REG_BASE + 0x1a04)               ; 32
REG_LCDC_HD = (REG_BASE + 0x1a10)               ; 32
REG_LCDC_VD = (REG_BASE + 0x1a14)               ; 32
REG_LCDC_MR = (REG_BASE + 0x1a18)               ; 32
REG_LCDC_HDPS = (REG_BASE + 0x1a20)             ; 32
REG_LCDC_VDPS = (REG_BASE + 0x1a24)             ; 32
REG_LCDC_L = (REG_BASE + 0x1a28)                ; 32
REG_LCDC_F = (REG_BASE + 0x1a2c)                ; 32
REG_LCDC_FO = (REG_BASE + 0x1a30)               ; 32
REG_LCDC_TSO = (REG_BASE + 0x1a40)              ; 32
REG_LCDC_TC1 = (REG_BASE + 0x1a44)              ; 32
REG_LCDC_TC0 = (REG_BASE + 0x1a48)              ; 32
REG_LCDC_TC2 = (REG_BASE + 0x1a4c)              ; 32
REG_LCDC_DMD = (REG_BASE + 0x1a60)              ; 32
REG_LCDC_IRAM = (REG_BASE + 0x1a64)             ; 32
REG_LCDC_MADD = (REG_BASE + 0x1a70)             ; 32
REG_LCDC_MLADD = (REG_BASE + 0x1a74)            ; 32
REG_LCDC_SADD = (REG_BASE + 0x1a80)             ; 32
REG_LCDC_SSP = (REG_BASE + 0x1a88)              ; 32
REG_LCDC_SEP = (REG_BASE + 0x1a8c)              ; 32
REG_LCDC_LUT_03 = (REG_BASE + 0x1aa0)           ; 32
REG_LCDC_LUT_47 = (REG_BASE + 0x1aa4)           ; 32
REG_LCDC_LUT_8B = (REG_BASE + 0x1aa8)           ; 32
REG_LCDC_LUT_CF = (REG_BASE + 0x1aac)           ; 32

;;; Clock Management Unit
REG_CMU_GATEDCLK0 = (REG_BASE + 0x1b00)         ; 32
REG_CMU_GATEDCLK1 = (REG_BASE + 0x1b04)         ; 32
REG_CMU_CLKCNTL = (REG_BASE + 0x1b08)           ; 32
REG_CMU_PLL = (REG_BASE + 0x1b0c)               ; 32
REG_CMU_SSCG = (REG_BASE + 0x1b10)              ; 32
REG_CMU_OPT = (REG_BASE + 0x1b14)               ; 32
REG_CMU_PROTECT = (REG_BASE + 0x1b24)           ; 32

;;; I2S interface
REG_I2S_CONTRL = (REG_BASE + 0x1c00)            ; 32
REG_I2S_DV_MCLK = (REG_BASE + 0x1c04)           ; 32
REG_I2S_DV_LRCLK = (REG_BASE + 0x1c08)          ; 32
REG_I2S_START = (REG_BASE + 0x1c0c)             ; 32
REG_I2S_HSDMAMD = (REG_BASE + 0x1c10)           ; 32
REG_I2S_FIFO_EMPTY = (REG_BASE + 0x1c14)        ; 32
REG_I2S_FIFO = (REG_BASE + 0x1c20)              ; 32
