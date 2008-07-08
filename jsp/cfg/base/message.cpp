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
 *  @(#) $Id: message.cpp,v 1.4 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/message.cpp,v 1.4 2003/12/15 07:32:13 takayuki Exp $

#include "base/message.h"

using namespace std;


/*****************************************************************************/

/*
 *   メッセージ出力用文字列整形ヘルパークラス
 */

    //デフォルトコンストラクタ
Formatter::Formatter(void) throw() : templatestring(""), accept(false)
{}

    //コンストラクタ
Formatter::Formatter(string src) throw() : templatestring(src)
{   shift();   }

    //コピーコンストラクタ
Formatter::Formatter(const Formatter & src) throw() : templatestring(src.templatestring), accept(src.accept)
{
    string work;

    work = src.content.str();
    content << work;
}

    //次の引数の挿入先へと移動
void Formatter::shift(void) throw()
{
    string::size_type pos;

    accept = false;
    if(!templatestring.empty()) {

        do {
            pos = templatestring.find_first_of("%");

            if(pos != string::npos) {

                    /* %があってそれが\%という形なら、%をそのまま出す */
                if(pos != string::npos && pos != 0 && templatestring.at(pos - 1) == '\\') {

                    if(pos > 1)
                        content << templatestring.substr(0, pos - 2);
                    content << '%';

                    templatestring = templatestring.substr(pos + 1);

                    continue;
                }
                else {
                        /* %がまだある */
                    if(pos != string::npos)
                        accept = true;
                    break;
                }
            }
        } while(pos != string::npos);

        if(pos != string::npos) {
            if(pos != 0)
                content << templatestring.substr(0, pos);
            templatestring = templatestring.substr(pos + 1);
        }
        else {
            content << templatestring;
            templatestring.erase();
        }
    }
}


    //未設定の引数を (null) で置換
void Formatter::shift_all(void) throw()
{
    while(accept)
        *this << "(null)";
}

    //内部メッセージの取り出し
string Formatter::str(void) throw()
{
    shift_all();
    return content.str();
}

    //テンプレート文字列の設定 (中身は消える)
void Formatter::assign(string src) throw()
{
    clear();
    templatestring.assign(src);
    shift();
}

    //内容の消去
void Formatter::clear(void) throw()
{
    if(!templatestring.empty())
        templatestring.erase();
    content.str(string(""));
    accept = false;
}

    //代入演算子
Formatter & Formatter::operator =(const Formatter & src) throw()
{
    string work;

    templatestring = src.templatestring;
    accept         = src.accept;

    work = src.content.str();
    content << work;

    return *this;
}


/*****************************************************************************/

/*
 *   メッセージ出力用文字列整形ヘルパークラス
 */

const char * Message::MessageControl::select(const char * msg1, const char * msg2, va_list vl) throw()
{
    const char * msg = msg1;
    int i = language;

    if(i > 0) {
        msg = msg2;

        while(--i > 0)
            msg = va_arg(vl, const char *);
    }

    return msg;
}

    //デフォルトコンストラクタ (テスト用)
Message::Message(void) throw()
{}

    //コンストラクタ
Message::Message(string src) throw() : Formatter(src)
{}

    //コンストラクタ (複数言語)
Message::Message(const char * src1, const char * src2, ...) throw()
{
    va_list vl;

    va_start(vl, src2);
    templatestring.assign(Singleton<MessageControl>::getInstance()->select(src1, src2, vl));
    shift();
}

    //コピーコンストラクタ
Message::Message(const Message & src) throw() : Formatter(src)
{}

    //派生クラスの初期化用関数 (複数言語)
void Message::_initialize(const char * src1, const char * src2, va_list vl) throw()
{
    content.clear();
    accept = false;
    templatestring.assign(Singleton<MessageControl>::getInstance()->select(src1, src2, vl));
    shift();
}

    //言語選択
void Message::selectLanguage(enum tagLanguage lang) throw()
{
    if(lang >= 0 && lang < LAST_LANG)
        Singleton<Message::MessageControl>::getInstance()->setLanguage(lang);
}

    //テンプレート文字列の設定 (中身は消える)
void Message::assign(const char * src1, const char * src2, ... ) throw()
{
    va_list vl;

    clear();

    va_start(vl, src2);
    templatestring.assign(Singleton<MessageControl>::getInstance()->select(src1, src2, vl));
    shift();
}

/*****************************************************************************/

/*
 *   冗長出力用文字列整形ヘルパークラス
 */

    //デフォルトコンストラクタ
VerboseMessage::VerboseMessage(void) throw() : Message()
{}

    //コンストラクタ
VerboseMessage::VerboseMessage(const char * src) throw() : Message(src)
{}

    //コンストラクタ
VerboseMessage::VerboseMessage(const string & src) throw() : Message(src)
{}

    //コンストラクタ
VerboseMessage::VerboseMessage(const char * src1, const char * src2, ... ) throw() : Message()
{
    va_list vl;

    va_start(vl, src2);
    _initialize(src1, src2, vl);
}

    //デストラクタ
VerboseMessage::~VerboseMessage(void) throw()
{   Singleton< VerboseControl >::getInstance()->operator << (str());    }

/*****************************************************************************/

/*
 *   例外メッセージ用文字列整形ヘルパークラス
 */

    //コンストラクタ
ExceptionMessage::ExceptionMessage(const char * src) throw() : Message(src), level(DEFAULT)
{}

    //コンストラクタ
ExceptionMessage::ExceptionMessage(const std::string & src) throw() : Message(src), level(DEFAULT)
{}

    //コンストラクタ
ExceptionMessage::ExceptionMessage(const char * src1, const char * src2, ...) throw() : Message(), level(DEFAULT)
{
    va_list vl;
    
    va_start(vl, src2);
    _initialize(src1, src2, vl);
}

    //コンストラクタ
ExceptionMessage::ExceptionMessage(enum tagLevel _level) throw() : Message(), level(_level)
{}

    //コンストラクタ
ExceptionMessage::ExceptionMessage(enum tagLevel _level, const char * src) throw() : Message(src), level(_level)
{}

    //コンストラクタ
ExceptionMessage::ExceptionMessage(enum tagLevel _level, const std::string & src) throw() : Message(src), level(_level)
{}

    //コンストラクタ
ExceptionMessage::ExceptionMessage(enum tagLevel _level, const char * src1, const char * src2, ...) throw() : Message(), level(_level)
{
    va_list vl;
    
    va_start(vl, src2);
    _initialize(src1, src2, vl);
}

    //コピーコンストラクタ
ExceptionMessage::ExceptionMessage(const ExceptionMessage & src) throw() : Message(src), level(src.level)
{}


/*****************************************************************************/

/*
 *   デバッグメッセージ用文字列整形ヘルパークラス
 */

    //コンストラクタ
DebugMessage::DebugMessage(string src) throw() : Formatter(src)
{}

    //デストラクタ
DebugMessage::~DebugMessage(void) throw()
{   Singleton<DebugControl>::getInstance()->operator << (str());    }

