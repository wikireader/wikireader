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
 *  @(#) $Id: jsp_common.h,v 1.8 2004/09/06 15:00:49 honda Exp $
 */

// $Header: /home/CVS/configurator/jsp/jsp_common.h,v 1.8 2004/09/06 15:00:49 honda Exp $


#ifndef JSP_COMMON_H
#define JSP_COMMON_H

#include "base/testsuite.h"

#include "base/mpstrstream.h"
#include "base/collection.h"
#include "base/except.h"
#include "base/event.h"

#include <string>


namespace ToppersJsp {

        //カーネル構成ファイル
    class SpecialtyFile : public MultipartStream, public RuntimeObject
    {
    protected:
        SpecialtyFile(void) throw();

        template<class T>
        static T * createInstance(std::string filename) throw(Exception)
        {
            T * old;
            T * result = 0;

                //ROT登録解除 オブジェクト削除 (唯一性の保障 (だけどシングルトンではない) )
            old = dynamic_cast<T *>(RuntimeObjectTable::getInstance(typeid(T)));
            if(old != 0)
                delete old;

            result = new(std::nothrow) T;
            if(result == 0)
                ExceptionMessage("Internal error : Object creation failure [ToppersJsp::KernelCfg::createKernelCfg]","内部エラー : オブジェクト生成時エラー [createKernelCfg]") << throwException;

            try {
                result->setFilename(filename);
            }
            catch(...) {
                if(result != 0)
                    delete result;
                throw;
            }
            return result;
        }

    };


    /*
     *  Visual C++ 6.0 : fatal error C1001: INTERNAL COMPILER ERROR (msc1.cpp:1794) 対策 
     *    (というより、多重登録しても古いインスタンスの登録チェックをしない)
     */
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#  define SPECIALTY_FILE(x)                                                             \
    class x : public SpecialtyFile {                                                    \
        public:                                                                         \
            struct CreationEvent { class x * file; };                                   \
            static inline x * createInstance(std::string filename) throw(Exception)     \
            {                                                                           \
                x * instance = new(std::nothrow) x;                                     \
                if(instance != 0)                                                       \
                    instance->setFilename(filename);                                    \
                return instance;                                                        \
            }                                                                           \
    }
#else
#  define SPECIALTY_FILE(x)                                                             \
    class x : public SpecialtyFile {                                                    \
        public:                                                                         \
            struct CreationEvent { class x * file; };                                   \
            static inline x * createInstance(std::string filename) throw(Exception)     \
            {   return SpecialtyFile::createInstance<x>(filename);   }                  \
    }
#endif

        //TOPPERS/JSPのコンフィギュレーションで出てくる特殊なファイル
    SPECIALTY_FILE(KernelCfg);
    SPECIALTY_FILE(KernelID);

        //バージョン間を越えた変換など
    std::string conv_includefile(std::string file) throw(); //インクルードファイル名
    std::string conv_kernelobject(std::string obj) throw(); //カーネルの変数名

        //定数など
//  std::string get_agreement(std::string filename = "") throw();   //文言の取得
}

#endif

