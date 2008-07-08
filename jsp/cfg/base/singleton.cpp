/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2002-2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: singleton.cpp,v 1.5 2003/12/15 07:32:14 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/singleton.cpp,v 1.5 2003/12/15 07:32:14 takayuki Exp $

#include "base/singleton.h"
#include "base/testsuite.h"

using namespace std;

    //Singleton 静的変数
bool SingletonBase::initialize = false;     //初期化済みフラグ
bool SingletonBase::is_valid   = true;      //インスタンス生成成功フラグ

struct SingletonBase::chain * SingletonBase::top  = 0;  //デストラクタチェーンのトップ


/************************************* テストスィート *************************************/

#ifdef TESTSUITE
#include "base/coverage_undefs.h"

    bool throw_DummyType;
    class DummyType {
    public:
        static int counter;
        int dummy_value;
        int order;
        SINGLETON_CONSTRUCTOR(DummyType) throw(bad_alloc) : dummy_value(0), order(++counter)
        {
            CHECKPOINT("DummyType::DummyType");
            if(throw_DummyType)
                throw bad_alloc();
        } 
        ~DummyType(void) throw(int)
        {
            CHECKPOINT("DummyType::~DummyType");
            if(throw_DummyType)
                throw 0;
            if(counter-- != order)
                CHECKPOINT("illegal order");
        }
    };

    SINGLETON_WRAPPER(DummyType2, DummyType)

    int DummyType::counter = 0;

    TESTSUITE(main, SingletonBase)
    {
        struct chain * old_top  = top;

        top  = 0;

        BEGIN_CASE("getInstance","getInstance") {
            BEGIN_CASE("1","単一のインスタンスが生成される") {
                TestSuite::clearCheckpoints();
                DummyType * dummy = Singleton<DummyType>::getInstance();
                TEST_CASE("1","コンストラクタが起動されている", TestSuite::isReached("DummyType::DummyType"));

                TestSuite::clearCheckpoints();
                TEST_CASE("2","インスタンスが生成される", Singleton<DummyType>::getInstance() == dummy);
                TEST_CASE("3","2回目以降はコンストラクタが起動されない", !TestSuite::isReached("DummyType::DummyType"));
                TEST_CASE("4","シングルトンの初期化が終わっている", SingletonBase::initialize);
                TEST_CASE("5","デストラクタチェーンが構成されている", SingletonBase::top != 0);
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("final_release","final_release") {
            BEGIN_CASE("1","インスタンスが削除できる") {
                TEST_CASE("0","[前提] 削除すべき要素がある", SingletonBase::top != 0);

                TestSuite::clearCheckpoints();
                SingletonBase::final_release();

                TEST_CASE("1","デストラクタが起動されている", TestSuite::isReached("DummyType::~DummyType"));
                TEST_CASE("2","デストラクタチェーンがなくなっている", SingletonBase::top == 0);
            } END_CASE;

            BEGIN_CASE("2","順番が正しい") {
                DummyType::counter = 0;

                DummyType  * dummy  = Singleton<DummyType>::getInstance();
                DummyType2 * dummy2 = Singleton<DummyType2>::getInstance();

                TEST_CASE("0","[前提] 生成順序が正しい", dummy->order == 1 && dummy2->order == 2);

                TestSuite::clearCheckpoints();
                SingletonBase::final_release();

                TEST_CASE("1","デストラクタが起動されている", TestSuite::isReached("DummyType::~DummyType"));
                TEST_CASE("2","デストラクタチェーンがなくなっている", SingletonBase::top == 0);
                TEST_CASE("3","破棄順序が正しい", !TestSuite::isReached("illegal order"));
            } END_CASE;

        } END_CASE;

        BEGIN_CASE("getInstance","getInstance") {
            BEGIN_CASE("1","生成時に例外") {
                BEGIN_CASE("1","getInstance()") {
                    throw_DummyType = true;
                    bool result = false;
                    try { Singleton<DummyType>::getInstance(); }
                    catch(...) { result = true; }

                    TEST_CASE("1","例外は起こる",result);
                    TEST_CASE("2","デストラクタチェーンには何もつながっていない", SingletonBase::top == 0);
                } END_CASE;

                SingletonBase::final_release();

                BEGIN_CASE("2","getInstance(nothrow)") {

                    DummyType * dummy;
                    throw_DummyType = true;
                    bool result = true;
                    try { dummy = Singleton<DummyType>::getInstance(nothrow); }
                    catch(...) { result = false; }

                    TEST_CASE("1","例外は起きない",result);
                    TEST_CASE("2","NULLが返る", dummy == 0);
                    TEST_CASE("3","デストラクタチェーンには何もつながっていない", SingletonBase::top == 0);
                } END_CASE;
            } END_CASE;
        } END_CASE;

        top  = old_top;
    }

#endif



