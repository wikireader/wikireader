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
 *  @(#) $Id: component.cpp,v 1.13 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/component.cpp,v 1.13 2003/12/15 07:32:13 takayuki Exp $

#include "base/component.h"

#include <iostream>

using namespace std;

    //コンポーネント全体が使用できるならtrue
bool Component::is_valid = true;

    //コンポーネントの登録
bool Component::ComponentInfo::addComponent(Component * component, int order) throw()
{
    bool result = false;

    if(this != 0 && component != 0) {
        componentList.insert(multimap<int, Component *>::value_type(order, component));
        result = true;
    }

    return result;
}

    //コンポーネントの登録解除
void Component::ComponentInfo::removeComponent(Component * component, int order) throw()
{
    if(this != 0 && component != 0) {
        multimap<int, Component *>::iterator scope;

        scope = componentList.lower_bound(order);
        while(scope != componentList.end()) {
                //もう指定された優先度はすべて見た
            if(scope->first != order)
                break;
        
                //指定されたコンポーネントに一致したら削除
            if(scope->second == component) {
                multimap<int, Component *>::iterator target = scope;
                ++ scope;
                componentList.erase(target);
            }
            else
                ++ scope;
        }
    }
}

    //コンポーネントの登録解除
void Component::ComponentInfo::removeComponent(Component * component) throw()
{
    if(this != 0 && component != 0) {
        multimap<int, Component *>::iterator scope;

            //指定されたコンポーネントに一致するものを全て削除
        scope = componentList.begin();
        while(scope != componentList.end()) {
            if(scope->second == component) {
                multimap<int, Component *>::iterator target = scope;
                ++ scope;
                componentList.erase(target);
            }
            else
                ++ scope;
        }

    }
}

    //コンポーネントの起動要求の発行
bool Component::ComponentInfo::activateComponent(Component * component) throw()
{
    bool result = false;

    if(this != 0 && component != 0) {
            //起動要求の末尾に加える
        activatedComponentList.push_back(component);
        result = true;
    }

    return result;
}

    //オプションのパース
void Component::ComponentInfo::parseOption(OptionParameter & option)
{
    if(this != 0) {
        multimap<int, Component *>::iterator scope;

            //全てのコンポーネントのparseOptionを実行
        scope = componentList.begin();
        while(scope != componentList.end()) {
            DebugMessage("Component::parseOption start  - %\n") << typeid(*scope->second).name();
            scope->second->parseOption(option);
            DebugMessage("Component::parseOption finish - %\n") << typeid(*scope->second).name();
            ++ scope;
        }
    }
}

    //コンポーネントの起動
void Component::ComponentInfo::activateComponent(void)
{
    if(this != 0) {
        list<Component *>::iterator scope;

        try {
                //起動要求のあった全てのコンポーネントを起動
            scope = activatedComponentList.begin();
            while(scope != activatedComponentList.end()) {
                DebugMessage("Component::body start  - %\n") << typeid(**scope).name();
                (*scope)->body();
                DebugMessage("Component::body finish - %\n") << typeid(**scope).name();
                ++ scope;
            }
                //要求リストをクリア
            activatedComponentList.clear();
        }
        catch(...) {
                //onFatalExit用に、activatedComponentListを起動をかけたコンポーネントだけに絞る
            ++ scope;
            if(scope != activatedComponentList.end())
                activatedComponentList.erase(scope, activatedComponentList.end());
        
            throw;
        }
    }
}

    //異常処理ハンドラの起動
void Component::ComponentInfo::onFatalExit(void)
{
    if(this != 0) {
        Component * handler;

            //起動要求のあった全てのコンポーネントを起動
        while(!activatedComponentList.empty()) {
            handler = *activatedComponentList.begin();
            activatedComponentList.pop_front();

                //ハンドラを起動
            DebugMessage("Component::onFatalExit start  - %\n") << typeid(*handler).name();
            handler->onFatalExit();
            DebugMessage("Component::onFatalExit fihish - %\n") << typeid(*handler).name();
        }
            //要求リストをクリア
        activatedComponentList.clear();
    }
}


/*
 *   Component : コンポーネント
 */

    //コンストラクタ
Component::Component(int order) throw() : RuntimeObject()
{
        //ここで初めてシングルトンインスタンスにアクセスするので、bad_allocが発生する可能性がある
    try {
        Singleton<ComponentInfo>::getInstance()->addComponent(this, order);
    }
    catch(bad_alloc) {
        is_valid = false;
    }
}

    //デストラクタ
Component::~Component(void) throw()
{   Singleton<ComponentInfo>::getInstance()->removeComponent(this);   }

    //コンポーネントの実行
void Component::executeComponents(int argc, char * argv [])
{
    OptionParameter & option = getOptionParameter();

    option.parseOption(argc, argv);
    executeComponents(option);
}

    //コンポーネントの実行
void Component::executeComponents(OptionParameter & option)
{
    if(isValid()) {

        _parseCommonOption(option);

        try {
                //オプションのパース
            Singleton<ComponentInfo>::getInstance()->parseOption(option);

                //全てのオプションが使用済みでなければエラー
            if(!option.validateOption()) {
                CHECKPOINT("Exception_UnknownOption");
                ExceptionMessage("Unknown option [%] found.","不明なオプションが指定された : %") << option.getInvalidOptions() << throwException;
            }
            else {
                    //コンポーネントの実行
                Singleton<ComponentInfo>::getInstance()->activateComponent(); 
            }
        }

            //例外の発生 -> 異常終了ハンドラの起動
        catch(...) {
            bool continuation;
            do {
                try {
                    continuation = true;
                    Singleton<ComponentInfo>::getInstance()->onFatalExit();
                    continuation = false;
                }
                catch(Exception & e) {
                    CHECKPOINT("Exception_onFatalExit");
                    cerr << Message("[Internal error] onFatalExit threw an exception! : ","[内部エラー] onFatalExit中の例外! : ") << e.getDetails() << '\n';
                }
            } while(continuation);

            throw;
        }
    }
}


    //一般的なオプションの処理
void Component::_parseCommonOption(OptionParameter & option) throw()
{
        //デバッグ処理
    if(option.find("debug")) {
        DebugMessage::setStream(&cerr);
        DebugMessage::setVerbose(true);
    }

        //言語選択
    if(option.find("lj") || option.find("-japanese"))
        Message::selectLanguage(Message::JAPANESE);
    if(option.find("le") || option.find("-english"))
        Message::selectLanguage(Message::ENGLISH);

        //冗長メッセージ
    if(option.find("v") || option.find("-verbose")) {
        VerboseMessage::setStream(&cout);
        VerboseMessage::setVerbose(true);
    }

        //バナー表示
    if(!getBanner().empty() && option.find("v") || option.find("-verbose") || option.find("h") || option.find("-help"))
        cout << getBanner() << endl;

        //ヘルプ表示
    if(option.find("h") || option.find("-help"))
        cout << '\n' << Message(
            "Global option\n"
            " -lj, --japanese : Use Japanese as default language\n"
            " -le, --english  : Use English as default language\n"
            " -v,  --verbose  : Verbose Message\n",
            "全体のオプション\n"
            " -lj, --japanese : 日本語で表示します\n"
            " -le, --english  : 英語で表示します\n"
            " -v,  --verbose  : 冗長メッセージを出力します\n");
}

    //バナーアクセサ
void Component::setBanner(string src) throw(bad_alloc)
{
    ComponentInfo * info = Singleton<ComponentInfo>::getInstance();
    info->setBanner(src);
}

string Component::getBanner(void) throw(bad_alloc)
{
    ComponentInfo * info = Singleton<ComponentInfo>::getInstance();
    return info->getBanner();
}

/*
 *   過去との互換性のためのパラメータ操作API
 */

    /* 指定されたオプションの存在を確認し、必要であればそのパラメータを取得する */
bool Component::findOption(const char * key1, const char * key2, std::string * element) throw()
{
    OptionParameter::OptionItem item;

    item = getOption(key1, key2, true);
    if(item.isValid() && element != 0 && item.hasParameter())
        *element = item[0];

    return item.isValid();
}

    /* 存在確認 + チェックをつける */
bool Component::checkOption(const char * key1, const char * key2) throw()
{   return getOption(key1, key2, true).isValid();   }

    /* 指定された名前のオプションパラメータを取得 */
OptionParameter::OptionItem Component::getOption(const char * key1, const char * key2, bool dirty) throw()
{
    OptionParameter::OptionItem item;
        
    if(key1 != 0)
        item = getOptionParameter().get(key1, dirty);
        
    if(key2 != 0) {
        if(item.isValid())
            item.mergeItem(getOptionParameter().get(string("-") + key2, dirty));
        else
            item = getOptionParameter().get(string("-") + key2, dirty);
    }

    return item;
}

    /* 2つのオプション項目をマージする (放っておいてもマージされるので気にしない) */
OptionParameter::OptionItem Component::mergeOption(const char * key1, const char * key2) throw()
{   return getOption(key1, key2);   }



/****************************************************** テストスィート ******************************************************/

#ifdef TESTSUITE
#include "coverage_undefs.h"

namespace {
    int counter = 0;

    class TestComponent : public Component
    {
    public:
        bool         check_parseOption;
        bool         check_body;
        bool         check_onFatalExit;
        bool         activation;
        bool         throw_parseOption;
        bool         throw_body;
        bool         throw_onFatalExit;
        int          actcnt;

        TestComponent(int order = GENERAL_PURPOSE) : Component(order)
        {
            check_parseOption = false;
            check_body        = false;
            check_onFatalExit = false;
            activation        = false;
            throw_parseOption = false;
            throw_body        = false;
            throw_onFatalExit = false;
        }

        void parseOption(OptionParameter & option) throw(Exception)
        {
            actcnt = ++counter;
            check_parseOption = true;
            if(activation)
                activateComponent();
            if(throw_parseOption)
                ExceptionMessage("exception","exception").throwException();
        }

        void body(void) throw(Exception)
        {
            actcnt = ++counter;
            check_body = true;
            if(throw_body)
                ExceptionMessage("exception","exception").throwException();
        }

        void onFatalExit(void) throw(Exception)
        {
            actcnt = ++counter;
            check_onFatalExit = true;
            if(throw_onFatalExit)
                ExceptionMessage("exception","exception").throwException();
        }
    };

    class DerivedTestComponent : public TestComponent
    {};
}


TESTSUITE_(main,ComponentInfo,Component)
{
    Singleton<ComponentInfo>::Context context;
    Singleton<ComponentInfo>::saveContext(context);
    Singleton<ComponentInfo>::renewInstance();

    BEGIN_CASE("addComponent","addComponent") {

        BEGIN_CASE("1","正しく追加できる") {
            ComponentInfo info;

            BEGIN_CASE("1","関数は成功する") {
                if(!info.addComponent((Component *)0x1234, 10))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","追加されている") {
                if(info.componentList.size() != 1)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("3", "内容が正しい") {
                if(info.componentList.begin()->first != 10 || info.componentList.begin()->second != (Component *)0x1234)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("2","NULLインスタンスは登録できない") {
            ComponentInfo info;

            BEGIN_CASE("1","関数は失敗する") {
                if(info.addComponent(0, 10))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","要素は追加されていない") {
                if(info.componentList.size() != 0)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("3","NULLインスタンスに操作するとfalseが返る") {
            if(((ComponentInfo *)0)->addComponent((Component *)0x1234, 10))
                TEST_FAIL;
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("removeComponent(Component *, int)","removeComponent(Component *, int)") {
        BEGIN_CASE("1", "正しく解除できる") {
            ComponentInfo info;

            info.addComponent((Component *)0x0123, 10); //38行目のifのelseを実行させるためのダミー
            info.addComponent((Component *)0x1234, 10);
            info.addComponent((Component *)0x1234, 20);

            info.removeComponent((Component *)0x1234, 10);
            info.removeComponent((Component *)0x0123, 10);

            BEGIN_CASE("1","要素数が1") {
                if(info.componentList.size() != 1)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","残った要素のorderは20") {
                if(info.componentList.begin()->first != 20)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("2", "NULLオブジェクトから実行しても大丈夫") {
            ((ComponentInfo *)0)->removeComponent(0, 0);
        } END_CASE;
    } END_CASE;
            
    BEGIN_CASE("removeComponent(Component *)","removeComponent(Component *)") {
        BEGIN_CASE("1", "正しく解除できる") {
            ComponentInfo info;
            multimap<int, Component *>::iterator scope;

            info.addComponent((Component *)0x1234, 10);
            info.addComponent((Component *)0x1235, 10);
            info.addComponent((Component *)0x1234, 20);
            info.addComponent((Component *)0x1235, 20);

            info.removeComponent((Component *)0x1234);

            BEGIN_CASE("1","要素数が2") {
                if(info.componentList.size() != 2)
                    TEST_FAIL;
            } END_CASE;

            scope = info.componentList.begin();
            BEGIN_CASE("2","残った要素の内容は正しい") {
                if(scope->first != 10 || scope->second != (Component *)0x1235)
                    TEST_FAIL;
                ++ scope;
                if(scope->first != 20 || scope->second != (Component *)0x1235)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("2", "NULLオブジェクトから実行しても大丈夫") {
            ((ComponentInfo *)0)->removeComponent(0);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("activateComponent","activateComponent") {
        BEGIN_CASE("1","正常に追加できる") {
            ComponentInfo info;

            BEGIN_CASE("1","関数は成功する") {
                if(!info.activateComponent((Component *)0x1234))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","起動要求リストに正しく追加されている") {
                if(info.activatedComponentList.size() != 1)
                    TEST_FAIL;
                if(*info.activatedComponentList.begin() != (Component *)0x1234)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("3","NULLオブジェクトから発行しても暴走しない") {
                ((ComponentInfo *)0)->activateComponent();
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("1","NULLインスタンスは追加されない") {
            ComponentInfo info;

            BEGIN_CASE("1","関数は失敗する") {
                if(info.activateComponent((Component *)0))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","起動要求リストは空のまま") {
                if(!info.activatedComponentList.empty())
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("Component::Component","Component::Component") {
        Singleton<ComponentInfo>::renewInstance();
        TestComponent * test = 0;

        BEGIN_CASE("0","前提条件を満たす") {
            if(Singleton<ComponentInfo>::getInstance()->componentList.size() != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("1","Componentをインスタンス化すると、勝手に登録される") {
            test = new TestComponent;
            if(Singleton<ComponentInfo>::getInstance()->componentList.size() != 1)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","登録されている内容は正しい") {
            if(Singleton<ComponentInfo>::getInstance()->componentList.begin()->second != test)
                TEST_FAIL;
        } END_CASE;
            
        BEGIN_CASE("3","破棄するとエントリが消える") {
            delete test;
            if(Singleton<ComponentInfo>::getInstance()->componentList.size() != 0)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("parseOption","parseOption") {

        BEGIN_CASE("1","parseOptionを実行すると登録されたコンポーネントのparseOptionが実行される") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;

            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());

            if(!test.check_parseOption || !test2.check_parseOption)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","起動順序が正しい (同一レベルなら登録順)") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;

            counter = 0;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());

            if(test.actcnt != 1 || test2.actcnt != 2 )
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","起動順序が正しい (優先度順)") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test(10);
            TestComponent test2(1);

            counter = 0;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());

            if(test.actcnt != 2 || test2.actcnt != 1 )
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","activateをかけるとactivateListに登録される") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            list<Component *>::iterator scope;

            test.activation = true;
            test2.activation = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());

            if(Singleton<ComponentInfo>::getInstance()->activatedComponentList.size() != 2)
                TEST_FAIL;
            scope = Singleton<ComponentInfo>::getInstance()->activatedComponentList.begin();
            if(*scope != &test)
                TEST_FAIL;
            ++ scope;
            if(*scope != &test2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("5","例外は抜けてくる") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            list<Component *>::iterator scope;
            bool result = false;

            Exception::setThrowControl(true);
            test.throw_parseOption = true;
            try {
                Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            }
            catch(...) {
                result = true;
            }

            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("6","例外前に登録されたactivateは残る") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            list<Component *>::iterator scope;
            bool result = false;

            Exception::setThrowControl(true);
            test.activation = true;
            test2.throw_parseOption = true;
            try {
                Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            }
            catch(...) {
                result = true;
            }

            if(!result)
                TEST_FAIL;

            if(Singleton<ComponentInfo>::getInstance()->activatedComponentList.size() != 1)
                TEST_FAIL;

            if(*Singleton<ComponentInfo>::getInstance()->activatedComponentList.begin() != &test)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("7","NULLインスタンスからのコールに耐える") {
            ((ComponentInfo *)0)->parseOption(getOptionParameter());
        } END_CASE;
    }END_CASE;

    BEGIN_CASE("activateCompoent/Component::body","activateCompoent/Component::body") {
        BEGIN_CASE("1","activateComponentをすると、起動要求を出したコンポーネントが起動される") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;

            test.activation = true;
            test2.activation = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            Singleton<ComponentInfo>::getInstance()->activateComponent();

            if(!test.check_body || !test2.check_body)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","起動要求を出さないコンポーネントは起動されない") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;

            test.activation = false;
            test2.activation = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            Singleton<ComponentInfo>::getInstance()->activateComponent();

            if(test.check_body || !test2.check_body)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","起動順序が正しい (同一レベルなら登録順)") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            counter = 0;

            test.activation = test2.activation = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            Singleton<ComponentInfo>::getInstance()->activateComponent();

            if(test.actcnt != 3 || test2.actcnt != 4 )
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","起動順序が正しい (優先度順)") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test(10);
            TestComponent test2(4);
            counter = 0;

            test.activation = test2.activation = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            Singleton<ComponentInfo>::getInstance()->activateComponent();

            if(test.actcnt != 4 || test2.actcnt != 3 )
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("5","例外は抜けてくる") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            list<Component *>::iterator scope;
            bool result = false;

            Exception::setThrowControl(true);
            test.activation = test2.activation = true;
            test.throw_body = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try {
                Singleton<ComponentInfo>::getInstance()->activateComponent();
            }
            catch(...) {
                result = true;
            }

            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("6","例外を起こしたら、それまでに起動したコンポーネントがactivatedComponentListに残る") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            bool result = false;

            Exception::setThrowControl(true);
            test.activation = test2.activation = true;
            test.throw_body = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try {
                Singleton<ComponentInfo>::getInstance()->activateComponent();
            }
            catch(...) {
                result = true;
            }

            if(!result)
                TEST_FAIL;

            if(Singleton<ComponentInfo>::getInstance()->activatedComponentList.size() != 1)
                TEST_FAIL;

            if(*Singleton<ComponentInfo>::getInstance()->activatedComponentList.begin() != &test)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("8","例外を起こしたら、それまでに起動したコンポーネントがactivatedComponentListに残る(2個目)") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            list<Component *>::iterator scope;
            bool result = false;

            Exception::setThrowControl(true);
            test.activation = test2.activation = true;
            test2.throw_body = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try {
                Singleton<ComponentInfo>::getInstance()->activateComponent();
            }
            catch(...) {
                result = true;
            }

            if(!result)
                TEST_FAIL;

            if(Singleton<ComponentInfo>::getInstance()->activatedComponentList.size() != 2)
                TEST_FAIL;
            scope = Singleton<ComponentInfo>::getInstance()->activatedComponentList.begin();
            if(*scope != &test)
                TEST_FAIL;
            ++ scope;
            if(*scope != &test2)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("onFatalExit","onFatalExit") {
        BEGIN_CASE("1","OnFatalExitをすると起動要求リストにあるコンポーネントが呼ばれる") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;

            test.activation = test2.activation = true;
            test2.throw_body = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try{ Singleton<ComponentInfo>::getInstance()->activateComponent(); } catch(...) {}
            Singleton<ComponentInfo>::getInstance()->onFatalExit();

            if(!test.check_onFatalExit || !test2.check_onFatalExit)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","起動要求を出さないコンポーネントは起動されない") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;

            test.activation = false;
            test2.activation = true;
            test2.throw_body = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try{ Singleton<ComponentInfo>::getInstance()->activateComponent(); } catch(...) {}
            Singleton<ComponentInfo>::getInstance()->onFatalExit();

            if(test.check_onFatalExit || !test2.check_onFatalExit)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","起動順序が正しい (同一レベルなら登録順)") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            counter = 0;

            test2.throw_body = true;
            test.activation = test2.activation = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try{ Singleton<ComponentInfo>::getInstance()->activateComponent(); } catch(...) {}
            Singleton<ComponentInfo>::getInstance()->onFatalExit();

            if(test.actcnt != 5 || test2.actcnt != 6 )
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("5","起動順序が正しい (優先度順)") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test(10);
            TestComponent test2(4);
            counter = 0;

            test.activation = test2.activation = true;
            test.throw_body = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try{ Singleton<ComponentInfo>::getInstance()->activateComponent(); } catch(...) {}
            Singleton<ComponentInfo>::getInstance()->onFatalExit();

            if(test.actcnt != 6 || test2.actcnt != 5 )
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("6","例外は抜けてくる") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            list<Component *>::iterator scope;
            bool result = false;

            Exception::setThrowControl(true);
            test.activation = test2.activation = true;
            test2.throw_body = true;
            test.throw_onFatalExit = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try{ Singleton<ComponentInfo>::getInstance()->activateComponent(); } catch(...) {}
            try {
                Singleton<ComponentInfo>::getInstance()->onFatalExit();
            }
            catch(...) {
                result = true;
            }

            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("7","例外が起きたとき、まだ実行されていないコンポーネントのハンドラ起動要求は残る") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;
            TestComponent test2;
            list<Component *>::iterator scope;
            bool result = false;

            Exception::setThrowControl(true);
            test.activation = test2.activation = true;
            test2.throw_body = true;
            test.throw_onFatalExit = true;
            Singleton<ComponentInfo>::getInstance()->parseOption(getOptionParameter());
            try{ Singleton<ComponentInfo>::getInstance()->activateComponent(); } catch(...) {}
            try{
                Singleton<ComponentInfo>::getInstance()->onFatalExit();
            }
            catch(...) {
                result = true;
            }

            if(!result)
                TEST_FAIL;

            if(Singleton<ComponentInfo>::getInstance()->activatedComponentList.size() != 1)
                TEST_FAIL;

            if(*Singleton<ComponentInfo>::getInstance()->activatedComponentList.begin() != &test2)
                TEST_FAIL;

        } END_CASE;

        BEGIN_CASE("8","NULLオブジェクトから発行しても大丈夫") {
            ((ComponentInfo *)0)->onFatalExit();
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("isActive","isActive") {
        BEGIN_CASE("1","起動したコンポーネントを判定できる") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;         
            Singleton<ComponentInfo>::getInstance()->activateComponent(&test);

            if(!Singleton<ComponentInfo>::getInstance()->isActive<TestComponent>())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","起動していないコンポーネントには反応しない") {
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;

            if(Singleton<ComponentInfo>::getInstance()->isActive<TestComponent>())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","派生したコンポーネントのインスタンスには反応する") {
            Singleton<ComponentInfo>::renewInstance();

            DerivedTestComponent test;
            Singleton<ComponentInfo>::getInstance()->activateComponent(&test);

            if(!Singleton<ComponentInfo>::getInstance()->isActive<TestComponent>())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","複数起動していても要求に合ったものを探せる") {   //isActiveのif(dynamic_cast<T *>(iterator->instance) != 0)のelse節を実行させるのが目的
            Singleton<ComponentInfo>::renewInstance();

            TestComponent test;         
            Singleton<ComponentInfo>::getInstance()->activateComponent(&test);
            DerivedTestComponent test2;
            Singleton<ComponentInfo>::getInstance()->activateComponent(&test2);

            if(!Singleton<ComponentInfo>::getInstance()->isActive<DerivedTestComponent>())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;


    Singleton<ComponentInfo>::restoreContext(context);
}

//--------


TESTSUITE(main,Component)
{
    SingletonBase::ContextChain chain;

    chain.saveContext<ComponentInfo>();
    chain.saveContext<OptionParameter>();
    chain.saveContext<VerboseMessage::VerboseControl>();
    chain.saveContext<Message::MessageControl>();
    chain.saveContext<DebugMessage::DebugControl>();

    BEGIN_CASE("executeComponent","executeComponent") {
        BEGIN_CASE("1","特に何もなけばparseOption/bodyが実行される") {
            chain.renewInstance();

            bool result = true;
            TestComponent test;
            test.activation = true;

            Exception::setThrowControl(true);
            try { executeComponents(getOptionParameter()); }
            catch(...) { result = false; }

            BEGIN_CASE("1","例外は起こらない") {
                if(!result)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","parseOption/bodyは実行される") {
                 if(!test.check_parseOption || !test.check_body || test.check_onFatalExit)
                     TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("2","処理してないオプションがある") {
            TestSuite::clearCheckpoints();
            chain.renewInstance();

            char * argv[] = { "test.exe", "-test" };
            bool result = false;
            TestComponent test;
            test.activation = true;

            getOptionParameter().parseOption(2, argv);
            Exception::setThrowControl(true);
            try { executeComponents(getOptionParameter()); }
            catch(...) { result = true; }

            BEGIN_CASE("1","例外が起こる") {
                if(!result)
                    TEST_FAIL;
                if(!TestSuite::isReached("Exception_UnknownOption"))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","parseOption/onFatalExitは実行されるがbodyは実行されない") {
                 if(!test.check_parseOption || test.check_body || !test.check_onFatalExit)
                     TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("3","オプションパース中に例外 (activateなし)") {
            chain.renewInstance();

            bool result = false;
            TestComponent test;
            test.throw_parseOption = true;

            Exception::setThrowControl(true);
            try { executeComponents(getOptionParameter()); }
            catch(...) { result = true; }

            BEGIN_CASE("1","例外が起こる") {
                if(!result)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","parseOptionは実行されるがbody/onFatalExitは実行されない") {
                 if(!test.check_parseOption || test.check_body || test.check_onFatalExit)
                     TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("4","オプションパース中に例外 (activateあり)") {
            chain.renewInstance();

            bool result = false;
            TestComponent test;
            test.throw_parseOption = true;
            test.activation = true;

            Exception::setThrowControl(true);
            try { executeComponents(getOptionParameter()); }
            catch(...) { result = true; }

            BEGIN_CASE("1","例外が起こる") {
                if(!result)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","parseOption/onFatalExitは実行されるがbodyは実行されない") {
                 if(!test.check_parseOption || test.check_body || !test.check_onFatalExit)
                     TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("5","bodyで例外") {
            chain.renewInstance();

            bool result = false;
            TestComponent test;
            test.throw_body = true;
            test.activation = true;

            Exception::setThrowControl(true);
            try { executeComponents(getOptionParameter()); }
            catch(...) { result = true; }

            BEGIN_CASE("1","例外が起こる") {
                if(!result)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","parseOption/body/onFatalExitとも実行される") {
                 if(!test.check_parseOption || !test.check_body || !test.check_onFatalExit)
                     TEST_FAIL;
            } END_CASE;
        } END_CASE;


        BEGIN_CASE("6","onFatalExitで例外") {
            TestSuite::clearCheckpoints();
            chain.renewInstance();

            bool result = false;
            TestComponent test;
            test.throw_onFatalExit = true;
            test.activation = true;
            TestComponent test2;
            test2.throw_body = true;
            test2.activation = true;

            Exception::setThrowControl(true);
            try { executeComponents(getOptionParameter()); }
            catch(...) { result = true; }

            BEGIN_CASE("1","例外が起こる") {
                if(!result)
                    TEST_FAIL;
                if(!TestSuite::isReached("Exception_onFatalExit"))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","最初のコンポーネントはparseOption/body/onFatalExitとも実行される") {
                 if(!test.check_parseOption || !test.check_body || !test.check_onFatalExit)
                     TEST_FAIL;
            } END_CASE;
            BEGIN_CASE("3","次のコンポーネントもparseOption/body/onFatalExitとも実行される") {
                 if(!test2.check_parseOption || !test2.check_body || !test2.check_onFatalExit)
                     TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("7","Component::is_validがfalseのときは実行されない") {
            chain.renewInstance();

            bool result = true;
            TestComponent test;
            test.activation = true;

            Component::is_valid = false;
            Exception::setThrowControl(true);
            try { executeComponents(getOptionParameter()); }
            catch(...) { result = false; }

            BEGIN_CASE("1","例外は起こらない") {
                if(!result)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","parseOption/bodyは実行されない") {
                 if(test.check_parseOption || test.check_body || test.check_onFatalExit)
                     TEST_FAIL;
            } END_CASE;

            Component::is_valid = true;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("_parseCommonOption","_parseCommonOption") {

        BEGIN_CASE("1", "-debugでDebugMessageが有効になる") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "-debug" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            Component::_parseCommonOption(param);

            if(!DebugMessage::getVerbose())
                TEST_FAIL;

        } END_CASE;

        BEGIN_CASE("2", "-v でVerboseMessageが有効になる") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "-v" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            Component::_parseCommonOption(param);

            if(!VerboseMessage::getVerbose())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3", "--verboseでVerboseMessageが有効になる") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "--verbose" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            Component::_parseCommonOption(param);

            if(!VerboseMessage::getVerbose())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4", "-lj でMessageが日本語になる") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "-lj" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            Component::_parseCommonOption(param);

            if(Message::getCurrentLanguage() != Message::JAPANESE)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("5", "--japanese でMessageが日本語になる") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "--japanese" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            Component::_parseCommonOption(param);

            if(Message::getCurrentLanguage() != Message::JAPANESE)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("6", "-le でMessageが英語になる") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "-le" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            Component::_parseCommonOption(param);

            if(Message::getCurrentLanguage() != Message::ENGLISH)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("7", "--english でMessageが英語になる") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "--english" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            Component::_parseCommonOption(param);

            if(Message::getCurrentLanguage() != Message::ENGLISH)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("8","ヘルプがでる (-h)") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "-h" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            cout.str("");
            Component::_parseCommonOption(param);

                //とりあえず何か出力されていることだけ確認しておく
            if(cout.str().empty())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("9","ヘルプがでる (--help)") {
            chain.renewInstance();

            char * argv[] = { "test.exe", "--help" };
            OptionParameter & param = getOptionParameter();

            param.parseOption(2, argv);
            cout.str("");
            Component::_parseCommonOption(param);

                //とりあえず何か出力されていることだけ確認しておく
            if(cout.str().empty())
                TEST_FAIL;
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("getOption","オプションパラメータの取得") {
        chain.renewInstance();

        char * argv[] = { "test.exe","-test","param","-test2","param2", "--test","param3","--test2","param4" };
        getOptionParameter().parseOption(9, argv);

        BEGIN_CASE("1","getOption(test)でparamが取れる") {
            OptionParameter::OptionItem item;

            item = getOption("test");
            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","チェック済みである", item.isChecked());
            TEST_CASE("3","1つのパラメータをもつ", item.countParameter() == 1);
            TEST_CASE("4","パラメータの内容があっている", item[0].compare("param") == 0);
        } END_CASE;

        BEGIN_CASE("2","getOption(test,test)で2つの連結が取れる") {
            OptionParameter::OptionItem item;

            TEST_CASE("0","[前提] --testにチェックはついていない", !getOptionParameter().get("-test",false).isChecked());

            item = getOption("test","test");

            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","チェック済みである", item.isChecked());
            TEST_CASE("3","チェック済みである (test)", getOptionParameter().get("test",false).isChecked());
            TEST_CASE("4","チェック済みである (-test)", getOptionParameter().get("-test",false).isChecked());
            TEST_CASE("5","2つのパラメータを持つ", item.countParameter() == 2);
            TEST_CASE("6","1つめのパラメータの値は正しい", item[0].compare("param") == 0);
            TEST_CASE("7","2つめのパラメータの値は正しい", item[1].compare("param3") == 0);
        } END_CASE;

        BEGIN_CASE("3","getOption(test,unknown)でtestのパラメータが取れる") {
            OptionParameter::OptionItem item;

            item = getOption("test","unknown");

            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","1つのパラメータを持つ", item.countParameter() == 1);
            TEST_CASE("3","パラメータの値は正しい", item[0].compare("param") == 0);
        } END_CASE;

        BEGIN_CASE("4","getOption(unknown,test)で -testのパラメータが取れる") {
            OptionParameter::OptionItem item;

            item = getOption("unknown","test");

            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","1つのパラメータを持つ", item.countParameter() == 1);
            TEST_CASE("3","パラメータの値は正しい", item[0].compare("param3") == 0);
        } END_CASE;

        BEGIN_CASE("5","getOption(unknown,unknown)で無効なアイテムが返る") {
            OptionParameter::OptionItem item;

            item = getOption("unknown","unknown");

            TEST_CASE("1","無効な値が返る", !item.isValid());
        } END_CASE;

        BEGIN_CASE("6","getOption(test2,test2,false)でチェックがつかない") {
            OptionParameter::OptionItem item;

            TEST_CASE("0","[前提] -test2にチェックはついていない", !getOptionParameter().get("test2",false).isChecked());
            TEST_CASE("0","[前提] --test2にチェックはついていない", !getOptionParameter().get("-test2",false).isChecked());

            item = getOption("test2","test2",false);

            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","チェック済みでない", !item.isChecked());
            TEST_CASE("3","チェック済みでない (test2)", !getOptionParameter().get("test2",false).isChecked());
            TEST_CASE("4","チェック済みでない (-test2)", !getOptionParameter().get("-test2",false).isChecked());
            TEST_CASE("5","2つのパラメータを持つ", item.countParameter() == 2);
            TEST_CASE("6","1つめのパラメータの値は正しい", item[0].compare("param2") == 0);
            TEST_CASE("7","2つめのパラメータの値は正しい", item[1].compare("param4") == 0);
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("findOption","オプションパラメータの取得") {
        chain.renewInstance();

        char * argv[] = { "test.exe","-test","param","--test","param3", "-noparam"};
        getOptionParameter().parseOption(6, argv);

        BEGIN_CASE("1","findOption(test,test)でparamが取れる") {
            string result;

            TEST_CASE("0","[前提] チェック済みではない (test)", !getOptionParameter().get("test",false).isChecked());
            TEST_CASE("0","[前提] チェック済みではない (-test)", !getOptionParameter().get("-test",false).isChecked());
            TEST_CASE("1","関数は成功する", findOption("test","test",&result));
            TEST_CASE("2","文字列にparamが返る", result.compare("param") == 0);
            TEST_CASE("3","チェック済み (test)", getOptionParameter().get("test",false).isChecked());
            TEST_CASE("3","チェック済み (-test)", getOptionParameter().get("-test",false).isChecked());
        } END_CASE;

        BEGIN_CASE("2","findOption(test,unknown)でparamが取れる") {
            string result;

            TEST_CASE("1","関数は成功する", findOption("test","unknown",&result));
            TEST_CASE("2","文字列にparamが返る", result.compare("param") == 0);
        } END_CASE;

        BEGIN_CASE("3","findOption(unknown,test)でparam3が取れる") {
            string result;

            TEST_CASE("1","関数は成功する", findOption("unknown","test",&result));
            TEST_CASE("2","文字列にparamが返る", result.compare("param3") == 0);
        } END_CASE;

        BEGIN_CASE("4","findOption(unknown,unknown)で無効なアイテムが返る") {
            string result;

            result.assign("dummy");

            TEST_CASE("1","関数は失敗する", !findOption("unknown","unknown",&result));
            TEST_CASE("2","文字列は書き換わらない", result.compare("dummy") == 0);
        } END_CASE;

        BEGIN_CASE("5","findOption(noparam,NULL,&result)") {
            string result;
            result.assign("dummy");

            TEST_CASE("1","関数は成功する", findOption("noparam", 0, &result));
            TEST_CASE("2","文字列は置き換わらない", result.compare("dummy") == 0);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("checkOption","オプションパラメータの存在確認 + チェック") {
        chain.renewInstance();

        char * argv[] = { "test.exe","-test","param","--test","param3", "-test2", "--test3"};
        getOptionParameter().parseOption(7, argv);

        BEGIN_CASE("1","checkOption(test,test)でparamが取れる") {
            string result;

            TEST_CASE("0","[前提] チェック済みではない (test)", !getOptionParameter().get("test",false).isChecked());
            TEST_CASE("0","[前提] チェック済みではない (-test)", !getOptionParameter().get("-test",false).isChecked());
            TEST_CASE("1","関数は成功する", checkOption("test","test"));
            TEST_CASE("2","チェック済み (test)", getOptionParameter().get("test",false).isChecked());
            TEST_CASE("3","チェック済み (-test)", getOptionParameter().get("-test",false).isChecked());
        } END_CASE;

        BEGIN_CASE("2","checkOption(test2,unknown)でparamが取れる") {
            string result;

            TEST_CASE("0","[前提] チェック済みではない (test2)", !getOptionParameter().get("test2",false).isChecked());
            TEST_CASE("1","関数は成功する", checkOption("test2","unknown"));
            TEST_CASE("2","チェック済み (test2)", getOptionParameter().get("test2",false).isChecked());
        } END_CASE;

        BEGIN_CASE("3","checkOption(unknown,test3)でparam3が取れる") {
            string result;

            TEST_CASE("0","[前提] チェック済みではない (test3)", !getOptionParameter().get("-test3",false).isChecked());
            TEST_CASE("1","関数は成功する", checkOption("unknown","test3"));
            TEST_CASE("2","チェック済み (test3)", getOptionParameter().get("-test3",false).isChecked());
        } END_CASE;

        BEGIN_CASE("4","checkOption(unknown,unknown)で無効なアイテムが返る") {
            string result;

            result.assign("dummy");

            TEST_CASE("1","関数は失敗する", !checkOption("unknown","unknown"));
        } END_CASE;
    } END_CASE;

        /* mergeOptionのチェック項目は、getOptionとほぼ同じ */
    BEGIN_CASE("mergeOption","オプションパラメータの結合") {
        chain.renewInstance();

        char * argv[] = { "test.exe","-test","param","-test2","param2", "--test","param3","--test2","param4" };
        getOptionParameter().parseOption(9, argv);

        BEGIN_CASE("1","mergeOption(test)でparamが取れる") {
            OptionParameter::OptionItem item;

            item = mergeOption("test");
            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","チェック済みである", item.isChecked());
            TEST_CASE("3","1つのパラメータをもつ", item.countParameter() == 1);
            TEST_CASE("4","パラメータの内容があっている", item[0].compare("param") == 0);
        } END_CASE;

        BEGIN_CASE("2","mergeOption(test,test)で2つの連結が取れる") {
            OptionParameter::OptionItem item;

            TEST_CASE("0","[前提] --testにチェックはついていない", !getOptionParameter().get("-test",false).isChecked());

            item = mergeOption("test","test");

            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","チェック済みである", item.isChecked());
            TEST_CASE("3","チェック済みである (test)", getOptionParameter().get("test",false).isChecked());
            TEST_CASE("4","チェック済みである (-test)", getOptionParameter().get("-test",false).isChecked());
            TEST_CASE("5","2つのパラメータを持つ", item.countParameter() == 2);
            TEST_CASE("6","1つめのパラメータの値は正しい", item[0].compare("param") == 0);
            TEST_CASE("7","2つめのパラメータの値は正しい", item[1].compare("param3") == 0);
        } END_CASE;

        BEGIN_CASE("3","mergeOption(test,unknown)でtestのパラメータが取れる") {
            OptionParameter::OptionItem item;

            item = mergeOption("test","unknown");

            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","1つのパラメータを持つ", item.countParameter() == 1);
            TEST_CASE("3","パラメータの値は正しい", item[0].compare("param") == 0);
        } END_CASE;

        BEGIN_CASE("4","mergeOption(unknown,test)で -testのパラメータが取れる") {
            OptionParameter::OptionItem item;

            item = mergeOption("unknown","test");

            TEST_CASE("1","有効な値が返る", item.isValid());
            TEST_CASE("2","1つのパラメータを持つ", item.countParameter() == 1);
            TEST_CASE("3","パラメータの値は正しい", item[0].compare("param3") == 0);
        } END_CASE;

        BEGIN_CASE("5","mergeOption(unknown,unknown)で無効なアイテムが返る") {
            OptionParameter::OptionItem item;

            item = mergeOption("unknown","unknown");

            TEST_CASE("1","無効な値が返る", !item.isValid());
        } END_CASE;

    } END_CASE;

    chain.restoreContext();
}
#endif


