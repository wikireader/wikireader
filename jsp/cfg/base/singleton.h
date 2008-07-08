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
 *  @(#) $Id: singleton.h,v 1.12 2004/09/06 15:00:49 honda Exp $
 */

// $Header: /home/CVS/configurator/base/singleton.h,v 1.12 2004/09/06 15:00:49 honda Exp $

#include "base/testsuite.h"

#ifndef SINGLETON_H
#define SINGLETON_H

#ifdef _MSC_VER
#  pragma warning(disable:4290) //C++ の例外の指定は無視されます。関数が __declspec(nothrow) でないことのみ表示されます。
#  pragma warning(disable:4291) //初期化コ−ドが例外をスロ−すると、'new' 演算子を使用しているとメモリを解放しません。
#  pragma warning(disable:4786) //デバッグ情報で識別子が 255 文字に切り捨てられました。
#endif

#include <new>
#include <stdexcept>

/*
 *  シングルトンパターン 実装
 */

    //シングルトン基底クラス
    //  #gccがfriend class Singletonを食ってくれないので基底を作る
class SingletonBase {
public:
        //デストラクタのチェーン
    struct chain {
        struct chain * next;
        void (*destructor)(void);
    };

        //シングルトンでインスタンスを生成したことを示すためのクラス
        //    このクラスはSingletonからしか実体化できないので、
        //    ほかのクラスからこれを引数にもつコンストラクタを使用することはできない
    class SingletonConstructionLocker
    {
    friend class SingletonBase;
    protected:
        SingletonConstructionLocker() {}
    };

private:
    static bool   initialize;       //初期化済みか (atexitを呼んだかどうか)
    static bool   is_valid;         //生成に失敗したシングルトンガあるかどうか
    static struct chain * top;      //デストラクタチェーン

        //デストラクタの起動
    static void final_release(void) throw()
    {
            //先頭から削除 (先入れ後出し)
        while(top != 0) {
            struct chain * node = top;
            top = node->next;

            (*node->destructor)();

            delete node;
        }
    }

protected:
        //チェーンにデストラクタを追加
    static bool addDestructor(void (* destructor)(void)) throw()
    {
        bool result = false;
        struct SingletonBase::chain * node = new(std::nothrow) struct SingletonBase::chain;
        if(node != 0) {

                //新しい要素を先頭に追加
            node->next       = top;
            node->destructor = destructor;
            
            top = node;

            result = true;

                //要素破棄のために最後に破棄ルーチンを起動させる
            if(!initialize) {
                atexit(final_release);
                initialize = true;
            }
        }
        return result;
    }

        //SingletonConstructionLockerファクトリ
    inline static SingletonConstructionLocker getLocker(void)
    {   return SingletonConstructionLocker();   }

public:
        //生成に失敗したシングルトンがないことを取得
    static bool isValid(void) throw()
    {   return is_valid;   }

#ifdef TESTSUITE
        /*
         *   コンテキストチェーン (テスト用シングルトンコンテキスト退避チェーン)
         */
    class ContextChain {
    protected:
        struct chain {
            struct chain * next;
            void * context;
            void (*restore)(void * & context) throw();
            void (*renew)(void) throw();
            void (*clear)(void);
        } * top;

    public:
        ContextChain(void) throw() : top(0)
        {}

        ~ContextChain(void) throw()
        {   restoreContext();   }

            /* コンテキストの復帰 */
        void restoreContext(void) throw()
        {
            while(top != 0) {
                struct chain * node = top;
                top = top->next;

                (*node->restore)(node->context);
                delete node;
            }
        }

            /* 全てのインスタンスを再生成 */
        void renewInstance(void) throw(std::bad_alloc)
        {
            struct chain * node = top;
            while(node != 0) {
                (*node->renew)();
                node = node->next;
            }
        }
            
            /* 全てのインスタンスを破棄 */
        void clearInstance(void) throw(std::bad_alloc)
        {
            struct chain * node = top;
            while(node != 0) {
                (*node->clear)();
                node = node->next;
            }
        }
            
            /* コンテキストの退避 */
        template<class T> void saveContext(void) throw(std::bad_alloc)
        {
            struct chain * node = new struct chain;

            node->next = top;
            node->restore = reinterpret_cast<void(*)(void*&)>(Singleton<T>::restoreContext);
            node->renew   = Singleton<T>::renewInstance;
            node->clear   = Singleton<T>::clearInstance;

            Singleton<T>::saveContext(reinterpret_cast<Singleton<T>::Context &>(node->context));

            top = node;
        }
    };
#endif


    TESTSUITE_PROTOTYPE(main)
};


template<class T>
class Singleton : SingletonBase
{
protected:
    Singleton(void);
    ~Singleton(void);

    static T * instance;

        //デストラクタ
    static void destructor(void) throw()
    {
        if(instance != 0) {
            try{ delete instance; } catch(...) {}
            instance = 0;
        }
    }

        //インスタンスの生成 (インスタンス生成 + デストラクタ登録 + エラー処理)
    static T * createInstance(std::nothrow_t) throw()
    {
        T * instance = 0;
        try {
            instance = new(std::nothrow) T(getLocker());

            if(instance != 0) {
                if(!addDestructor(destructor))
                    throw false;    //catch節を実行させたい
            }
        }
        catch(...) {
            if(instance != 0) {
                try { delete instance; } catch(...) {}
                instance = 0;
            }
        }
        return instance;
    }

        //インスタンス生成 (bad_alloc例外をスローする実装)
    static T * createInstance(void) throw(std::bad_alloc)
    {
        instance = createInstance(std::nothrow);
        if(instance == 0)
            throw std::bad_alloc();
        return instance;
    }

public:
        //インスタンスの取得
    inline static T * getInstance(void) throw(std::bad_alloc)
    {
        if(instance == 0)
            instance = createInstance();
        return instance;
    }

        //インスタンスの取得
    inline static T * getInstance(std::nothrow_t) throw()
    {
        if(instance == 0)
            instance = createInstance(std::nothrow);
        return instance;
    }

#ifdef TESTSUITE    /* テスト用 */

    typedef T * Context;    //退避用

        //新しいインスタンスの生成
    static void renewInstance(void) throw(std::bad_alloc)
    {
        destructor();                     //今のインスタンスを破棄
        instance = new T(getLocker());    //新しいインスタンスを生成 (デストラクタが登録されてしまうのでcreateInstanceを呼んではいけない)
    }

        //インスタンス破棄
    static void clearInstance(void) throw()
    {   destructor();   }

        //コンテキスト退避
    static void saveContext(Context & context) throw()
    {
        context  = instance;
        instance = 0;
    }

        //コンテキスト復帰
    static void restoreContext(Context & context) throw()
    {
        destructor();           //今のインスタンスを破棄
        instance = context;     //退避したインスタンスを復帰
    }

#endif /* TESTSUITE */

};



    //Singleton 静的変数
template<class T> T * Singleton<T>::instance = 0;

#define SINGLETON_CONSTRUCTOR(x)  explicit x(const SingletonBase::SingletonConstructionLocker &)
#define SINGLETON_CONSTRUCTOR_(x) explicit x(const SingletonBase::SingletonConstructionLocker & _singleton)

#define SINGLETON_WRAPPER(derived, base) class derived : public base { public: SINGLETON_CONSTRUCTOR_(derived) throw() : base(_singleton) {} };

#endif /* SINGLETON_H */

