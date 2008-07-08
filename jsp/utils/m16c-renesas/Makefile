#
#  TOPPERS/JSP Kernel
#      Toyohashi Open Platform for Embedded Real-Time Systems/
#      Just Standard Profile Kernel
# 
#  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
#                              Toyohashi Univ. of Technology, JAPAN
#  Copyright (C) 2003-2004 by Platform Development Center
#                                          RIOCH COMPANY,LTD. JAPAN
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
#  @(#) $Id: Makefile,v 1.3 2004/10/07 07:30:37 honda Exp $
# 

#
#  コンパイルオプションの定義
#
CFLAGS = -g -O2 -Wall
INCLUDES = -I.

#
#  MinGWを使用しているときには、次のコメントを外す
#
#CFLAGS := $(CFLAGS) -D__MINGW__

#
#  リンク時に追加するライブラリ
#
#LIBS = -lstdc++

#
#  オブジェクトファイル名の定義
#
COMMON_OBJS = StdAfx.o
OFF_OBJS = m16coffset.o $(COMMON_OBJS)
VEC_OBJS = m16cvec.o  $(COMMON_OBJS)

VPATH = ./m16cvec ./m16coffset

#
#  ターゲットファイル
#
all: m16coffset m16cvec

#
#  ソースファイルのあるディレクトリに関する定義
#
vpath %.cpp $(VPATH)

#
#  m16coffset のリンク
#
m16coffset: $(OFF_OBJS)
	$(CC) $(CFLAGS) -o m16coffset $(OFF_OBJS) $(LIBS)

#
#  m16cvec のリンク
#
m16cvec: $(VEC_OBJS)
	$(CC) $(CFLAGS) -o m16cvec $(VEC_OBJS) $(LIBS)

#
#  コンパイル結果の消去
#
clean:
	rm -f *.o
	rm -f m16coffset.exe m16cvec.exe

cleandep:
	rm -f Makefile.depend

realclean: cleandep clean

#
#  依存関係ファイルの生成
#
depend: cleandep $(OFF_OBJS:.o=.d) $(VEC_OBJS:.o=.d)

#
#  依存関係ファイルをインクルード
#
-include Makefile.depend

#
#  コンパイラドライバの定義
#
CC = g++

#
#  デフォルトのコンパイルルールの定義
#
%.o: %.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

#
#  デフォルトの依存関係作成ルールの定義
#
%.d: %.cpp
	$(CC) -M $(CFLAGS) $(INCLUDES) $< >> Makefile.depend
