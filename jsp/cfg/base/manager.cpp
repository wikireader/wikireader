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
 *  @(#) $Id: manager.cpp,v 1.11 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/manager.cpp,v 1.11 2003/12/15 07:32:13 takayuki Exp $

#ifdef _MSC_VER
#  pragma warning(disable:4786)
#  define CDECL __cdecl
#else
#  define CDECL
#endif

#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "base/event.h"
#include "base/message.h"
#include "base/component.h"
#include "base/directorymap.h"

using namespace std;

Event<StartupEvent>   startupEvent;
Event<ShutdownEvent>  shutdownEvent;

int CDECL main(int argc, char ** argv)
{
    ShutdownEvent evt;
    OptionParameter & option= getOptionParameter();

    evt.exit_code = EXIT_FAILURE;

    if(SingletonBase::isValid() && Component::isValid()) {

        option.parseOption(argc, argv, "-h");

        try {
            startupEvent.raise();
            Component::executeComponents(option);
            evt.exit_code = EXIT_SUCCESS;
        }
        catch(Exception & e)
        {
            cerr << Message("Program failed in its process by following reason.\n","プログラムは致命的なエラーにより中断されました.\n");
            cerr << "  " << e.getDetails() << '\n';
        }
        catch(ExceptionMessage & e) {
            CHECKPOINT("catch(ExceptionMessage)");
            cerr << "Program failed : " << e << '\n';
        }
        catch(bad_alloc & e) {
            CHECKPOINT("catch(bad_alloc)");
            cerr << "Program failed : bad_alloc (" << e.what() << ")\n";
        }
        catch(...) {
            CHECKPOINT("catch(...)");
            cerr << "Program filed : Unknown Exception Occured\n";
        }
    }
    else
        cerr << "Program initialization failure\n";

    try {
        DebugMessage("shutdown event - exitcode(%)\n") << evt.exit_code;
        shutdownEvent.raise(evt);
    }
    catch(Exception & e)
    {
        DebugMessage("exception was raised in the event - %\n") << e.getDetails();
        cerr << Message("Program failed in its process by following reason.\n","プログラムは致命的なエラーにより中断されました.\n");
        cerr << "  " << e.getDetails() << '\n';
    }

    catch(...) {
        DebugMessage("exception was raised in the event\n");
        CHECKPOINT("catch(...) in shutdownEvent::raise");
        evt.exit_code = EXIT_FAILURE;
    }

    DebugMessage("Exit code (%)\n") << evt.exit_code;

    return evt.exit_code;
}

/************************************* テストスィート *************************************/

#ifdef TESTSUITE
#include "coverage_undefs.h"

namespace {

    extern void testsuite(TestSuiteControl & _suite_control);
    TestSuite testsuite_onTop("Entrypoint(main)", testsuite);

    class TestComponent : public Component
    {
    public:
        int mode;

        void parseOption(OptionParameter &)
        {
            switch(mode) {
                case 0:
                    CHECKPOINT("TestComponent::case 0");
                    break;
                case 1:
                    ExceptionMessage("exception").throwException();
                case 2:
                    throw bad_alloc();
            }
        }
    };

    void dummyStartupHandler(StartupEvent &)
    {
        TestSuite::check("dummyStartupHandler");
    }

    int dummyShutdownHandler_exit_code;
    void dummyShutdownHandler(ShutdownEvent & evt)
    {
        dummyShutdownHandler_exit_code = evt.exit_code;
        TestSuite::check("dummyShutdownHandler");
    }

        //テストスィートの本体
    void testsuite(TestSuiteControl & _suite_control)
    {
        SingletonBase::ContextChain chain;

        chain.saveContext<Component::ComponentInfo>();
        chain.saveContext<Event<StartupEvent>::handler_list_type>();
        chain.saveContext<Event<ShutdownEvent>::handler_list_type>();

        BEGIN_CASE("1","main関数単体 + コンポーネントなし") {
            chain.renewInstance();

            Event<StartupEvent>::add(dummyStartupHandler);
            Event<ShutdownEvent>::add(dummyShutdownHandler);

            BEGIN_CASE("1","普通に実行する") {
                TestSuite::clearCheckpoints();

                char * argv[] = { "test.exe" };

                Exception::setThrowControl(true);
                TestComponent test;
                test.mode = 0;

                TEST_CASE("1","main関数はEXIT_SUCCESSを返す", main(1, argv) == EXIT_SUCCESS);

                TEST_CASE("2","例外が実行されない catch(Exception)", !TestSuite::isReached("catch(Exception)"));
                TEST_CASE("3","例外が実行されない catch(bad_alloc)", !TestSuite::isReached("catch(bad_alloc)"));
                TEST_CASE("4","例外が実行されない catch(...)",       !TestSuite::isReached("catch(...)"));
                TEST_CASE("5","Componentが実行されている", TestSuite::isReached("TestComponent::case 0"));
                TEST_CASE("6","スタートアップハンドラが起動されている", TestSuite::isReached("dummyStartupHandler"));
                TEST_CASE("7","シャットダウンハンドラが起動されている", TestSuite::isReached("dummyShutdownHandler"));
                TEST_CASE("8","シャットダウンハンドラはEXIT_SUCCESSを受けている", dummyShutdownHandler_exit_code == EXIT_SUCCESS);
            } END_CASE;

            BEGIN_CASE("2","Exception例外を返す") {
                TestSuite::clearCheckpoints();

                char * argv[] = { "test.exe" };

                Exception::setThrowControl(true);
                TestComponent test;
                test.mode = 1;

                TEST_CASE("1","main関数はEXIT_FAILUREを返す", main(1, argv) == EXIT_FAILURE);

                TEST_CASE("2","例外が実行される   catch(Exception)", TestSuite::isReached("catch(Exception)"));
                TEST_CASE("3","例外が実行されない catch(bad_alloc)", !TestSuite::isReached("catch(bad_alloc)"));
                TEST_CASE("4","例外が実行されない catch(...)",       !TestSuite::isReached("catch(...)"));
                TEST_CASE("5","スタートアップハンドラが起動されている", TestSuite::isReached("dummyStartupHandler"));
                TEST_CASE("6","シャットダウンハンドラが起動されている", TestSuite::isReached("dummyShutdownHandler"));
                TEST_CASE("7","シャットダウンハンドラはEXIT_FAILUREを受けている", dummyShutdownHandler_exit_code == EXIT_FAILURE);
            } END_CASE;

            BEGIN_CASE("3","bad_alloc例外を返す") {
                TestSuite::clearCheckpoints();

                char * argv[] = { "test.exe" };

                Exception::setThrowControl(true);
                TestComponent test;
                test.mode = 2;

                TEST_CASE("1","main関数はEXIT_FAILUREを返す", main(1, argv) == EXIT_FAILURE);

                TEST_CASE("2","対応するcatchのみが実行されている", !TestSuite::isReached("catch(Exception)") && TestSuite::isReached("catch(bad_alloc)") && !TestSuite::isReached("catch(...)"));
                TEST_CASE("3","スタートアップハンドラが起動されている", TestSuite::isReached("dummyStartupHandler"));
                TEST_CASE("4","シャットダウンハンドラが起動されている", TestSuite::isReached("dummyShutdownHandler"));
                TEST_CASE("5","シャットダウンハンドラはEXIT_FAILUREを受けている", dummyShutdownHandler_exit_code == EXIT_FAILURE);
            } END_CASE;
        } END_CASE;

        chain.restoreContext();
    }

}

#endif


