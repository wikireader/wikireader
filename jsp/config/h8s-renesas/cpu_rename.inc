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
;  @(#) $Id: cpu_rename.inc,v 1.2 2007/03/23 07:59:26 honda Exp $
;

 .AIFDEF _CPU_RENAME_INC_
 .AELSE

_CPU_RENAME_INC_: .DEFINE ""


;
;　シンボルのリネーム
;

;
;　共通部
;

;　　　変数
_runtsk:                .DEFINE         "__kernel_runtsk"
_schedtsk:              .DEFINE         "__kernel_schedtsk"
_reqflg:                .DEFINE         "__kernel_reqflg"
_enadsp:                .DEFINE         "__kernel_enadsp"
;　　　関数
_call_texrtn            .DEFINE         "__kernel_call_texrtn"

;
;　cpu_config.c
;

;　　　変数
_intnest:               .DEFINE         "__kernel_intnest"
_iscpulocked:           .DEFINE         "__kernel_iscpulocked"
_task_intmask:          .DEFINE         "__kernel_task_intmask"
;　　　関数
_cpu_experr:            .DEFINE         "__kernel_cpu_experr"

;
;　cpu_support.src
;

;　　　関数
_dispatch:              .DEFINE         "__kernel_dispatch"
_exit_and_dispatch:     .DEFINE         "__kernel_exit_and_dispatch"
_activate_r:            .DEFINE         "__kernel_activate_r"
_no_reg_exception       .DEFINE         "__kernel_no_reg_exception"
_current_ccr            .DEFINE         "__kernel_current_ccr"
_set_ccr                .DEFINE         "__kernel_set_ccr"
_current_exr            .DEFINE         "__kernel_current_exr"
_set_exr                .DEFINE         "__kernel_set_exr"
;　　その他
_common_interrupt_process .DEFINE       "__kernel_common_interrupt_process"

;
;　start.src
;
start                   .DEFINE         "__kernel_start"
_clear_b_section        .DEFINE         "__kernel_clear_b_section"

 .AENDI ;  _CPU_RENAME_INC_
