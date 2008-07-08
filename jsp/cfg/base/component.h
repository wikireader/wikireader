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
 *  @(#) $Id: component.h,v 1.8 2003/12/20 06:51:58 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/component.h,v 1.8 2003/12/20 06:51:58 takayuki Exp $

#include "base/testsuite.h"
#include "base/singleton.h"
#include "base/except.h"
#include "base/message.h"
#include "base/option.h"
#include "base/collection.h"
#include "base/directorymap.h"

#include <list>
#include <map>

#ifndef COMPONENT_H
#define COMPONENT_H

class Component : public RuntimeObject
{
public:
        //コンポーネントの起動優先度
    enum tagProcessStage {
        INITIALIZE,                 //とにかく真っ先に動きたい処理

        BEFORE_PARSE,               //パース前
        PARSER,                     //パーサ
        AFTER_PARSE,                //パース後

        PREDECESSOR,                //汎用の前
        GENERAL_PURPOSE,            //汎用
        SUCCESSOR,                  //汎用の後

        BEFORE_CODEGENERATION,      //コード生成前
        CODEGENERATOR,              //コード生成
        AFTER_CODEGENERATION,       //コード生成後

        FINALIZE
    };

        //コンポーネントに共通のデータを持つ/操作するクラス
    class ComponentInfo
    {
    protected:
        std::string                     banner;
        std::multimap<int, Component *> componentList;
        std::list<Component *>          activatedComponentList;

            //コンストラクタ (テスト用)
        ComponentInfo(void) throw() {}
    public:
            //コンストラクタ
        SINGLETON_CONSTRUCTOR(ComponentInfo) {}

            //コンポーネントの追加
        bool addComponent(Component * component, int order) throw();

            //コンポーネントの登録解除
        void removeComponent(Component * component, int order) throw();     //特定の優先度だけを解除
        void removeComponent(Component * component) throw();                //全て解除

            //コンポーネントの起動要求
        bool activateComponent(Component * component) throw();

            //オプションのパース
        void parseOption(OptionParameter & option);

            //コンポーネントの起動
        void activateComponent(void);

            //異常処理ハンドラの起動
        void onFatalExit(void);

            //コンポーネントTが起動しているかを判定
        template<class T>
        bool isActive(void) const throw()
        {
            std::list<Component *>::const_iterator scope;

            scope = activatedComponentList.begin();
            while(scope != activatedComponentList.end()) {
                if(dynamic_cast<T *>(*scope) != 0)
                    return true;
                ++ scope;
            }
            return false;
        }

            //バナー変数へのアクセサ
        inline void setBanner(std::string src)
        {   banner = src;    }
        inline std::string getBanner(void) const
        {   return banner;   }


        TESTSUITE_PROTOTYPE(main)
    };


protected:
    static bool is_valid;       //コンポーネントの初期化に成功したかどうかを持つフラグ

    /*
     *   作業関数
     */

        //コンポーネントの起動
    inline bool activateComponent(void) throw()
    {   return Singleton<ComponentInfo>::getInstance()->activateComponent(this);   }

        //一般的なオプションの処理
    static void _parseCommonOption(OptionParameter & option) throw();


        //自分に先行するコンポーネントの起動判定
    template<class T>
    static bool isActive(void) throw()
    {   
        bool result = false;
        ComponentInfo * info = Singleton<ComponentInfo>::getInstance(std::nothrow);
        if(info != 0)
            result = info->isActive<T>();
        return result;
    }


        /*
         *   過去との互換性のためのパラメータ操作API
         */

    static bool findOption (const char * key1, const char * key2 = 0, std::string * = 0) throw();
    static bool checkOption(const char * key1, const char * key2 = 0) throw();
    static OptionParameter::OptionItem getOption  (const char * key1, const char * key2 = 0, bool dirty = true) throw();
    static OptionParameter::OptionItem mergeOption(const char * key1, const char * key2 = 0) throw();

public:
        //コンストラクタ
    Component(int order = GENERAL_PURPOSE) throw();

        //デストラクタ
    virtual ~Component(void) throw();

        //有効判定
    static bool isValid(void) throw()
    {   return is_valid && SingletonBase::isValid();   }

        //コンポーネントの実行
    static void executeComponents(OptionParameter & option);
    static void executeComponents(int argc, char * argv []);

        //バナーアクセサ
    static void        setBanner(std::string) throw(std::bad_alloc);
    static std::string getBanner(void)        throw(std::bad_alloc);

    /*
     *   イベントハンドラ
     */

        //オプションのパース
    virtual void parseOption(OptionParameter &)
    { parseOption(*Singleton<Directory>::getInstance()); }
    
    virtual void parseOption(Directory &) {}

        //コンポーネントの本体
    virtual void body(void)        { body(*Singleton<Directory>::getInstance()); }
    virtual void body(Directory &) {}

        //異常終了ハンドラ
    virtual void onFatalExit(void) {}



    TESTSUITE_PROTOTYPE(main)
};


#endif



