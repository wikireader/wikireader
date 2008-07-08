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
 *  @(#) $Id: jsp_staticapi.cpp,v 1.16 2003/12/24 07:33:26 takayuki Exp $
 */

// $Header: /home/CVS/configurator/jsp/jsp_staticapi.cpp,v 1.16 2003/12/24 07:33:26 takayuki Exp $

#include "jsp/jsp_defs.h"

#include "base/parser.h"
#include "base/mpstrstream.h"
#include <iostream>

using namespace std;

DECLARE_API(INCLUDE,"INCLUDE")
{
    Token token;
    Directory * node;
    Directory * scope;

    last = NULL;
    p.getToken(token, Token::STRINGLITERAL);
    token.chopLiteral();

    if( (token[0] != '\"' || token[token.size()-1] != '\"') &&
        (token[0] != '<' || token[token.size()-1] != '>') )
        ExceptionMessage("An include filename must be enclosed with \"...\" or <...>","ファイル名は \"...\" か <...> の形でなければなりません").throwException();

    node = container.openChild(OBJECTTREE,INCLUDEFILE,NULL);
    last = node;

        //重複チェック
    scope = node->getFirstChild();
    while(scope != 0)
    {
        if(token.compare((*scope).toString()) == 0)
        {
            cerr << p.getStreamLocation() << ' ' << (Message("Include file % was already specified (ignored).","インクルードファイル%が複数回指定されました (無視します)") << token) << endl;
            return;
        }
        scope = scope->getNext();
    }
    
    node->addChild(new Directory(token));
}

DECLARE_API(CRE_TSK,"CRE_TSK")
{
    Token token;
    string work;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, TASK);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"tskatr,exinf,task,itskpri,stksz");
    p.getToken(",","NULL","}",NULL);

    (*node)["texatr"] = "TA_NULL";
    (*node)["texrtn"] = "NULL";

    if(container["/cpu"].toString().compare("nios32") == 0) 
    {
        (*node)["hi_limit"] = "TSK_HI_LIMIT";
        (*node)["lo_limit"] = "TSK_LO_LIMIT";
    }
}

DECLARE_API(VATT_TRW,"VATT_TRW")
{
    Token token;
    Directory * node;

    if(container["/cpu"].toString().compare("nios32") != 0)
        ExceptionMessage("VATT_TRW is not supported except for the processor 'nios32'.","VATT_TRWはnios32プロセッサでのみサポートされます").throwException();

    p.getToken(token);
    node = find(container[OBJECTTREE], token, TASK);

    p.getToken(",","{",NULL);
    parseParameters(p, node, "hi_limit,lo_limit");
    p.getToken("}");
}

DECLARE_API(DEF_TEX,"DEF_TEX")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = find(container[OBJECTTREE], token, TASK);

    p.getToken(",","{",NULL);
    parseParameters(p, node, "texatr,texrtn");
    p.getToken("}");
}

DECLARE_API(CRE_SEM,"CRE_SEM")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, SEMAPHORE);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"sematr,isemcnt,maxsem");
    p.getToken("}");
}

DECLARE_API(CRE_FLG,"CRE_FLG")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, EVENTFLAG);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"flgatr,iflgptn");
    p.getToken("}");
}

DECLARE_API(CRE_DTQ,"CRE_DTQ")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, DATAQUEUE);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"dtqatr,dtqcnt");
    p.getToken(",","NULL","}",NULL);
}

DECLARE_API(CRE_MBX,"CRE_MBX")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, MAILBOX);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"mbxatr,maxmpri");
    p.getToken(",","NULL","}",NULL);
}

DECLARE_API(CRE_MPF,"CRE_MPF")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, FIXEDSIZEMEMORYPOOL);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"mpfatr,blkcnt,blksz");
    p.getToken(",","NULL","}",NULL);
}

DECLARE_API(CRE_CYC,"CRE_CYC")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, CYCLICHANDLER);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"cycatr,exinf,cychdr,cyctim,cycphs");
    p.getToken("}");
}

DECLARE_API(DEF_INH,"DEF_INH")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, INTERRUPTHANDLER, false);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"inhatr,inthdr");
    p.getToken("}");
}

DECLARE_API(DEF_EXC,"DEF_EXC")
{
    Token token;
    Directory * node;

    p.getToken(token);
    node = allocate(container[OBJECTTREE], token, EXCEPTIONHANDLER, false);
    (*node)["position"] = p.getStreamLocation();

    p.getToken(",","{",NULL);
    parseParameters(p,node,"excatr,exchdr");
    p.getToken("}");
}

DECLARE_API(ATT_INI,"ATT_INI")
{
    Token token;
    Directory * node;

    node = container.openChild(OBJECTTREE,INITIALIZER,NULL)->addChild();
    last = node;

    (*node)["position"] = p.getStreamLocation();

    p.getToken("{",NULL);
    parseParameters(p,node,"iniatr,exinf,inirtn");
    p.getToken("}");
}

DECLARE_API(ATT_FIN, "VATT_TER")
{
    Token token;
    Directory * node;

    node = container.openChild(OBJECTTREE,TERMINATOR,NULL)->addChild();
    last = node;

    (*node)["position"] = p.getStreamLocation();

    p.getToken("{",NULL);
    parseParameters(p,node,"teratr,exinf,terrtn");
    p.getToken("}");
}


