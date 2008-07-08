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
#  @(#) $Id: genvector.pl,v 1.5 2007/03/23 07:25:47 honda Exp $
# 


#
#  ベクタテーブル生成スクリプト
#  　直前に生成されるtmp_script.plを読み込むことによって実行できるようになる。
#

require "getopt.pl";

#  オプションの定義
#
#  -s <vector size>	ベクターテーブルのサイズ

#
#  tmp_script.pl内から呼ばれる関数の定義
#  　割込みハンドラをベクタテーブルに登録する。
#
sub define_inh {
	my($inhno, $inthdr) = @_;
	
	if ($inhno == 0) {
		print STDERR <<ERRMESSAGE
In generating vector.src
Error in DEF_INH($inhno, {TA_HLNG, $inthdr});
\t Macro of interrupt handler number $inhno isn't defined.
\t If you define macro $inhno in header files,
\t it's necessary to use "#include" directive in configuration files
\t to genarate vector.src. 
\t (And it's also necessary to use static API "INCLUDE()" 
\t in configuration files to genarate kernel_cfg.c. )
\t Check configuration files and header files.

ERRMESSAGE
		;
		exit(1);
	}
	else {
		# ベクタテーブルに割込みハンドラを登録
		#　　 割込みハンドラ名の前後に"__kernel_"と"_entry"を付加
		$vector_table[$inhno] = "__kernel_" . $inthdr . "_entry";
	}
}	

#
#  オプションの処理
#
do Getopt("s");

if ($opt_s == 0) {
	print STDERR "genvector.pl:\n";
	print STDERR "\t -s option(vector size) is necessary.\n";
	print STDERR "\t check jsp/tools/\$(CPU)/configuration/call_configurator.bat.\n";
	print STDERR "\t \$(CPU) is H8-RENESAS or H8S-RENESAS.\n";
	exit(1);
}

$vector_size = $opt_s;

# すべてのベクタを「未登録」として初期化
for ($i=0; $i<$vector_size; $i++) {
	$vector_table[$i] = "_no_reg_exception";
}

# リセットベクタの定義
$vector_table[0] = "_start";

# 自動生成されたスクリプトの読み込みと実行
require "./tmp_script.pl";

# ベクタテーブルの出力
for ($i=0; $i<$vector_size; $i++) {
	printf "\t.DATA.L %s\t;  %d, 0x%02x\n", $vector_table[$i] , $i, $i;
}

