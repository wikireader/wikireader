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
 *  @(#) $Id: garbage.cpp,v 1.6 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/garbage.cpp,v 1.6 2003/12/15 07:32:13 takayuki Exp $

#include "base/garbage.h"

#include <stdexcept>
#include <algorithm>

using namespace std;

TrashBox * TrashBox::current_box = 0;

//----------------------------------------------------------------
// Garbage : ゴミ

    //コンストラクタ
Garbage::Garbage(void) throw()
{
        //ゴミ箱に関連付ける
    assigned_box = TrashBox::getCurrentTrashBox();
    if(assigned_box->isValid())
        cookie = assigned_box->addGarbage(this);
}

    //デストラクタ
Garbage::~Garbage(void) throw()
{
    rescue();
}

    //ゴミ救出
void Garbage::rescue(void) throw()
{
    if(assigned_box->isValid()) {
        assigned_box->recoverGarbage(cookie);
        assigned_box = 0;
    }
}


//----------------------------------------------------------------
// TrashBox : ゴミを入れる箱

TrashBox::TrashBox(void) throw()
{
        //ゴミ箱の差し替え
    previous_box = current_box;
    current_box  = this;
}

TrashBox::~TrashBox(void) throw()
{
        //ゴミがなくなるまで削除
    while(!garbage.empty()) {
            //ふた
        try{   cleanup();   }
        catch(...) {}
    }

        //ゴミ箱の差し替え
    current_box = previous_box;
}

/*
    //ゴミ箱に入れたゴミを取り除く
void TrashBox::recoverGarbage(Garbage * _garbage, TrashBox::Cookie cookie) throw()
{
    if(isValid() && _garbage != 0) {
        bool forward = true;
        list<Garbage *>::iterator scope;

        if(!garbage.empty()) {
            scope = garbage.erase(cookie);

                //自分が始末した
            if(scope != garbage.end() || garbage.empty())
                forward = false;
        }

            //親ゴミ箱に回送
        if(forward && previous_box->isValid())
            previous_box->recoverGarbage(_garbage, cookie);
    }
}
*/
    /*  修正時のメモ 
              関連付けられたゴミ箱が消えることは無い(生成期間はゴミ箱のほうが長いはず)ので、親に回送する必要は無い。よってif(forward...節は不要。
              自分が始末できないゴミは無いので(cleanupはrecoverGargabeを呼ばない)、eraseの返却値のチェックは不要。
              個別削除要求はゴミから出されるので、実行された時点でゴミは1つ以上存在するはずなので、emptyチェックは不要.
              この時点でeraseだけになり、第一引数は不要。
    */

    //ゴミ箱に入れたゴミを取り除く  
void TrashBox::recoverGarbage(TrashBox::Cookie cookie) throw()
{   garbage.erase(cookie);  }

    //ゴミ箱を空にする
void TrashBox::cleanup(void)
{
        //自分がトップレベルゴミ箱でなかったら失敗
    if(current_box != this)
        throw std::runtime_error("TrashBox::cleanup can be performed from the top level trash box only.");

    try {
        while(!garbage.empty())
            delete *garbage.begin();        //ゴミリストから要素を外すのは子の役目
    }
    catch(...) {
        garbage.erase(garbage.begin());     //例外を起こした最初の要素を削除
        throw;                              //再送
    }
}



/****************************************** テストスィート ******************************************/

#ifdef TESTSUITE
#include "coverage_undefs.h"

namespace { int counter = 0; }

#ifdef _MSC_VER
    class DummyGarbage : public Garbage
    {
    public:
        int * count;
        bool throw_exception;

        DummyGarbage(int * _count = 0) : count(_count), throw_exception(false)
        { TestSuite::check("DummyGarbage::DummyGarbage");  }

        ~DummyGarbage(void) throw(int)
        {
            if(count != 0) *count = ++counter;
            if(throw_exception) throw 0;
            TestSuite::check("DummyGarbage::~DummyGarbage"); 
        }

    };
#elif __GNUC__
    class DummyGarbage : public Garbage
    {
    public:
        int * count;

        DummyGarbage(int * _count = 0) : count(_count)
        { TestSuite::check("DummyGarbage::DummyGarbage");  }

        ~DummyGarbage(void) throw()
        {
            if(count != 0) *count = ++counter;
            TestSuite::check("DummyGarbage::~DummyGarbage"); 
        }
    };
#endif

TESTSUITE(main, TrashBox)
{
    BEGIN_CASE("1","ゴミ箱を作ると登録される") {
        TrashBox mybox;
        TEST_CASE("1", "作ったゴミ箱が現在のゴミ箱になっている", TrashBox::current_box == &mybox);
        
        {
            TrashBox mybox2;
            TEST_CASE("2", "作ったゴミ箱が現在のゴミ箱になっている (2)", TrashBox::current_box == &mybox2);
            TEST_CASE("3", "もともとのゴミ箱が保存されている", mybox2.previous_box == &mybox);
        }

        TEST_CASE("4", "もとのゴミ箱に戻る", TrashBox::current_box == &mybox);
    } END_CASE;

    BEGIN_CASE("2","isValid") {
        TrashBox mybox;

        TEST_CASE("1","作ったゴミ箱は正常", mybox.isValid());
        TEST_CASE("2","NULL箱は異常", !((TrashBox *)0)->isValid());
    } END_CASE;

    BEGIN_CASE("3","operator new") {
        BEGIN_CASE("1","new TrashBoxはbad_alloc例外を返す") {
            bool result = false;

            try { TrashBox * box = new TrashBox; }
            catch(bad_alloc) { result = true; }

            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","new(nothrow) TrashBoxはNULLを返す") {
            bool result = true;
            TrashBox * box;

            try { box = new(nothrow) TrashBox; }
            catch(...) { result = false; }

            TEST_CASE("1", "new(nothrow)は例外を返さない", result);
            TEST_CASE("2", "new(nothrow)はNULLを返す",   box == 0);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("4","基本的な生成削除") {
        BEGIN_CASE("1","ちゃんとゴミ箱から外せる") {
            TrashBox mybox;

            DummyGarbage * garbage = new DummyGarbage;

            TEST_CASE("0","[前提] ゴミが入っている", std::find(mybox.garbage.begin(), mybox.garbage.end(), garbage) != mybox.garbage.end());
            delete garbage;
            TEST_CASE("1","ゴミが消えている", std::find(mybox.garbage.begin(), mybox.garbage.end(), garbage) == mybox.garbage.end());


        } END_CASE;

        BEGIN_CASE("2","親のゴミ箱に入っているものもゴミ箱から外せる") {
            TrashBox mybox;
            DummyGarbage * garbage = new DummyGarbage;
            TEST_CASE("0","[前提] ゴミが入っている", find(mybox.garbage.begin(), mybox.garbage.end(), garbage) != mybox.garbage.end());

            TrashBox secondbox;
            delete garbage;

            TEST_CASE("1","ゴミが消えている", find(mybox.garbage.begin(), mybox.garbage.end(), garbage) == mybox.garbage.end());
           
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("5","TrashBox::cleanup") {
        BEGIN_CASE("1","動的に作ったオブジェクトが破棄できる") {
            TrashBox mybox;
            DummyGarbage * garbage;

            TestSuite::clearCheckpoints();

            garbage = new DummyGarbage;
            TEST_CASE("0","[前提] コンストラクタが起動されている", TestSuite::isReached("DummyGarbage::DummyGarbage"));

            mybox.cleanup();
            TEST_CASE("1","デストラクタが起動されている", TestSuite::isReached("DummyGarbage::~DummyGarbage"));
        } END_CASE;

#ifdef _MSC_VER
        BEGIN_CASE("2","例外はもれる") {
            TrashBox mybox;
            DummyGarbage * garbage;

            TestSuite::clearCheckpoints();

            garbage = new DummyGarbage;
            garbage->throw_exception = true;

            bool result = false;
            try { mybox.cleanup(); }
            catch(...) { result = true; }

            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","例外を起こしたオブジェクトが破壊されている (2重破棄にならない)") {
            TrashBox mybox;
            DummyGarbage * garbage;
            DummyGarbage * garbage2;

            TestSuite::clearCheckpoints();

            garbage = new DummyGarbage;
            garbage->throw_exception = true;
            garbage2 = new DummyGarbage;
            garbage2->throw_exception = true;

            try { mybox.cleanup(); }
            catch(...) {}
            try { mybox.cleanup(); }    //ここでAccessViolationが起こらない
            catch(...) {}

            if(!mybox.garbage.empty())
                TEST_FAIL;
        } END_CASE;
#endif

        BEGIN_CASE("4","削除の順序が正しい") {
            TrashBox mybox;
            DummyGarbage * garbage;
            DummyGarbage * garbage2;
            DummyGarbage * garbage3;
            int g  = 0;
            int g2 = 0;
            int g3 = 0;

            TestSuite::clearCheckpoints();

            garbage  = new DummyGarbage(&g);
            garbage2 = new DummyGarbage(&g2);
            garbage3 = new DummyGarbage(&g3);

            mybox.cleanup();

            TEST_CASE("1","最初に登録されたものは最後に削除",g == 3);
            TEST_CASE("2","次に登録されたものは2番目に削除",g2 == 2);
            TEST_CASE("3","次に登録されたものは最初に削除",g3 == 1);
        } END_CASE;

        BEGIN_CASE("5","トップレベルでないゴミ箱はcleanupできない") {
            TrashBox outerbox;
            TrashBox innerbox;

            bool result = false;
            try {   outerbox.cleanup();   }
            catch(std::runtime_error)
            {   result = true;   }

            if(!result)
                TEST_FAIL;
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("6","デストラクタによる破棄") {
        BEGIN_CASE("1","動的に作ったオブジェクトが破棄できる (TrashBox::~TrashBox)") {
            {
                TrashBox mybox;
                DummyGarbage * garbage;

                TestSuite::clearCheckpoints();

                garbage = new DummyGarbage;
                TEST_CASE("0","[前提] コンストラクタが起動されている", TestSuite::isReached("DummyGarbage::DummyGarbage"));
            }
            TEST_CASE("1","デストラクタが起動されている", TestSuite::isReached("DummyGarbage::~DummyGarbage"));
        } END_CASE;

        BEGIN_CASE("2","例外はもれない") {
            bool result = true;
            try{
                TrashBox mybox;
                DummyGarbage * garbage;

                TestSuite::clearCheckpoints();

                garbage = new DummyGarbage;
                TEST_CASE("0","[前提] コンストラクタが起動されている", TestSuite::isReached("DummyGarbage::DummyGarbage"));
            }
            catch(...)
            { result = false; }
            TEST_CASE("1","例外はもれない", result);
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("7","rescue") {
        DummyGarbage * garbage;
        {
            TrashBox mybox;
            garbage = new DummyGarbage;
            garbage->rescue();

            TestSuite::clearCheckpoints();
        }
        TEST_CASE("1","rescueしたゴミは削除されない", !TestSuite::isReached("DummyGarbage::~DummyGarbage"));
        delete garbage;
    } END_CASE;

    BEGIN_CASE("8","静的なオブジェクトが多重破棄されない") {
        TrashBox outerbox;
        {
            DummyGarbage garbage;
            TrashBox innerbox;
            DummyGarbage garbage2;

            TEST_CASE("0","[前提] コンストラクタが起動されている", TestSuite::isReached("DummyGarbage::DummyGarbage"));
        }   //ここで2重破棄でMACVにならない
    } END_CASE;
}

#endif



