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

#　処理内容
#  　・jsp/kernelにあるソースコードをAPI毎にコピーする。
#  　　（関数単位でリンクできるようにする）
#  　・コピーする際に、ファイル先頭に#define __tskiniなどのマクロ定義を追加する。
#  　
#  使い方
#  　　perl copy_kernel_source.pl src_path dst_path
#  　　　引数
#  　　　　src_path：jsp/kernelへの相対パス
#  　　　　dst_path：ファイルのコピー先への相対パス


$src_path = $ARGV[0];
$dst_path = $ARGV[1];

# 元になるソースファイル名と追加するマクロ名の定義
%hash_array = (
 "task.c"
	=> [qw(tskini tsksched tskrun tsknrun tskdmt tskact tskext tskpri tskrot tsktex)],
 "wait.c"
	=> [qw(waimake waicmp waitmo waitmook waican wairel wobjwai wobjwaitmo wobjpri)],
 "time_event.c"
	=> [qw(tmeini tmeup tmedown tmeins tmedel isig_tim)],
 "syslog.c"
	=> [qw(logini vwri_log vrea_log vmsk_log logter)],
 "task_manage.c"
	=> [qw(act_tsk iact_tsk can_act ext_tsk ter_tsk chg_pri get_pri)],
 "task_sync.c"
	=> [qw(slp_tsk tslp_tsk wup_tsk iwup_tsk can_wup rel_wai irel_wai sus_tsk rsm_tsk frsm_tsk dly_tsk)],
 "task_except.c"
	=> [qw(ras_tex iras_tex dis_tex ena_tex sns_tex)],
 "semaphore.c"
	=> [qw(semini sig_sem isig_sem wai_sem pol_sem twai_sem)],
 "eventflag.c"
	=> [qw(flgini flgcnd set_flg iset_flg clr_flg wai_flg pol_flg twai_flg)],
 "dataqueue.c"
	=> [qw(dtqini dtqenq dtqfenq dtqdeq dtqsnd dtqrcv snd_dtq psnd_dtq ipsnd_dtq tsnd_dtq fsnd_dtq ifsnd_dtq rcv_dtq prcv_dtq trcv_dtq)],
 "mailbox.c"
	=> [qw(mbxini snd_mbx rcv_mbx prcv_mbx trcv_mbx)],
 "mempfix.c"
	=> [qw(mpfini mpfget get_mpf pget_mpf tget_mpf rel_mpf)],
 "time_manage.c"
	=> [qw(set_tim get_tim vxget_tim)],
 "cyclic.c"
	=> [qw(cycini cycenq sta_cyc stp_cyc cyccal)],
 "sys_manage.c"
	=> [qw(rot_rdq irot_rdq get_tid iget_tid loc_cpu iloc_cpu unl_cpu iunl_cpu dis_dsp ena_dsp sns_ctx sns_loc sns_dsp sns_dpn vsns_ini)],
 "interrupt.c"
	=> [qw(inhini)],
 "exception.c"
	=> [qw(excini vxsns_ctx vxsns_loc vxsns_dsp vxsns_dpn vxsns_tex)]
);

#
#　メインの処理
#
foreach $file (keys(%hash_array)) {
	@array = @{$hash_array{$file}};
	foreach $api (@array) {
		&copy_file($file, $api);
	}
}


#
#　第1引数：元になるソースコードのファイル名（パスを含まず）
#　第2引数：条件コンパイル用のマクロ定義を追加するAPI、関数名
#　　　　　　→出力するファイル名も兼ねている。
#
sub copy_file {
	my($filename, $api) = @_;
	open(OUTFILE, ">$dst_path/$api.c") || die "Cannot open $dst_path/$api.c";
	
	# API毎のマクロ定義を追加
	print OUTFILE "#define __$api\n";
	
	# jsp/kernelからファイルをコピー
	open(INFILE, "$src_path/$filename") || die "Cannot open $src_path/$filename";
	while ($line = <INFILE>) {
		print OUTFILE $line;
	}
	
	close(INFILE);
	close(OUTFILE);
}
