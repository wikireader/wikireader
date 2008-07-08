#! /usr/bin/perl
#
#  TOPPERS/JSP Kernel
#      Toyohashi Open Platform for Embedded Real-Time Systems/
#      Just Standard Profile Kernel
# 
#  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
#                              Toyohashi Univ. of Technology, JAPAN
#  Copyright (C) 2004 by Embedded and Real-Time Systems Laboratory
#              Graduate School of Information Science, Nagoya Univ., JAPAN
#  Copyright (C) 2005-2007 by Industrial Technology Institute,
#                              Miyagi Prefectural Government, JAPAN
# 
#  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
#  によって公表されている GNU General Public License の Version 2 に記
#  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
#  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
#  利用と呼ぶ）することを無償で許諾する．
#  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
#      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
#      スコード中に含まれていること．
#  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
#      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
#      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
#      の無保証規定を掲載すること．
#  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
#      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
#      と．
#    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
#        作権表示，この利用条件および下記の無保証規定を掲載すること．
#    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
#        報告すること．
#  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
#      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
# 
#  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
#  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
#  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
#  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
# 
#  @(#) $Id: gen_intentry.pl,v 1.1 2007/03/23 07:25:47 honda Exp $
# 


#
#  割込みの入口処理生成スクリプト
#  　直前に生成されるtmp_script.plを読み込むことによって実行できるようになる。
#
#　　ここだけH8S版と処理が異なるため、分離している。
#


#
#  tmp_script.pl内から呼ばれる関数の定義
#　　 割込みの入口処理を出力する。
#
sub define_inh {
	my($inhno, $handler) = @_;
	
	printf " INTHDR_ENTRY %s, %s_intmask\n", $handler , $handler;
}	

#
# 割込みの入口処理の出力
#
print <<INTENTRY

;
;  割込みの入口処理の定義
;    このファイルにはユーザー定義の割込みハンドラ（C言語ルーチン）名を
;    記述する
;
        .SECTION P, CODE, ALIGN=2

;
;    割込みの入口処理を生成するマクロの使い方
;      （割込み要因毎に異なる部分）
;
;    マクロINTHDR_ENTRY C_ROUTINE, INTMASK
;      パラメータ
;          C_ROUTINE：C言語ルーチンの関数名（先頭の'_'は付けない）
;          INTMASK：  割込み許可時に割込みマスクに設定する値
;                      IPM_LEVEL1、IPM_LEVEL2のいずれかにすること
;                      自分と同じレベルの割込みをマスクするため、
;                      IPMには１つ上のレベルを設定する。
;

INTENTRY
;

# 自動生成されたスクリプトの読み込みと実行
require "./tmp_script.pl";

print "\n .END\n";

