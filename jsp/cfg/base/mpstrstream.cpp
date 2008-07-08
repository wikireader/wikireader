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
 *  @(#) $Id: mpstrstream.cpp,v 1.9 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/mpstrstream.cpp,v 1.9 2003/12/15 07:32:13 takayuki Exp $

#include "base/mpstrstream.h"

using namespace std;

/*
 *   複数部位からなるストリームの一部位に相当するクラス
 */

    //コンストラクタ
MultipartStream::Part::Part(string _name) throw() : name(_name)
{}

    //コピーコンストラクタ
MultipartStream::Part::Part(const MultipartStream::Part & src) throw() : name("")
{
    if(src.isValid()) {
        name.assign(src.name);
        setContents(src.getContents());
    }
}

    //ストリームに書いた内容を受け取る
string MultipartStream::Part::getContents(void) const throw(Exception)
{
    string result;

    if(isValid())
        result = stream.str();
    else
        ExceptionMessage("Operation was performed against an invalid stream.","無効なストリームに対して操作が行われました").throwException();

    return result;
}

    //ストリームの内容を直接設定する
void MultipartStream::Part::setContents(string contents) throw(Exception)
{
    if(isValid()) {
        stream.clear();
        stream << contents;
    }
    else
        ExceptionMessage("Operation was performed against an invalid stream.","無効なストリームに対して操作が行われました").throwException();
}



/*
 *   複数部位からなるストリームの一部位に相当するクラス
 */

    //コンストラクタ
MultipartStream::MultipartStream(string _filename) throw() : filename(_filename), current(0), dirty(false), output(true)
{
    dirty = isValid();  //空であってもファイルを作らないといけないので...   
}

MultipartStream::~MultipartStream(void) throw()
{
    if(isValid() && dirty) {
            //蓋
        try { serialize(); }
        catch(...) {}
    }
}

    //ファイル名の関連付け
string MultipartStream::setFilename(string _filename) throw(Exception)
{
    string result;

    if(this != 0) {
        if(!_filename.empty()) {
            result   = filename;
            filename = _filename;
            dirty    = isValid();
        }
        else
            ExceptionMessage("Empty filename should not be allowed.","空のファイル名は使用できない").throwException();
    }
    else
        ExceptionMessage("Invalid object can not perform the request.","無効なオブジェクトに対する要求は実行できない").throwException();

    return result;
}

    //ファイル出力 (本当にファイルに出力したときだけtrue)
bool MultipartStream::serialize(void) throw(Exception)
{
    bool result = false;

    if(isValid() && dirty && output) {

        fstream file(filename.c_str(), ios::out);
        if(file.is_open()) {
            list<Part>::iterator scope;

                //全ての部位の内容を出力
            scope = parts.begin();
            while(scope != parts.end()) {
                file << scope->getContents();
                ++ scope;
            }

            file.close();
            dirty  = false;
            result = true;
        }
        else {
            ExceptionMessage("File could not open [%]","ファイルが開けない [%]") << filename << throwException;
            disableOutput();    //デストラクタがもう一度挑戦するので
        }
    }
    else {
        if(!isValid())
            ExceptionMessage("Invalid object can not perform the request.","無効なオブジェクトに対する要求は実行できない").throwException();
    }

    return result;
}

    //部位の作成
MultipartStream & MultipartStream::createPart(string name, bool precedence) throw(Exception)
{
    if(this != 0) {
        list<Part>::iterator scope;
        list<Part>::iterator newnode;

            //同じ名前がないことを確認
        scope = parts.begin();
        while(scope != parts.end()) {
            if(scope->getName().compare(name) == 0) {
                ExceptionMessage("The part \"%\" is already created.","部位名[%]はすでに利用されている") << name << throwException;
                break;
            }
            ++ scope;
        }

            //同じ名前がないなら要素を追加
        if(scope == parts.end()) {
            
                //挿入位置の検索
            if(current != 0) {
                scope = parts.begin();
                while(scope != parts.end() && &(*scope) != current)
                    ++ scope;

                    //挿入位置の調節 (現在位置を後ろにずらす when precedence = false; )
                if(scope != parts.end() && !precedence)
                    ++ scope;
            }
            else
                scope = parts.end();

                //要素の挿入
            newnode = parts.insert(scope, Part(name));
            current = &(*newnode);
        }
    }else
        ExceptionMessage("Invalid object can not perform the request.","無効なオブジェクトに対する要求は実行できない").throwException();

    return *this;
}

    //部位の選択
MultipartStream & MultipartStream::movePart(string name) throw(Exception)
{
    list<Part>::iterator scope;

    if(this != 0 && !name.empty()) {

            //名前が一致するものを探す
        scope = parts.begin();
        while(scope != parts.end()) {
            if(scope->getName() == name) {
                current = &(*scope);
                break;
            }
            ++ scope;
        }

            //見つからなかった
        if(scope == parts.end())
            ExceptionMessage("Unknown part [%] specified.","無効な識別名 [%]") << name << throwException;
    }
    else{
        if(this == 0)
            ExceptionMessage("Invalid object can not perform the request.","無効なオブジェクトに対する要求は実行できない").throwException();
        else //if(name.empty()) //手前のifの条件より、このifは常に真
            ExceptionMessage("Empty identifier was passed as a name of part.","空文字が渡された").throwException();
    }

    return *this;
}


/********************************** テストスィート **********************************/

#ifdef TESTSUITE

#include "coverage_undefs.h"

#include <iomanip>
#include <cstdio>

#ifdef _MSC_VER
#  pragma warning(disable:4101)   //ローカル変数は一度も使われていません
#endif

TESTSUITE_(main,Part, MultipartStream)
{
    BEGIN_CASE("constructor/isValid","コンストラクタ / 正当性判定") {
        BEGIN_CASE("1","名前つきで作成したら有効なパートが作れる") {
            Part part("test");
            if(!part.isValid())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","名前なしで作成したら無効なパートになる") {
            Part part("");
            if(part.isValid())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","NULLは無効なパート") {
            if(((Part *)0)->isValid())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","NULLオブジェクトでコピーコンストラクタを起動しても失敗しない") {
            Part * part = 0;
            Part work(*part);
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("operator <<","operator <<") {
        BEGIN_CASE("1", "出力できる") {
            Part part("test");

            part << "test";

            if(part.stream.str().compare("test") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "iomanipが使える") {
            Part part("test");

            part << setw(8) << setbase(16) << setfill('0') << 0x1234567;

            if(part.stream.str().compare("01234567") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3", "無効なストリームに書いたら例外が起る") {
            Part part("");
            bool result = false;

            if(part.isValid())
                TEST_FAIL;

            Exception::setThrowControl(true);
            try {
                Message::selectLanguage(Message::NEUTRAL);
                part << setw(8) << setbase(16) << setfill('0') << 0x1234567;
            }
            catch(Exception & e) {
                if(e.getDetails().compare("Operation was performed against an invalid stream.") == 0)
                    result = true;
            }
            if(!result)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("getContents","getContents") {
        BEGIN_CASE("1", "書いた内容が取得できる") {
            Part part("test");

            part << "test";

            if(part.getContents().compare("test") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "書いて読んでまた書いて読める") {
            Part part("test");

            part << "abc";
            if(part.getContents().compare("abc") != 0)
                TEST_FAIL;

            part << "def";
            if(part.getContents().compare("abcdef") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3", "無効なストリームから読んだら例外が起る") {
            Part part("");
            bool result = false;

            if(part.isValid())
                TEST_FAIL;

            Exception::setThrowControl(true);
            try {
                string work = part.getContents();
            }
            catch(Exception & e) {
                if(e.getDetails().compare("Operation was performed against an invalid stream.") == 0)
                    result = true;
            }
            if(!result)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("setContents","setContents") {
        BEGIN_CASE("1", "設定した内容が取得できる") {
            Part part("test");

            part.setContents("test");

            if(part.getContents().compare("test") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "setContentsで設定したあとに追記できる") {
            Part part("test");

            part.setContents("abc");
            part << "def";
            if(part.getContents().compare("abcdef") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3", "無効なストリームに設定したら例外が起る") {
            Part part("");
            bool result = false;

            if(part.isValid())
                TEST_FAIL;

            Exception::setThrowControl(true);
            try {
                part.setContents("test");
            }
            catch(Exception & e) {
                if(e.getDetails().compare("Operation was performed against an invalid stream.") == 0)
                    result = true;
            }
            if(!result)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("getName","getName") {
        BEGIN_CASE("1", "パート名が正しく取得できる") {
            Part part("name_of_stream");

            if(part.getName().compare("name_of_stream") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "不正なパートの名前を取ると例外が起る") {
            Part part("");
            bool result = false;

            Exception::setThrowControl(true);
            try { string work = part.getName(); }
            catch(Exception & e) {
                if(e.getDetails().compare("Operation was performed against an invalid stream.") == 0)
                    result = true;
            }
            if(!result)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("CopyConstructor","コピーコンストラクタ") {
        Part source("test");
        source << "abcdefg";

        Part dest(source);
        BEGIN_CASE("1","ストリームの内容が正しくコピーできている") {
            if(dest.getContents().compare("abcdefg") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","部位名が正しくコピーできている") {
            if(dest.getName() != source.getName())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;
}

TESTSUITE(main,MultipartStream)
{
    BEGIN_CASE("constructor/isValid","constructor/isValid") {
        BEGIN_CASE("1","ファイル名を指定して生成したオブジェクトは有効") {
            MultipartStream mps("test.dat");

            if(!mps.isValid())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","ファイル名を持たないオブジェクトは無効") {
            MultipartStream mps;

            if(mps.isValid())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","空のファイル名を持つオブジェクトは無効") {
            MultipartStream mps("");

            if(mps.isValid())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","NULLオブジェクトは無効") {
            if(((MultipartStream *)0)->isValid())
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("setFilename","setFilename") {
        BEGIN_CASE("1","ファイル名を指定せずに生成したファイルに名前をつける") {
            MultipartStream mps;
            if(mps.isValid())
                TEST_FAIL;

            BEGIN_CASE("1","ストリームは有効になる") {
                mps.setFilename("test");
                if(!mps.isValid())
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","ストリームの名前が変更されている") {
                if(mps.filename.compare("test") != 0)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("2","ファイル名を指定して生成したファイルに名前をつける") {
            MultipartStream mps("initialname");
            if(!mps.isValid())
                TEST_FAIL;

            BEGIN_CASE("1","ストリームは有効になる") {
                mps.setFilename("test");
                if(!mps.isValid())
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","ストリームの名前が変更されている") {
                if(mps.filename.compare("test") != 0)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("3","空文字を渡すと例外") {
            MultipartStream mps;
            bool result = false;

            Exception::setThrowControl(true);
            try { mps.setFilename(""); }
            catch(Exception & e)
            {   result = true;   }
            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","NULLオブジェクトに名前をつけると例外") {
            bool result = false;

            Exception::setThrowControl(true);
            try { ((MultipartStream *)0)->setFilename("test"); }
            catch(Exception & e)
            {   result = true;   }
            if(!result)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("serialize","serialize") {

        BEGIN_CASE("1", "NULLオブジェクトに対するシリアライズは例外") {
            bool result = false;

            Exception::setThrowControl(true);
            try 
            {   ((MultipartStream *)0)->serialize();   }
            catch(Exception & e)
            {   result = true;   }

            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2", "無効なオブジェクトに対するシリアライズも例外") {
            bool result = false;
            MultipartStream mps;

            Exception::setThrowControl(true);
            try
            {   mps.serialize();   }
            catch(Exception & e)
            {   result = true;   }

            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3", "正常にシリアライズできる") {
            MultipartStream mps("debug.out");
            Part part("abc");

            part << "abcdefg";
            mps.parts.push_back(part);
            mps.dirty = true;

            ::remove("debug.out");
            BEGIN_CASE("1", "シリアライズはtrueを返す") {
                if(!mps.serialize())
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2", "出力したファイルの内容が正しい") {
                if(!TestSuite::compareFileContents("debug.out","abcdefg"))
                    TEST_FAIL;
            } END_CASE;
            
        } END_CASE;

        BEGIN_CASE("4", "内容があってもdirty=falseなら出力されない") {
            MultipartStream mps("debug.out");
            Part part("abc");

            part << "abcdefg";
            mps.parts.push_back(part);
            mps.dirty = false;

            ::remove("debug.out");
            BEGIN_CASE("1", "シリアライズはfalseを返す") {
                if(mps.serialize())
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2", "ファイルは出力されていない") {
                fstream file("debug.out",ios::in);
                if(file.is_open())
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("5", "内容があっても出力禁止なら出力されない") {
            MultipartStream mps("debug.out");
            Part part("abc");

            part << "abcdefg";
            mps.parts.push_back(part);
            mps.dirty = true;

            mps.disableOutput();

            ::remove("debug.out");
            BEGIN_CASE("1", "シリアライズはfalseを返す") {
                if(mps.serialize())
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2", "ファイルは出力されていない") {
                fstream file("debug.out",ios::in);
                if(file.is_open())
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("Destructor","Destructor") {
        BEGIN_CASE("1","作って壊す") {
            MultipartStream mps("debug.out");
            Part part("abc");

            part << "abcdefg";
            mps.parts.push_back(part);
            mps.dirty = true;

            ::remove("debug.out");
        } END_CASE;

        BEGIN_CASE("2","内容がファイルに格納されている") {
            if(!TestSuite::compareFileContents("debug.out","abcdefg"))
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("createPart","createPart") {
        BEGIN_CASE("1","パートを登録する") {
            MultipartStream mps("debug.out");
            mps.disableOutput();

            BEGIN_CASE("1","例外なく登録できる") {
                Exception::setThrowControl(true);
                mps .createPart("abc")
                    .createPart("def")
                    .createPart("ghi");
            } END_CASE;

            BEGIN_CASE("2","中身が登録順で並んでいる") {
                list<Part>::iterator scope;
                scope = mps.parts.begin();
                if(scope->getName().compare("abc") != 0)
                    TEST_FAIL;
                ++ scope;
                if(scope->getName().compare("def") != 0)
                    TEST_FAIL;
                ++ scope;
                if(scope->getName().compare("ghi") != 0)
                    TEST_FAIL;
                ++ scope;
                if(scope != mps.parts.end())
                    TEST_FAIL;
            } END_CASE;

        } END_CASE;

        BEGIN_CASE("2","パートを逆順で登録する") {
            MultipartStream mps("debug.out");
            mps.disableOutput();

            BEGIN_CASE("1","例外なく登録できる") {
                Exception::setThrowControl(true);
                mps .createPart("abc",true)
                    .createPart("def",true)
                    .createPart("ghi",true);
            } END_CASE;

            BEGIN_CASE("2","中身が登録した逆順で並んでいる") {
                list<Part>::iterator scope;
                scope = mps.parts.begin();
                if(scope->getName().compare("ghi") != 0)
                    TEST_FAIL;
                ++ scope;
                if(scope->getName().compare("def") != 0)
                    TEST_FAIL;
                ++ scope;
                if(scope->getName().compare("abc") != 0)
                    TEST_FAIL;
                ++ scope;
                if(scope != mps.parts.end())
                    TEST_FAIL;
            } END_CASE;

        } END_CASE;

        BEGIN_CASE("3","NULLに対する操作で例外が発生する") {
            bool result = false;
            Exception::setThrowControl(true);
            try {
                ((MultipartStream *)0)->createPart("test");
            }
            catch(...)
            {   result = true;   }
            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","同じ名前のパートを作ると例外が起る") {
            BEGIN_CASE("1","同じ名前のパートを作ると例外が起る") {
                bool result = false;
                MultipartStream mps("debug.out");
                Exception::setThrowControl(true);

                try {
                    mps .createPart("abc")
                        .createPart("def")
                        .createPart("abc");
                }
                catch(...)
                {   result = true;   }

                if(!result)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","例外を封じても正しく動く") {
                bool result = true;
                MultipartStream mps("debug.out");
                Exception::setThrowControl(false);

                BEGIN_CASE("1","例外はスローされない") {
                try {
                        mps .createPart("abc")
                            .createPart("def")
                            .createPart("abc");
                    }
                    catch(...)
                    {   result = false;   }

                    if(!result)
                        TEST_FAIL;
                } END_CASE;

                BEGIN_CASE("2","現在位置は変化しない") {
                    if(mps.current == 0 || mps.current->getName().compare("def") != 0)
                        TEST_FAIL;
                } END_CASE;
            } END_CASE;
        } END_CASE;

        Exception::setThrowControl(true);

        BEGIN_CASE("5","登録すると現在位置が変化する") {
            MultipartStream mps("debug.out");
            mps.disableOutput();

            BEGIN_CASE("1","登録すると位置が変化する (1)") {
                mps.createPart("abc");
                if(mps.current == 0 || mps.current->getName().compare("abc") != 0)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","登録すると位置が変化する (2)") {
                mps.createPart("def");
                if(mps.current == 0 || mps.current->getName().compare("def") != 0)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("3","登録すると位置が変化する (3)") {
                mps.createPart("ghi");
                if(mps.current == 0 || mps.current->getName().compare("ghi") != 0)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("opeator <<","operator <<") {

        BEGIN_CASE("1","operator <<が使える") {
            MultipartStream mps("debug.out");

            mps.createPart("test");

            mps << "abcdefghijklmn";
        } END_CASE;

        BEGIN_CASE("2","出力された中身が正しい") {
            if(!TestSuite::compareFileContents("debug.out","abcdefghijklmn"))
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","NULLオブジェクトに出力すると例外") {
            bool result = false;
            Exception::setThrowControl(true);
            try {
                *((MultipartStream *)0) << "test";
            }
            catch(...)
            {   result = true;   }
            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","パートを全く作っていないオブジェクトに出力すると例外") {
            bool result = false;
            Exception::setThrowControl(true);
            try {
                MultipartStream mps("debug.out");
                mps.disableOutput();
                *((MultipartStream *)0) << "test";
            }
            catch(...)
            {   result = true;   }
            if(!result)
                TEST_FAIL;
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("movePart/operator []","movePart/operator []") {
        BEGIN_CASE("1","普通に移動する") {
            MultipartStream mps("debug.out");
            mps.disableOutput();

            mps .createPart("abc")
                .createPart("def")
                .createPart("ghi");

            BEGIN_CASE("1","移動できる") {
                mps.movePart("def");

                if(mps.current->getName().compare("def") != 0)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("2","無効な名前を入れると例外") {
                bool result = false;
                Exception::setThrowControl(true);

                try {
                    mps.movePart("unknwon");
                }
                catch(...)
                {   result = true;   }
                if(!result)
                    TEST_FAIL;
            } END_CASE;

            BEGIN_CASE("3","空文字を入れると例外") {
                bool result = false;
                Exception::setThrowControl(true);

                try {
                    mps.movePart("");
                }
                catch(...)
                {   result = true;   }
                if(!result)
                    TEST_FAIL;
            } END_CASE;
        } END_CASE;

        BEGIN_CASE("2", "NULLオブジェクト相手に操作すると例外") {
            bool result = false;
            Exception::setThrowControl(true);

            try {
                ((MultipartStream *)0)->movePart("");
            }
            catch(...)
            {   result = true;   }
            if(!result)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;
}


#endif




