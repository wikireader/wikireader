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
 *  @(#) $Id: event.cpp,v 1.5 2003/12/20 06:51:58 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/event.cpp,v 1.5 2003/12/20 06:51:58 takayuki Exp $

#include "base/event.h"

using namespace std;

/***************************** テストスィート *****************************/

#ifdef TESTSUITE
#include "base/coverage_undefs.h"

namespace {
    class Event_fortest : public Event<int> 
    {   TESTSUITE_PROTOTYPE(main)   };

    int * ptr_i_dummy_handler;
    void dummy_handler(int & i)
    {
        ptr_i_dummy_handler = &i;
        ++ i;
        TestSuite::check("dummy_handler");
    }

    class EventHandler : public Event_fortest::Handler
    {
    public:
        int * ptr_i;

        EventHandler(void)  throw() { TestSuite::check("EventHandler::EventHandler");  }
        ~EventHandler(void) throw() { TestSuite::check("EventHandler::~EventHandler"); }
        void handler(Event_fortest::value_type & i) { ptr_i = &i; ++i; TestSuite::check("EventHandler::handler"); }
    };
}

TESTSUITE(main,Event_fortest)
{
    BEGIN_CASE("add","add") {
        BEGIN_CASE("1","addでハンドラが登録できる") {
            Singleton<Event_fortest::handler_list_type>::renewInstance();

            Event_fortest::add(dummy_handler);

            TEST_CASE("1","ハンドラリストが1増えている", Singleton<Event_fortest::handler_list_type>::getInstance()->size() == 1);
            TEST_CASE("2","ハンドラリストの中身は正しい", *Singleton<Event_fortest::handler_list_type>::getInstance()->begin() == dummy_handler);
        } END_CASE;

        BEGIN_CASE("2","同じハンドラを複数回登録できる") {
            Singleton<Event_fortest::handler_list_type>::renewInstance();

            Event_fortest::add(dummy_handler);
            Event_fortest::add(dummy_handler);
            Event_fortest::add(dummy_handler);

            TEST_CASE("1","ハンドラリストの中身は3", Singleton<Event_fortest::handler_list_type>::getInstance()->size() == 3);
            TEST_CASE("2","ハンドラリストの中身は正しい", *Singleton<Event_fortest::handler_list_type>::getInstance()->begin() == dummy_handler);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("remove","remove") {
        BEGIN_CASE("1","addで登録したハンドラを削除できる") {
            Singleton<Event_fortest::handler_list_type>::renewInstance();

            Event_fortest::add(dummy_handler);
            Event_fortest::remove(dummy_handler);

            TEST_CASE("1","ハンドラリストの要素数は0", Singleton<Event_fortest::handler_list_type>::getInstance()->size() == 0);
        } END_CASE;

        BEGIN_CASE("2","登録した全てのハンドラが消える") {
            Singleton<Event_fortest::handler_list_type>::renewInstance();

            Event_fortest::add(dummy_handler);
            Event_fortest::add(dummy_handler);
            Event_fortest::add(dummy_handler);
            Event_fortest::remove(dummy_handler);

            TEST_CASE("1","ハンドラリストの要素数は0", Singleton<Event_fortest::handler_list_type>::getInstance()->size() == 0);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("constructor/destructor","constructor/destructor") {
        Singleton<Event_fortest::Handler::instance_list_type>::renewInstance();

        TEST_CASE("0","[前提] ハンドラリストの中身は0", Singleton<Event_fortest::Handler::instance_list_type>::getInstance()->size() == 0);
        {
            EventHandler evhdr;
            TEST_CASE("1","ハンドラリストに登録されている", Singleton<Event_fortest::Handler::instance_list_type>::getInstance()->size() == 1);
            TEST_CASE("2","正しく登録されている", *Singleton<Event_fortest::Handler::instance_list_type>::getInstance()->begin() == &evhdr);
        }
        TEST_CASE("3","ハンドラは解除されている", Singleton<Event_fortest::Handler::instance_list_type>::getInstance()->size() == 0);
    } END_CASE;

    BEGIN_CASE("raise","raise") {
        BEGIN_CASE("1","登録した静的なハンドラが実行される") {
            Singleton<Event_fortest::handler_list_type>::renewInstance();
            TestSuite::clearCheckpoints();

            Event_fortest::add(dummy_handler);

            int i = 0;
            Event_fortest::raise(i);

            TEST_CASE("1","ハンドラが実行される",TestSuite::isReached("dummy_handler"));
            TEST_CASE("2","引数間で見ている変数は同一", &i == ptr_i_dummy_handler);
            TEST_CASE("3","値がインクリメントされている", i == 1);
        } END_CASE;

        BEGIN_CASE("2","登録したインスタンスのハンドラが実行される") {
            Singleton<Event_fortest::handler_list_type>::renewInstance();
            TestSuite::clearCheckpoints();
            EventHandler evhdr;

            int i = 0;
            Event_fortest::raise(i);

            TEST_CASE("1","ハンドラが実行される",TestSuite::isReached("EventHandler::handler"));
            TEST_CASE("2","引数間で見ている変数は同一", &i == evhdr.ptr_i);
            TEST_CASE("3","値がインクリメントされている", i == 1);
        } END_CASE;

        BEGIN_CASE("3","両方同時 (順番は特に気にしない)") {
            Singleton<Event_fortest::handler_list_type>::renewInstance();
            TestSuite::clearCheckpoints();
            EventHandler evhdr;

            Event_fortest::add(dummy_handler);

            int i = 0;
            Event_fortest::raise(i);

            TEST_CASE("1","ハンドラが実行される",TestSuite::isReached("dummy_handler"));
            TEST_CASE("2","ハンドラが実行される",TestSuite::isReached("EventHandler::handler"));
            TEST_CASE("3","値がインクリメントされている", i == 2);
        } END_CASE;

        BEGIN_CASE("4","複数登録") {
            Singleton<Event_fortest::handler_list_type>::renewInstance();
            TestSuite::clearCheckpoints();
            EventHandler evhdr;
            EventHandler evhdr2;

            Event_fortest::add(dummy_handler);
            Event_fortest::add(dummy_handler);

            int i = 0;
            Event_fortest::raise(i);

            TEST_CASE("1","ハンドラが実行される",TestSuite::isReached("dummy_handler"));
            TEST_CASE("2","ハンドラが実行される",TestSuite::isReached("EventHandler::handler"));
            TEST_CASE("3","値がインクリメントされている", i == 4);
        } END_CASE;
    } END_CASE;
}


#endif



