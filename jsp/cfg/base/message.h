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
 *  @(#) $Id: message.h,v 1.5 2003/12/20 06:51:58 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/message.h,v 1.5 2003/12/20 06:51:58 takayuki Exp $

#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef _MSC_VER
#  pragma warning(disable:4290) //C++ の例外の指定は無視されます。関数が __declspec(nothrow) でないことのみ表示されます。
#  pragma warning(disable:4786) //デバッグ情報で識別子が255文字に切り捨てられました。
#endif

#include <string>
#include <sstream>
#include <stdarg.h>
#include <stdexcept>

//#include "base/testsuite.h"
#include "base/singleton.h"
#include "base/except.h"

    /*
     *   文字列整形ヘルパークラス
     */
class Formatter
{
protected:
    std::string       templatestring;    //テンプレート文字列
    std::stringstream content;           //文字列整形と書式済み文字列可能場所用ストリーム
    bool              accept;            //引数を受入可能かどうか

        //次の引数の挿入先へと移動
    void shift(void) throw();

        //未設定の引数を (null) で置換
    void shift_all(void) throw();

public:
        //デフォルトコンストラクタ
    Formatter(void) throw();

        //コンストラクタ
    Formatter(std::string src) throw();

        //コピーコンストラクタ
    Formatter(const Formatter & src) throw();

        //テンプレート文字列の設定 (中身は消える)
    void assign(std::string src) throw();

        //内容のクリア
    void clear(void) throw();

        //代入演算子
    Formatter & operator =(const Formatter & src) throw();

        //引数の設定
    template<class T>
    Formatter & operator << (const T & src) throw()
    {
        std::string::size_type pos;

            //引数を受入可能なら
        if(accept) {
            pos = content.str().size();
            content << src;

                /* 空文字でなければ次の挿入場所に移動 */
            if(pos != content.str().size())
                shift();
        }
        return *this;
    }

        //内部メッセージの取り出し
    std::string str(void) throw();

        //標準出力ストリームへの出力用 operator << 定義
    friend inline std::ostream & operator << (std::ostream & out, Formatter msg) throw()
    {
        out << msg.content.str();
        return out;
    }

//    TESTSUITE_PROTOTYPE(main)
};

    /*
     *   メッセージ出力用に2言語対応を行った文字列整形ヘルパークラス
     */
class Message : public Formatter
{
public:
    enum tagLanguage
    {
        ENGLISH  = 0,
        JAPANESE = 1,

        LAST_LANG,              //最後の位置
        NEUTRAL  = ENGLISH,     //標準言語
    };

        /* 出力メッセージ言語制御クラス */
    class MessageControl
    {
    protected:
        int language;   //どの言語を使用するのか

    public:
            //コンストラクタ
        SINGLETON_CONSTRUCTOR(MessageControl) throw() : language(NEUTRAL) {};

            //アクセサ
        inline void setLanguage(int _language) throw()  { language = _language; }
        inline int  getLanguage(void) const    throw()  { return language; }

        const char * select(const char * msg1, const char * msg2, va_list vl ) throw();
    };

protected:

        //派生クラス用コンストラクタ
    void _initialize(const char * src1, const char * src2, va_list vl) throw();

public:
        //デフォルトコンストラクタ
    Message(void) throw();

        //コンストラクタ (単一言語)
    Message(std::string src) throw();

        //コンストラクタ (複数言語)
    Message(const char * src1, const char * src2, ... ) throw();

        //コピーコンストラクタ
    Message(const Message & src) throw();

        //テンプレート文字列の設定 (中身は消える)
    void assign(std::string src) throw() { Formatter::assign(src); }
    void assign(const char * src1, const char * src2, ... ) throw();

        //言語選択
    static void selectLanguage(enum tagLanguage lang = NEUTRAL) throw();
        
        //言語の取得
    static enum tagLanguage getCurrentLanguage(void) throw()
    {   return static_cast<enum tagLanguage>(Singleton<MessageControl>::getInstance()->getLanguage());   }

    //TESTSUITE_PROTOTYPE(main)
};


    /*
     *   冗長出力用文字列整形ヘルパークラス
     */
class VerboseMessage : public Message
{
public:
        //共通の冗長出力情報を持つクラス
    class VerboseControl
    {
    public:
        bool           verbose;     //冗長出力を行うかどうか
        std::ostream * out;         //出力先

            //コンストラクタ
        SINGLETON_CONSTRUCTOR(VerboseControl) throw() : verbose(false), out(0)
        {}

            //冗長出力制御
        template<class T>
        inline const VerboseControl & operator << (const T & src) const throw()
        {
            if(verbose && out != 0)
                (*out) << src;
            return *this;
        }

            //verboseアクセサ
        void setVerbose(bool _verbose) throw()
        {   verbose = _verbose;   }
        bool getVerbose(void) const throw()
        {   return verbose;   }

            //outアクセサ
        void setStream(std::ostream * _out) throw()
        {   out = _out;   }
        std::ostream * getStream(void) throw()
        {   return out;   }
    };

        //冗長出力ストリーム (単純な抑止機構つきラッパークラス)
    class VerboseStream
    {
    protected:
        std::ostream * out;

    public:
        VerboseStream(std::ostream * _out = 0) throw() : out(_out) {};

        template<class T>
        VerboseStream & operator << (const T & src) throw()
        {
            if(out != 0)
                (*out) << src;
            return *this;
        }
    };

protected:
        //デフォルトコンストラクタ (テスト用)
    VerboseMessage(void) throw();

public:

        //コンストラクタ
    VerboseMessage(const char * src) throw();
    VerboseMessage(const std::string & src) throw();
    VerboseMessage(const char * src1, const char * src2, ... ) throw();

        //デストラクタ
    ~VerboseMessage(void) throw();

        //冗長出力制御 (アクセサ回送)
    inline static void setVerbose(bool _verbose) throw()
    {   Singleton<VerboseControl>::getInstance()->setVerbose(_verbose);   }

        //冗長出力先ストリーム設定 (アクセサ回送)
    inline static void setStream(std::ostream * _stream) throw()
    {   Singleton<VerboseControl>::getInstance()->setStream(_stream);   }

        //冗長出力制御の状態取得
    inline static bool getVerbose(void) throw()
    {   return Singleton<VerboseControl>::getInstance()->getVerbose();   }

        //冗長出力先ストリームの取得 (設定したものと同じものは出てこない)
    inline static VerboseStream getStream(void) throw()
    {   return VerboseStream(getVerbose() ? Singleton<VerboseControl>::getInstance()->getStream() : 0);   }

    //TESTSUITE_PROTOTYPE(main)
};

    /*
     *   例外メッセージ用文字列整形ヘルパークラス
     */
class ExceptionMessage : public Message
{
public:
        //例外の危険度レベル
    enum tagLevel {
        DEFAULT = 0,
        FATAL   = 1,
        WARNING = 2
    };

#ifdef EXCEPT_H
    class ExceptionMessageException : public Exception
    {
    friend class ExceptionMessage;
    protected:
        ExceptionMessageException(int _code, std::string _details) throw() : Exception("ExceptionMessage", _code, _details) {}
    };
#endif

protected:
    enum tagLevel level;

public:
        //コンストラクタ
    ExceptionMessage(const char * src) throw();
    ExceptionMessage(const std::string & src) throw();
    ExceptionMessage(const char * src1, const char * src2, ...) throw();

    ExceptionMessage(enum tagLevel level = DEFAULT) throw();
    ExceptionMessage(enum tagLevel level, const char * src) throw();
    ExceptionMessage(enum tagLevel level, const std::string & src) throw();
    ExceptionMessage(enum tagLevel level, const char * src1, const char * src2, ...) throw();

    ExceptionMessage(const ExceptionMessage & src) throw();


        //危険度レベル参照
    inline bool operator == (enum tagLevel _level) const throw()
    {   return level == _level;   }

        //危険度レベル参照
    inline bool operator != (enum tagLevel _level) const throw()
    {   return !(operator ==(_level));   }

        //引数の設定 (返却値の型をMessage & から ExceptionMessage & にするための小細工)
    template<class T>
    inline ExceptionMessage & operator << (const T & src) throw()
    {
        Message::operator << ( src );
        return *this;
    }

#ifdef EXCEPT_H     //Exceptionクラスを使用する場合
        //例外の発生
    bool throwException(void) throw(Exception)
    {
        ExceptionMessageException exc(level, str());
        return exc.throwException();
    }
        //マニピュレータ
    inline ExceptionMessage & operator << ( ExceptionMessage & (* func)(ExceptionMessage &) ) throw(Exception)
    {   return (*func)(*this);   }

#else               //stdexceptを使用する場合
        //例外の発生
    template<class T>
    bool throwException(void) throw(T)
    {
        throw T(str());
        return true;
    }
        //マニピュレータ
    inline ExceptionMessage & operator << ( ExceptionMessage & (* func)(ExceptionMessage &) ) throw(T)
    {   return (*func)(*this);   }
#endif

    //TESTSUITE_PROTOTYPE(main)
};

#ifdef EXCEPT_H     //Exceptionクラスを使用する場合

        //例外をスローするためのマニピュレータ
    inline ExceptionMessage & throwException(ExceptionMessage & excmsg) throw(Exception)
    {
        excmsg.throwException();
        return excmsg;
    }

#else
        //例外をスローするためのマニピュレータ
    template<class T>
    inline ExceptionMessage & throwException(ExceptionMessage & excmsg) throw(T)
    {
        excmsg.throwException<T>();
        return excmsg;
    }

#endif



    /*
     *   デバッグ情報出力用文字列整形ヘルパークラス
     */
class DebugMessage : public Formatter
{
public:
        //共通の冗長出力情報を持つクラス (シングルトンにするのでVerboseControlを同じものをクラス化する)
    class DebugControl : public VerboseMessage::VerboseControl
    { public: SINGLETON_CONSTRUCTOR_(DebugControl) throw() : VerboseMessage::VerboseControl(_singleton) {} };


protected:
        //デフォルトコンストラクタ (テスト用)
    DebugMessage(void) throw();

public:

        //コンストラクタ
    DebugMessage(std::string src) throw();

        //デストラクタ
    ~DebugMessage(void) throw();

        //冗長出力制御 (アクセサ回送)
    inline static void setVerbose(bool _verbose) throw()
    {   Singleton<DebugControl>::getInstance()->setVerbose(_verbose);   }

        //冗長出力先ストリーム設定 (アクセサ回送)
    inline static void setStream(std::ostream * _stream) throw()
    {   Singleton<DebugControl>::getInstance()->setStream(_stream);   }

        //冗長出力制御の状態取得
    inline static bool getVerbose(void) throw()
    {   return Singleton<DebugControl>::getInstance()->getVerbose();   }

        //冗長出力先ストリームの取得 (設定したものと同じものは出てこない)
    inline static VerboseMessage::VerboseStream getStream(void) throw()
    {   return VerboseMessage::VerboseStream(getVerbose() ? Singleton<DebugControl>::getInstance()->getStream() : 0);   }

//  TESTSUITE_PROTOTYPE(main)
};


#endif  //MESSAGE_H


