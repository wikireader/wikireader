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
 *  @(#) $Id: except.h,v 1.9 2003/12/20 06:51:58 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/except.h,v 1.9 2003/12/20 06:51:58 takayuki Exp $

#ifndef EXCEPT_H
#define EXCEPT_H

#ifdef _MSC_VER
#  pragma warning(disable:4290) //C++ の例外の指定は無視されます。関数が __declspec(nothrow) でないことのみ表示されます。
#endif

//#include "testsuite.h"

#include <string>
#include <typeinfo>

    //例外基底クラス
class Exception
{
private:
    static bool is_throwable;

protected:
    int         code;       //例外コード
    std::string classname;  //この例外を生成したクラスの名前 (識別用)
    std::string details;    //例外の説明

        //コンストラクタ
    Exception(std::string classname = "Exception", int code = 0, std::string details = "") throw();
    Exception(std::string classname, std::string details) throw();

        //デストラクタ
public:
    Exception(const Exception & src) throw();

    virtual ~Exception(void) throw();

        //is-an-instance-of関係の評価
    inline bool isInstanceOf(const char * _classname) const throw()
    {   return classname.compare(_classname) == 0;   }

        //例外コードを取得する
    inline int getCode(void) const throw()
    {   return code;   }

        //例外の説明を取得する
    inline std::string getDetails(void) const throw()
    {   return details;   }

        //現在の例外制御フラグの状態に従って例外を発行する
    inline bool throwException(void)
    {
        if(is_throwable)
            throw *this;
        return is_throwable;
    }

        //is_throwableのアクセサ
    static void setThrowControl(bool _throwable) throw()
    {   is_throwable = _throwable;   }

    static bool getThrowControl(void) throw()
    {   return is_throwable;   }


//  TESTSUITE_PROTOTYPE(main)
};


#define EXCEPTION(x)       class x : public Exception { public: x(void) throw() : Exception(#x)       {} };
#define EXCEPTION_(x,y)    class x : public Exception { public: x(void) throw() : Exception(#x, y)    {} };
#define EXCEPTION__(x,y,z) class x : public Exception { public: x(void) throw() : Exception(#x, y, z) {} };

#endif



