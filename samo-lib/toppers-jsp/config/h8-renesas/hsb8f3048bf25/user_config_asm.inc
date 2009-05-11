;
;  TOPPERS/JSP Kernel
;      Toyohashi Open Platform for Embedded Real-Time Systems/
;      Just Standard Profile Kernel
;
;  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
;                              Toyohashi Univ. of Technology, JAPAN
;  Copyright (C) 2001-2007 by Industrial Technology Institute,
;                              Miyagi Prefectural Government, JAPAN
;  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
;                   Tomakomai National College of Technology, JAPAN
;
;  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation
;  によって公表されている GNU General Public License の Version 2 に記
;  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
;  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
;  利用と呼ぶ）することを無償で許諾する．
;  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
;      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
;      スコード中に含まれていること．
;  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
;      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
;      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
;      の無保証規定を掲載すること．
;  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
;      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
;      と．
;    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
;        作権表示，この利用条件および下記の無保証規定を掲載すること．
;    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
;        報告すること．
;  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
;      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
;
;  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
;  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
;  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
;  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
;
;  @(#) $Id: user_config_asm.inc,v 1.7 2007/03/23 07:58:33 honda Exp $
;

;
;　ユーザー・コンフィギュレーション情報の定義
;

 .AIFDEF _USER_CONFIG_ASM_H_
 .AELSE

_USER_CONFIG_ASM_H_: .DEFINE ""

;  非タスクコンテキスト用スタックポインタの初期値
STACKTOP:               .DEFINE         "H'ffff0c" ; 内蔵RAM

;　chg_ipmをサポートするかどうかの定義
;　　C言語用のuser_config.hにも同様の定義が必要なので注意
;
SUPPORT_CHG_IPM:        .DEFINE         " "

;　クロック周波数 [Hz]
;　　C言語用のuser_config.hにも同様の定義が必要なので注意
;
CPU_CLOCK:              .DEFINE         "24576000"

;
;　シリアルに関する定義
;

;  サポートするシリアルデバイスの数（最大 2）
;　　C言語用のuser_config.hにも同様の定義が必要なので注意
TNUM_PORT:              .DEFINE         "1"

; エラー割り込みを、入力割り込みと別に操作する場合はコメントを外す。
;　　user_config_asm.incにも同様の定義が必要なので注意
;
;H8_CFG_SCI_ERR_HANDLER:                .DEFINE         " "

 .AENDI ;  _USER_CONFIG_ASM_H_

