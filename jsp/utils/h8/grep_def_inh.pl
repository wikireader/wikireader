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
#  @(#) $Id: grep_def_inh.pl,v 1.1 2007/03/23 07:19:00 honda Exp $
# 


#
#  ベクタテーブル生成用フィルタ
#  　コンフィギュレーション・ファイルのプリプロセス結果を
#　　ベクタテーブル生成スクリプト(perl)に変換する。
#　　生成されたスクリプトはgenvector.plから呼び出される。
#
#
#　使い方
#　　grep_def_inh.pl file_name
#　　　引数file_name：入力データファイル
#　　　　　　　　　　　コンフィギュレーション・ファイルのプリプロセス
#　　　　　　　　　　　結果が渡されることを想定している。

#　処理内容
#  　・DEF_INH文を&define_inh(xx, "*******");の形に変換して出力する。
#  　
#  　備考
#  　・第2引数（割込みハンドラ属性）にTA_HLNGが指定されていると仮定している
#  　　　・第2引数はチェックを行っていない
#  　　　・TA_HLNGの値（0x00u）を文字列置換の正規表現に用いている

#
#　DEF_INH文の途中で改行されても正しく処理するため、
#　一度、すべての行を連結してから、;で区切り直す。
#
#　基本的に別の環境で作った入力データを読み込むことは考慮していない。
#　つまり、改行コードの違いで、上記のwhile文で一行ずつ読み込むつもりで
#　すべての行が1度に読み込まれてしまう可能性は無視している。
#
$text = "";
while($line = <>) {
	#　#で始まる行はプリプロセッサによるコメントなので、スキップする
	if ($line !~ /^#/) {
		$text .= $line;
	}
}

#　環境に依存しない改行コードの置換
#　　・正規表現を(\x0D?\x0A?)としないのは、空文字にヒットさせないため
$text =~ s/(\x0D\x0A)|\x0D|\x0A//g;

$text =~ s/(\s)+//g;		#　空白文字を削除
@list = split(/;/, $text);	#　;毎に区切る

#
#  &define_inh(xx, "*******");の形で出力する。
#
#　ここで、ラベル名の前後に__kernel_と_entryを付加しないのは
#　gcc版とHEW版で共通に使えるようにするため。
#　HEW版では、tmp_script.plが以下の２つの目的で使用される。
#　　・ベクタテーブルの生成
#　　・割込みの出入口処理の生成
#
foreach $line (@list) {
	if ($line =~ s/^DEF_INH\(/\&define_inh\(/) {
		
		# TA_HLNGの値が0x00uであることを仮定している
		$line =~ s/,{0x00u,/\,"/;
		$line =~ s/}\)/"\)/g;
		
		#  割込み番号の末尾にu,ulが付いている場合は削除する
		$line =~ s/((ul)|(lu)|(u)|(l))\,/\,/i;
		printf "\t%s;\n", $line;
	}
}

# ライブラリとして呼び出せるように戻り値を１にする。
print "1;\n";

