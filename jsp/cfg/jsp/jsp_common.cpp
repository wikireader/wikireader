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
 *  @(#) $Id: jsp_common.cpp,v 1.4 2003/12/20 06:51:58 takayuki Exp $
 */

#include "jsp/jsp_common.h"

#include "base/option.h"

#include <ctime>

#if defined(_MSC_VER) || defined(__BORLANDC__)
#  define  GETLOGINNAME_WINDOWS
#  include <windows.h>
#elif __GNUC__
#  define  GETLOGINNAME_UNIX
#  include <unistd.h>
#endif

using namespace std;

#if 0
namespace {
    string get_loginname(void) throw()
    {
        string result("TOPPERS/JSP KernelConfigurator");

#ifdef GETLOGINNAME_WINDOWS
        char  buffer[1024];
        DWORD sz = 1024;
        ::GetUserName(buffer, &sz);
        result.assign(buffer);
#elif defined(GETLOGINNAME_UNIX)
        result.assign(getlogin());
#endif

        return result;
    }
}
#endif


    //インクルードファイルの形を変更する
string ToppersJsp::conv_includefile(string file) throw()
{
    OptionParameter & option = getOptionParameter();
    string result;

        //ilオプションが付いたときには "<...>" 形式で出力
    if(option["il"].isValid())
        result = string("<") + file + ">";
    else
        result = string("\"") + file + "\"";

    return result;
}

    //カーネルの変数名の形を変更する
string ToppersJsp::conv_kernelobject(string obj) throw()
{
    OptionParameter & option = getOptionParameter();
    string result;

    result = obj;

        // TOPPERS/JSP rel 1.3より後はカーネル変数の頭に _kernel_ をつける
    if(!option["1.3"].isValid())
        result = string("_kernel_") + obj;

    return result;
}

ToppersJsp::SpecialtyFile::SpecialtyFile(void) throw() : MultipartStream(), RuntimeObject(true)
{}

#if 0
string ToppersJsp::get_agreement(string filename) throw()
{
    const char * base_agreement =
        "/*\n"
        " *  TOPPERS/JSP Kernel\n"
        " *      Toyohashi Open Platform for Embedded Real-Time Systems/\n"
        " *      Just Standard Profile Kernel\n"
        " * \n"
        " *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory\n"
        " *                              Toyohashi Univ. of Technology, JAPAN\n"
        " * \n"
        " *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation \n"
        " *  によって公表されている GNU General Public License の Version 2 に記\n"
        " *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア\n"
        " *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，\n"
        " *  利用と呼ぶ）することを無償で許諾する．\n"
        " *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作\n"
        " *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー\n"
        " *      スコード中に含まれていること．\n"
        " *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使\n"
        " *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用\n"
        " *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記\n"
        " *      の無保証規定を掲載すること．\n"
        " *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使\n"
        " *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ\n"
        " *      と．\n"
        " *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著\n"
        " *        作権表示，この利用条件および下記の無保証規定を掲載すること．\n"
        " *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに\n"
        " *        報告すること．\n"
        " *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損\n"
        " *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．\n"
        " * \n"
        " *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お\n"
        " *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も\n"
        " *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直\n"
        " *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．\n"
        " * \n";

    string work(base_agreement);

        //CVSのID風のものをくっつける
    time_t t;
    time(&t);

    work += string(" * @(#)$Id: ");
    work += filename;
    work += " ";
    work += static_cast<const char *>(asctime(localtime(&t)));
    
    work.erase(work.size() - 1);    //asctimeで改行が入るのでchomp

    work += " ";
    work += get_loginname();
    work += " Exp $\n */\n\n";

    return work;
}
#endif


/***************************************** テストスィート *****************************************/

#ifdef TESTSUITE

class SpecialtyFile_test : public ToppersJsp::SpecialtyFile
{
public:
    ~SpecialtyFile_test(void) throw() { TestSuite::check("SpecialtyFile::~SpecialtyFile"); }
    TESTSUITE_PROTOTYPE(main)
};

TESTSUITE(main, SpecialtyFile_test)
{
    BEGIN_CASE("constructor","constructor") {
        BEGIN_CASE("1","生成するとROTに乗る") {
            SpecialtyFile_test file;

            if(RuntimeObjectTable::getInstance<SpecialtyFile_test>() == 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","破棄の対象になっている") {
            Singleton<RuntimeObjectTable>::Context context;
            Singleton<RuntimeObjectTable>::saveContext(context);

            Singleton<RuntimeObjectTable>::renewInstance();

            new SpecialtyFile_test;

            TEST_CASE("0","[前提] 登録されている", RuntimeObjectTable::getInstance<SpecialtyFile_test>() != 0);
            
            TestSuite::clearCheckpoints();
            Singleton<RuntimeObjectTable>::renewInstance(); //インスタンスの再生成でROTが削除される

            TEST_CASE("1","ROTから解除されている", RuntimeObjectTable::getInstance<SpecialtyFile_test>() == 0);
            TEST_CASE("2","デストラクタが起動されている", TestSuite::isReached("SpecialtyFile::~SpecialtyFile"));

            Singleton<RuntimeObjectTable>::restoreContext(context);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("createInstance","createInstance") {
        BEGIN_CASE("1","インスタンスが生成できる") {
            SpecialtyFile_test * file = SpecialtyFile::createInstance<SpecialtyFile_test>("test.txt");

            file->disableOutput();  //出力されても困るので

            TEST_CASE("1","非NULLが返る", file != 0);
            TEST_CASE("2","ファイル名が正常",  file->getFilename().compare("test.txt") == 0);
        } END_CASE;

        BEGIN_CASE("2","空文字を渡すと例外") {

            bool result = false;
            try {
                SpecialtyFile::createInstance<SpecialtyFile_test>("");
            }
            catch(Exception &) {
                result = true;
            }

            TEST_CASE("1","例外が起こる", result);
            TEST_CASE("2","オブジェクトはない", RuntimeObjectTable::getInstance<SpecialtyFile_test>() == 0);
        } END_CASE;
    } END_CASE;
}

namespace {
    extern void testsuite(TestSuiteControl & _suite_control);
    TestSuite testsuite_onTop("TOPPERS/JSP Common Function(main)", testsuite);

            //テストスィートの本体
    void testsuite(TestSuiteControl & _suite_control)
    {
        Singleton<OptionParameter>::Context context;
        Singleton<OptionParameter>::saveContext(context);

        BEGIN_CASE("conv_includefile","conv_includefile") {
            BEGIN_CASE("1","-ilが付いていないとダブルクォートでくくる") {
                Singleton<OptionParameter>::renewInstance();

                if(ToppersJsp::conv_includefile("test.h").compare("\"test.h\"") != 0)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("1","-ilが付いていると<>でくくる") {
                Singleton<OptionParameter>::renewInstance();
                char * argv[] = {"test.exe","-il"};
                getOptionParameter().parseOption(2, argv);

                if(ToppersJsp::conv_includefile("test.h").compare("<test.h>") != 0)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("conv_kernelobject","conv_kernelobject") {
            BEGIN_CASE("1","-1.3が付いていないと_kernel_が付く") {
                Singleton<OptionParameter>::renewInstance();

                if(ToppersJsp::conv_kernelobject("test").compare("_kernel_test") != 0)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("1","-1.3が付いているとそのまま出てくる") {
                Singleton<OptionParameter>::renewInstance();
                char * argv[] = {"test.exe","-1.3"};
                getOptionParameter().parseOption(2, argv);

                if(ToppersJsp::conv_kernelobject("test").compare("test") != 0)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        Singleton<OptionParameter>::restoreContext(context);

    }
}
#endif


