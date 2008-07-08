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
 *  @(#) $Id: mpstrstream.h,v 1.9 2003/12/20 06:51:58 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/mpstrstream.h,v 1.9 2003/12/20 06:51:58 takayuki Exp $

#ifndef MPSTRSTREAM_H
#define MPSTRSTREAM_H

#ifdef _MSC_VER
#  pragma warning(disable:4290) //C++ の例外の指定は無視されます。関数が __declspec(nothrow) でないことのみ表示されます。
#  pragma warning(disable:4786) //デバッグ情報で識別子が255文字に切り捨てられました。
#endif

#include "testsuite.h"
#include "base/except.h"
#include "base/message.h"
#include "base/event.h"

#include <fstream>
#include <sstream>
#include <string>
#include <list>


    //複数部位からなる出力用ストリームのクラス
class MultipartStream : public Event<ShutdownEvent>::Handler
{
public:

        //複数部位からなる出力用ストリームの一部位に相当するクラス
    class Part
    {
    protected:
        std::string       name;
        std::stringstream stream;

    public:
            //コンストラクタ
        Part(std::string name) throw();
        Part(const Part & src) throw();

            //正当性判定
        inline bool isValid(void) const throw()
        {   return (this != 0) && !name.empty() && stream.good();   }

            //ストリームへの書込み
        template<class T>
        Part & operator << ( T src ) throw(Exception)
        {
            if(isValid())
                stream << src;
            else
                ExceptionMessage("Operation was performed against an invalid stream.","無効なストリームに対して操作が行われました").throwException();

            return *this;
        }

            //ストリームに書いた内容を受け取る
        std::string getContents(void) const throw(Exception);

            //ストリームの内容を直接設定する
        void setContents(std::string contents) throw(Exception);

            //ストリーム名の参照
        inline const std::string & getName(void) const throw(Exception)
        {
            if(!isValid())
                ExceptionMessage("Operation was performed against an invalid stream.","無効なストリームに対して操作が行われました").throwException();
            return name;
        }

            //代入演算子
        inline Part & operator = (const Part & src) throw()
        {
            name = src.name;
            stream.clear();
            stream << src.stream.str();
        
            return *this;
        }

        TESTSUITE_PROTOTYPE(main)
    };


protected:
    std::string         filename;       //関連付けられたファイル名
    std::list<Part>     parts;          //部位集合(順序つき)
    Part *              current;        //今見ている部位
    bool                dirty;          //ファイルに出力する要あらばtrue (何か書き込んだら... ではなく、まだ出力してないかどうか)
    bool                output;         //出力しても良いならtrue

    virtual void handler(ShutdownEvent & evt)
    {   serialize();    }
  
public:
        //コンストラクタ
    MultipartStream(std::string filename = "") throw();

        //デストラクタ
    virtual ~MultipartStream(void) throw();

        //正当性判定
    inline bool isValid(void) const throw()
    {   return (this != 0) && !filename.empty();   }

        //ファイル名
    std::string setFilename(std::string filename) throw(Exception);
    std::string getFilename(void) const throw() { return filename; };

        //ストリームの内容をファイルに出力
    bool serialize(void) throw(Exception);

        //部位の生成
    MultipartStream & createPart(std::string name, bool precedence = false) throw(Exception);

        //部位の選択
    MultipartStream & movePart(std::string name) throw(Exception);

        //出力するかどうかの設定
    inline void enableOutput(void) throw()
    {   if(isValid()) output = true;   }
    inline void disableOutput(void) throw()
    {   if(isValid()) output = false;   }

        //ダーティビットの設定
    inline void setDirty(bool _dirty = true) throw()
    {   dirty = _dirty;   }

        //出力
    template <class T>
    inline MultipartStream & operator << ( T src ) throw(Exception)
    {
        if(this != 0 && current != 0) {
            setDirty();
            (*current) << src;
        }
        else
            ExceptionMessage("Invalid operation against an invalid object","無効なオブジェクトに対する要求").throwException();
        return *this;
    }

        //部位の選択 (Synonym)
    inline MultipartStream & operator[] (std::string name) throw(Exception)
    {   return movePart(name);   }

    TESTSUITE_PROTOTYPE(main)
};

#endif


