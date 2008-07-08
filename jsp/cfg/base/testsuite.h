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
 *  @(#) $Id: testsuite.h,v 1.4 2003/12/20 06:51:58 takayuki Exp $
 */

/*
 *   テストスィート実行補助 クラス/マクロライブラリ
 */

// $Header: /home/CVS/configurator/base/testsuite.h,v 1.4 2003/12/20 06:51:58 takayuki Exp $


#ifdef TESTSUITE

    //シナリオ終端マクロ(Exceptionクラスの有無によって中身を替えたいのでここにおく)
#  undef END_CASE
#  ifdef EXCEPT_H
#    define END_CASE }catch(TestSuite::Fail & x) { _suite_control.fail_in_suite(x.getMessage()); } catch(Exception & exc) { _suite_control.fail_in_suite(string("不明な例外 : ") + exc.getDetails()); } catch(...) { _suite_control.fail_in_suite("テスト実行中に不明な例外を受け取ったため中断された"); } _suite_control.leave_case(); }
#  else
#    define END_CASE }catch(TestSuite::Fail & x) { _suite_control.fail_in_suite(x.getMessage()); } catch(...) { _suite_control.fail_in_suite("テスト実行中に不明な例外を受け取ったため中断された"); } _suite_control.leave_case(); }
#  endif

#endif


#ifndef TESTSUITE_H
#define TESTSUITE_H

    //テストスィートをかけるかどうか
//#define TESTSUITE


#if defined(TESTSUITE) //&& defined(_DEBUG)

    //ストリームの内容をテストで使いたいので入れ替える
#include <iostream>
#define cin  _cin
#define cout _cout
#define cerr _cerr

#include <sstream>

namespace std {
    extern stringstream _cin;
    extern stringstream _cout;
    extern stringstream _cerr;
}

#include <string>
#include <list>
#include <set>
#include <map>


    //標準のテストスィート関数のプロトタイプ宣言用マクロ
#define TESTSUITE_PROTOTYPE(suite) \
    public: \
        static void test_suite_##suite(TestSuiteControl & _suite_control);


    //標準のテストスィート関数の定義用マクロ (わざとスィート実行の条件コンパイル用の定義名と同じ名前にする)
#undef TESTSUITE
#define TESTSUITE(suite,cls) \
    TestSuite TestSuite_##cls##_##suite(#cls "(" #suite ")", cls::test_suite_##suite); \
    void cls::test_suite_##suite(TestSuiteControl & _suite_control)

#define TESTSUITE_(suite,cls,spr) \
    TestSuite TestSuite_##spr##_##cls##_##suite(#spr "::" #cls "(" #suite ")", spr::cls::test_suite_##suite); \
    void spr::cls::test_suite_##suite(TestSuiteControl & _suite_control)

    //多重実行防止用マクロ
#define PROCEED_ONCE static bool _proceed_once_flag_ = false; if(_proceed_once_flag_) return; else _proceed_once_flag_ = true;

    //シナリオ開始マクロ
#define BEGIN_CASE(x,y) if(_suite_control.enter_case(x,y,__FILE__,__LINE__)) { try {

    //シナリオ失敗マクロ
#define TEST_FAIL     throw TestSuite::Fail();
#define TEST_FAIL_(x) throw TestSuite::Fail(x);

    //短いシナリオ用マクロ
#define TEST_CASE(x,y,z)   BEGIN_CASE(x,y) { if(!(z)) TEST_FAIL;    } END_CASE
#define TEST_CASE_(x,y,z,u) BEGIN_CASE(x,y) { if(!(z)) TEST_FAIL_(u); } END_CASE

    //到達判定マクロ
#define CHECKPOINT(x) TestSuite::check(x);

    //シングルトン退避
#define SINGLETON(x)    Singleton<x>::Context context;     Singleton<x>::saveContext(context);
#define SINGLETON_(x,y) Singleton<x>::Context context_##y; Singleton<x>::saveContext(context_##y);

/*
 *  テストスィートの実行制御クラス
 */
class TestSuiteControl {
protected:
    std::list<std::string> session;         //現在のセッション名スタック

    std::string    target_session;          //処理対象セッション名
    std::string    current_session;         //現在のスィートシナリオ名
    std::string    current_description;     //現在のスィートシナリオの説明
    std::ostream * out;                     //説明の出力先
    bool           fail;                    //失敗したシナリオがあったかどうか
    bool           verbose;                 //冗長出力するか否か

    unsigned int   case_count;              //ケースの総数
    unsigned int   fail_count;              //失敗したケースの数

        //シナリオ名の取得
    std::string get_suitename(const char * name = "") const;


public:
        //コンストラクタ
    TestSuiteControl(std::ostream & _out, const char * _pattern = "");

        //テストシナリオの表題設定 (返却値 : 実行する必要があるかどうか)
    bool        _enter_case(const char * session, const char * description, const char * filename = "", long lineno = -1);
    inline void leave_case(void) { leave_session(); }

        //シングルステップ実行時に中に飛び込まないためのラッパ
    inline bool enter_case(const char * session, const char * description, const char * filename = "", long lineno = -1)
    {   return _enter_case(session, description, filename, lineno);   }

        //テストセッション名 (シナリオプレフィクス) の設定/解除
    void enter_session(const std::string & session);
    void leave_session(void);

        //テストシナリオの失敗報告
    void fail_in_suite(std::string msg="");                                           

        //失敗したシナリオの有無の確認
    inline bool is_failed(void) const
    {   return fail;   }

        //冗長出力の設定
    inline void set_verbose(bool _verbose = true)
    {   verbose = _verbose;   }

        //実行したテストケースの数
    inline unsigned int countCases(void) const
    {   return case_count;   }

        //失敗したテストケースの数
    inline unsigned int countFails(void) const
    {   return fail_count;   }
};

    //シングルトンのテストスィートのためにここでインクルード
#include "base/singleton.h"

/*
 *  テストスィート実行クラス
 */
class TestSuite {
public:
    class Fail  //テストケース失敗通知に用いる例外用クラス
    {
    protected:
        std::string msg;

    public:
            //コンストラクタ
        Fail(std::string src = "") throw() : msg(src) {}

            //コピーコンストラクタ
        Fail(const Fail & src) throw() : msg(src.msg) {}
        
            //メッセージを取得
        inline const std::string & getMessage(void) const throw()
        {   return msg;   }
    };

    typedef void (*SuiteFunction)(TestSuiteControl &);

        /* クラス毎のテストスィートに関する情報を保持するクラス */
    class SuiteContainerNode
    {
    protected:
        std::string   session;
        SuiteFunction suite;
    
    public:
            //スィート情報コンストラクタ
        SuiteContainerNode(const char * session, SuiteFunction suite);
        SuiteContainerNode(const SuiteContainerNode & src);

            //スィートの実行
        bool invoke(TestSuiteControl & suite_control);

            //スィート名の取得
        inline const std::string & getName(void) const throw()
        {   return session;   }
    };

        //実行すべきテストスィートを持つインスタンスのリストの型
    class SuiteContainer : public std::list<SuiteContainerNode>
    {   public: SINGLETON_CONSTRUCTOR(SuiteContainer) {}   };

        /* 実行オプション */
    enum tagPerformOption {
        THROUGH = 1,    //すべて通しで実行する (いちいち止めない)
        VERBOSE = 2,    //冗長出力

        DEFAULT = THROUGH
    };

        /* チェックポイントを格納するクラス */
    class Checkpoint : public std::set<std::string>
    { public: SINGLETON_CONSTRUCTOR(Checkpoint) throw() {} };

protected:
    bool            fail;       //最後に実行したスィートが失敗したがどうかを保持する変数
    static bool     starvated;  //newに失敗するかどうか

public:
        //コンストラクタ
    TestSuite(const char * session, SuiteFunction suite);

        //デストラクタ
    ~TestSuite(void);

        //テストスィートの実行  (返却値:スィートが正しく実行されたかどうか)
    static bool performTestSuite(int flags = THROUGH, const char * pattern = "");

        //テストスィートの実行 (引数変換)
    inline static bool performTestSuite(const char * pattern)
    {   return performTestSuite(THROUGH, pattern);   }

        //登録されているトップレベルスィートの一覧表示
    static void listTestSuite(void);

        /*
         *   チェックポイント用関数
         */

        //通過したことを報告
    inline static void check(std::string checkname)
    {   Singleton<Checkpoint>::getInstance()->insert(checkname);   }

        //これまでに受けた報告を削除
    inline static void clearCheckpoints(void)
    {   Singleton<Checkpoint>::getInstance()->clear();   }

        //その位置に来たか
    static bool isReached(std::string checkname);

        /*
         *   テスト用作業関数
         */

        //ファイルの中身を開いてチェックする
    static bool compareFileContents(const char * filename, const char * filecontents, bool remove = true);
};

/*
 *  スィート実行用main関数
 */
extern int main(int argc, char ** argv);

#define main pseudo_main    //通常のmainを差し換え

    //簡易ソフトウェアトレース機能のインクルード
#include "base/coverage_defs.h"

#else

/*********************************************************************************************/
/*
 *  テストを実行しないときのクラス定義
 */

#define TESTABLEOBJECT(x)
#define TESTABLEOBJECT_(x,y)
#define TESTSUITE_PROTOTYPE(x)
#define TESTSUITE_(x,y)
#define PROCEED_ONCE

#define BEGIN_CASE(x,y)
#define END_CASE
#define TEST_FAIL

#define CHECKPOINT(x)

/*
    *  テストスィート実行クラス
    */
class TestSuite {
public:
        //テストスィートの実行  (返却値:スィートを正しく実行したかどうか)
    inline static bool performTestSuite(void) { return true; };
};

#endif

#endif /* TESTSUITE_H */

