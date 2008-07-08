
        ＝ TOPPERS/JSPカーネル ユーザズマニュアル ＝
                    （m16c:TM開発環境）

        （Release 1.4 対応，最終更新: 15-May-2006）

------------------------------------------------------------------------
 TOPPERS/JSP Kernel
     Toyohashi Open Platform for Embedded Real-Time Systems/
     Just Standard Profile Kernel

 Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
                             Toyohashi Univ. of Technology, JAPAN
 Copyright (C) 2003-2004 by Platform Development Center
                                         RIOCH COMPANY,LTD. JAPAN
 Copyright (C) 2006 by Embedded and Real-Time Systems Laboratory
               Graduate School of Information Science, Nagoya Univ., JAPAN

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

このドキュメントでは、M16C用のルネサステクノロジ社の開発ツールを使用し
て，TOPPERS/JSPカーネルを構築するために必要なファイルについて解説する．

1. ディレクトリの構成

jsp
  +--config
  |     +---m16c-renesas
  |            +---oaks16
  |            +---oaks16_mini
  |            +---m3029
  +--tools
  |     +-----M16C-RENESAS
  +--utils
        +-----m16c-renesas


2.ディレクトリとファイルの説明

M16C依存部のファイルは，config/m16c-renesasディレクトリに置く．構築に
際して，割込みやエクセプションのベクトルを自動生成するツール(m16cvec. 
exe)と，TCBのオフセット値をアセンブラで書かれたソースに設定するツール
(m16co ffset.exe)の構築環境と，makeを用いた開発フローの場合に必要とな
る，ソースの依存関係を生成するperlスクリプトをtools/m16c-renesasデレク
トリィに置く．統合開発環境TMを用いてTOPPERS/JSPを構築するためのファイ
ルをtools/M16C-RENESASデレクトリィに置く．


2.1 config/m16c-renesas

a. Makefile.config
    makeコマンドを用いてm16c用TOPPERS/JSPを構築する場合のmakeファイル
b. oaks16/Makefile.config 
    makeコマンドを用いてm16c-OAKS16用TOPPERS/JSPを構築する場合のmakeファイル
c. oaks16_mini/Makefile.config 
    makeコマンドを用いてm16c-OAKS16 MINI用TOPPERS/JSPを構築する場合のmakeファイル
d. その他 ---- M16C依存部のコード


2.2 utils/m16c-renesas

a. makedep.m16c
    makeを用いて開発を場合、ソースの依存関係を生成するPERLスクリプト
b. m16cutils.dsw 
    m16cvec.exeとm16coffset.exeコマンドVC++を用いて構築するためのワークスペースファイル
c. m16cutils.opt 
    VC++用のOPTファイル
d: m16cutils/m16coffset/m16coffset.dsp
    m16coffset作成のプロジェクトファイル
e: m16cutils/m16coffset/m16coffset.cpp
    m16coffsetコマンドC++言語記述
f: m16cutils/m16coffset/StdAfx.h
    標準システムインクルードファイル
g: m16cutils/m16coffset/StdAfx.cpp
    標準システムインクルードファイルを含むソースファイル
h: m16cutils/m16coffset/m16cvec.dsp
    m16cvec作成のプロジェクトファイル
i: m16cutils/m16coffset/m16cvec.cpp
    m16cvecコマンドC++言語記述
j: m16cutils/m16coffset/StdAfx.h
    標準システムインクルードファイル
k: m16cutils/m16coffset/StdAfx.cpp
    標準システムインクルードファイルを含むソースファイル


2.3 utils/M16C-RENESAS

a.Jsp14sample1.tmi,Jsp14sample1.tmk 
   TMを用いてOAKS16用のTOPPERS/JSPを構築するためのプロジェクトファイル
b.Jsp14sample1m.tmi,Jsp14samplem1.tmk
   TMを用いてOAKS16 MINI用のTOPPERS/JSPを構築するためのプロジェクトファイル
c.Jsp14sample1_m3029.tmi,Jsp14samplem1_m3029.tmk 
   TMを用いてM3029用のTOPPERS/JSPを構築するためのプロジェクトファイル
d.Jsp14sample1.id 
   生成したMOTファイルをOAKS16ボードのフラッシュROMの書き込む時に使用するIDファイル
e.Jsp14sample1m.id 
   生成したMOTファイルをOAKS16 MINIボードのフラッシュROMの書き込む時に使用するIDファイル
f.Jsp14sample1_m3029.id 
   生成したMOTファイルを M3029のフラッシュROMの書き込む時に使用するIDファイル
g.sample1.cfg 
   OAKS16, M3029用のTOPPERS/JSPを構築するためのコンフィギュファイル
h.sample1m.cfg 
   OAKS16 MINI用のTOPPERS/JSPを構築するためのコンフィギュファイル


2.4  sample

a.Makefile.m16c-oaks16 
   makeコマンドを用いてOAKS16用のTOPPERS/JSPを構築するためのmakeファイル


