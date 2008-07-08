/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2003-2004 by CHUO ELECTRIC WORKS Co.,LTD. JAPAN
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
 */
/*
 *	ターゲットシステム依存モジュール
 *	(株)中央製作所製μITRON搭載SH1CPUボード用
 */
 
#include <s_services.h>
#include "kernel_id.h"

#include <sh1.h>
#include "sys_support.h"

/*
 *  TINET使用時
 */
#ifdef	SUPPORT_INET4
#include <sh1_sil.h>
#include <tinet_sys_config.h>
/*
 *  NIC (RTL8019AS) に関する定義
 *  TINET使用時tinet_sys_config.h へ移動すること
 */
/*#define ED_BASE_ADDRESS	0x06000000	*/	/* NIC のレジスタベースアドレス */
/*#define INHNO_IF_ED		IRQ5		*/	/* IRQ5 */
/*#define ED_PRI		7		*/	/* 優先度=7 */
/*#define ED_LEVEL0		0		*/	/* 割込み禁止  */
/*#define ED_IPR		IPRB		*/	/* 優先度レジスタ名 */
/*#define ED_IPR_SHIFT		8		*/	/* 優先度レジスタ内ビット位置 */
#endif	/* SUPPORT_INET4 */

/*
 *  ROMICE使用のNMIまでのROMベクタテーブル領域
 */
#define ROMVECT_START	0			/* ROMのベクタテーブルは０番地から 	*/
#define ROMVECT_SIZE	((NMI+1)*sizeof(FP))	/* ROMのベクタテーブルサイズ 		*/

/*
 *  低レベルのターゲットシステム依存の初期化 _hardware_init_hook
 *
 *  スタートアップモジュールの中で，メモリの初期化の前に呼び出される．
 */
void
hardware_init_hook(void)
{
	/* WCR3:ウエイトコントロールレジスタ３
	 * WPU=1    WAIT端子プルアップ
	 * A02LW=00 エリア0,2を1ﾛﾝｸﾞｳｪｲﾄ
	 * A6LW=00  エリア6  を1ﾛﾝｸﾞｳｪｲﾄ
	 */
	sil_wrh_mem(BSC_WCR3, 0x8000); 
	
	/* BCR:バスコントロールレジスタ
	 * DRANE=0   ｴﾘｱ1は外部メモリ空間
	 * IOE=0     ｴﾘｱ6は外部メモリ空間
	 * WARP=0    ﾉｰﾏﾙﾓｰﾄﾞ:外部ｱｸｾｽと内部ｱｸｾｽを同時に行わない
	 * RDDTY=0   RD信号HI T1の50%
	 * BAS=0     WRH,WRL,A0信号有効
	 */
	sil_wrh_mem(BSC_BCR, 0x0000);
	
	/* WCR1:ウエイトコントロールレジスタ１
	 * read cycle stateで
	 * wait端子サンプルしない
	 * RWx=0  エリア1,3,4,5,7 1ｽﾃｰﾄ固定
	 * RWx=0  エリア0,2,6 1ｽﾃｰﾄ+ﾛﾝｸﾞｳｪｲﾄ（計3ｽﾃｰﾄ）
	 * WW1=1  エリア1外部ﾒﾓﾘ 2ｽﾃｰﾄ
	 */
	sil_wrh_mem(BSC_WCR1, 0x00ff);
	
	/* WCR2:ウエイトコントロールレジスタ２
	 * DMAないので初期値のまま
	 */
	sil_wrh_mem(BSC_WCR2, 0xffff); 
	
	/*
	 * ＰＡ、ＰＢ出力バッファ初期化
	 */
	sil_wrh_mem(PA_DR, VAL_PA_DR);
	sil_wrh_mem(PB_DR, VAL_PB_DR); 

	/*
	 * ＰＡ入出力初期化（sys_support.h参照）
	 */
	sil_wrh_mem(PFC_PACR1, VAL_PFC_PACR1);
	sil_wrh_mem(PFC_PACR2, VAL_PFC_PACR2);
                                        
	sil_wrh_mem(PFC_PAIOR, VAL_PFC_PAIOR);
                    
	sil_wrh_mem(PFC_PBCR1, VAL_PFC_PBCR1);
	sil_wrh_mem(PFC_PBCR2, VAL_PFC_PBCR2);
	sil_wrh_mem(PFC_PBIOR, VAL_PFC_PBIOR);

	sil_wrh_mem(PFC_CASCR, VAL_PFC_CASCR);

}

/*
 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処理 software_init_hook
 *
 *  スタートアップモジュールの中で，カーネルを起動する前に呼び出される．
 */
#ifdef	PARTNER
void
software_init_hook(void)
{
	/*
	 *  ROMICE使用時にROMベクタテーブルを転送
	 */
	extern FP vector_table[];
	memcpy(vector_table, ROMVECT_START, ROMVECT_SIZE);
}
#endif

/*
 *  NIC ハードウェア割り込み許可
 *  TINET使用時必要
 */
#ifdef	SUPPORT_INET4
void
ed_ena_inter(void)
{
	SIL_PRE_LOC;
	SIL_LOC_INT();
	define_int_plevel(ED_IPR, ED_PRI, ED_IPR_SHIFT);// 優先度=ED_PRI
	SIL_UNL_INT();
}

/*
 *  NIC ハードウェア割り込み禁止
 *  TINET使用時必要
 */
void
ed_dis_inter(void)
{
	SIL_PRE_LOC;
	SIL_LOC_INT();
	define_int_plevel(ED_IPR, ED_LEVEL0, ED_IPR_SHIFT);// 優先度=0
	SIL_UNL_INT();
}
#endif	/* SUPPORT_INET4 *//* end */

