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
 *  @(#) $Id: collection.cpp,v 1.3 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/collection.cpp,v 1.3 2003/12/15 07:32:13 takayuki Exp $

#include "base/collection.h"


using namespace std;

    //特に何もしないコンストラクタ
Collection::Collection(void) throw()
{}

    //デストラクタ
Collection::~Collection(void) throw()
{
        //念のために例外にふたをする
    try { clear(); }
    catch(...) {}
}

    //インスタンスをコレクションに追加 (順序は登録順)
bool Collection::addInstance(Collectable * _instance, bool _destruction) throw()
{
    bool result = false;

    if(isValid() && _instance != 0) {

            // 同じインスタンスを2回登録しないようにする
        list<Element>::iterator scope;

        scope = container.begin();
        while(scope != container.end()) {
            if(scope->instance == _instance)
                break;
            ++ scope;
        }

        if(scope == container.end()) {
                // 末尾に要素を追加
            struct Element element;
            element.instance    = _instance;
            element.destruction = _destruction;
            container.push_back(element);
        }
        else {
                // 破棄指示は最新に直しておく
            scope->destruction = _destruction;
        }

        result = true;
    }

    return result;
}

    //指定されたCollectableインスタンスに相当する場所を指すイテレータの取得 (getInstance)
list<Collection::Element>::const_iterator Collection::_findInstance(const Collectable * predecessor) const throw()
{
    list<Element>::const_iterator result;

    result = container.begin();

        //predecessorが指定されていた場合は探す
    if(predecessor != 0) {
        while(result != container.end()) {
            if(result->instance == predecessor) {
                ++ result;   //検索はpredecessorの次から
                break;
            }
            ++ result;
        }
    }

    return result;
}

    //コレクションからインスタンスを取得
Collectable * Collection::getInstance(const type_info & type, const Collectable * predecessor) const throw()
{
    Collectable * result;

    result = 0;
    if(isValid()) {

        list<Element>::const_iterator scope;

            // 先行するインスタンスの場所を探す
        scope = _findInstance(predecessor);

            // 指定された型を持つ次のインスタンスを探す
        while(scope != container.end()) {
            if(typeid(*scope->instance) == type) {
                result = scope->instance;
                break;
            }
            ++ scope;
        }
    }

    return result;
}

    //コレクションからインスタンスを除外 (破棄はしない)
bool Collection::removeInstance(const Collectable * instance) throw()
{
    bool result = false;

    if(isValid() && instance != 0) {
        list<Element>::iterator scope;

        scope = container.begin();
        while(scope != container.end()) {
            if(scope->instance == instance) {
                    //要素の削除
                container.erase(scope);
                result = true;
                break;
            }
            ++ scope;
        }
    }

    return result;
}

    //全要素の破棄
void Collection::clear(void)
{
    if(isValid()) {
        list<Element>::iterator scope;
        list<Element>::iterator next;

            //破棄フラグがついているインスタンスを破棄
        scope = container.begin();
        while(scope != container.end()) {

            next = scope;       //ROT系はdeleteすると自分を消しにかかるので、イテレータを保存するために先に次を取得しておく
            ++ next;

            if(scope->destruction)
                delete scope->instance;

            scope = next;
        }

        container.clear();
    }
}

/************************************************* テストスィート *************************************************/

#ifdef TESTSUITE
#include "coverage_undefs.h"

class TestCollectable : public Collectable
{};

class TestCollectable2 : public Collectable
{   public: ~TestCollectable2(void) throw() { TestSuite::check("TestCollectable2::~TestCollectable2"); }   };

#ifdef _MSC_VER
class TestCollectable3 : public Collectable
{   public: ~TestCollectable3(void) throw(int) { throw 0; }   };
#endif

class TestCollectable4 : public TestCollectable2
{   public: ~TestCollectable4(void) throw() { TestSuite::check("TestCollectable4::~TestCollectable4"); }   };

TESTSUITE(main, Collection)
{
    BEGIN_CASE("addInstance","addInstance") {
        BEGIN_CASE("1","インスタンスは正しく登録できる") {
            Collection col;
            TestCollectable test;
            TestCollectable2 test2;

            BEGIN_CASE("1","登録できTRUEが返る (1個目)") {
                if(!col.addInstance(&test,  false))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","登録できtrueが返る (2個目)") {
                if(!col.addInstance(&test2, false))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("3","個数が合ってる") {
                if(col.container.size() != 2)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("4","順番があってる") {
            list<Element>::iterator scope;
                scope = col.container.begin();
                if(scope->instance != &test || scope->destruction != false)
                    TEST_FAIL;
                ++ scope;
                if(scope->instance != &test2 || scope->destruction != false)
                    TEST_FAIL;
                ++ scope;
                if(scope != col.container.end())
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("2","NULLインスタンスは登録されない") {
            Collection col;

            BEGIN_CASE("1","NULLインスタンスを渡すとfalseが返る") {
                if(col.addInstance(0))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","何も登録されていない") {
                if(col.container.size() != 0)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("3","無効なインスタンスに対する操作でfalseが返る") {
            Collection col;

            if(col.addInstance(0))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","同一のインスタンスを2回登録しても1個だけしか登録されない") {
            Collection col;
            TestCollectable test;

            TEST_CASE("0","[前提] 一回目は正しく成功する", col.addInstance(&test, false));

            TEST_CASE("1","関数は正しく終了する", col.addInstance(&test, false));
            TEST_CASE("2","インスタンス数は増えていない", col.container.size() == 1);

        } END_CASE;

    } END_CASE;

    BEGIN_CASE("getInstance(type_info)","getInstance(type_info)") {
        Collection col;
        TestCollectable test;
        TestCollectable test_2;
        TestCollectable test_3;
        TestCollectable2 test2;
        TestCollectable2 test2_2;
        TestCollectable2 test2_3;

        col.addInstance(test);
        col.addInstance(test2);
        col.addInstance(test_2);
        col.addInstance(test2_2);
        col.addInstance(test_3);
        col.addInstance(test2_3);

        Collectable * instance = 0;

        BEGIN_CASE("1","最初の要素を取得できる") {
            instance = col.getInstance(typeid(TestCollectable));
            if(instance != &test)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","次の要素を取得できる") {
            instance = col.getInstance(typeid(TestCollectable), instance);
            if(instance != &test_2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","さらに次の要素を取得できる") {
            instance = col.getInstance(typeid(TestCollectable), instance);
            if(instance != &test_3)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","そのまた次の要素を取得したらNULLが返る") {
            instance = col.getInstance(typeid(TestCollectable), instance);
            if(instance != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("5","最初の要素を取得できる") {
            instance = col.getInstance(typeid(TestCollectable2));
            if(instance != &test2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("6","次の要素を取得できる") {
            instance = col.getInstance(typeid(TestCollectable2), instance);
            if(instance != &test2_2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("7","さらに次の要素を取得できる") {
            instance = col.getInstance(typeid(TestCollectable2), instance);
            if(instance != &test2_3)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("8","そのまた次の要素を取得したらNULLが返る") {
            instance = col.getInstance(typeid(TestCollectable), instance);
            if(instance != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("9","基底クラスではひっかからない") {
            instance = col.getInstance(typeid(Collectable));
            if(instance != 0)
                TEST_FAIL;
        } END_CASE;

        TEST_CASE("10","NULLに対して実行するとNULLが返る", ((Collection *)0)->getInstance(typeid(Collectable)) == 0);
    } END_CASE;

    BEGIN_CASE("getInstance<T>()","getInstance<T>()") {
        Collection col;
        TestCollectable test;
        TestCollectable test_2;
        TestCollectable test_3;
        TestCollectable2 test2;
        TestCollectable2 test2_2;
        TestCollectable2 test2_3;

        col.addInstance(test);
        col.addInstance(test2);
        col.addInstance(test_2);
        col.addInstance(test2_2);
        col.addInstance(test_3);
        col.addInstance(test2_3);

        Collectable * instance = 0;

        BEGIN_CASE("1","最初の要素を取得できる") {
            instance = col.getInstance<TestCollectable>();
            if(instance != &test)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","次の要素を取得できる") {
            instance = col.getInstance<TestCollectable>(instance);
            if(instance != &test_2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","さらに次の要素を取得できる") {
            instance = col.getInstance<TestCollectable>(instance);
            if(instance != &test_3)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","そのまた次の要素を取得したらNULLが返る") {
            instance = col.getInstance<TestCollectable>(instance);
            if(instance != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("5","最初の要素を取得できる") {
            instance = col.getInstance<TestCollectable2>();
            if(instance != &test2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("6","次の要素を取得できる") {
            instance = col.getInstance<TestCollectable2>(instance);
            if(instance != &test2_2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("7","さらに次の要素を取得できる") {
            instance = col.getInstance<TestCollectable2>(instance);
            if(instance != &test2_3)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("8","そのまた次の要素を取得したらNULLが返る") {
            instance = col.getInstance<TestCollectable2>(instance);
            if(instance != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("9","基底クラスで引っ掛ける その1") {
            instance = col.getInstance<Collectable>();
            if(instance != &test)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("10","基底クラスで引っ掛ける その2") {
            instance = col.getInstance<Collectable>(instance);
            if(instance != &test2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("11","基底クラスで引っ掛ける その3") {
            instance = col.getInstance<Collectable>(instance);
            if(instance != &test_2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("12","基底クラスで引っ掛ける その4") {
            instance = col.getInstance<Collectable>(instance);
            if(instance != &test2_2)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("13","基底クラスで引っ掛ける その5") {
            instance = col.getInstance<Collectable>(instance);
            if(instance != &test_3)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("14","基底クラスで引っ掛ける その6") {
            instance = col.getInstance<Collectable>(instance);
            if(instance != &test2_3)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("15","基底クラスで引っ掛ける その7") {
            instance = col.getInstance<Collectable>(instance);
            if(instance != 0)
                TEST_FAIL;
        } END_CASE;

        TEST_CASE("16","NULLに対して実行するとNULLが返る", ((Collection *)0)->getInstance<Collectable>() == 0);

    } END_CASE;

    BEGIN_CASE("removeInstance","removeInstance") {

        BEGIN_CASE("1","正常に削除できる") {
            Collection col;
            TestCollectable test;
            TestCollectable test2;

            col.addInstance(test);
            col.addInstance(test2);

            BEGIN_CASE("1","存在する要素をremoveしたらtrue") {
                if(!col.removeInstance(&test))
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","ちゃんと消えている") {
                if(col.container.size() != 1 || col.container.begin()->instance != &test2)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("2","NULLインスタンスに実行するとfalse") {
            if(((Collection *)0)->removeInstance(0))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","登録していないインスタンスを指定すると失敗する") {
            Collection col;
            TestCollectable test;
            TestCollectable test2;

            col.addInstance(test);

            if(col.removeInstance(&test2))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","NULLインスタンスを渡すと失敗する") {
            Collection col;
            TestCollectable test;
            col.addInstance(test);

            if(col.removeInstance(0))
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("clear","clear") {
        BEGIN_CASE("1","実体を登録してclear") {
            Collection col;
            TestCollectable2 test2;

            TestSuite::clearCheckpoints();
            col.addInstance(test2);
            col.clear();

            BEGIN_CASE("1","要素がなくなっている") {
                if(col.container.size() != 0)
                    TEST_FAIL;
            } END_CASE;

            TEST_CASE("2","test2は破棄されない", !TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        } END_CASE;

        BEGIN_CASE("2","ポインタを登録してclear") {
            Collection col;
            TestCollectable2 * test2 = new TestCollectable2;
            
            TestSuite::clearCheckpoints();
            col.addInstance(test2);
            col.clear();

            BEGIN_CASE("1","要素がなくなっている") {
                if(col.container.size() != 0)
                    TEST_FAIL;
            } END_CASE;

            TEST_CASE("2","test2は破棄される", TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        } END_CASE;

        BEGIN_CASE("3","NULLオブジェクトに対して実行しても暴走しない") {
            ((Collection *)0)->clear();
        } END_CASE;

#ifdef _MSC_VER
        BEGIN_CASE("4","例外が出るようなデストラクタをもつインスタンスを破棄すると例外が漏れる") {
            Collection col;
            bool result = false;
            TestCollectable3 * test = new TestCollectable3;

            col.addInstance(test);
            try {
                col.clear();
            }
            catch(...) {
                result = true;
            }


            if(!result)
                TEST_FAIL;
        } END_CASE;
#endif
    } END_CASE;

    BEGIN_CASE("destructor","destructor") {
        BEGIN_CASE("1","実体を登録して破棄") {
            Collection * col = new Collection;
            TestCollectable2 test2;

            TestSuite::clearCheckpoints();
            col->addInstance(test2);
            delete col;

            TEST_CASE("1","test2は破棄されない", !TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        } END_CASE;

        BEGIN_CASE("2","ポインタを登録してclear") {
            Collection * col = new Collection;
            TestCollectable2 * test2 = new TestCollectable2;

            TestSuite::clearCheckpoints();
            col->addInstance(test2);
            delete col;

            TEST_CASE("1","test2は破棄される", TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        } END_CASE;

#ifdef _MSC_VER
        BEGIN_CASE("3","例外が出るようなデストラクタを持つインスタンスを破棄しても例外が漏れない") {
            Collection * col = new Collection;
            TestCollectable3 * test2 = new TestCollectable3;
            bool result = true;
            
            TestSuite::clearCheckpoints();
            col->addInstance(test2);

            try {
                delete col;
            }
            catch(...) {
                result = false;
            }

            if(!result)
                TEST_FAIL;
        } END_CASE;
#endif
    } END_CASE;

    BEGIN_CASE("deleteInstance","deleteInstance") {
        BEGIN_CASE("1","インスタンスを削除できる") {
            Collection col;
            TestCollectable2 * test = new TestCollectable2;

            TestSuite::clearCheckpoints();
            col.addInstance(test);

            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance<TestCollectable2>() != 0);

            col.deleteInstance<TestCollectable2>();
            TEST_CASE("1","インスタンスが消えている", col.getInstance<TestCollectable2>() == 0);
            TEST_CASE("2","デストラクタが実行されている", TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        }END_CASE;

        BEGIN_CASE("2","関係ないクラスを指定したら削除されない") {
            Collection col;
            TestCollectable2 * test = new TestCollectable2;

            TestSuite::clearCheckpoints();
            col.addInstance(test);

            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance<TestCollectable2>() != 0);

            col.deleteInstance<TestCollectable>();
            TEST_CASE("1","インスタンスが消えていない", col.getInstance<TestCollectable2>() != 0);
            TEST_CASE("2","デストラクタが実行されていない", !TestSuite::isReached("TestCollectable2::~TestCollectable2"));
            delete test;
        }END_CASE;

        BEGIN_CASE("3","同じクラスだけでなく派生先も消える") {
            Collection col;
            TestCollectable2 * test = new TestCollectable2;
            TestCollectable4 * test2 = new TestCollectable4;

            TestSuite::clearCheckpoints();
            col.addInstance(test);
            col.addInstance(test2);

            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance(typeid(TestCollectable2)) != 0);
            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance(typeid(TestCollectable4)) != 0);

            col.deleteInstance<TestCollectable2>();
            TEST_CASE("1","インスタンスがすべて消えている", col.getInstance<TestCollectable2>() == 0);
            TEST_CASE("2","TestCollectable2のデストラクタが実行されている", TestSuite::isReached("TestCollectable2::~TestCollectable2"));
            TEST_CASE("3","TestCollectable4のデストラクタが実行されている", TestSuite::isReached("TestCollectable4::~TestCollectable4"));
        }END_CASE;

        BEGIN_CASE("4","実体は破棄対象にならない") {
            Collection col;
            TestCollectable2 test;

            TestSuite::clearCheckpoints();
            col.addInstance(test);

            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance<TestCollectable2>() != 0);

            col.deleteInstance<TestCollectable2>();
            TEST_CASE("1","インスタンスが消えている", col.getInstance<TestCollectable2>() == 0);
            TEST_CASE("2","デストラクタが実行されていない", !TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        }END_CASE;

        BEGIN_CASE("5","NULLオブジェクトに対して発行しても暴走しない") {
            ((Collection *)0)->deleteInstance<TestCollectable>();
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("replaceInstance","replaceInstance") {
        BEGIN_CASE("1","登録済みのインスタンスを入れ替える") {
            Collection col;
            TestCollectable2 test;
            TestCollectable2 test2;

            TestSuite::clearCheckpoints();
            col.addInstance(test);

            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance(typeid(TestCollectable2)) != 0);
            
            TEST_CASE("1", "関数は成功する", col.replaceInstance<TestCollectable2>(test2));
            TEST_CASE("2", "インスタンスは入れ替わっている", col.getInstance(typeid(TestCollectable2)) == &test2);
            TEST_CASE("3", "test2以外にインスタンスはない", col.getInstance(typeid(TestCollectable2), &test2) == 0);
            TEST_CASE("4", "デストラクタは実行されてない", !TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        } END_CASE;

        BEGIN_CASE("2","登録済みのインスタンスを派生クラスのインスタンスで入れ替える") {
            Collection col;
            TestCollectable2 test;
            TestCollectable4 test2;

            TestSuite::clearCheckpoints();
            col.addInstance(test);

            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance(typeid(TestCollectable2)) != 0);
            
            TEST_CASE("1", "関数は成功する", col.replaceInstance<TestCollectable2>(test2));
            TEST_CASE("2", "インスタンスは入れ替わっている", col.getInstance<TestCollectable2>() == &test2);
            TEST_CASE("3", "test2以外にインスタンスはない", col.getInstance(typeid(TestCollectable2)) == 0);
            TEST_CASE("4", "デストラクタは実行されてない", !TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        } END_CASE;

        BEGIN_CASE("3","無関係なクラスを指定して入れ替える") {
            Collection col;
            TestCollectable2 test;
            TestCollectable4 test2;

            TestSuite::clearCheckpoints();
            col.addInstance(test);

            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance(typeid(TestCollectable2)) != 0);
            
            TEST_CASE("1", "関数は失敗する", !col.replaceInstance<TestCollectable>(test2));
            TEST_CASE("2", "インスタンスは残っている", col.getInstance<TestCollectable2>() == &test);
        } END_CASE;

        BEGIN_CASE("4","登録済みのインスタンスを入れ替える (動的確保)") {
            Collection col;
            TestCollectable2 * test = new TestCollectable2;
            TestCollectable2 test2;

            TestSuite::clearCheckpoints();
            col.addInstance(test);

            TEST_CASE("0","[前提] ちゃんと登録されている", col.getInstance(typeid(TestCollectable2)) != 0);
            
            TEST_CASE("1", "関数は成功する", col.replaceInstance<TestCollectable2>(test2));
            TEST_CASE("2", "インスタンスは入れ替わっている", col.getInstance(typeid(TestCollectable2)) == &test2);
            TEST_CASE("3", "test2以外にインスタンスはない", col.getInstance(typeid(TestCollectable2), &test2) == 0);
            TEST_CASE("4", "デストラクタは実行されてる", TestSuite::isReached("TestCollectable2::~TestCollectable2"));
        } END_CASE;

    } END_CASE;
}


class TestRuntimeObject : public RuntimeObject
{
public:
    TestRuntimeObject(void) { CHECKPOINT("TestRuntimeObject::TestRuntimeObject"); }
    ~TestRuntimeObject(void) { CHECKPOINT("TestRuntimeObject::~TestRuntimeObject"); }
};

class TestRuntimeObject2 : public RuntimeObject
{
public:
    TestRuntimeObject2(void) : RuntimeObject(true) { CHECKPOINT("TestRuntimeObject2::TestRuntimeObject2"); }
    ~TestRuntimeObject2(void) { CHECKPOINT("TestRuntimeObject2::~TestRuntimeObject2"); }
};

TESTSUITE(main, RuntimeObjectTable)
{
    Singleton<RuntimeObjectTable>::Context context;
    Singleton<RuntimeObjectTable>::saveContext(context);

    BEGIN_CASE("getInstance","getInstance") {
        BEGIN_CASE("1","getInstanceは正しく機能する") {
            Singleton<RuntimeObjectTable>::renewInstance();
            TestCollectable test;

            Singleton<RuntimeObjectTable>::getInstance()->addInstance(test);

            BEGIN_CASE("1","getInstance(typeid)") {
                if(RuntimeObjectTable::getInstance(typeid(TestCollectable)) != &test)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","getInstance<T>()") {
                if(RuntimeObjectTable::getInstance<TestCollectable>() != &test)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("RuntimeObject","RuntimeObject") {
        BEGIN_CASE("1","生成すると登録され、破棄で消滅する") {
            TestRuntimeObject * ro;

            TestSuite::clearCheckpoints();

            TEST_CASE("0","[前提] 登録されていない", RuntimeObjectTable::getInstance<TestRuntimeObject>() == 0);

            ro = new TestRuntimeObject;

            TEST_CASE("1","コンストラクタが実行されている", TestSuite::isReached("TestRuntimeObject::TestRuntimeObject"));
            TEST_CASE("2","登録されている", RuntimeObjectTable::getInstance<TestRuntimeObject>() == ro);

            delete ro;

            TEST_CASE("3","デストラクタが実行されている", TestSuite::isReached("TestRuntimeObject::~TestRuntimeObject"));
            TEST_CASE("4","登録が解除されている", RuntimeObjectTable::getInstance<TestRuntimeObject>() == 0);

        } END_CASE;

        BEGIN_CASE("2","自動破棄のチェック") {
            TestRuntimeObject * ro;
            TestRuntimeObject2 * ro2;

            TestSuite::clearCheckpoints();

            TEST_CASE("0","[前提] 登録されていない", RuntimeObjectTable::getInstance<TestRuntimeObject>() == 0);
            TEST_CASE("0","[前提] 登録されていない", RuntimeObjectTable::getInstance<TestRuntimeObject2>() == 0);

            ro = new TestRuntimeObject;
            ro2 = new TestRuntimeObject2;

            TEST_CASE("1","コンストラクタが実行されている(ro)", TestSuite::isReached("TestRuntimeObject::TestRuntimeObject"));
            TEST_CASE("2","コンストラクタが実行されている(ro2)", TestSuite::isReached("TestRuntimeObject2::TestRuntimeObject2"));
            TEST_CASE("3","登録されている(ro)", RuntimeObjectTable::getInstance<TestRuntimeObject>() == ro);
            TEST_CASE("4","登録されている(ro2)", RuntimeObjectTable::getInstance<TestRuntimeObject2>() == ro2);

            Singleton<RuntimeObjectTable>::getInstance()->clear();

            TEST_CASE("5","デストラクタは実行されていない (ro)", !TestSuite::isReached("TestRuntimeObject::~TestRuntimeObject"));
            TEST_CASE("6","デストラクタは実行されている (ro2)", TestSuite::isReached("TestRuntimeObject2::~TestRuntimeObject2"));
            TEST_CASE("7","登録が解除されている", RuntimeObjectTable::getInstance<TestRuntimeObject>() == 0);
            TEST_CASE("8","登録が解除されている", RuntimeObjectTable::getInstance<TestRuntimeObject2>() == 0);

            delete ro;
        } END_CASE;

    } END_CASE;

    Singleton<RuntimeObjectTable>::restoreContext(context);
}

#endif

