;
;  TOPPERS/JSP Kernel
;      Toyohashi Open Platform for Embedded Real-Time Systems/
;      Just Standard Profile Kernel
;
;  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
;                              Toyohashi Univ. of Technology, JAPAN
;  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
;                   Tomakomai National College of Technology, JAPAN
;  Copyright (C) 2001-2007 by Industrial Technology Institute,
;                              Miyagi Prefectural Government, JAPAN
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
;  @(#) $Id: h8s2339f.inc,v 1.2 2007/03/23 07:59:26 honda Exp $
;

;  H8S/2339F用定義（アセンブラ用）

 .AIFDEF _H8S2339F_INC_
 .AELSE

_H8S2339F_INC_: .DEFINE ""

        .INCLUDE "util.inc"

;
;  内部レジスタの定義（下位16ビットのみ）
;

;    バスコントローラ
ABWCR:                   .DEFINE         "H'fed0"
ASTCR:                   .DEFINE         "H'fed1"
WCRH:                    .DEFINE         "H'fed2"
WCRL:                    .DEFINE         "H'fed3"
BCRH:                    .DEFINE         "H'fed4"
BCRL:                    .DEFINE         "H'fed5"

PFCR1:                   .DEFINE         "H'ff45"
PFCR2:                   .DEFINE         "H'ffac"


;    I/Oポート
P1DR:                    .DEFINE         "H'ff60"
P2DR:                    .DEFINE         "H'ff61"
P3DR:                    .DEFINE         "H'ff62"
P4DR:                    .DEFINE         "H'ff63"
P5DR:                    .DEFINE         "H'ff64"
P6DR:                    .DEFINE         "H'ff65"
P7DR:                    .DEFINE         "H'ff66"
P8DR:                    .DEFINE         "H'ff67"
P9DR:                    .DEFINE         "H'ff68"
PADR:                    .DEFINE         "H'ff69"
PBDR:                    .DEFINE         "H'ff6a"
PCDR:                    .DEFINE         "H'ff6b"
PDDR:                    .DEFINE         "H'ff6c"
PEDR:                    .DEFINE         "H'ff6d"
PFDR:                    .DEFINE         "H'ff6e"
PGDR:                    .DEFINE         "H'ff6f"

P1DDR:                   .DEFINE         "H'feb0"
P2DDR:                   .DEFINE         "H'feb1"
P3DDR:                   .DEFINE         "H'feb2"
P4DDR:                   .DEFINE         "H'feb3"
P5DDR:                   .DEFINE         "H'feb4"
P6DDR:                   .DEFINE         "H'feb5"
P7DDR:                   .DEFINE         "H'feb6"
P8DDR:                   .DEFINE         "H'feb7"
P9DDR:                   .DEFINE         "H'feb8"
PADDR:                   .DEFINE         "H'feb9"
PBDDR:                   .DEFINE         "H'feba"
PCDDR:                   .DEFINE         "H'febb"
PDDDR:                   .DEFINE         "H'febc"
PEDDR:                   .DEFINE         "H'febd"
PFDDR:                   .DEFINE         "H'febe"
PGDDR:                   .DEFINE         "H'febf"

 .AENDI ; _H8S2339F_INC_

