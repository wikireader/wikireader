/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
 * 
 *  上記著作権者は，以下の (1)～(4) の条件か，Free Software Foundation 
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
 *  Tab Size : 8
 */

#ifndef	_DBG_MON_H_
#define	_DBG_MON_H_

/*
 *  定義
 *  definitions
 */

#define		TPS_E_OK			(0x00)
#define		TPS_E_NG			(0x01)

#define		TPS_FLAG_OFF			(0x00)
#define		TPS_FLAG_ON			(0x01)

#define		TPS_SIZE_LOGCTL			(0x0c)
#define		TPS_OSET_MAXRECORD		(0x00)
#define		TPS_OSET_RDSTART		(0x04)
#define		TPS_OSET_WTSTART		(0x06)
#define		TPS_OSET_OWCTRL			(0x08)
#define		TPS_OSET_OWSTATE		(0x09)

#define		TPS_SIZE_DISPLOG		(0x08)
#define		TPS_OSET_DIPS_SYSCLK		(0x00)
#define		TPS_OSET_DISP_CTXTYPE		(0x04)
#define		TPS_OSET_DISP_CTXID		(0x6)

#define		TPS_SIZE_KOBJLOG		(0x10)
#define		TPS_OSET_KOBJ_SYSCLK		(0x00)
#define		TPS_OSET_KOBJ_OBJTYPE		(0x04)
#define		TPS_OSET_KOBJ_ACCTYPE		(0x05)
#define		TPS_OSET_KOBJ_OBJID		(0x06)
#define		TPS_OSET_KOBJ_RESULT		(0x08)
#define		TPS_OSET_KOBJ_CTXTYPE		(0x09)
#define		TPS_OSET_KOBJ_CTXID		(0x0a)
#define		TPS_OSET_KOBJ_DATAID		(0x0c)

#define		TPS_CTXTYPE_TASK		(0x00)
#define		TPS_CTXTYPE_CYCHDR		(0x01)
#define		TPS_CTXTYPE_INTHDR		(0x02)
#define		TPS_CTXTYPE_EXCHDR		(0x03)
#define		TPS_CTXTYPE_IDLE		(0x10)

#define		TPS_OBJTYPE_FLAG		(0x01)
#define		TPS_OBJTYPE_MBOX		(0x03)

#define		TPS_ACCTYPE_SND			(0x00)
#define		TPS_ACCTYPE_RCV			(0x01)
#define		TPS_ACCTYPE_OTHER		(0x02)

#define		TPS_MAX_LOGTYPE			(0x02)
#define		TPS_LOGTYPE_DISP		(0x00)
#define		TPS_LOGTYPE_KOBJ		(0x01)

#define		TPS_MAX_DISPLOG			(4096)
#define		TPS_MAX_KOBJLOG			(4096)

#define		TPS_C_TIMER_HANDLER		(timer_handler_entry)


#ifndef	_MACRO_ONLY

#include "itron.h"

/*
 *  マクロ
 *  macro
 */
#define		TPS_M_4PARS2BCD(bP1, bP2, bP3, bP4)	((unsigned long)(\
							 ((unsigned long)bP1 << 24) |\
							 ((unsigned long)bP2 << 16) |\
							 ((unsigned long)bP3 <<  8) |\
							 ((unsigned long)bP4)))
/*
 *  構造体
 *  Structures
 */
typedef	struct {
	unsigned long	ulMaxRecCount;			/* 最大記録件数			*/
							/* Maximum record count		*/
	unsigned short	usRefPoint;			/* ログ取得開始位置		*/
							/* Oldest log position		*/
	unsigned short	usRecPoint;			/* ログ記録位置			*/
							/* Recording position		*/
	unsigned char	bOWEnable;			/* オーバーライト許可フラグ	*/
							/* OverWrite Enable flag	*/
	unsigned char	bOWState;			/* オーバーライト発生フラグ	*/
							/* OverWrite state flag		*/
	unsigned char	bRsv[2];			/* Reserved */
} TPS_LOGREC_CTRL;


typedef	struct {
	SYSTIM		ulRecTime;			/* ログ記録時刻			*/
							/* Recorded time		*/
	unsigned char	bCtxType;			/* 処理種別			*/
							/* Context type			*/
	unsigned char	bRsv;				/* Reserved			*/
	unsigned short	usCtxId;			/* 実行状態に移行した処理のID	*/
							/* Context ID			*/
} TPS_DISPLOG;


typedef	struct {
	SYSTIM		ulRecTime;			/* ログ記録時刻			*/
							/* Recorded time		*/
	unsigned char	bObjType;			/* カーネルオブジェクト種別	*/
							/* Kernel object type		*/
	unsigned char	bAccessType;			/* アクセス種別			*/
							/* Access type			*/
	unsigned short	usObjectId;			/* アクセスが発生したカーネルオブジェクトのID */
							/* ID of accessed kernel object	*/
	unsigned char	bResult;			/* 処理結果			*/
							/* Result			*/
	unsigned char	bCtxType;			/* アクセスした処理の種別	*/
							/* Context type			*/
	unsigned short	usCtxId;			/* アクセスした処理のID		*/
							/* Context ID			*/
	unsigned long	ulDataId;			/* データID			*/
							/* Data ID			*/
} TPS_KOBJLOG;


typedef	struct {					/* ディスパッチログ記録領域		*/
							/* Recording area for dispatch log	*/
	TPS_LOGREC_CTRL	stRecCtrl;			/* ログ記録領域制御情報			*/
							/* Control information			*/
	TPS_DISPLOG	stDispLog[TPS_MAX_DISPLOG];	/* ディスパッチログ			*/
} TPS_DISPATCH_INFO;					/* Log buffer				*/


typedef	struct {					/* カーネルオブジェクト記録領域		*/
							/* Recording area for kernel object	*/
							/* access log				*/
	TPS_LOGREC_CTRL	stRecCtrl;			/* ログ記録領域制御情報			*/
							/* Control information			*/
	TPS_KOBJLOG	stKObjLog[TPS_MAX_KOBJLOG];	/* カーネルオブジェクトアクセスログ	*/
} TPS_KOBJACCESS_INFO;					/* Log buffer				*/


typedef	struct {					/* OS種別格納テーブル			*/
							/* OS identity information table	*/
	char	chOSName[20];				/* OS名称				*/
							/* OS name				*/
	char	chOSVersion[10];			/* バージョン情報			*/
							/* OS version*/
} TPS_OSIDENT_INFO;

typedef	struct {
	unsigned short	usUnitSize;			/* オブジェクト情報基本サイズ	*/
							/* Unit size of target		*/
	unsigned short	usObjCnt;			/* オブジェクト生成数		*/
							/* Generated count		*/
	void		*pObjInfoAddr;			/* オブジェクト情報配置アドレス	*/
} TPS_OBJINFO_TABLE;					/* assigned address		*/


/*
 *  プロトタイプ宣言
 *  Proto-type declarations
 */
extern	void	tpsInitMonitor(VP_INT);
extern	void	tpsRecDispatchLog(unsigned char, void *);
extern	void	tpsRecKObjAccessLog(unsigned short, void *, unsigned long, unsigned long);

extern	void	timer_handler_entry(void);

/*
 *  外部変数
 *  variables for external
 */
extern	void	*_kernel_tps_SymStack[];
extern	void	*_kernel_tps_CycHdrTorch;


#endif	/* _MACRO_ONLY */

#endif	/* _DBG_MON_H_ */
