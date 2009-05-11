/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005-2007 by Y.D.K.Co.,LTD Technologies company
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
 *  @(#) $Id: sys_config.c,v 1.2 2007/05/21 01:33:50 honda Exp $
 */

/*
 *    ターゲットシステム依存モジュール
 */

#include "jsp_kernel.h"
#include <ns9360.h>


/*
 * GPIO Setup 
 */
static void	cpuSetupGPIO(void);
/*
 * CS0 Setup
 */
static void	cpuSetupCS0(void);
/*
 * CS2 Setup
 */
static void	cpuSetupCS2(void);
/*
 * CS3 Setup
 */
static void	cpuSetupCS3(void);
/*
 * CS5 Setup
 */
static void	cpuSetupCS5(void);
/*
 * CS6 Setup
 */
static void	cpuSetupCS6(void);
/*
 * CS7 Setup
 */
static void	cpuSetupCS7(void);


/*
 * 割込みハンドラ登録用テーブル
 */
FP   int_table[MAX_INT_NUM];
UW   int_mask_table[MAX_INT_NUM];

/*
 * IRQとINT IDの対応テーブル、IRQに対応するINT IDを設定する。
 * このテーブルのデータから、IntConfigRegistarを設定する。
 * 未使用のIRQにはINT_ID_NONEをセットする
 */
const	UW	int_id_table[] = {
	INT_ID_NONE,			/* IRQ0 INT ID  : unuse							*/
	INT_ID_NONE,			/* IRQ1 INT ID  : unuse							*/
	INT_ID_NONE,			/* IRQ2 INT ID  : unuse							*/
	INT_ID_NONE,			/* IRQ3 INT ID  : unuse							*/
	INT_ID_TIMER0,			/* IRQ4 INT ID  : TIMER 0 Interrupt				*/
	INT_ID_NONE,			/* IRQ5 INT ID  : unuse							*/
	INT_ID_NONE,			/* IRQ6 INT ID  : unuse							*/
	INT_ID_NONE,			/* IRQ7 INT ID  : unuse							*/
	INT_ID_BBUS_INT,		/* IRQ8 INT ID  : BBus Aggregate Interrupt		*/
	INT_ID_NONE,			/* IRQ9 INT ID  : unuse							*/
	INT_ID_NONE,			/* IRQ10 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ11 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ12 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ13 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ14 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ15 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ16 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ17 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ18 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ19 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ20 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ21 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ22 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ23 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ24 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ25 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ26 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ27 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ28 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ29 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ30 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ31 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ32 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ33 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ34 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ35 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ36 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ37 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ38 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ39 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ40 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ41 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ42 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ43 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ44 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ45 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ46 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ47 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ48 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ49 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ50 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ51 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ52 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ53 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ54 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ55 INT ID : unuse							*/
	INT_ID_SERC_TX,			/* IRQ56 INT ID : SER C Tx Interrupt			*/
	INT_ID_SERC_RX,			/* IRQ57 INT ID : SER C Rx Interrupt			*/
	INT_ID_SERA_TX,			/* IRQ58 INT ID : SER A Tx Interrupt			*/
	INT_ID_SERA_RX,			/* IRQ59 INT ID : SER A Rx Interrupt			*/
	INT_ID_NONE,			/* IRQ60 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ61 INT ID : unuse							*/
	INT_ID_NONE,			/* IRQ62 INT ID : unuse							*/
	INT_ID_NONE				/* IRQ63 INT ID : unuse							*/
};


/*--- GPIO 設定情報 ---*/
/*
 **** Control Registers ****
 */
/* Control Registers #1 (GPIO[0]〜[31]) */
const	UW	BbusGpio_ContSts1 = (BBUS_GPIO_CONT_STS1_GPIO2 | BBUS_GPIO_CONT_STS1_GPIO16);

/* Control Registers #2 (GPIO[32]〜[63]) */
const	UW	BbusGpio_ContSts2 = (BBUS_GPIO_CONT_STS2_GPIO36 | BBUS_GPIO_CONT_STS2_GPIO37 
                                            | BBUS_GPIO_CONT_STS2_GPIO38 | BBUS_GPIO_CONT_STS2_GPIO39);
	
/* Control Registers #3 (GPIO[64]〜[72]) */
const	UW	BbusGpio_ContSts3 = 0;

/*
 **** Configration Registers ****
 */

/* Configration Registers #1 (GPIO[0]〜[7]) */
const	UW	BbusGpio_Config1 = (BBUS_GPIO_CONFIG1_GPIO0 | BBUS_GPIO_CONFIG1_GPIO1
                              | BBUS_GPIO_CONFIG1_GPIO2 | BBUS_GPIO_CONFIG1_GPIO3
                              | BBUS_GPIO_CONFIG1_GPIO4 | BBUS_GPIO_CONFIG1_GPIO5
                              | BBUS_GPIO_CONFIG1_GPIO6 | BBUS_GPIO_CONFIG1_GPIO7);

/* Configration Registers #2 (GPIO[8]〜[15]) */
const	UW	BbusGpio_Config2 = (BBUS_GPIO_CONFIG2_GPIO8 | BBUS_GPIO_CONFIG2_GPIO9
                              | BBUS_GPIO_CONFIG2_GPIO10 | BBUS_GPIO_CONFIG2_GPIO11
                              | BBUS_GPIO_CONFIG2_GPIO12 | BBUS_GPIO_CONFIG2_GPIO13
                              | BBUS_GPIO_CONFIG2_GPIO14 | BBUS_GPIO_CONFIG2_GPIO15);

/* Configration Registers #3 (GPIO[16]〜[23]) */
const	UW	BbusGpio_Config3 = (BBUS_GPIO_CONFIG3_GPIO16 | BBUS_GPIO_CONFIG3_GPIO17
                              | BBUS_GPIO_CONFIG3_GPIO18 | BBUS_GPIO_CONFIG3_GPIO19
                              | BBUS_GPIO_CONFIG3_GPIO20 | BBUS_GPIO_CONFIG3_GPIO21
                              | BBUS_GPIO_CONFIG3_GPIO22 | BBUS_GPIO_CONFIG3_GPIO23);

/* Configration Registers #4 (GPIO[24]〜[31]) */
const	UW	BbusGpio_Config4 = (BBUS_GPIO_CONFIG4_GPIO24 | BBUS_GPIO_CONFIG4_GPIO25
                              | BBUS_GPIO_CONFIG4_GPIO26 | BBUS_GPIO_CONFIG4_GPIO27
                              | BBUS_GPIO_CONFIG4_GPIO28 | BBUS_GPIO_CONFIG4_GPIO29
                              | BBUS_GPIO_CONFIG4_GPIO30 | BBUS_GPIO_CONFIG4_GPIO31);

/* Configration Registers #5 (GPIO[32]〜[39]) */
const	UW	BbusGpio_Config5 = (BBUS_GPIO_CONFIG5_GPIO32 | BBUS_GPIO_CONFIG5_GPIO33
                              | BBUS_GPIO_CONFIG5_GPIO34 | BBUS_GPIO_CONFIG5_GPIO35
                              | BBUS_GPIO_CONFIG5_GPIO36 | BBUS_GPIO_CONFIG5_GPIO37
                              | BBUS_GPIO_CONFIG5_GPIO38 | BBUS_GPIO_CONFIG5_GPIO39);

/* Configration Registers #6 (GPIO[40]〜[47]) */
const	UW	BbusGpio_Config6 = (BBUS_GPIO_CONFIG6_GPIO40 | BBUS_GPIO_CONFIG6_GPIO41
                              | BBUS_GPIO_CONFIG6_GPIO42 | BBUS_GPIO_CONFIG6_GPIO43
                              | BBUS_GPIO_CONFIG6_GPIO44 | BBUS_GPIO_CONFIG6_GPIO45
                              | BBUS_GPIO_CONFIG6_GPIO46 | BBUS_GPIO_CONFIG6_GPIO47);

/* Configration Registers #7 (GPIO[48]〜[55]) */
const	UW	BbusGpio_Config7 = (BBUS_GPIO_CONFIG7_GPIO48 | BBUS_GPIO_CONFIG7_GPIO49
                              | BBUS_GPIO_CONFIG7_GPIO50 | BBUS_GPIO_CONFIG7_GPIO51
                              | BBUS_GPIO_CONFIG7_GPIO52 | BBUS_GPIO_CONFIG7_GPIO53
                              | BBUS_GPIO_CONFIG7_GPIO54 | BBUS_GPIO_CONFIG7_GPIO55);

/* Configration Registers #8 (GPIO[56]〜[63]) */
const	UW	BbusGpio_Config8 = (BBUS_GPIO_CONFIG8_GPIO56 | BBUS_GPIO_CONFIG8_GPIO57
                              | BBUS_GPIO_CONFIG8_GPIO58 | BBUS_GPIO_CONFIG8_GPIO59
                              | BBUS_GPIO_CONFIG8_GPIO60 | BBUS_GPIO_CONFIG8_GPIO61
                              | BBUS_GPIO_CONFIG8_GPIO62 | BBUS_GPIO_CONFIG8_GPIO63);

/* Configration Registers #9 (GPIO[64]〜[71]) */
const	UW	BbusGpio_Config9 = (BBUS_GPIO_CONFIG9_GPIO64 | BBUS_GPIO_CONFIG9_GPIO65
                              | BBUS_GPIO_CONFIG9_GPIO66 | BBUS_GPIO_CONFIG9_GPIO67
                              | BBUS_GPIO_CONFIG9_GPIO68 | BBUS_GPIO_CONFIG9_GPIO69
                              | BBUS_GPIO_CONFIG9_GPIO70 | BBUS_GPIO_CONFIG9_GPIO71);

/* Configration Registers #10 (GPIO[72]) */
const	UW	BbusGpio_Config10 = (BBUS_GPIO_CONFIG10_GPIO72);


/*
 *  ターゲットシステム依存の初期化
 */
void
sys_initialize()
{
    /*
     *  ARMのベクタテーブルへ割込みハンドラを登録
     */
    arm_install_handler(IRQ_Number,IRQ_Handler);
  
    /*
     *  sys_putc が可能になるようにUARTを初期化
     */
    uart_init(LOGTASK_PORTID);
    /*
     * BBusの割込み許可
     */
     sil_wrw_mem((VP)BBBIE_REG, sil_rew_mem((VP)BBBIE_REG) | BBBI_GLBL);
}


/*
 *  ターゲットシステムの終了
 */
void
sys_exit(void)
{
    syslog(LOG_EMERG, "End Kernel.....!");
    while(1);
}


/*
 *  ターゲットシステムの文字出力
 */
void
sys_putc(char c)
{
    if (c == '\n') {
        uart_putc('\r');
    }
  uart_putc(c);  
}


/*
 *  割込みハンドラの設定
 *
 *  割込み番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 */
void
define_inh(INHNO inhno, FP inthdr)
{
	VW	reg;
	
    assert(inhno < MAX_INT_NUM);
    int_table[inhno] = inthdr;
    
    if(INT_ID_BBUS_F > inhno)
    {
		/*
		 * BBusBridge経由ではない割込みの登録
		 */
	    sil_wrw_mem((VP)(IVARV_REG+inhno*4),(VW)inthdr);     /* Interruput Vector Reg の設定 */
	    
	    /* IntConfigRegの設定 */
	    reg = sil_rew_mem((VP)((INT_CONFIG_REG+inhno)&0xfffffffc));
	    reg &= ~(0xff000000 >> (inhno%4)*8);
	    reg |= ((int_id_table[inhno] | INT_CFG_BIT_IT)<<24) >> ((inhno%4)*8);
	    sil_wrw_mem((VP)((INT_CONFIG_REG+inhno)&0xfffffffc), reg);
	}
}


/*
 * 割込みハンドラ起動時に割込みコントローラーにセットするマスク値を設定．
 */
void
define_inh_mask(INHNO inhno, UW mask)
{
    assert(inhno < MAX_INT_NUM);
    int_mask_table[inhno] = mask;
}


/*
 *  未定義の割込みが入った場合の処理
 */
void
undef_interrupt(){
    syslog(LOG_EMERG, "Unregistered Interrupt occurs.");
    while(1);
}

/*
 *  BBus割込みサービスルーチン
 */
void
BBus_isr()
{
	UW	i;
	UW	int_sts;
	
	int_sts = sil_rew_mem((VP)(BBBIS_REG)) & BBBIS_MASK;
	for( i = INT_ID_BBUS_F ; i < MAX_INT_NUM ; ++i )
	{
		if(int_sts & 0x80000000)
		{
			if(0 == int_table[i])
			{
				 undef_interrupt();
			}
			else
			{
				int_table[i]();
			}
		}
		int_sts <<= 1;
	}
}

/*
 *  CS、GPIO設定
 */
void cpu_CsGpioInit(void)
{
    BOOL    debugger, software_restart;
	software_restart = 0;
        cpuSetupGPIO();
		*(UW*)(BBUS_GPIO_CONT1_REG) |= 0x10;

	
    /* リスタートフラグの取得＆セット */
    software_restart = ((~AHB_ABT_CFG_REG_restart) & sil_rew_mem((VP)AHB_ABT_CFG_REG));
    														/* Restart Flag get		*/
	
	sil_wrw_mem((VP)AHB_ABT_CFG_REG , (AHB_ABT_CFG_REG_restart | sil_rew_mem((VP)AHB_ABT_CFG_REG)));
															/* Restart Flag set	*/

	
    /* デバッグフラグの取得 */
    debugger = ((~AHB_ABT_CFG_REG_debug) & sil_rew_mem((VP)AHB_ABT_CFG_REG));	/* Debug Flag get */
 
    if (!software_restart)
    {
        /* Setup the GPIO ports. */
        cpuSetupGPIO();

		*(UW*)(BBUS_GPIO_CONT1_REG) |= 0x10;

		/* Enable Instruction Cache */
		cpuEnableICache();

        /* Setup the Static chip selects */
        cpuSetupCS0();							/* CS0 setup 			*/
        cpuSetupCS2();							/* CS2 setup 			*/
        cpuSetupCS3();							/* CS3 setup 			*/

        /* Setup the Dynamic chip selects */
        cpuSetupCS5();							/* CS5 setup 			*/
        cpuSetupCS6();							/* CS6 setup 			*/
        cpuSetupCS7();							/* CS7 setup 			*/

    }
    else
    {
        /* Setup the GPIO ports. */
        cpuSetupGPIO();
        ;
        cpuEnableICache();
    }
    return;
}


/*
 * CS0 Setup
 */
static void	cpuSetupCS0(void)
{
	/*
	 * ターゲットに合わせて初期化処理を記述する
	 * Bootローダーにて設定済みの場合、ここでの設定は不要
	 */
}
/*
 * CS2 Setup
 */
static void	cpuSetupCS2(void)
{
	/*
	 * ターゲットに合わせて初期化処理を記述する
	 * Bootローダーにて設定済みの場合、ここでの設定は不要
	 */
}
/*
 * CS3 Setup
 */
static void	cpuSetupCS3(void)
{
	/*
	 * ターゲットに合わせて初期化処理を記述する
	 * Bootローダーにて設定済みの場合、ここでの設定は不要
	 */
}
/*
 * CS5 Setup
 */
static void	cpuSetupCS5(void)
{
	/*
	 * ターゲットに合わせて初期化処理を記述する
	 * Bootローダーにて設定済みの場合、ここでの設定は不要
	 */
}
/*
 * CS6 Setup
 */
static void	cpuSetupCS6(void)
{
	/*
	 * ターゲットに合わせて初期化処理を記述する
	 * Bootローダーにて設定済みの場合、ここでの設定は不要
	 */
}
/*
 * CS7 Setup
 */
static void	cpuSetupCS7(void)
{
	/*
	 * ターゲットに合わせて初期化処理を記述する
	 * Bootローダーにて設定済みの場合、ここでの設定は不要
	 */
}

/*
 * GPIO Setup 
 */
static void	cpuSetupGPIO(void)
{
	UW config;

	/*
	 **** Control Registers ****
	 */
	/* 該当pinをGPIO出力に設定した場合にデフォルト出力を指定したい場合に以下を記述する */
	
	/* Control Registers #1 (GPIO[0]〜[31]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONT1_REG , BbusGpio_ContSts1);

	/* Control Registers #2 (GPIO[32]〜[63]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONT2_REG , BbusGpio_ContSts2);
	
	/* Control Registers #3 (GPIO[64]〜[72]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONT3_REG , BbusGpio_ContSts3);

	/*
	 **** Configration Registers ****
	 */
	/* Configration Registers #1 (GPIO[0]〜[7]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG1_REG , BbusGpio_Config1);

	/* Configration Registers #2 (GPIO[8]〜[15]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG2_REG , BbusGpio_Config2);

	/* Configration Registers #3 (GPIO[16]〜[23]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG3_REG , BbusGpio_Config3);

	/* Configration Registers #4 (GPIO[24]〜[31]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG4_REG , BbusGpio_Config4);

	/* Configration Registers #5 (GPIO[32]〜[39]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG5_REG , BbusGpio_Config5);

	/* Configration Registers #6 (GPIO[40]〜[47]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG6_REG , BbusGpio_Config6);

	/* Configration Registers #7 (GPIO[48]〜[55]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG7_REG , BbusGpio_Config7);

	/* Configration Registers #8 (GPIO[56]〜[63]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG8_REG , BbusGpio_Config8);

	/* Configration Registers #9 (GPIO[64]〜[71]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG9_REG , BbusGpio_Config9);

	/* Configration Registers #10 (GPIO[72]) */
	sil_wrw_mem((VP) BBUS_GPIO_CONFIG10_REG , BbusGpio_Config10);
}


