/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2005 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  シリアルI/Oデバイス（SIO）ドライバ（Xstormy16用）
 */

#include <kernel.h>
#include <s_services.h>
#include <hw_serial.h>
#include <cpu_sfrs.h>

/*
 *  非同期送信でのTxReadyのタイムアウト
 */
#define TXREADY_TIMEOUT 400

/*
 *  SIOPCB*チェックのためのマジック値
 */
#define MAGIC_SIOPCB 0xabcd

/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
static SIOPCB siopcb_table[TNUM_PORT] = {
    { MAGIC_SIOPCB, __U0CR, 0x1400 },
    { MAGIC_SIOPCB, __U1CR, 0x4800 },
} ;

/*
 *  SIOドライバの初期化ルーチン
 */
void sio_initialize(void)
{
    /* nothing to do here */
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *sio_opn_por(ID siopid, VP_INT exinf)
{
    SIOPCB* siopcb ;
    UH uhBase ;
                                /*
                                 *  オープン出来るポートかのチェック
                                 */
    if ( siopid < 1 || siopid > TNUM_PORT ) return NULL ;
    siopcb = &siopcb_table[siopid-1] ;
    if ( siopcb->bNotExist ) return NULL ;
                                /*
                                 *  既にオープン済みの場合
                                 */
    if ( siopcb->bOpened ) {
        siopcb->vpiExinf = exinf ;
        return siopcb ;
    }
                                /*
                                 *  SIOの初期化と存在のチェック
                                 */
    uhBase = siopcb->uhBase ;
    __SFR(uhBase) = __UxCR_RUN|__UxCR_TXEMPTY ;
    if ( ( __SFR(uhBase) &
        ( __UxCR_RUN|__UxCR_BAUDRATE|__UxCR_PARITY|__UxCR_TXIE|
        __UxCR_RXIE) ) != __UxCR_RUN ) {
        siopcb->bNotExist = TRUE ;
        return NULL ;
    }
                                /*
                                 *  管理テーブルの初期化
                                 */
    siopcb->vpiExinf = exinf ;
    siopcb->bOpened = TRUE ;
    siopcb->bTxIe = FALSE ;
    siopcb->bRxIe = FALSE ;
    siopcb->bLastIsCr = FALSE ;
                                /*
                                 *  割り込みハンドラの登録と許可
                                 */
    define_inh( INHNO_SIO, sio_handler ) ;
    __UNLOCK_EXCP() ;
    __SFRW(__EXCPL) |= siopcb->uhExcpMask ;
    __LOCK_EXCP() ;
    return siopcb ;
}

/*
 *  シリアルI/Oポートのクローズ
 */
void sio_cls_por(SIOPCB *siopcb)
{
                                /*
                                 *  クローズ出来るポートかのチェック
                                 */
    if ( !siopcb || siopcb->uhMagic != MAGIC_SIOPCB ||
        siopcb->bNotExist || !siopcb->bOpened ) return ;
                                /*
                                 *  管理テーブルの変更
                                 */
    siopcb->bOpened = FALSE ;
                                /*
                                 *  割り込みの禁止
                                 */
    __UNLOCK_EXCP() ;
    __SFRW(__EXCPL) &= ~siopcb->uhExcpMask ;
    __LOCK_EXCP() ;
                                /*
                                 *  SIOの停止
                                 */
    __SFR(siopcb->uhBase) = 0 ;
}

/*
 *  SIOの割込みハンドラ
 */
void sio_handler(void)
{
                                /*
                                 *  全てのポートについてチェック
                                 */
    ID portid ;
    for ( portid = 0 ; portid < TNUM_PORT ; portid++ ) {
        SIOPCB *siopcb = &siopcb_table[portid] ;
        UH uhBase = siopcb->uhBase ;
                                /*
                                 *  送信完了割り込みか
                                 */
        if ( ( __SFR(uhBase) & ( __UxCR_TXIE | __UxCR_TXEMPTY ) ) ==
            ( __UxCR_TXIE | __UxCR_TXEMPTY ) ) {
            sio_ierdy_snd( siopcb->vpiExinf ) ;
        }
                                /*
                                 *  受信割り込みか
                                 */
        if ( ( __SFR(uhBase) & ( __UxCR_RXIE | __UxCR_RXREADY ) ) ==
            ( __UxCR_RXIE | __UxCR_RXREADY ) ) {
            sio_ierdy_rcv( siopcb->vpiExinf ) ;
        }
    }
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL sio_snd_chr(SIOPCB *siopcb, char c)
{
    UH uhBase ;
                                /*
                                 *  有効なポートかどうかのチェック
                                 */
    if ( !siopcb || siopcb->uhMagic != MAGIC_SIOPCB ||
        siopcb->bNotExist || !siopcb->bOpened ) return FALSE ;
    uhBase = siopcb->uhBase ;
                                /*
                                 *  送信可能かどうかのチェック
                                 */
    if ( ! ( __SFR(uhBase) & __UxCR_TXEMPTY ) ) return FALSE ;
                                /*
                                 *  送信データの書き込み
                                 */
    __SFRW(uhBase+4) = __UxTX_STOPBIT | c ;
    return TRUE ;
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT sio_rcv_chr(SIOPCB *siopcb)
{
    UH uhBase ;
                                /*
                                 *  有効なポートかどうかのチェック
                                 */
    if ( !siopcb || siopcb->uhMagic != MAGIC_SIOPCB ||
        siopcb->bNotExist || !siopcb->bOpened ) return -1 ;
    uhBase = siopcb->uhBase ;
                                /*
                                 *  受信データ有無のチェック
                                 */
    if ( ! ( __SFR(uhBase) & __UxCR_RXREADY ) ) return -1 ;
                                /*
                                 *  受信データの取り出し
                                 */
    return __SFRW(uhBase+2) & 0xff ;
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void sio_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
    UH uhBase ;
                                /*
                                 *  有効なポートかどうかのチェック
                                 */
    if ( !siopcb || siopcb->uhMagic != MAGIC_SIOPCB ||
        siopcb->bNotExist || !siopcb->bOpened ) return ;
    uhBase = siopcb->uhBase ;
                                /*
                                 *  送信割り込みの許可
                                 */
    if ( cbrtn == SIO_ERDY_SND ) {
        __SFR(uhBase) |= __UxCR_TXIE ;
        siopcb->bTxIe = TRUE ;
    }
                                /*
                                 *  受信割り込みの許可
                                 */
    if ( cbrtn == SIO_ERDY_RCV ) {
        __SFR(uhBase) |= __UxCR_RXIE ;
        siopcb->bRxIe = TRUE ;
    }
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void sio_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
    UH uhBase ;
                                /*
                                 *  有効なポートかどうかのチェック
                                 */
    if ( !siopcb || siopcb->uhMagic != MAGIC_SIOPCB ||
        siopcb->bNotExist || !siopcb->bOpened ) return ;
    uhBase = siopcb->uhBase ;
                                /*
                                 *  送信割り込みの禁止
                                 */
    if ( cbrtn == SIO_ERDY_SND ) {
        __SFR(uhBase) &= ~__UxCR_TXIE ;
        siopcb->bTxIe = FALSE ;
    }
                                /*
                                 *  受信割り込みの禁止
                                 */
    if ( cbrtn == SIO_ERDY_RCV ) {
        __SFR(uhBase) &= ~__UxCR_RXIE ;
        siopcb->bRxIe = FALSE ;
    }
}

/*
 *  デフォルトの送信可能コールバック
 */
void sio_ierdy_snd(VP_INT exinf) __attribute__((__weak__)) ;
void sio_ierdy_snd(VP_INT exinf)
{
}

/*
 *  デフォルトの受信通知コールバック
 */
void sio_ierdy_rcv(VP_INT exinf) __attribute__((__weak__)) ;
void sio_ierdy_rcv(VP_INT exinf)
{
}

/*
 *  シリアルI/Oポートへの非同期文字出力
 */
void sio_asnd_chr( INT c )
{
    SIOPCB* siopcb ;
    UH uhBase ;
    int i ;
                                /*
                                 *  CPUロック状態にする
                                 */
    BOOL bLocked ;
    if ( sns_loc() ) bLocked = TRUE ;
    else {
        bLocked = FALSE ;
        if ( sns_ctx() ) iloc_cpu() ;
        else loc_cpu() ;
    }
                                /*
                                 *  非同期用ポートを開く
                                 */
    if ( ( siopcb = sio_opn_por( LOGTASK_PORTID,
        siopcb_table[LOGTASK_PORTID-1].vpiExinf ) ) != NULL ) {
        uhBase = siopcb->uhBase ;
                                /*
                                 *  LFならまずCRを送出
                                 */
        if ( c == '\n' ) {
            for ( i = 0 ; i < TXREADY_TIMEOUT ; i++ ) {
                if ( __SFR(uhBase) & __UxCR_TXEMPTY ) break ;
            }
            if ( i < TXREADY_TIMEOUT ) {
                __SFRW(uhBase+4) = __UxTX_STOPBIT | '\r' ;
            }
        }
                                /*
                                 *  データ送出
                                 */
        for ( i = 0 ; i < TXREADY_TIMEOUT ; i++ ) {
            if ( __SFR(uhBase) & __UxCR_TXEMPTY ) break ;
        }
        if ( i < TXREADY_TIMEOUT ) {
            __SFRW(uhBase+4) = __UxTX_STOPBIT | c ;
        }
    }
                                /*
                                 *  CPUロックを元に戻す
                                 */
    if ( !bLocked ) {
        if ( sns_ctx() ) iunl_cpu() ;
        else unl_cpu() ;
    }
}

/*
 *  シリアルI/Oポートからの非同期文字入力
 */
INT sio_arcv_chr( void )
{
    SIOPCB* siopcb ;
    UH uhBase ;
    INT c = -1 ;
                                /*
                                 *  タスクコンテキストからのみ使用可
                                 */
    if ( sns_ctx() || sns_loc() || sns_dsp() ) return -1 ;
                                /*
                                 *  CPUロック状態にする
                                 */
    loc_cpu() ;
                                /*
                                 *  非同期用ポートを開く
                                 */
    if ( ( siopcb = sio_opn_por( LOGTASK_PORTID,
        siopcb_table[LOGTASK_PORTID-1].vpiExinf ) ) != NULL ) {
        uhBase = siopcb->uhBase ;
                                /*
                                 *  受信待ち
                                 */
        while(1) {
            if ( __SFR(uhBase) & __UxCR_RXREADY ) {
                c = __SFRW(uhBase+2) & 0xff ;
                if ( c == '\r' ) {
                    c = '\n' ;
                    siopcb->bLastIsCr = TRUE ;
                }
                else {
                    if ( c == '\n' && siopcb->bLastIsCr ) c = -1 ;
                    siopcb->bLastIsCr = FALSE ;
                }
            }
            if ( c != -1 ) break ;
            unl_cpu() ;
            dly_tsk( 50 ) ;
            loc_cpu() ;
        }
    }
                                /*
                                 *  CPUロックを解除
                                 */
    unl_cpu() ;
    return c ;
}

