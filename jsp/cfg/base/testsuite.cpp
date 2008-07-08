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
 *  @(#) $Id: testsuite.cpp,v 1.3 2003/12/15 07:32:14 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/testsuite.cpp,v 1.3 2003/12/15 07:32:14 takayuki Exp $

#include "base/testsuite.h"

#include "base/coverage_defs.h"
#include "base/coverage_undefs.h"

#ifdef TESTSUITE

#undef  main           //テストスィート用のmainを定義するのでundefする
#undef  cin
#undef  cout
#undef  cerr

#include <stdlib.h>     //EXIT_SUCCESS|EXIT_FAILURE用
#include <fstream>      //作業関数用
#include <cstdio>
#include <iomanip>

using namespace std;

/* ストリーム入換え用の変数定義 */
namespace std {
    stringstream _cin;
    stringstream _cout;
    stringstream _cerr;
}

/*
 *  テストスィートの実行制御クラス ; class TestSuiteControl
 */


    //コンストラクタ
TestSuiteControl::TestSuiteControl(ostream & _out, const char * _pattern) : target_session(_pattern), out(&_out), fail(false), verbose(false), case_count(0), fail_count(0)
{}

    //シナリオ名の取得
string TestSuiteControl::get_suitename(const char * name) const
{
    string result;
    list<string>::const_iterator scope;

    scope = session.begin();
    while(scope != session.end()) {
        result += *scope;
        result += '.';
        ++ scope;
    }

    if(name != NULL)
        result += name;

    return result;
}


    //テストシナリオの表題設定 (返却値 : 実行する必要があるかどうか)
bool TestSuiteControl::_enter_case(const char * session, const char * description, const char * filename, long lineno)
{
    bool         result;
    stringstream buf;

        //表題の設定
    current_session.assign(get_suitename(session));

        //詳細情報の設定
    if(description != 0)
        buf << description;

        //ファイル名 (filename:lineno) の生成
    if(filename != 0) {
        string            work(filename);
        string::size_type pos;

            //フルパスで来たら短く切る
        pos = work.find_last_of("/\\");
        if(pos != string::npos)
            work.erase(0, pos + 1);

        buf << '(' << work;
        if(lineno != -1)
            buf << ':' << lineno;
        buf << ')';
    }
    current_description = buf.str();
    
        //実行する必要の有無の確認
    result = (target_session.empty() || current_session.find(target_session) != string::npos);

    if(result) {
        if(verbose) {
            string indent;
            indent.assign(2 * this->session.size(), ' ');
            (*out) << indent << '[' << session << "] " << current_description << endl;

            //(*out) << '[' << current_session << "] " << current_description << endl;
        }

        enter_session(session);
        ++ case_count;
    }

    return result;
}

    //テストセッション名 (シナリオプレフィクス) の設定
void TestSuiteControl::enter_session(const string & _session)
{   session.push_back(_session);   }

    //テストセッション名 (シナリオプレフィクス) の解除
void TestSuiteControl::leave_session(void)
{   session.pop_back();   }

    //テストシナリオの失敗報告
void TestSuiteControl::fail_in_suite(string msg)
{
    fail = true;
    (*out) << "Failed : [" << current_session << "]\n   " << current_description << endl;
    if(!msg.empty())
        (*out) << "     #" << msg << endl;
    ++ fail_count;
}


    //スィート情報コンストラクタ
TestSuite::SuiteContainerNode::SuiteContainerNode(const char * _session, SuiteFunction _suite) : session(_session), suite(_suite)
{}

    //スィート情報コンストラクタ
TestSuite::SuiteContainerNode::SuiteContainerNode(const SuiteContainerNode & src) : session(src.session), suite(src.suite)
{}

    //スィートの実行
bool TestSuite::SuiteContainerNode::invoke(TestSuiteControl & suite_control)
{
    if(suite != 0) {
        suite_control.enter_session(session);
        (*suite)(suite_control);
        suite_control.leave_session();
    }

    return !suite_control.is_failed();
}

/*
 *  テストスィート実行クラス
 */

    //コンストラクタ
TestSuite::TestSuite(const char * session, SuiteFunction suite) : fail(false)
{   Singleton<SuiteContainer>::getInstance()->push_back(SuiteContainerNode(session, suite));   }


    //デストラクタ
TestSuite::~TestSuite(void)
{}


    //テストスィートの実行  (返却値:スィートを実行したかどうか)
bool TestSuite::performTestSuite(int flags, const char * pattern)
{
    bool                        result;
    TestSuiteControl            suite_control(cerr, pattern);
    SuiteContainer *            container;
    SuiteContainer::iterator    scope;

        //実行スィートリストの参照
    container = Singleton<SuiteContainer>::getInstance();

        //冗長出力設定
    if( (flags & VERBOSE) != 0 )
        suite_control.set_verbose();

        //全登録スィートの実行
    scope = container->begin();
    while(scope != container->end()) {

        result = scope->invoke(suite_control);

            //いちいち止める or 通しで実行
        if( !result && (flags & THROUGH) == 0)
            break;

        ++ scope;
    }

        //エラーがなければOK表示
    if(!suite_control.is_failed())
        cerr << "OK (" << suite_control.countCases() << " passed)" << endl;
    else
        cerr << "Failed (" << suite_control.countFails() << '/' << suite_control.countCases() << ')' << endl;

    return !suite_control.is_failed();    //スィートを正常に実行できたらtrue
}


    //登録されているトップレベルスィートの一覧表示
void TestSuite::listTestSuite(void)
{
    SuiteContainer::iterator  scope;
    int                       count;
    SuiteContainer *          container;

    count     = 0;
    container = Singleton<SuiteContainer>::getInstance();
    
    scope = container->begin();
    while(scope != container->end()) {
        cout << "  ";
        cout.width(3);
        cout << count << ':' << scope->getName() << endl;

        ++ count;
        ++ scope;
    }
    cout << "Total " << count << " sets of suite exist.\n";
}



/*
 *   チェックポイント用関数
 */

bool TestSuite::isReached(std::string checkname)
{
    set<string>::iterator scope;

    scope = Singleton<Checkpoint>::getInstance()->find(checkname);

    return scope != Singleton<Checkpoint>::getInstance()->end();
}


/*
 *   テストスィート実行用作業関数
 */

    //ファイル内容比較
bool TestSuite::compareFileContents(const char * filename, const char * filecontents, bool _remove)
{
    fstream file(filename,ios::in);
    int     ch;
    bool    result = true;

        //内容チェック
    while(result && *filecontents != '\x0' && file.good()) {
        ch = file.get();

        if(ch != (int)*filecontents)
            result = false;

        ++ filecontents;
    }

        //EOFに達しているか
    if(result) {
        if(!file.good() || file.get() != EOF)
            result = false;
    }

    file.close();

    if(_remove)
        ::remove(filename);

    return result;
}

    
/*
    *   テストスィート実行用main関数
    */
int main(int argc, char ** argv)
{
    int    result;
    int    pos;
    int    flags;
    string category;

    flags = TestSuite::DEFAULT;

    for(pos = 1; pos < argc; ++ pos) {
        if(*argv[pos] == '-') {
            switch(argv[pos][1]) {
            case 'v': 
                flags |= TestSuite::VERBOSE;
                break;
            case 'V': 
                flags &= ~TestSuite::VERBOSE;
                break;
            case 't': 
                flags |= TestSuite::THROUGH;
                break;
            case 'T': 
                flags &= ~TestSuite::THROUGH;
                break;
            case 'l': 
                TestSuite::listTestSuite();
                return EXIT_SUCCESS;
            case 'h':
                cerr << "Usage : program (option) (suite)\n"
                            " -v, -V  : Verbose output on(-v) / off(-V)\n"
                            " -t, -T  : Go through with the suite, on(-t) / off(-T)\n"
                            " -l      : List the top level suites\n";
                return EXIT_SUCCESS;
            default:
                cerr << "Unknown option [" << argv[pos] << "]\n";
                return EXIT_FAILURE;
            }
        }
        else {
            if(!category.empty()) {
                cerr << "Two or more category specified.\n";
                return EXIT_FAILURE;
            }
            category.assign(argv[pos]);
        }
    }

    result = EXIT_FAILURE;
    if(TestSuite::performTestSuite(flags, category.c_str()))
        result = EXIT_SUCCESS;

#ifdef COVERAGE
    fstream fout("coverage.txt",ios::out);
    if(fout.is_open()) {
        Coverage::printCoverage(fout);
        fout.close();
    }
#endif

    return result;
}

#endif //TESTSUITE

