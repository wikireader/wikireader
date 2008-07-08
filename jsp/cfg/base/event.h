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
 *  @(#) $Id: event.h,v 1.9 2004/09/06 15:00:49 honda Exp $
 */

// $Header: /home/CVS/configurator/base/event.h,v 1.9 2004/09/06 15:00:49 honda Exp $

#ifndef EVENT_H
#define EVENT_H

#include "base/testsuite.h"
#include "base/singleton.h"

#include <list>

    //イベントフックの取りまとめ役
template<typename T>
class Event
{
public:
    typedef void (*handler_type)(T &);

    class handler_list_type : public std::list<handler_type>
    {   public: SINGLETON_CONSTRUCTOR(handler_list_type) throw() {}   };

    /*
     *   クラスの中のハンドラ用
     */

    class Handler
    {
    friend class Event;
    public:
        class instance_list_type : public std::list<Handler *>
        {   public: SINGLETON_CONSTRUCTOR(instance_list_type) throw() {}   };

        virtual void handler(T & ev) = 0;

    protected:
            //登録
        Handler(void) throw()
        {   Singleton<instance_list_type>::getInstance()->push_back(this);   }
        
            //登録解除
        virtual ~Handler(void) throw()
        {
            typename Event<T>::Handler::instance_list_type * list = Singleton<instance_list_type>::getInstance(std::nothrow);
            typename Event<T>::Handler::instance_list_type::iterator scope;

            if(list != 0) {
                scope = list->begin();
                while(scope != list->end()) {
                    if(*scope == this) {
                        typename Event<T>::Handler::instance_list_type::iterator target = scope;
                        ++ scope;
                        list->erase(target);
                    }
                    else
                        ++ scope;
                }
            }
        }
    };

public:
    typedef T value_type;

    Event(void) {}
    virtual ~Event(void) {}

    /*
     *  スタティックなハンドラ用
     */

        //イベントにハンドラを追加
    inline static void add(handler_type func) throw(std::bad_alloc)
    {   Singleton<handler_list_type>::getInstance()->push_back(func);   }

        //イベントからハンドラを削除
    static void remove(handler_type func) throw()
    {
        typename Event<T>::handler_list_type * list;
        typename Event<T>::handler_list_type::iterator scope;

        list = Singleton<handler_list_type>::getInstance(std::nothrow);
        if(list != 0) {
            scope = list->begin();
            while(scope != list->end()) {
                if(*scope == func) {
                    typename handler_list_type::iterator target = scope;
                    ++ scope;
                    list->erase(target);
                }
                else
                    ++ scope;
            }
        }
    }

    /*
     *  共通操作
     */
        //イベントの発生
    static void raise(T & ev)
    {
            typedef typename Event<T>::handler_list_type handler_list_type_t; 
            typedef typename Event<T>::Handler::instance_list_type instance_list_type_t;

        /* 登録された静的な関数にイベントを発行 */ {
           handler_list_type_t * list;
            typename handler_list_type_t::iterator scope;

            list = Singleton<handler_list_type_t>::getInstance();   //throw(bad_cast)

            scope = list->begin();
            while(scope != list->end()) {
                (**scope)(ev);
                ++ scope;
            }
        }

        /* 登録されたクラスインスタンスにイベントを発行 */ {
            instance_list_type_t * list;
            typename instance_list_type_t::iterator scope;

            list = Singleton<instance_list_type_t>::getInstance();  //throw(bad_cast)

            scope = list->begin();
            while(scope != list->end()) {
                (*scope)->handler(ev);
                ++ scope;
            }
        }
    }

        //イベントの発生 (引数なし)
    inline static void raise(void)
    {
        T ev;
        raise(ev);
    }
};


    /*
     *  汎用イベント
     */

    //コンストラクタが起動し終わったあと、真っ先に起るイベント
class StartupEvent {};


    //成功失敗を問わず、プログラムが終わるときに起るイベント
struct ShutdownEvent
{
    int exit_code;
};

#endif



