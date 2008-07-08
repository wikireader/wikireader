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
 *  @(#) $Id: clause.cpp,v 1.3 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/clause.cpp,v 1.3 2003/12/15 07:32:13 takayuki Exp $

#include "base/clause.h"
#include "base/message.h"

using namespace std;

/*
 *   構文上の一節を処理するクラス
 */

    //指定した名前空間の節マップを取得
Clause::map_type * Clause::getClauseMap(string name) throw()
{
    map_type * result = NULL;
    Namespace::iterator scope;

    Namespace * const ns = Singleton<Namespace>::getInstance(nothrow);
    if(ns!= 0) {
        scope = ns->find(name);
        if(scope != ns->end())
            result = &(scope->second);
    }

    return result;
}

    //節の登録
void Clause::addClause(string _namespace, string _identifier) throw()
{
    if(isValid()) {
        Namespace * const ns = Singleton<Namespace>::getInstance(nothrow);
        if(ns != 0)
            (*ns)[_namespace][_identifier] = this;
//          ns->operator [](_namespace).insert(map_type::value_type(_identifier, this));        //上書きさせない場合
    }
}

    //トークンに対して節識別名を生成する
string Clause::getClauseIdentifier(Token token) throw()
{
    string result;

    switch(token.getType()) {
        case Token::IDENTIFIER:
        case Token::PUNCTUATOR:
            result = token.getLiteral();    
            break;
        case Token::INTEGER:
            result.assign(INTEGER_CLAUSE_IDENTIFIER);
            break;
        case Token::LITERAL:
            result.assign(LITERAL_CLAUSE_IDENTIFIER);
            break;
        case Token::OPERATOR:
            result.assign(OPERATOR_CLAUSE_IDENTIFIER);
            break;
        default:
            result = string();
    }

    return result;
}

    //節の解析
bool Clause::parseClause(string ns, Parser & p)
{
    bool result = false;
    map_type::iterator scope;
    map_type * clause = getClauseMap(ns);

        //ガード節
    if(clause == 0 || p.eof())
        return false;

        //トークンから識別名を取り出す
    Token  token;
    string identifier;

    token      = p.getToken();
    identifier = getClauseIdentifier(token);
    
        //節処理部の実行
    scope = clause->find(identifier);
    if(scope != clause->end()) {

            //デバッグ用メッセージ出力
        DebugMessage("Clause::parseClause(%) - %\n") << ns << identifier;

        try {
            scope->second->before(token, p);
            scope->second->body(token, p);  //ヒットしたものを実行
            scope->second->after(token, p);
            result = true;
        }
        catch(...) {
            scope->second->onFail(token, p);    //失敗したときの後始末をお願いする
            throw;
        }
    }
    else
        p.putback(token);   //識別子にマッチするものが登録されていない

    return result;
}

    //firstで始まる節があるか
bool Clause::hasClause(string ns, string first) throw()
{
    map_type * clause = getClauseMap(ns);
    if(clause == 0)
        return false;

    return clause->find(first) != clause->end();
}

    //パース前処理
void Clause::before(const Token &, Parser &)
{}

    //パース後処理
void Clause::after(const Token &, Parser &)
{}

    //標準の例外発生時の後始末ルーチン
void Clause::onFail(const Token &, Parser & p) throw()
{
    Token token;

        //最後まで読みきる
    while(!p.eof())
        token = p.getToken();
}

    //節を複数の識別名に対応付ける
void Clause::addMultipleClauses(string ns, string id_list) throw()
{
    string            id;
    string::size_type pos;
    string::size_type prev;

        //カンマで区切られた要素毎にaddClauseを発行
    if(isValid()) {
        prev = 0;
        do {
            pos = id_list.find_first_of(',', prev);
            id  = id_list.substr(prev, pos - prev);

            addClause(ns, id);

            prev = pos + 1;
        } while(pos != string::npos);
    }
}

    //[ヘルプ用] 登録されている節の一覧を作成する
std::string Clause::makeClauseList(string ns, int width) throw()
{
    string result;
    map_type::iterator scope;
    map_type * clause = getClauseMap(ns);
    size_t max_length = 0;

        //最大の文字列長を求める
    scope = clause->begin();
    while(scope != clause->end()) {
        size_t i = scope->first.size();
        if(i > max_length)
            max_length = i;
        ++ scope;
    }

        //一覧を作成
    size_t max_column = width/(max_length+1);
    size_t column = 0;
    scope = clause->begin();
    while(scope != clause->end()) {
        size_t i = scope->first.size();

        result += scope->first;

        ++ column;
        ++ scope;

            //末尾処理
        if(column == max_column || scope == clause->end())
            result += '\n';
        else {
            result += ',';
            result += string(max_length - i, ' ');
        }
    }

    return result;
}


    /*
     *   静的API
     */

    //パース前処理
void StaticAPI::before(const Token & first, Parser & p)
{
    Token token = p.getToken();
    if(token != Token::LPAREN) {
        ExceptionMessage("StaticAPI [%] lacks left-parenthesis '('.","静的API[%]には括弧'('が欠けている") << first.getLiteral() << throwException;
    }
}

    //パース後処理
void StaticAPI::after(const Token & first, Parser & p)
{
    Token token = p.getToken();
    if(token != Token::RPAREN) {
        ExceptionMessage("StaticAPI [%] lacks right-parenthesis ')'.","静的API[%]には括弧')'が欠けている") << first.getLiteral() << throwException;
    }
    else {
        token = p.getToken();
        if(token != ";")
            ExceptionMessage("StaticAPI [%] lacks ';' at the end of declaration.","静的API[%]の末尾に';'が欠けている") << first.getLiteral() << throwException;
    }
}

    //失敗時の回復処理
void StaticAPI::onFail(const Token & , Parser & p) throw()
{   skip(p, false);   }


    //スキップ
    //   invalid_api : 次に最初に読み出されるAPIは無効
void StaticAPI::skip(Parser & p, bool invalid_api) throw()
{
        //セミコロン or 次の静的APIの出現まで
    Token token;
    bool  condition; //アレ対策

    if(invalid_api)
        token = p.getToken();   //次のAPI名は読み飛ばす

    do {
        token = p.getToken(true);

        condition = (!token.isValid()) ||                                                   //無効なトークンか
                    (token == Token::PUNCTUATOR && token == ";") ||                         //セミコロンか
                    (token == Token::IDENTIFIER && hasStaticAPI(token.getLiteral()));       //静的APIか

    } while(!condition);

    if(token == Token::IDENTIFIER)
        p.putback(token);
}

    /*
     *   プリプロセスディレクティブ
     */

    //パース後処理
void Directives::after(const Token & first, Parser & p) throw(Exception)
{
    Token token = p.getToken(true);
    if(!token.isValid() || !p.isLocatedonHeadofLine())
        ExceptionMessage("Directive [%] has a wrong parameter or misses a new-line.","ディレクティブ[%]に不正な引数があるか、改行がない") << first.getLiteral() << throwException;
}

    //パース失敗時の回復処理
void Directives::onFail(const Token & , Parser & p) throw()
{
        //次の改行まで読み飛ばす
    Token token;

    do {
        token = p.getToken(true);
    } while(token.isValid() && !p.isLocatedonHeadofLine());
}

/************************************************* テストスィート *************************************************/

#ifdef TESTSUITE

#include "coverage_undefs.h"
#include <sstream>

namespace {
    class Clause_test : public Clause
    {
    public:
        Token        first;
        Token        second;
        bool         throw_exception;

        Clause_test(void) throw() : first(Token::ERROR), throw_exception(false) {}

        void body(const Token & _first, Parser & _p)
        {
            TestSuite::check("Clause_test::body");
            first = _first;
            second = _p.getToken();
            if(throw_exception)
                throw 0;
        }

        void before(const Token & , Parser & )
        {
            TestSuite::check("Clause_test::before");
        }

        void after(const Token &, Parser &)
        {
            TestSuite::check("Clause_test::after");
        }

        void onFail(const Token & _first, Parser & p) throw()
        {
            TestSuite::check("Clause_test::onFail");
            first = _first;
        }

        void onFail_super(const Token & _first, Parser & p) throw()
        {   Clause::onFail(_first, p);   }
    };
}

TESTSUITE(main, Clause)
{
    SingletonBase::ContextChain chain;
    chain.saveContext<Namespace>();

    BEGIN_CASE("getClauseIdentifier","getClauseIdentifier") {
        BEGIN_CASE("1","普通の識別子はそのままが中身になる") {
            Token token(Token::IDENTIFIER, "test");

            if(getClauseIdentifier(token).compare("test") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","数値は中身に関係なく" INTEGER_CLAUSE_IDENTIFIER "が返る") {
            Token token(Token::INTEGER, "", "", 0);

            if(getClauseIdentifier(token).compare(INTEGER_CLAUSE_IDENTIFIER) != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","リテラルは中身に関係なく" LITERAL_CLAUSE_IDENTIFIER "が返る") {
            Token token(Token::LITERAL, "");

            if(getClauseIdentifier(token).compare(LITERAL_CLAUSE_IDENTIFIER) != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","演算子は中身に関係なく" OPERATOR_CLAUSE_IDENTIFIER "が返る") {
            Token token(Token::OPERATOR);

            if(getClauseIdentifier(token).compare(OPERATOR_CLAUSE_IDENTIFIER) != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("5","空白のトークンには空文字(==非受理)が返る") {
            Token token(Token::SPACE);

            if(!getClauseIdentifier(token).empty())
                TEST_FAIL;
        } END_CASE;

        TEST_CASE("6","エラートークンには空文字(==非受理)が返る", getClauseIdentifier(Token(Token::ERROR)).empty());
        TEST_CASE("7","EOSトークンには空文字(==非受理)が返る",   getClauseIdentifier(Token(Token::END_OF_STREAM)).empty());

    } END_CASE;

    BEGIN_CASE("getClauseMap","getClauseMap") {
        BEGIN_CASE("1","何も登録していない空間にはNULLが返る") {
            chain.renewInstance();
            if(Clause::getClauseMap("unknown") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","要素があれば非0が返る") {
            chain.renewInstance();
            Singleton<Namespace>::getInstance()->operator []("test");

            if(Clause::getClauseMap("test") == 0)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("addClause","addClause") {
        BEGIN_CASE("1","オブジェクトを登録できる") {
            chain.renewInstance();
            Clause_test ct;
            Clause_test ct2;

            BEGIN_CASE("1","普通に登録する") {
                ct.addClause("test","first_literal");

                TEST_CASE("1","名前空間が増えている", Singleton<Namespace>::getInstance()->size() == 1);
                TEST_CASE("2","\"test\"という空間がある", Singleton<Namespace>::getInstance()->find("test") != Singleton<Namespace>::getInstance()->end());
                TEST_CASE("3","getClauseでアドレスが取れる", Clause::getClauseMap("test") != 0); 
                TEST_CASE("4","名前空間の節の数は1",  Clause::getClauseMap("test")->size() == 1);
                TEST_CASE("5","\"first_literal\"のノードがある", Clause::getClauseMap("test")->find("first_literal") != Clause::getClauseMap("test")->end());
                TEST_CASE("6","関連付けられた値が正しい", (*Clause::getClauseMap("test"))["first_literal"] == &ct);
            } END_CASE;

            BEGIN_CASE("2","2個目を登録する") {
                ct2.addClause("test","second_literal");

                TEST_CASE("1","名前空間が増えていない", Singleton<Namespace>::getInstance()->size() == 1);
                TEST_CASE("2","\"test\"という空間がある", Singleton<Namespace>::getInstance()->find("test") != Singleton<Namespace>::getInstance()->end());
                TEST_CASE("3","名前空間の節の数は2",  Clause::getClauseMap("test")->size() == 2);
                TEST_CASE("4","\"second_literal\"のノードがある", Clause::getClauseMap("test")->find("second_literal") != Clause::getClauseMap("test")->end());
                TEST_CASE("5","関連付けられた値が正しい", (*Clause::getClauseMap("test"))["second_literal"] == &ct2);
            } END_CASE;

            BEGIN_CASE("3","2個目を1個目で上書き登録する") {
                ct.addClause("test","second_literal");
                TEST_CASE("1","上書きされている", (*Clause::getClauseMap("test"))["second_literal"] == &ct);
            } END_CASE;

            BEGIN_CASE("4","違う名前空間に登録する") {
                ct.addClause("TEST","first_literal");

                TEST_CASE("1","名前空間が増えている", Singleton<Namespace>::getInstance()->size() == 2);
                TEST_CASE("2","\"TEST\"という空間がある", Singleton<Namespace>::getInstance()->find("TEST") != Singleton<Namespace>::getInstance()->end());
                TEST_CASE("3","getClauseでアドレスが取れる", Clause::getClauseMap("TEST") != 0); 
                TEST_CASE("4","名前空間の節の数は1",  Clause::getClauseMap("TEST")->size() == 1);
                TEST_CASE("5","\"first_literal\"のノードがある", Clause::getClauseMap("TEST")->find("first_literal") != Clause::getClauseMap("TEST")->end());
                TEST_CASE("6","関連付けられた値が正しい", (*Clause::getClauseMap("TEST"))["first_literal"] == &ct);
            } END_CASE;

        } END_CASE;

        BEGIN_CASE("2","無効なオブジェクトは登録されない") {
            chain.renewInstance();

            ((Clause_test *)0)->addClause("test","first_literal");

            TEST_CASE("1","名前空間が増えていない", Singleton<Namespace>::getInstance()->size() == 0);
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("hasClause","hasClause") {
        Clause_test ct;
        chain.renewInstance();

        TEST_CASE("1","何も要素がないときにも正常に動作する", !Clause::hasClause("dummy","null"));

        ct.addClause("test","first_literal");

        TEST_CASE("2","登録した要素を探せ、trueが返る", Clause::hasClause("test","first_literal"));
        TEST_CASE("3","存在しない要素にfalseが返る", !Clause::hasClause("dummy","null"));
    } END_CASE;

    BEGIN_CASE("addMultipleClauses","addMultipleClauses") {
        BEGIN_CASE("1","単一の節を登録できる") {
            chain.renewInstance();
            Clause_test ct;

            ct.addMultipleClauses("test","first");

            TEST_CASE("1","登録した節がある", Clause::hasClause("test","first"));
        } END_CASE;

        BEGIN_CASE("2","複数の節を登録する") {
            chain.renewInstance();
            Clause_test ct;

            ct.addMultipleClauses("test","first,second,third");

            TEST_CASE("1","登録した節がある", Clause::hasClause("test","first"));
            TEST_CASE("2","登録した節がある", Clause::hasClause("test","second"));
            TEST_CASE("3","登録した節がある", Clause::hasClause("test","third"));
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("Clause::onFail","Clause::onFail") {
        stringstream buf;
        Parser p(&buf, "test");
        Token token;
        Clause_test ct;

        buf.str("first second third 4 5 6 siebt acht neunt 0xa");

        ct.onFail_super(token, p);
        TEST_CASE("1","ストリームは最後まで進んでいる", p.eof());
    } END_CASE;

    BEGIN_CASE("parseClause","parseClause") {
        chain.renewInstance();
        Clause_test ct;

        ct.addClause("test","first");

        BEGIN_CASE("1","名前空間を選択して正しく節を実行できる") {
            stringstream buf;
            Parser p(&buf, "test");
            buf.str("first second");

            TestSuite::clearCheckpoints();
            TEST_CASE("1","関数は成功する", Clause::parseClause("test", p));
            TEST_CASE("2","Clause::beforeが実行されている", TestSuite::isReached("Clause_test::before"));
            TEST_CASE("3","Clause::bodyが実行されている", TestSuite::isReached("Clause_test::body"));
            TEST_CASE("4","bodyのfirstが正しい", ct.first == "first");
            TEST_CASE("5","bodyで読み出したトークンが正しい", ct.second == "second");
            TEST_CASE("6","Clause::afterが実行されている", TestSuite::isReached("Clause_test::after"));
        } END_CASE;

        BEGIN_CASE("2","節の処理中に例外が起こるとonFailが呼ばれる") {
            stringstream buf;
            Parser p(&buf, "test");
            buf.str("first second");

            ct.throw_exception = true;
            TestSuite::clearCheckpoints();

            bool result = false;
            try { Clause::parseClause("test", p); }
            catch(...){ result = true; }

            TEST_CASE("1","例外が起こる", result);
            TEST_CASE("2","Clause::bodyが実行されている", TestSuite::isReached("Clause_test::body"));
            TEST_CASE("3","Clause::onFailが実行されている", TestSuite::isReached("Clause_test::onFail"));
        } END_CASE;

        BEGIN_CASE("3","存在しない名前空間を指定する") {
            stringstream buf;
            Parser p(&buf, "test");
            buf.str("first second");

            TEST_CASE("1","関数は失敗する", !Clause::parseClause("unknown", p));
        } END_CASE;

        BEGIN_CASE("4","EOFに達したストリームを指定する") {
            stringstream buf;
            Parser p(&buf, "test");
            buf.str("");
            buf.get();

            TEST_CASE("0","[前提]ストリームは終端に達している", buf.eof());
            TEST_CASE("1","関数は失敗する", !Clause::parseClause("unknown", p));
        } END_CASE;
    } END_CASE;

    chain.restoreContext();
}




namespace {
    class StaticAPI_test : public StaticAPI
    {
    public:
        StaticAPI_test(void) throw() : StaticAPI() {}
        StaticAPI_test(string src) throw() : StaticAPI(src) {}

        void body(const Token & , Parser &) { TestSuite::check("StaticAPI::body"); }
        void onFail_super(const Token & first, Parser & p) throw() { StaticAPI::onFail(first, p); }
    };
}

TESTSUITE(main, StaticAPI)
{
    Singleton<Namespace>::Context context;
    Singleton<Namespace>::saveContext(context);

    Exception::setThrowControl(true);

    BEGIN_CASE("regist","regist") {
        BEGIN_CASE("1","単一の名前を指定して登録できる") {
            Singleton<Namespace>::renewInstance();
            StaticAPI_test api;

            api.regist("VTST_API");

            TEST_CASE("1","APIは正しく登録できている",StaticAPI::hasStaticAPI("VTST_API"));
        } END_CASE;

        BEGIN_CASE("2","複数の名前を指定して登録する") {
            Singleton<Namespace>::renewInstance();
            StaticAPI_test api;

            api.regist("API_1,API_2,API_3");

            TEST_CASE("1","APIは正しく登録できている",StaticAPI::hasStaticAPI("API_1"));
            TEST_CASE("2","APIは正しく登録できている",StaticAPI::hasStaticAPI("API_2"));
            TEST_CASE("3","APIは正しく登録できている",StaticAPI::hasStaticAPI("API_3"));
        } END_CASE;

        BEGIN_CASE("3","上書きする") {
            Singleton<Namespace>::renewInstance();
            StaticAPI_test api;
            StaticAPI_test api2;
            StaticAPI_test api3;

            api.regist("test");
            TEST_CASE("1","APIは正しく登録できている",StaticAPI::hasStaticAPI("test"));
            TEST_CASE("2","APIハンドラが正しい", (*StaticAPI::getClauseMap(NAMESPACE_STATICAPI))["test"] == &api);

            api2.regist("test");
            TEST_CASE("3","APIハンドラが上書きされている", (*StaticAPI::getClauseMap(NAMESPACE_STATICAPI))["test"] == &api2);

            api3.regist("test");
            TEST_CASE("4","APIハンドラが上書きされている", (*StaticAPI::getClauseMap(NAMESPACE_STATICAPI))["test"] == &api3);
        } END_CASE;

        BEGIN_CASE("4","コンストラクタで登録") {
            Singleton<Namespace>::renewInstance();
            StaticAPI_test api("API1,API2,API3");

            TEST_CASE("1","APIは正しく登録できている",StaticAPI::hasStaticAPI("API1"));
            TEST_CASE("2","APIは正しく登録できている",StaticAPI::hasStaticAPI("API2"));
            TEST_CASE("3","APIは正しく登録できている",StaticAPI::hasStaticAPI("API3"));
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("parseStaticAPI","parseStaticAPI") {
        BEGIN_CASE("1","静的APIを正しく処理できる") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            StaticAPI_test api("API");

            buf.str("API();");
            Parser p(&buf, "test");

            TestSuite::clearCheckpoints();
            bool result = true;
            try { StaticAPI::parseStaticAPI(p); }
            catch(...) { result = false; }

            TEST_CASE("1","例外は起らない", result);
            TEST_CASE("2","静的APIの本体が呼ばれている", TestSuite::isReached("StaticAPI::body"));
        } END_CASE;

        BEGIN_CASE("2","'('忘れで例外") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            StaticAPI_test api("API");

            buf.str("API);");
            Parser p(&buf, "test");

            TestSuite::clearCheckpoints();
            bool result = false;
            try { StaticAPI::parseStaticAPI(p); }
            catch(...) { result = true; }

            TEST_CASE("1","例外が起る", result);
            TEST_CASE("2","静的APIの本体が呼ばれない", !TestSuite::isReached("StaticAPI::body"));
        } END_CASE;

        BEGIN_CASE("3","')'忘れで例外") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            StaticAPI_test api("API");

            buf.str("API(; NEXT");
            Parser p(&buf, "test");

            TestSuite::clearCheckpoints();
            bool result = false;
            try { StaticAPI::parseStaticAPI(p); }
            catch(...) { result = true; }

            TEST_CASE("1","例外が起る", result);
            TEST_CASE("2","静的APIの本体が呼ばれている", TestSuite::isReached("StaticAPI::body"));
        } END_CASE;

        BEGIN_CASE("4","';'忘れで例外") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            StaticAPI_test api("API");

            buf.str("API() NEXT");
            Parser p(&buf, "test");

            TestSuite::clearCheckpoints();
            bool result = false;
            try { StaticAPI::parseStaticAPI(p); }
            catch(...) { result = true; }

            TEST_CASE("1","例外が起る", result);
            TEST_CASE("2","静的APIの本体が呼ばれている", TestSuite::isReached("StaticAPI::body"));
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("onFail","onFail") {
        BEGIN_CASE("1","セミコロンまで読み飛ばし") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Parser p(&buf, "test");
            StaticAPI_test api;
            Token token;
            buf.str("api parameter;next_api next_api_parameter;");

            api.onFail_super(Token(), p);

            TEST_CASE("1","セミコロンの次が読める", p.getToken() == "next_api");
        } END_CASE;

        BEGIN_CASE("2","次のAPIまで読み飛ばし") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Parser p(&buf, "test");
            StaticAPI_test api("api,next_api");
            buf.str("api parameter_1 parameter_2 next_api next_api_parameter;");

            api.onFail_super(p.getToken(), p);

            TEST_CASE("1","次のAPI名が読める", p.getToken() == "next_api");
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("skip","skip") {
        BEGIN_CASE("1","セミコロンまで読み飛ばし") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Parser p(&buf, "test");
            StaticAPI_test api;
            Token token;
            buf.str("api parameter;next_api next_api_parameter;");

            StaticAPI::skip(p);

            TEST_CASE("1","セミコロンの次が読める", p.getToken() == "next_api");
        } END_CASE;

        BEGIN_CASE("2","次のAPIまで読み飛ばし") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Parser p(&buf, "test");
            StaticAPI_test api("api,next_api");
            buf.str("parameter_1 parameter_2 next_api next_api_parameter;");

            StaticAPI::skip(p);

            TEST_CASE("1","次のAPI名次が読める", p.getToken() == "next_api");
        } END_CASE;

        BEGIN_CASE("3","先頭がAPI名でも読み飛ばしできる") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Parser p(&buf, "test");
            StaticAPI_test api("api,next_api");
            buf.str("api parameter_1 parameter_2 next_api next_api_parameter;");

            StaticAPI::skip(p);

            TEST_CASE("1","次のAPI名が読める", p.getToken() == "next_api");
        } END_CASE;
    } END_CASE;
    
    Singleton<Namespace>::restoreContext(context);
}


namespace {
    class Directives_test : public Directives
    {
    public:
        bool throws;

        Directives_test(void) throw() : Directives(), throws(false) {}
        Directives_test(string src) throw() : Directives(src), throws(false)  {}

        void body(const Token & , Parser & ) throw(int)
        {
            TestSuite::check("Directives::body");
            if(throws)
                throw 0;
        }
    };
}

TESTSUITE(main, Directives)
{
    Singleton<Namespace>::Context context;
    Singleton<Namespace>::saveContext(context);

    Exception::setThrowControl(true);

    BEGIN_CASE("regist","regist") {
        BEGIN_CASE("1","単一の名前を指定して登録できる") {
            Singleton<Namespace>::renewInstance();
            Directives_test api;

            api.regist("include");

            TEST_CASE("1","APIは正しく登録できている",Directives::hasDirectives("include"));
        } END_CASE;

        BEGIN_CASE("2","複数の名前を指定して登録する") {
            Singleton<Namespace>::renewInstance();
            Directives_test api;

            api.regist("pragma,define,endif");

            TEST_CASE("1","ディレクティブは正しく登録できている",Directives::hasDirectives("pragma"));
            TEST_CASE("2","ディレクティブは正しく登録できている",Directives::hasDirectives("define"));
            TEST_CASE("3","ディレクティブは正しく登録できている",Directives::hasDirectives("endif"));
        } END_CASE;

        BEGIN_CASE("3","上書きする") {
            Singleton<Namespace>::renewInstance();
            Directives_test api;
            Directives_test api2;
            Directives_test api3;

            api.regist("test");
            TEST_CASE("1","ディレクティブは正しく登録できている",Directives::hasDirectives("test"));
            TEST_CASE("2","ディレクティブハンドラが正しい", (*Directives::getClauseMap(NAMESPACE_DIRECTIVES))["test"] == &api);

            api2.regist("test");
            TEST_CASE("3","ディレクティブハンドラが上書きされている", (*Directives::getClauseMap(NAMESPACE_DIRECTIVES))["test"] == &api2);

            api3.regist("test");
            TEST_CASE("4","ディレクティブハンドラが上書きされている", (*Directives::getClauseMap(NAMESPACE_DIRECTIVES))["test"] == &api3);
        } END_CASE;

        BEGIN_CASE("4","コンストラクタで登録") {
            Singleton<Namespace>::renewInstance();
            Directives_test api("pragma,define,endif");

            TEST_CASE("1","ディレクティブは正しく登録できている",Directives::hasDirectives("pragma"));
            TEST_CASE("2","ディレクティブは正しく登録できている",Directives::hasDirectives("define"));
            TEST_CASE("3","ディレクティブは正しく登録できている",Directives::hasDirectives("endif"));
        } END_CASE;
    } END_CASE;
    
    BEGIN_CASE("parseDirectives","parseDirectives") {
        BEGIN_CASE("1","ディレクティブを正しく処理できる") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Directives_test api("pragma");

            buf.str("pragma\nnext\n");
            Parser p(&buf, "test");

            TestSuite::clearCheckpoints();
            bool result = true;
            try { Directives::parseDirectives(p); }
            catch(...) { result = false; }

            TEST_CASE("1","例外は起らない", result);
            TEST_CASE("2","ディレクティブの本体が呼ばれている", TestSuite::isReached("Directives::body"));
        } END_CASE;

        BEGIN_CASE("2","改行なし(不正パラメータ)で例外") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Directives_test api("pragma");

            buf.str("pragma next\n");
            Parser p(&buf, "test");

            TestSuite::clearCheckpoints();
            bool result = false;
            try { Directives::parseDirectives(p); }
            catch(...) { result = true; }

            TEST_CASE("1","例外が起る", result);
            TEST_CASE("2","ディレクティブの本体が呼ばれている", TestSuite::isReached("Directives::body"));
        } END_CASE;

        BEGIN_CASE("3","改行なし(EOF)で例外") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Directives_test api("pragma");

            buf.str("pragma");
            Parser p(&buf, "test");

            TestSuite::clearCheckpoints();
            bool result = false;
            try { Directives::parseDirectives(p); }
            catch(...) { result = true; }

            TEST_CASE("1","例外が起る", result);
            TEST_CASE("2","ディレクティブの本体が呼ばれている", TestSuite::isReached("Directives::body"));
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("onFail","onFail") {
        BEGIN_CASE("1","bodyで例外が起るとonFailが次の改行まで読み飛ばす") {
            Singleton<Namespace>::renewInstance();
            stringstream buf;
            Directives_test api("pragma");
            buf.str("pragma parameter\nnext\n");
            Parser p(&buf, "test");

            api.throws = true;
            bool result = false;
            try { Directives::parseDirectives(p); }
            catch(...) { result = true; }

            TEST_CASE("1","例外が起る", result);
            TEST_CASE("2","残った内容が正しい", p.getToken() == "next");
            TEST_CASE("3","トークンは行頭", p.isHeadofLine());
        } END_CASE;
    } END_CASE;
            
    Singleton<Namespace>::restoreContext(context);
}

#endif



