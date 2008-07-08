/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: defs.h,v 1.10 2003/12/15 07:32:13 takayuki Exp $
 */


// $Header: /home/CVS/configurator/base/defs.h,v 1.10 2003/12/15 07:32:13 takayuki Exp $

#ifndef DEFS_H
#define DEFS_H

    /*
     *  処理系に依存する部分
     *      関数名称 : mktemp, popen, pclose
     */

#ifdef CALL_EXTERNAL_PROGRAM

    /* Microsoft Visual C++ */
#if defined(_MSC_VER) || defined(__MINGW__)
#  include <io.h>
#  define   popen _popen
#  define   pclose _pclose
#  define   mktemp _mktemp

    /* GNU Compiler Collection on Linux environment*/
#elif defined(__GNUC__)
#  include  <stdlib.h>
#  define   mktemp mkstemp

    /* GNU Compiler Collection on Cygwin */
#elif defined(__CYGWIN__)
#  include  <stdlib.h>
#  define   mktemp mkstemp

    /* Borland C++ Compiler */
#elif defined(__BORLANDC__)
#  include <io.h>
#  include <stdio.h>
#  define   popen  ::std::_popen
#  define   pclose ::std::_pclose
#  define   mktemp ::std::_mktemp

#endif


    /*
     *  fc_binutils.cppが使用するプログラムの定義
     */

#ifdef _MSC_VER
#  define   CMD_PREPROCESSOR "\"C:\\Program Files\\Microsoft Visual Studio\\VC98\\Bin\\cl.exe\" /E /nologo"
#  define   CMD_SYMBOLLISTER "nm.exe"
#  define   CMD_OBJDUMP      "objdump.exe"
#  define   CMD_OBJCOPY      "objcopy.exe"
#  define   CMD_GREP         "grep.exe"
#else
#  define   CMD_PREPROCESSOR "cpp -ansi"
#  define   CMD_SYMBOLLISTER "nm"
#  define   CMD_OBJDUMP      "objdump"
#  define   CMD_OBJCOPY      "objcopy"
#  define   CMD_GREP         "grep"
#endif

#endif /* CALL_EXTERNAL_PROGRAM */

#endif /* DEFS_H */

