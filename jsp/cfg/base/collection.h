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
 *  @(#) $Id: collection.h,v 1.6 2003/12/20 06:51:58 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/collection.h,v 1.6 2003/12/20 06:51:58 takayuki Exp $

#ifndef COLLECTION_H
#define COLLECTION_H

#include "base/testsuite.h"
#include "base/singleton.h"

#include <typeinfo>
#include <list>

    //Collectionに登録したいオブジェクトのクラスの基底クラス
class Collectable
{
public:
             Collectable(void) throw() {}   //特に何もしない
    virtual ~Collectable(void) throw() {}   //特に何もしない
};



    //ある型のインスタンスを登録して保持するためのクラス
class Collection
{
public:
    struct Element
    {
        Collectable * instance;
        bool          destruction;
    };

protected:
    std::list<Element> container;

        //predecessorの次を指すイテレータを得る (getInstance)
    std::list<Element>::const_iterator _findInstance(const Collectable * predecesor) const throw();

public:
        //コンストラクタ & デストラクタ
             Collection(void)  throw();
    virtual ~Collection(void) throw();

        //有効判定
    inline bool isValid(void) const throw()
    {   return this != 0;   }

        //コレクションへの追加 (ポインタ用)
    bool addInstance(Collectable * instance, bool destruction = true)  throw();
    
        //コレクションへの追加 (実体用)
    inline bool addInstance(Collectable & instance, bool destruction = false) throw()
    {   return addInstance(&instance, destruction);   }

        //コレクションからの取得 (完全一致)
    Collectable * getInstance(const std::type_info & type, const Collectable * predecessor = 0) const throw();

        //コレクションからの取得 (派生したものもサーチ)
    template<class T> T * getInstance(const Collectable * predecessor = 0) const throw()
    {
        T * result = 0;

        if(isValid()) {
            std::list<Element>::const_iterator iterator = _findInstance(predecessor);

            while(iterator != container.end()) {
                result = dynamic_cast<T *>(iterator->instance);
                if(result != 0)
                    break;
                ++ iterator;
            }
        }
        return result;
    }

        //コレクションから派生先を含む全てのクラスインスタンスを削除 (破棄対象の場合は破棄も行う)
    template<class T> void deleteInstance(void) throw()
    {
        if(isValid()) {
            std::list<Element>::iterator iterator;

            iterator = container.begin();
            while(iterator != container.end()) {
                if(dynamic_cast<T *>(iterator->instance) != 0) {

                        //削除対象ならインスタンスを削除
                    if(iterator->destruction)
                        delete iterator->instance;

                        //リストから除外
                    std::list<Element>::iterator target = iterator;
                    ++ iterator;
                    container.erase(target);
                }
                else
                    ++ iterator;
            }
        }
    }

        //関連付けられたインスタンスを入れ替える
    template<class T> inline bool replaceInstance(Collectable * instance, bool destruction = true) throw()
    {
            //自分と無関係なクラスはインデックスとして指定できない
        if(dynamic_cast<T *>(instance) == 0)
            return false;

        deleteInstance<T>();
        return addInstance(instance, destruction);
    }

        //関連付けられたインスタンスを入れ替える
    template<class T> inline bool replaceInstance(Collectable & instance, bool destruction = false) throw()
    {
            //自分と無関係なクラスはインデックスとして指定できない
        if(dynamic_cast<T *>(&instance) == 0)
            return false;

        deleteInstance<T>();
        return addInstance(&instance, destruction);
    }


        //コレクションからインスタンスを除外 (破棄はしない)
    bool removeInstance(const Collectable * instance) throw();

        //全要素の破棄
    void clear(void);

    TESTSUITE_PROTOTYPE(main)
};


    //実行時オブジェクト参照テーブル
class RuntimeObjectTable : protected Collection
{
friend class RuntimeObject;
public:
    SINGLETON_CONSTRUCTOR(RuntimeObjectTable) throw() {}

    static Collectable * getInstance(const std::type_info & type, const Collectable * predecessor = 0) throw()
    {
        RuntimeObjectTable * table = Singleton<RuntimeObjectTable>::getInstance(std::nothrow);
        Collectable * result = 0;

        if(table != 0)
            result = table->Collection::getInstance(type, predecessor);
        return result;
    }

        //コレクションからの取得 (派生したものもサーチ)
    template<class T> static T * getInstance(const Collectable * predecessor = 0) throw()
    {
        RuntimeObjectTable * table = Singleton<RuntimeObjectTable>::getInstance();
        T * result = 0;

        if(table != 0) {
            std::list<Element>::const_iterator iterator = table->_findInstance(predecessor);

            while(iterator != table->container.end()) {
                result = dynamic_cast<T *>(iterator->instance);
                if(result != 0)
                    break;
                ++ iterator;
            }
        }
        return result;
    }

        /* Visual C++ 6.0 Fatal error C1001対策 */
    template<class T> static T * getInstance(T ** dest, const Collectable * predecessor = 0) throw()
    {
        RuntimeObjectTable * table = Singleton<RuntimeObjectTable>::getInstance();
        T * result = 0;

        if(table != 0) {
            std::list<Element>::const_iterator iterator = table->_findInstance(predecessor);

            while(iterator != table->container.end()) {
                result = dynamic_cast<T *>(iterator->instance);
                if(result != 0)
                    break;
                ++ iterator;
            }
        }
        if(dest != 0)
            *dest = result;

        return result;
    }

    TESTSUITE_PROTOTYPE(main)
};

    //実行時オブジェクト
class RuntimeObject : public Collectable
{
public:
    RuntimeObject(bool destruction = false) throw()
    {
        RuntimeObjectTable * table = Singleton<RuntimeObjectTable>::getInstance();
        if(table != 0)
            table->addInstance(this, destruction);
    }

    virtual ~RuntimeObject(void) throw()
    {
        RuntimeObjectTable * table = Singleton<RuntimeObjectTable>::getInstance();
        if(table != 0)
            table->removeInstance(this);
    }
};

    /* 呪 fatal error C1001: INTERNAL COMPILER ERROR (msc1.cpp:1794) */
#if _MSC_VER < 1300 
#  define getRuntimeObjectInstance(x) (dynamic_cast<x *>(RuntimeObjectTable::getInstance(typeid(x))))       //正確には透過ではないが、これで逃げるしかない
#else
#  define getRuntimeObjectInstance(x) (RuntimeObjectTable::getInstance<x>())
#endif


#endif



