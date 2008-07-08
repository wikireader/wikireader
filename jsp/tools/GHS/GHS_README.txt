
        ＝ TOPPERS/JSPカーネル ユーザズマニュアル ＝
                    （GHSデバッグ環境）

        （Release 1.4 対応，最終更新: 24-Dec-2003）

------------------------------------------------------------------------
 TOPPERS/JSP Kernel
     Toyohashi Open Platform for Embedded Real-Time Systems/
     Just Standard Profile Kernel

 Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
                             Toyohashi Univ. of Technology, JAPAN
 Copyright (C) 2003 by Advanced Data Controls, Corp

 上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 によって公表されている GNU General Public License の Version 2 に記
 述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 利用と呼ぶ）することを無償で許諾する．
 (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
     権表示，この利用条件および下記の無保証規定が，そのままの形でソー
     スコード中に含まれていること．
 (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
     用できる形で再配布する場合には，再配布に伴うドキュメント（利用
     者マニュアルなど）に，上記の著作権表示，この利用条件および下記
     の無保証規定を掲載すること．
 (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
     用できない形で再配布する場合には，次のいずれかの条件を満たすこ
     と．
   (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
       作権表示，この利用条件および下記の無保証規定を掲載すること．
   (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
       報告すること．
 (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
     害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．

 本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
------------------------------------------------------------------------

このドキュメントでは、TOPPERS/JSPのGreen Hills Software(GHS)社が開発した統合開発環境MULTIでの構築に必要なファイルについて解説する。

1. ディレクトリの構成

jsp
  +--config
  |     +---armv4-ghs
  |     |      +---integrator
  |     +---sh3-ghs
  |            +---ms7727cp01
  |            +---solution_engine
  +--tools
        +-----GHS
               +---ghs_hook_bld
               +---kernel_bld
               +---sample


2.対応プロセッサ

・SH3(solution engineとms7727)
・armv4(Integrator/AP、コアARM966ESとARM920T)


3.　ディレクトリとファイルの説明

GNU環境での実装と異なるファイルをconfig/armv4-ghsまたはconfig/sh3-ghsディレクトリに置かれている。
また、tools/GHSディレクトリにはカーネルなどをビルドするためのファイルやGHS開発環境に依存するファイルが置かれている。

3.1 config/armv4-ghs

a. armv4t-ghs.bld ---- armv4依存部とintegratorのビルドファイル
b. integrator/integrator.bld ---- integrator依存部のビルドファイル
c. その他 ---- armv4依存部のコード
armv4t-ghs.bldからintegrator.bldがインクルードされており、armv4t-ghs.bldをプロジェクトにインクルードすれば、自動的にintegrator.bldがインクルードされる。

3.2 config/sh3-ghs

a. sh3_ms7727-ghs.bld ---- sh3とms7727依存部のビルドファイル
b. sh3_solution-ghs.bld ---- sh3とsolution_engine依存部のビルドファイル
c. ms7727cp01/ms7727cp01.bld ---- ms7727依存部のビルドファイル
d. solution_engine/solution.bld ---- solution_engine依存部のビルドファイル

sh3_ms7727-ghs.bldからms7727cp01.bldがインクルードされており、sh3_ms7727-ghs.bldをプロジェクトにインクルードすれば、自動的にms7727cp01.bldがインクルードされる。sh3_solution.bldも同様である。

3.3 tools/GHS/kernel_bld

このディレクトリはカーネルやシスタスクなどをビルドするためのファイルが置かれている。

a. kernel.bld ---- カーネルをライブラリにビルドするためのファイル。このビルドファイルには以下のビルドファイルが含まれている。
 ・ cyclic.bld 
 ・ dataqueue.bld 
 ・ eventflag.bld
 ・ exception.bld
 ・ interrupt.bld
 ・ mailbox.bld
 ・ mempfix.bld
 ・ semaphore.bld
 ・ sys_manage.bld
 ・ syslog.bld
 ・ task.bld
 ・ task_except.bld
 ・ task_manage.bld
 ・ task_sync.bld
 ・ time_event.bld
 ・ time_manage.bld
 ・ wait.bld

kernel.bldをプロジェクトにインクルードすれば、自動的にカーネルのライブラリがプロジェクトにインクルードされ、プロジェクトをビルドするとカーネルもビルドされる。kernel.bldは汎用性のため、CPUオプションやインクルードファイルのサーチパースなどが設定されておらず、単独ではビルドできない。カーネルだけをビルドする場合、kernel.bldだけをインクルードしている別のビルドファイルを作成し、そのビルドファイルにCPUオプションやサーチパースの設定をすれば良い。

b. library.bld ---- log_output.c, strerror.c, t_perror.c, vasyslog.cをオブジェクトにビルドするためのファイルである。

c. systask.bld ---- serial.c, timer.c, logtask.c をオブジェクトファイルにビルドするためのファイルである。



3.4  tools/GHS/ghs_hook_bld

このディレクトリはトレースログマクロなどのカーネルのデバッグ情報をMULTIデバッガで表示させるためのコードが置かれている。

a. ghs_hook.bld ---- デバッグ情報を表示されるためのコードのビルドファイル
b. その他 ---- デバッグ情報を表示されるためのコード

トレースログマクロ機能を使用するならプロジェクトのビルドファイルのファイルオプションでGHS_HOOKを定義しなければならない。さらに、GHS_HOOK=2を指定するとトレースデータは、必要なものだけを選択して取得することができます。GHS_HOOK=1がデフォルトで、全てのトレースデータが取得される。

BUFF_SIZE=xxxを定義すれば、トレースログマクロ用のバッファサイズを指定できる。ただし、BUFF_SIZE は必ず、1024(1kbyte) , 2048(2kbyte) , 4096(4kbyte) , 8192(8kbyte) , 16384(16kbyte) の中から指定してください。BUFF_SIZEのデフォルト値は16kbyteである。


3.5 tools/GHS/sample

このディレクトリはサンプルプログラムのコードのビルドファイルが置かれている。

a. arm920t.bld ---- Integrator/ARM920T用のサンプルプログラムのビルドファイル
b. solution_engine.bld ---- SH3/Solution Engine用のサンプルプログラムのビルドファイル
c. linker_arm.lnk ---- ARMV4のリンカーファイル
d. linker_sh3.lnk ---- SH3のリンカーファイル
e. kernel.bld ---- 3.3節を参照
f. systask.bld ---- 3.3節を参照
g. library.bld ---- 3.3節を参照
h. armv4t-ghs.bldまたはsh3_solution-ghs.bld ---- 3.1節または3.2節を参照
i. user_program.bld --- サンプルプログラムのコードのビルドファイル。
j. configuration.bld --- コンフィギュレーションファイルを処理するためのビルドファイル
k. その他 --- サンプルプログラムのコード

arm920t.bldまたはsolution_engine.bldの構成は以下に示す。

arm920t.bld/solution_engine.bld
    |-------kernel.bld
    |-------systask.bld
    |-------library.bld
    |-------armv4t-ghs.bld/sh3_solution-ghs.bld
    |-------ghs_hook.bld
    |-------user_program.bld
    |------------|---configuration.bld
    |------------|---------|----sample1.cfg
    |------------|----sample1.c
    |------------|----sample1.h
    |-------linker_arm.lnk/linker_sh3.lnk


第1節に示したディレクトリ構成では、arm920t.bldまたはsolution_engine.bldを何も変更しないでビルドできるが、ディレクトリの構成が異なる場合、サーチパースを変える必要がある。

