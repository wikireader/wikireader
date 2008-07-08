/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: clause.h,v 1.4 2003/12/20 06:51:58 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/clause.h,v 1.4 2003/12/20 06:51:58 takayuki Exp $

#ifndef CLAUSE_H
#define CLAUSE_H

#include "base/testsuite.h"
#include "base/parser.h"
#include "base/singleton.h"
#include "base/except.h"

#include <map>
#include <string>

#define INTEGER_CLAUSE_IDENTIFIER   "##INTEGER" //2連の#と識別子を一気に取り出すことはできないので名前が重なることはない
#define LITERAL_CLAUSE_IDENTIFIER   "##STRING"
#define OPERATOR_CLAUSE_IDENTIFIER  "##OPERATOR"

    //構文上の一節を処理するクラス
class Clause
{
public:
        //ある名前空間に属する節のマップ
    typedef std::map<std::string, Clause *> map_type;

        //名前空間
    class Namespace : public std::map<std::string, map_type>
    { public: SINGLETON_CONSTRUCTOR(Namespace) throw() {} };

protected:
    static map_type * getClauseMap(std::string ns) throw();

        //トークンから識別名を生成する
    static std::string getClauseIdentifier(Token token) throw();

        //自身を登録する
    void   addClause(std::string ns, std::string identifier) throw();

        //自分を複数のleading identifierに対応付ける
    void   addMultipleClauses(std::string ns, std::string identifier_list) throw();

protected:  /* インタフェース */

        //パーサ本体
    virtual void before(const Token & first, Parser & p);
    virtual void body  (const Token & first, Parser & p) = 0;
    virtual void after (const Token & first, Parser & p);

        //パース失敗時の回復処理
    virtual void onFail(const Token & first, Parser & p) throw();

public:
    Clause(void) throw() {}
    virtual ~Clause(void) throw() {}

        //有効判定
    inline bool isValid(void) const throw()
    {   return (this != 0);   }

        //節の解析
    static bool parseClause(std::string ns, Parser & p);

        //firstで始まる節があるか
    static bool hasClause(std::string ns, std::string first) throw();

        //[ヘルプ用] 登録されている節の一覧を作成する
    static std::string makeClauseList(std::string ns, int width = 80) throw();

    TESTSUITE_PROTOTYPE(main)
};


    /*
     *   静的API
     */
#define NAMESPACE_STATICAPI "StaticAPI"

class StaticAPI : public Clause
{
protected:

    virtual void before(const Token & first, Parser & p);
    virtual void after (const Token & first, Parser & p);

        //パース失敗時の回復処理
    virtual void onFail(const Token & first, Parser & p) throw();

public:
        //デフォルトコンストラクタ (何もしない)
    StaticAPI(void) throw() {}      
        //API名を指定して生成 (registをよぶ)
    StaticAPI(std::string apinamelist) throw()
    {   addMultipleClauses(NAMESPACE_STATICAPI, apinamelist);   }

    inline void regist(std::string apinamelist) throw()
    {   addMultipleClauses(NAMESPACE_STATICAPI, apinamelist);   }

        //節の解析
    static bool parseStaticAPI(Parser & p)
    {   return Clause::parseClause(NAMESPACE_STATICAPI, p);   }

        //firstで始まる節があるか
    inline static bool hasStaticAPI(std::string first) throw()
    {   return Clause::hasClause(NAMESPACE_STATICAPI, first);   }

        //[ヘルプ用] 登録されている節の一覧を作成する
    inline static std::string makeClauseList(int width = 80) throw()
    {   return Clause::makeClauseList(NAMESPACE_STATICAPI, width);   }

        //スキップ
    static void skip(Parser & p, bool invalid_api = true) throw();

    TESTSUITE_PROTOTYPE(main)
};


    /*
     *   プリプロセスディレクティブ
     */
#define NAMESPACE_DIRECTIVES "Directives"

class Directives : public Clause
{
protected:

    virtual void after(const Token & first, Parser & p) throw(Exception);

        //パース失敗時の回復処理
    virtual void onFail(const Token & first, Parser & p) throw();

public:
        //デフォルトコンストラクタ (何もしない)
    Directives(void) throw() {}      
        //API名を指定して生成 (registをよぶ)
    Directives(std::string apinamelist) throw()
    {   addMultipleClauses(NAMESPACE_DIRECTIVES, apinamelist);   }

    inline void regist(std::string apinamelist) throw()
    {   addMultipleClauses(NAMESPACE_DIRECTIVES, apinamelist);   }

        //節の解析
    inline static bool parseDirectives(Parser & p)
    {   return Clause::parseClause(NAMESPACE_DIRECTIVES, p);   }

        //firstで始まる節があるか
    inline static bool hasDirectives(std::string first) throw()
    {   return Clause::hasClause(NAMESPACE_DIRECTIVES, first);   }

        //[ヘルプ用] 登録されている節の一覧を作成する
    inline static std::string makeClauseList(int width = 80) throw()
    {   return Clause::makeClauseList(NAMESPACE_DIRECTIVES, width);   }

    TESTSUITE_PROTOTYPE(main)
};

#endif



