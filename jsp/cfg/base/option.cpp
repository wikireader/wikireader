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
 *  @(#) $Id: option.cpp,v 1.3 2003/12/15 07:32:14 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/option.cpp,v 1.3 2003/12/15 07:32:14 takayuki Exp $

#include "base/option.h"
#include <assert.h>
#include <algorithm>

using namespace std;

    //別のOptionItemが持つパラメータを自分に追加する
void OptionParameter::OptionItem::mergeItem(const OptionItem & src)
{
    vector<string>::const_iterator scope;

        //全てのパラメータを自分の後ろに追加
    scope = src.param.begin();
    while(scope != src.param.end()) {
        param.push_back(*scope);
        ++ scope;
    }
}

    //文字列の先頭にある空白文字を取り除く
namespace {
    void TrimString(string & src)
    {
        string::size_type pos;

        pos = src.find_first_not_of(" \t\r\n");
        if(pos != 0)
            src.erase(0, pos);
    }
}

    //オプションパラメータを解析してOptionItemを生成する
bool OptionParameter::OptionItem::createItem(OptionItem & _dest, string & argv)
{
    bool              result;
    string            param;
    string::size_type pos;
    OptionItem        dest;

    result = true;

    TrimString(argv);
    if(argv.empty())
        result = false;     //中身が空っぽ
    else {
            //オプションのチェック
        if(argv[0] == '-') {
            pos = argv.find_first_of(" =");

            if(argv.substr(0,pos).compare("-") == 0) {  // - だけのオプションは受け付けない
                result = false;
            }
            else {
                if(pos != 1) {
                    if(pos != string::npos) {
                        dest = argv.substr(1, pos - 1);
                        argv.erase(0, pos + 1);
                    }
                    else {
                        dest = argv.substr(1);
                        argv.erase();
                    }
                }
            }
        }

        TrimString(argv);
        while(!argv.empty() && argv[0] != '-') {
            if(argv[0] == '"') {

                    //対になるダブルクォートを探す
                pos = argv.find_first_of('"',1);
                if(pos == string::npos) {
                    result = false;
                    break;
                }

                    //ダブルクォートの中身を取る
                param = argv.substr(1, pos - 1);

                    //ダブルクォートを消す
                ++ pos;
            }
            else {

                    //次の区切りを探す
                pos = argv.find_first_of(" \t\r\n");
                if(pos == string::npos)
                    param = argv;
                else
                    param = argv.substr(0,pos);
            }

            dest.addParameter(param);
            argv.erase(0, pos);
            
            TrimString(argv);
        }
    }

    if(result)
        _dest = dest;

    return result;
}

    //無効なオプションアイテムの生成
OptionParameter::OptionItem OptionParameter::OptionItem::createInvalidItem(void) throw()
{
    OptionItem result;

    result.assign("");
    return result;
}


    //全てのパラメータ結合したものを取得
string OptionParameter::OptionItem::getParameters(string punctuator) const throw()
{
    string result;

    vector<string>::const_iterator scope;

    if(isValid()) {
        scope = param.begin();
        while(scope != param.end()) {
            result += punctuator;
            result += *scope;
            ++ scope;
        }

        if(!result.empty())
            result.erase(0, punctuator.size()); //先頭の区切り子を削除
    }

    return result;
}

    //オプションの個数のチェック
bool OptionParameter::OptionItem::checkParameterCount(size_t required, bool allow_zero) const throw()
{
    bool result = true;
    if(isValid()) {
        size_t count = countParameter();
        if(count != required || (!allow_zero && count == 0))
            result = false;
    }
    else
        result = false;
    return result;
}

        //オプションパラメータのパース
bool OptionParameter::parseOption(int _argc, char const * const * _argv, char const * _default) throw()
{
    bool        result;
    string      arguments;
    OptionItem  work;
    int         i;

    map<string, OptionItem>::iterator scope;

        //プログラム名の取得
    if(_argc > 0)
        program_name.assign(_argv[0]);

        //全てつなげる
    for(i=1;i<_argc;++i) {
        arguments += _argv[i];
        arguments += ' ';
    }

    if(arguments.empty() && _default != 0)
        arguments.assign(_default);

    cmdline += arguments;

    result = true;

        //各オプションの解析
    while(!arguments.empty()) {

        if(!OptionItem::createItem(work, arguments)) {
            result = false;
            break;
        }

            //同一のオプションがいたらマージする
        scope = container.find(work);
        if(scope != container.end())
            scope->second.mergeItem(work);
        else
            container.insert(std::map<std::string, OptionItem>::value_type(work, work));
    }

    return result;
}

    //要素の検出
map<string, OptionParameter::OptionItem>::iterator OptionParameter::_find(const std::string & name, bool check) throw()
{
    map<string, OptionParameter::OptionItem>::iterator scope;

    scope = container.find(name);

        //要素を使用したことを記録しておく
    if(check && scope != container.end())
        scope->second._check();

    return scope;
}


    //オプションアイテムの参照
OptionParameter::OptionItem OptionParameter::get(const std::string & name, bool check) throw()
{
    OptionItem                        result;
    map<string, OptionItem>::iterator scope;

    scope = _find(name, check);
    if(scope != container.end())
        result = scope->second;
    else
        result = OptionItem::createInvalidItem();

    return result;
}


    //全てのオプションが使用されたかどうかの確認
bool OptionParameter::validateOption(void) const throw()
{
    map<string, OptionItem>::const_iterator scope;
    bool result = true;

        //全てのオプションがチェック済みであることを確認
    scope = container.begin();
    while(scope != container.end()) {
        if(!scope->second.isChecked()) {
            result = false;
            break;
        }
        ++ scope;
    }

    return result;
}


    //使用されなかったオプション列を取得
string OptionParameter::getInvalidOptions(void) const throw()
{
    map<string, OptionItem>::const_iterator scope;
    string result;

        //使用されなかった全てのオプションに対して
    scope = container.begin();
    while(scope != container.end()) {
        const OptionItem & item = scope->second;
        if(!item.isChecked()) {
            result += " -";
            result += item;

                //オプションパラメータの列挙
            size_t params = item.countParameter();
            for(size_t i=0; i<params; ++i) {
                result += " \"";
                result += item[i];
                result += '\"';
            }
        }
        ++ scope;
    }

        //先頭の空白を取り除く
    if(!result.empty())
        result = result.substr(1);

    return result;
}

/***************** テストスィート *****************/

#ifdef TESTSUITE
#include "coverage_undefs.h"


TESTSUITE_(main,OptionItem,OptionParameter)
{
    BEGIN_CASE("01","デフォルトパラメータが" DEFAULT_PARAMETER "になっている") {
        OptionItem work;
        if(work.compare(DEFAULT_PARAMETER) != 0)
            TEST_FAIL;
    } END_CASE;

    //----

    BEGIN_CASE("02","コンストラクタチェック (checked == false, パラメータ名)") {

        BEGIN_CASE("1","OptionItem(void)") {
            OptionItem work;
            if(work.checked != false)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","OptionItem(const string &)") {
            OptionItem work("test");
            if(work.compare("test") != 0 || work.checked != false)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","OptionItem(const OptionItem &)") {
            OptionItem dummy("test");
            OptionItem work(dummy);
            if(work.compare("test") != 0 || work.checked != false)
                TEST_FAIL;
        } END_CASE;

    } END_CASE;

    //----

    BEGIN_CASE("03", "addParameter / countParameter / operator []") {
        const char * container[] = { "first", "second", "third", "forth", "fifth", NULL };
        const char ** param = container;
        int i;

        OptionItem work;

        BEGIN_CASE("1", "作った直後のcountParameterは0") {
            if(work.countParameter() != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "addParameterするたびに1ずつ増えるはず") {
            while(*param != NULL) {
                size_t count = work.countParameter();

                work.addParameter(*param);

                if(work.countParameter() != count + 1)
                    TEST_FAIL;

                ++ param;
            }
        } END_CASE;

        BEGIN_CASE("3", "入れたものはちゃんと入ってる") {
            i = 0;
            param = container;
            while(*param != NULL) {
                if(work[i].compare(*param) != 0)
                    TEST_FAIL;

                ++ i;
                ++ param;
            }
        } END_CASE;

        BEGIN_CASE("4", "operator[]に範囲外のパラメータを入れると空文字が返る") {
            if(!work[10000].empty())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    //----

    BEGIN_CASE("04", "hasParameter") {
        OptionItem work;

        BEGIN_CASE("1", "中身がないときはfalseが返る") {
            if(work.hasParameter())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "中身があるときはtrueが返る") {
            work.addParameter("dummy");
            if(!work.hasParameter())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;
        
    //----

    BEGIN_CASE("mergeItem", "mergeItem") {
        OptionItem work;
        OptionItem work2;

        work.addParameter("one");
        work2.addParameter("two");
        
        work.mergeItem(work2);

        BEGIN_CASE("1","マージされている") {
            if(work[0].compare("one") != 0 || work[1].compare("two") != 0 || work.countParameter() != 2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","変更元は変化なし") {
            if(work2[0].compare("two") != 0 || work2.countParameter() != 1)
                TEST_FAIL;
        } END_CASE;

    } END_CASE;
        
    //----

    BEGIN_CASE("isChecked/checked", "isChecked/check") {
        OptionItem work;

        BEGIN_CASE("1","最初はunchecked状態") {
            if(work.isChecked() != false)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","checkをつける") {
            work._check();
            if(work.isChecked() != true)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","もう一回やってもトグルにならない") {
            work._check();
            if(work.isChecked() != true)
                TEST_FAIL;
        } END_CASE;

    } END_CASE;

    //----

    BEGIN_CASE("CopyConstructor", "コピーコンストラクタでパラメータが移るか") {
        OptionItem src;
        src.addParameter("one");

        OptionItem dest(src);

        if(dest.countParameter() != 1 || dest[0].compare("one") != 0)
            TEST_FAIL;
    } END_CASE;

    //----

    BEGIN_CASE("TrimString", "TrinStringがちゃんと動くか") {
        BEGIN_CASE("1","先頭の空白文字が消える") {
            string work(" \r\n\ttest");
            TrimString(work);
            if(work.compare("test") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","空白文字だけから成る文字列") {
            string work("     ");
            TrimString(work);
            if(!work.empty())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    //----

    BEGIN_CASE("createItem", "createItem関数の動作検証") {
        BEGIN_CASE("1", "空っぽの文字列をいれると失敗する") {
            OptionItem work;
            string param("");
            if(OptionItem::createItem(work, param))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "引数なしオプション") {
            OptionItem work;
            string     arg("-test");
            if(!OptionItem::createItem(work, arg))
                TEST_FAIL;
            if(work.compare("test") != 0 || work.hasParameter())
                TEST_FAIL;
            if(!arg.empty())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3", "連続する引数なしオプション") {
            OptionItem work;
            string     arg("-test -test2");
            if(!OptionItem::createItem(work, arg))
                TEST_FAIL;
            if(arg.compare("-test2") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4", "引数つきオプション(-test param)") {
            OptionItem work;
            string     arg("-test param");
            if(!OptionItem::createItem(work, arg))
                TEST_FAIL;
            if(work.compare("test") != 0)
                TEST_FAIL;
            if(work.countParameter() != 1 || work[0].compare("param") != 0)
                TEST_FAIL;
            if(!arg.empty())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("5", "引数つきオプション(-test=param)") {
            OptionItem work;
            string     arg("-test=param");
            if(!OptionItem::createItem(work, arg))
                TEST_FAIL;
            if(work.compare("test") != 0)
                TEST_FAIL;
            if(work.countParameter() != 1 || work[0].compare("param") != 0)
                TEST_FAIL;
            if(!arg.empty())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("6", "引数つきオプション(-test=\"a b c\")") {
            OptionItem work;
            string     arg("-test=\"a b c\"");
            if(!OptionItem::createItem(work, arg))
                TEST_FAIL;
            if(work.compare("test") != 0)
                TEST_FAIL;
            if(work.countParameter() != 1 || work[0].compare("a b c") != 0)
                TEST_FAIL;
            if(!arg.empty())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("7", "連続した引数つきオプション(-test one two)") {
            OptionItem work;
            string     arg("-test one two");
            if(!OptionItem::createItem(work, arg))
                TEST_FAIL;
            if(work.compare("test") != 0)
                TEST_FAIL;
            if(work.countParameter() != 2 || work[0].compare("one") != 0 || work[1].compare("two") != 0)
                TEST_FAIL;
            if(!arg.empty())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("8", "ハイフン") {
            BEGIN_CASE("1", "ハイフンだけの引数は受け付けない") {
                OptionItem work;
                string     arg("-");
                if(OptionItem::createItem(work, arg))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2", "ハイフンだけの引数は受け付けない(連続)") {
                OptionItem work;
                string     arg("- - -");
                if(OptionItem::createItem(work, arg))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("3", "ダブルクォートでくくられたハイフンは受け付ける") {
                OptionItem work;
                string     arg("\"-\"");
                if(!OptionItem::createItem(work, arg))
                    TEST_FAIL;
                if(work.compare(DEFAULT_PARAMETER) != 0 || work.countParameter() != 1 || work[0].compare("-") != 0)
                    TEST_FAIL;
            } END_CASE;
    
        } END_CASE;

        BEGIN_CASE("9", "対になるダブルクォートが欠けた引数は受け付けない") {
            OptionItem work;
            string     arg("-test=\"a b c");
            if(OptionItem::createItem(work, arg))
                TEST_FAIL;
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("isValid/createInvalidItem", "無効を示すインスタンスを生成でき、それを判定できる") {
        OptionItem work;

        BEGIN_CASE("1","デフォルトコンストラクタで生成したオブジェクトは有効") {
            if(!work.isValid())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","無効インスタンスは当然無効") {
            work = OptionItem::createInvalidItem();
            if(work.isValid())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("getParameters","getParameters") {
        BEGIN_CASE("1","正しくオプションを結合できる") {
            OptionItem work;
            string     arg("-test a b c d e f g");

            TEST_CASE("0","[前提]オプション解析に成功する", OptionItem::createItem(work,arg));
            TEST_CASE("1","結合できる", work.getParameters().compare("a b c d e f g") == 0);
            TEST_CASE("2","区切り氏を指定して結合できる", work.getParameters(",").compare("a,b,c,d,e,f,g") == 0);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("checkParameterCount","checkParameterCount") {
        BEGIN_CASE("1","オプションの数があっていれはtrue") {
            OptionItem work;
            string     arg("-test a b c d e f g");

            TEST_CASE("0","[前提]オプション解析に成功する", OptionItem::createItem(work,arg));

            if(!work.checkParameterCount(7))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","オプションの数が間違っているとfalse") {
            OptionItem work;
            string     arg("-test a b c d e f g");

            TEST_CASE("0","[前提]オプション解析に成功する", OptionItem::createItem(work,arg));

            if(work.checkParameterCount(2))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","オプションがなくて、!allow_zeroならfalse") {
            OptionItem work;
            string     arg("-test");

            TEST_CASE("0","[前提]オプション解析に成功する", OptionItem::createItem(work,arg));

            if(work.checkParameterCount(1,false))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","オプションがなくて、allow_zeroならtrue") {
            OptionItem work;
            string     arg("-test");

            TEST_CASE("0","[前提]オプション解析に成功する", OptionItem::createItem(work,arg));

            if(work.checkParameterCount(1,true))
                TEST_FAIL;
        } END_CASE;

    } END_CASE;
}

/********************************* テストスィート *********************************/

TESTSUITE(main,OptionParameter)
{
    SINGLETON(OptionParameter);

    BEGIN_CASE("countItem","オプションアイテムの数が正しく取得できる") {
        OptionParameter param;
        if(param.countItem() != 0)
            TEST_FAIL;

        param.container.insert(pair<string, OptionItem>(DEFAULT_PARAMETER, OptionItem()));
        if(param.countItem() != 1)
            TEST_FAIL;
    } END_CASE;

    BEGIN_CASE("opeator []","operator []で要素にアクセスできる") {
        OptionParameter param;
        OptionItem work;
        string arg("-test param");

        OptionItem::createItem(work, arg);
        param.container.insert(pair<string, OptionItem>(work, work));

        BEGIN_CASE("1", "operator [] (const string &)で総祖にアクセスできる") {
            if(param[string("test")][0].compare("param") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "operator [] (const char *)で総祖にアクセスできる") {
            if(param["test"][0].compare("param") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3", "operator []で触ると、checkが入る") {
            if(!param["test"].isChecked())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4", "operator []に無効なキーを入れると、無効なデータが返ってくる") {
            if(param["nonexist"].isValid())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("get","要素へのアクセス") {
        OptionParameter param;
        OptionItem work;

        OptionItem::createItem(work, string("-test param"));
        param.container.insert(pair<string, OptionItem>(work, work));
        OptionItem::createItem(work, string("-test2 param2"));
        param.container.insert(pair<string, OptionItem>(work, work));

        BEGIN_CASE("1","get(test)で要素が取得でき、チェックが入る") {
            TEST_CASE("1","要素が存在する", (work = param.get("test")).isValid());
            TEST_CASE("2","チェックが入っている", work.isChecked() );
            TEST_CASE("3","要素はパラメータを持つ", work.hasParameter() );
        } END_CASE;

        BEGIN_CASE("2","get(test2,false)で要素が取得でき、チェックはつかない") {
            TEST_CASE("1","要素が存在する", (work = param.get("test2",false)).isValid());
            TEST_CASE("2","チェックが入っていない", !work.isChecked() );
        } END_CASE;

        BEGIN_CASE("3","すでにチェックのついたtestに対し、get(test,false)で要素が取得でき、チェック済み") {
            TEST_CASE("1","要素が存在する", (work = param.get("test",false)).isValid());
            TEST_CASE("2","チェックが入っていない", work.isChecked() );
        } END_CASE;

        BEGIN_CASE("4","get(無効な名前)で無効な要素が返る") {
            TEST_CASE("1","要素が存在する", !param.get("unknown").isValid());
        } END_CASE;
    } END_CASE;


    BEGIN_CASE("parseOption","パラメータのパースが正常にできるか") {
        BEGIN_CASE("01","中身が何もないパラメータを与えても成功する") {
            OptionParameter param;
            char ** argv = { NULL };

            if(!param.parseOption(0, argv))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("02","プログラム名はオプションとして解析されない") {
            OptionParameter param;
            char * argv[] = { "test.exe", NULL };

            if(!param.parseOption(1, argv))
                TEST_FAIL;
            if(param.countItem() != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("03","パラメータがそれなりに解析される") {
            OptionParameter param;
            char * argv[] = { "test.exe", "-test", "param", "-test2=\"dummy\"", NULL };

            if(!param.parseOption(4, argv))
                TEST_FAIL;
            if(param.countItem() != 2)
                TEST_FAIL;
            
            if(param[string("test")][0].compare("param") != 0)
                TEST_FAIL;

            if(param["test2"][0].compare("dummy") != 0)
                TEST_FAIL;

            if(param.getProgramName().compare("test.exe") != 0)
                TEST_FAIL;

        } END_CASE;

        BEGIN_CASE("04","引数列が取れる") {
            OptionParameter param;
            char * argv[] = { "test.exe", "-test", "param", "-test2=\"dummy\"", NULL };

            if(!param.parseOption(4, argv))
                TEST_FAIL;

            if(param.getCommandLine().compare("-test param -test2=\"dummy\" ") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("05","何も引数が無いときにはデフォルト引数をひいてくる") {
            OptionParameter param;
            char * argv[] = { "test.exe", NULL };

            if(!param.parseOption(1, argv, "-default=parameter"))
                TEST_FAIL;

            if(param.getCommandLine().compare("-default=parameter ") != 0)
                TEST_FAIL;
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("04","参照マークをつける") {
        OptionParameter param;
        char * argv[] = { "test.exe", "-test", "param", "-test2=\"dummy\"", NULL };

        if(!param.parseOption(4, argv))
            TEST_FAIL;

        if(param.container.find(string("test"))->second.isChecked())
            TEST_FAIL;

        param.check("test");
        if(!param.container.find(string("test"))->second.isChecked())
            TEST_FAIL;
    } END_CASE;

    BEGIN_CASE("05","validateOption") {
        OptionParameter param;
        char * argv[] = { "test.exe", "-test", "param", "-test2=\"dummy\"", NULL };

        if(!param.parseOption(4, argv))
            TEST_FAIL;

        BEGIN_CASE("1","何も使用していないときにはfalse") {
            if(param.validateOption())
                TEST_FAIL;
        } END_CASE;

        param["test"]._check();
        BEGIN_CASE("2","ほかにチェックしていないオプションがあればfalse") {
            if(param.validateOption())
                TEST_FAIL;
        } END_CASE;

        param["test2"]._check();
        BEGIN_CASE("3","全て使用されればtrue") {
            if(!param.validateOption())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("06","getInvalidOptions") {
        OptionParameter param;
        char * argv[] = { "test.exe", "-test", "param", "-test2=\"dummy\"", NULL };

        if(!param.parseOption(4, argv))
            TEST_FAIL;

        BEGIN_CASE("1","何も使用していないときには全てのオプションが無効") {
            if(param.getInvalidOptions().compare("-test \"param\" -test2 \"dummy\"") != 0)
                TEST_FAIL;
        } END_CASE;

        param["test"]._check();
        BEGIN_CASE("2","ほかにチェックしていないオプションがあればそれが取れる") {
            if(param.getInvalidOptions().compare("-test2 \"dummy\"") != 0)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("07","find") {
        OptionParameter param;
        char * argv[] = { "test.exe", "-test", "param", "-test2=\"dummy\"", NULL };

        if(!param.parseOption(4, argv))
            TEST_FAIL;

        BEGIN_CASE("1","存在するオプションに対してtrueが返る") {
            if(!param.find("test") || !param.find("test2"))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","存在しないオプションに対してfalseが返る") {
            if(param.find("unknown"))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","空文字に対してfalseが返る") {
            if(param.find(""))
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    Singleton<OptionParameter>::restoreContext(context);
}

#endif


