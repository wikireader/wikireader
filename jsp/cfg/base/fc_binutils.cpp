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
 *  @(#) $Id: fc_binutils.cpp,v 1.21 2004/09/04 15:50:13 honda Exp $
 */

#if defined(FILECONTAINER_BINUTILS) || defined(TESTSUITE)

#ifdef _MSC_VER
#pragma warning(disable:4786) //デバッグ文字列を255文字に切り詰めた
#endif

#include "base/filecontainer.h"
#include <string>
#include <map>
#include <fstream>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#define _isspace(x)  isspace(x)
#define _isprint(x)  isprint(x)

#define SIZE_LOADPAGE 65536         //バイナリデータを格納するページ単位

#define SIZE_TO_CONFIRM_BINARYFILE 128  //ファイルがバイナリを含むかどうかを確認するのに読み出すデータの長さ (バッファを取るのであまり大きくしないこと)

#define MAGIC_SYMBOL "_checker_magic_number"
#define MAGIC_NUMBER 0x01234567                         //4バイトの整数

#define CMD_GNUNM      "nm"
#define CMD_GNUOBJCOPY "objcopy"

#define MAKE_BASEADDRESS(x)   ((x) & ~(SIZE_LOADPAGE-1))
#define MAKE_OFFSETADDRESS(x) ((x) &  (SIZE_LOADPAGE-1))

using namespace std;

namespace {

    class FileContainerBinutilsImpl : public FileContainer
    {
    public:
        typedef void (interceptor_func_t)(fstream &, const string &);      //不意に訪れたバイナリファイルの襲撃に対応する関数の型

    protected:
        string                 symbol_prefix;
        map<string, address_t> symbol_table;
        map<address_t, char *> contents;

        address_t last_address;     //キャッシュもどき
        char *    last_page;

            //データ取り込み
        void loadSymbols(fstream & file)  throw(Exception);
        void loadDataContents(fstream & file) throw(Exception);

            //contentsへ1バイト書き込み
        void writeByte(address_t address, unsigned int) throw(Exception);

            //自動処理
        void searchSymbolPrefix(void) throw();
        void searchByteOrder(void)    throw();

    public:
        FileContainerBinutilsImpl(void) throw();
        virtual ~FileContainerBinutilsImpl(void) throw();

            /* インタフェース部 */
        virtual void        attachModule(const string & filename) throw(Exception);
        virtual void        loadContents(void * dest, address_t address, size_t size) throw(Exception);
        virtual address_t   getSymbolAddress(const string & symbol) throw(Exception);
        virtual std::string getArchitecture(void) throw();

        TESTSUITE_PROTOTYPE(main)
    };

    namespace {
        FileContainerBinutilsImpl instance_of_FileContainerBinutilsImpl;
    }

        /* コンストラクタ */
    FileContainerBinutilsImpl::FileContainerBinutilsImpl(void) throw()
        : symbol_prefix(""), symbol_table(), contents(), last_address(0), last_page(0)
    {}

        /* デストラクタ : データバッファの解放 */
    FileContainerBinutilsImpl::~FileContainerBinutilsImpl(void) throw()
    {
        map<address_t, char *>::iterator scope;

        scope = contents.begin();
        while(scope != contents.end()) {
            delete [] scope->second;
            ++ scope;
        }
        symbol_table.clear();
        contents.clear();
    }

        /* ファイル名をカンマで二つに分ける */
    void splitFilename(const string & src, string & first, string & second) throw(Exception)
    {
        if(!src.empty()) {
            string::size_type pos;

            pos = src.find_first_of(',');
            if(pos != string::npos) {
                first  = src.substr(0, pos);
                second = src.substr(pos + 1);
            }
            else {
                    //ファイル名が一つしか指定されていない
                first  = src;
                second = src;
            }
        }
        else
            ExceptionMessage("[FCBI] Empty filename could not be accepted.","[FCBI] ファイル名がありません").throwException();
    }

        /* ファイルがバイナリデータを持っているかどうかを判定 */
    bool hasBinaryContents(fstream & file) throw()
    {
        assert(file.is_open());

        bool        result = false;
        char        buffer[SIZE_TO_CONFIRM_BINARYFILE];
        streamsize  length;

        file.read(buffer, SIZE_TO_CONFIRM_BINARYFILE);
        length = file.gcount();

        for(streamsize i = 0; i < length; ++ i) {
            if(buffer[i] < 0 || !(_isprint(buffer[i]) || _isspace(buffer[i]))){
                result = true;
                break;
            }
        }

		if(!result) {
	        file.clear();
    	    file.seekg(0, ios::beg);    //先頭に戻しておく
		}

        return result;
    }

        /* テキストファイルを開く (バイナリだった場合には対処) */
    void openTextFile(fstream & file, const string & filename, FileContainerBinutilsImpl::interceptor_func_t * interceptor) throw(Exception)
    {
        assert(!filename.empty());
        assert(!file.is_open());

        file.open(filename.c_str(), ios::in|ios::binary);
        if(!file.is_open()) {
            ExceptionMessage("File '%' could not be opened.","ファイル '%' は開けません") << filename << throwException;
            return;
        }

            /* バイナリファイルだったら... */
        while(hasBinaryContents(file)) {
            file.close();
            
            if(interceptor != 0) {
                (*interceptor)(file, filename);
                interceptor = 0;    //対処は一回のみ
            }

            if(!file.is_open()) {
                break;
            }
        }

            /* ファイルが開けなかったら例外 */
        if(!file.is_open())
            ExceptionMessage("Program failed to convert the binary '%' into suitable style. Please specify a suitable TEXT file.",
                            "プログラムはバイナリファイル'%'の変換に失敗しました。正しいテキストファイルを指定し直してください。")
                                << filename << throwException;
    }

        /* 一時的なファイル名の生成 */
    const char * makeTemporaryFilename(void) throw()
    {
        static char filename[10];

        sprintf(filename, "cfg%06x", (int)(rand() & 0xffffffl));

        return filename;
    }


        /* バイナリをGNU-NMを使って変換する */
    void interceptWithGnuNM(fstream & file, const string & filename) throw(Exception)
    {
        assert(!file.is_open());

        string cmdline;
        string symfile;

        symfile.assign(makeTemporaryFilename());
        cmdline = string(CMD_GNUNM) + " " + filename + " > " + symfile;
        VerboseMessage("[EXEC] %\n") << cmdline;

		system(cmdline.c_str());

            /* 正しく開けたらファイルを削除 */
        file.open(symfile.c_str(), ios::in);
        if(file.is_open()) {
            remove(symfile.c_str());
        }
    }

        /* バイナリをGNU-OBJCOPYを使って変換する */
    void interceptWithGnuObjcopy(fstream & file, const string & filename) throw(Exception)
    {
        assert(!file.is_open());

        string cmdline;
        string srecfile;

        srecfile.assign(makeTemporaryFilename());
        cmdline = string(CMD_GNUOBJCOPY) + " -F srec " + filename + " " + srecfile;
        VerboseMessage("[EXEC] %\n") << cmdline;

		system(cmdline.c_str());

            /* 正しく開けたらファイルを削除 */
        file.open(srecfile.c_str(), ios::in);
        if(file.is_open()) {
            remove(srecfile.c_str());
        }
    }

        /* 16進から10進への変換 (ポインタ移動, 長さ指定付き) */
    unsigned int hextodec(const char * & src, size_t length) throw()
    {
        assert(length <= sizeof(unsigned int) * 2);

        unsigned int result = 0;
        unsigned int digit;

        while(length-- > 0) {
            if(*src >= '0' && *src <= '9')
                digit = *src - '0';
            else if(*src >= 'A' && *src <='F')
                digit = *src - 'A' + 10;
            else if(*src >= 'a' && *src <='f')
                digit = *src - 'a' + 10;
            else
                break;

            ++ src;
            result = (result << 4) | (digit & 0xf);
        }

        return result;
    }

        /* NMが出力した行をパース */
    bool readGnuNmLine(fstream & file, FileContainer::address_t & address, string & attribute, string & symbolname) throw()
    {
        assert(file.is_open());

        string src;
        string addr;
        string::size_type pos1;
        string::size_type pos2;

            /* 中身を空にしておく */
        address = 0;
        if(!attribute.empty())
            attribute.erase();
        if(!symbolname.empty())
            symbolname.erase();

            //次の行を取得 (空行, 未定義シンボルは読み飛ばす)
        do {
            if(file.eof())
                return false;

            getline(file, src, '\n');
        } while(src.empty() || src.at(0) == ' ');

            //行を分解
        pos1 = src.find_first_of(' ');
        addr = src.substr(0, pos1);

        pos2 = src.find_first_of(' ', pos1 + 1);
        attribute  = src.substr(pos1 + 1, pos2 - pos1 - 1);
        symbolname = src.substr(pos2 + 1);

            //アドレスのパース  (注 : なんでこんなちまちまやってるかというと、アドレスが32bitを超えるターゲットがいるから)
        while(!addr.empty()) {
            size_t length = addr.size();
            const char * src = addr.c_str();
            if(length > sizeof(unsigned int) * 2)
                length = sizeof(unsigned int) * 2;
            address = (address << (length * 2)) | (hextodec(src, length));
            addr.erase(0, length);
        }

        return true;
    }

        /* シンボルの読み込み */
    void FileContainerBinutilsImpl::loadSymbols(fstream & file) throw(Exception)
    {
        assert(file.is_open());

        address_t address;
        string    attribute;
        string    symbolname;

        while(readGnuNmLine(file, address, attribute, symbolname)) {
            symbol_table.insert(map<string, address_t>::value_type(symbolname, address));
        }

		VerboseMessage("% symbols loaded\n") << symbol_table.size() << &throwException;

        file.close();
    }

        /* contentsに1バイト書き込み */
    void FileContainerBinutilsImpl::writeByte(address_t address, unsigned int value) throw(Exception)
    {
        address_t & base = last_address;
        char *    & page = last_page;

            /* キャッシュもどきが使えないなら、ページを探す */
        if(MAKE_BASEADDRESS(address) != last_address || last_page == 0) {
            map<address_t, char *>::iterator scope;

            base  = MAKE_BASEADDRESS(address);
            scope = contents.find(base);
            if(scope == contents.end()) {
                page = new(nothrow) char [SIZE_LOADPAGE];
                if(page == 0) {
                    ExceptionMessage("Not enough memory available to store the contents","空きメモリ不足のため、データの格納に失敗しました").throwException();
                    return;
                }
                contents.insert(map<address_t,char*>::value_type(base, page));
            }
            else
                page = scope->second;
        }

        *(page + (address - base)) = static_cast<char>(value & 0xff);
    }

        /* 末尾の空白文字を切り取る */
    void trimString(string & src) throw()
    {
        string::size_type pos;

        pos = src.find_last_not_of(" \t\r\n");
        if(pos != string::npos && pos != src.size())
            src.erase(pos + 1);
    }

        /* モトローラSレコードを一行読み込む */
        /*
                The general format of an S-record follows: 
                +-------------------//------------------//-----------------------+
                | type | count | address  |            data           | checksum |
                +-------------------//------------------//-----------------------+
        */
    bool readRecord(fstream & file, string & dest) throw(Exception)
    {
        unsigned int sum;
        unsigned int count;
        unsigned int i;
        const char * pos;

            /* 次の行を読み込む */
        do {
                //getlineがReadFileを呼んでブロックするので、確実にEOFを反応させるためにこうする
            int ch = file.get();
            if(ch == EOF)
                return false;

            file.putback(static_cast<char>(ch));
            getline(file, dest);
        } while(dest.empty());

        trimString(dest);

            /* 正当性の判定 */

        if(dest[0] != 'S')      //行頭が'S'で始まらない
            ExceptionMessage("The file is not a Motorola S-Record file.","モトローラSフォーマットで無い行が見つかりました") << throwException;

        pos = dest.c_str() + 2;
        count = hextodec(pos, 2);
        if(dest.size() != (count + 2)*2)
            ExceptionMessage("Illegal S-Record found (count unmatched).","不正なSレコードがあります (サイズ不一致)") << throwException;

        sum = count;
        for(i = 0; i < count; ++ i)
            sum += hextodec(pos, 2);

        if((sum & 0xff) != 0xff)
            ExceptionMessage("Illegal S-Record found (check-sum unmatched).","不正なSレコードがあります (チェックサム不一致)") << throwException;

        return true;
    }

        /* 次の開始アドレスを得る */
    FileContainer::address_t parseRecordAddress(const string & src, FileContainer::address_t base) throw()
    {
        const char * record = src.c_str();
        FileContainer::address_t result = 0;

        record += 4;
        switch(*(record - 3)) {
            case '1':
                result = hextodec(record, 4);
                break;
            case '2':
                result = hextodec(record, 6);
                break;
            case '3':
                result = hextodec(record, 8);
                break;
            case '5':
                result = base;
                break;
            default:
                break;
        }

        return result;
    }

        /* データ部分だけを残してチョップ */
    void chopRecord(string & src) throw()
    {
        string::size_type start;

        switch(src[1]) {
            case '1':  start = 4 + 4;  break;
            case '2':  start = 4 + 6;  break;
            case '3':  start = 4 + 8;  break;
            default:   start = 4;      break;
        }

            //先頭4バイト + アドレス部 + 最後のサムを取り除く
        src = src.substr(start, src.size() - start - 2);
    }

        /* プログラムデータの読み込み */
    void FileContainerBinutilsImpl::loadDataContents(fstream & file) throw(Exception)
    {
        assert(file.is_open());

        address_t address;
        string    line;

        address = 0;
        while(readRecord(file, line)) {

            address = parseRecordAddress(line, address);

            chopRecord(line);

                /* データの格納 */
            const char * pos = line.c_str();
            while(*pos != '\x0') {
                unsigned int data = hextodec(pos, 2);
                writeByte(address, data);
                ++ address;
            }
        }

        file.close();
    }

        /* シンボルプレフィクスの自動判定 */
    void FileContainerBinutilsImpl::searchSymbolPrefix(void) throw()
    {
        const char *  candidate_list[] = {"", "_", NULL};
        const char ** candidate;

        for(candidate = candidate_list; *candidate != NULL; ++ candidate) {
            map<string, address_t>::const_iterator scope;
            string symbol;
            
            symbol = string(*candidate) + MAGIC_SYMBOL;
            scope  = symbol_table.find(symbol);

            if(scope != symbol_table.end())
                break;
        }

        if(*candidate != NULL)
            symbol_prefix.assign(*candidate);
    }

        /* エンディアンの自動判定 */
    void FileContainerBinutilsImpl::searchByteOrder(void) throw()
    {
        address_t address;
        union {
            char         buffer[4];
            unsigned int value;
        };

        try {
            value   = 0;
            address = getSymbolAddress(MAGIC_SYMBOL);
            loadContents(buffer, address, 4);

            if(value == MAGIC_NUMBER) {
                byteorder = HOSTORDER;
            }
            else {
                buffer[0] ^= buffer[3], buffer[3] ^= buffer[0], buffer[0] ^= buffer[3]; // swap(buffer[0], buffer[3])
                buffer[1] ^= buffer[2], buffer[2] ^= buffer[1], buffer[1] ^= buffer[2]; // swap(buffer[1], buffer[2])

                if(value == MAGIC_NUMBER)
                    byteorder = HOSTORDER == LITTLE ? BIG : LITTLE;
                else
                    throw false;
            }
        }
        catch(...) {}
    }

        /* モジュールのアタッチ -> シンボル読出し, データ格納 */
    void FileContainerBinutilsImpl::attachModule(const string & filename) throw(Exception)
    {
        fstream file;
        string  symbol_filename;
        string  contents_filename;

        splitFilename(filename, symbol_filename, contents_filename);

        openTextFile(file, symbol_filename, interceptWithGnuNM);
        loadSymbols(file);

        openTextFile(file, contents_filename, interceptWithGnuObjcopy);
        loadDataContents(file);

        searchSymbolPrefix();
        searchByteOrder();
    }

        /* 格納している内容の取得 */
    void FileContainerBinutilsImpl::loadContents(void * _dest, address_t address, size_t size) throw(Exception)
    {
        char * dest = static_cast<char *>(_dest);

        while(size > 0) {
            map<address_t, char *>::const_iterator scope;

            address_t  base   = MAKE_BASEADDRESS(address);
            address_t  offset = MAKE_OFFSETADDRESS(address);
            size_t     transfer_size = size;

            if(transfer_size > SIZE_LOADPAGE - offset)
                transfer_size = SIZE_LOADPAGE - offset;

            scope = contents.find(base);
            if(scope == contents.end())
                ExceptionMessage("[Internel error] Memory read with unmapped address","[内部エラー] マップされてないアドレスを使ってメモリリードが行われました").throwException();

            memcpy(dest, scope->second + offset, transfer_size);

            dest += transfer_size;
            size -= transfer_size;
        }
    }

        /* シンボルのアドレスの取得 */
    FileContainer::address_t FileContainerBinutilsImpl::getSymbolAddress(const string & symbol) throw(Exception)
    {
        string symbolname;
        map<string, address_t>::const_iterator scope;

        symbolname = symbol_prefix + symbol;

        scope = symbol_table.find(symbolname);
        if(scope == symbol_table.end())
            ExceptionMessage("Unknown symbol '%'","不明なシンボル名 '%'") << symbol << throwException;

        return scope->second;
    }

        /* アーキテクチャ名の取得 */
    string FileContainerBinutilsImpl::getArchitecture(void) throw()
    {
        if(byteorder == LITTLE)
            return "Little endian target (with GNU/Binutils)";
        else
            return "Big endian target (with GNU/Binutils)";
    }

}

//---------------------------------------------

#ifdef TESTSUITE
#include "base/coverage_undefs.h"

namespace {
    fstream * interceptor_file;
    string    interceptor_filename;
    void interceptor(fstream & file, const string & filename)
    {
        CHECKPOINT("interceptor");
        interceptor_file = &file;
        interceptor_filename = filename;

        if(filename.compare("textfile") == 0) {
            remove(filename.c_str());
            file.open(filename.c_str(), ios::out);
            file << "text";
            file.close();

            file.open(filename.c_str(), ios::in|ios::binary);
        }
        else if(filename.compare("binaryfile") == 0) {
            remove(filename.c_str());
            file.open(filename.c_str(), ios::out|ios::binary);
            file.write("\x1",1);
            file.close();

            file.open(filename.c_str(), ios::in|ios::binary);
        }
    }
}

TESTSUITE(main, FileContainerBinutilsImpl)
{
    PREDECESSOR("TFileContainer");

    SingletonBase::ContextChain chain;
    chain.saveContext<RuntimeObjectTable>();

    BEGIN_CASE("splitFilename","splitFilename") {
        BEGIN_CASE("1","カンマの前後で切れる") {
            string first, second;
            
            splitFilename("a,b", first, second);
            TEST_CASE("1","firstの中身は正しい", first.compare("a") == 0);
            TEST_CASE("2","secondの中身は正しい", second.compare("b") == 0);
        } END_CASE;

        BEGIN_CASE("2","カンマの無い引数を与えると、両方に同じ中身が入る") {
            string first, second;

            splitFilename("abc", first, second);
            TEST_CASE("1","firstの中身は正しい", first.compare("abc") == 0);
            TEST_CASE("2","secondの中身は正しい", second.compare("abc") == 0);
        } END_CASE;

        BEGIN_CASE("3","空文字を与えると例外") {
            bool result = false;
            string first, second;
            try { splitFilename("", first, second); } catch(Exception &) { result = true; }
            if(!result)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("hasBinaryContents","hasBinaryContents") {
        BEGIN_CASE("1","テキストファイルを食わせる") {
            fstream file("test", ios::out);
            file << "This is a sample text file.";
            file.close();

            file.open("test",ios::in|ios::binary);
            TEST_CASE("1","関数はfalseを返す", !hasBinaryContents(file));
            TEST_CASE("2","fileはeofに達していない", !file.eof());
            file.close();

            remove("test");
        } END_CASE;

        BEGIN_CASE("2","バイナリデータを食わせる") {
            fstream file("test", ios::out|ios::binary);
            file << "This is a sample text file.";
            file.write("\x0\x1\x2\x3", 4);
            file.close();

            file.open("test",ios::in|ios::binary);
            TEST_CASE("1","関数はtrueを返す", hasBinaryContents(file));
            TEST_CASE("2","fileはeofに達していない", !file.eof());
            file.close();

            remove("test");
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("openTextFile","openTextFile") {
        BEGIN_CASE("1","テキストファイルを指定する") {
            TestSuite::clearCheckpoints();
            fstream file("test", ios::out);
            file << "This is a sample text file.";
            file.close();

            bool result = true;
            try { openTextFile(file, "test", interceptor); } catch(Exception &) { result = false; }

            TEST_CASE("1","例外は起きない", result);
            TEST_CASE("2","ファイルが開かれている", file.is_open());
            TEST_CASE("3","interceptorはコールされていない", !TestSuite::isReached("interceptor"));

            string work;
            getline(file, work, '\n');
            TEST_CASE("4","読み出された内容が正しい", work.compare("This is a sample text file.") == 0);

            file.close();
            remove("test");
        } END_CASE;

        BEGIN_CASE("2","バイナリファイルを指定する (interceptorはファイルを開かない)") {
            TestSuite::clearCheckpoints();
            fstream file("test", ios::out|ios::binary);
            file.write("\x1", 1);
            file.close();

            bool result = false;
            try { openTextFile(file, "test", interceptor); } catch(Exception &) { result = true; }

            TEST_CASE("1","例外が起きる", result);
            TEST_CASE("2","ファイルが開かれている", !file.is_open());
            TEST_CASE("3","interceptorがコールされている", TestSuite::isReached("interceptor"));
            TEST_CASE("4","interceptorの引数が正しい (file)", interceptor_file == &file);
            TEST_CASE("5","interceptorの引数が正しい", interceptor_filename.compare("test") == 0);

            file.close();
            remove("test");
        } END_CASE;

        BEGIN_CASE("3","存在しないファイルを指定する") {
            TestSuite::clearCheckpoints();
            fstream file;
            bool result = false;
            try { openTextFile(file, "___unknown___", interceptor); } catch(Exception &) { result = true; }

            TEST_CASE("1","例外が起きる", result);
            TEST_CASE("2","ファイルが開かれていない", !file.is_open());
            TEST_CASE("3","interceptorがコールされていない", !TestSuite::isReached("interceptor"));
        } END_CASE;

        BEGIN_CASE("4","interceptorがテキストファイルを生成する") {
            TestSuite::clearCheckpoints();
            fstream file("textfile", ios::out|ios::binary);
            file.write("\x1", 1);
            file.close();

            bool result = true;
            try { openTextFile(file, "textfile", interceptor); } catch(Exception &) { result = false; }

            TEST_CASE("1","例外は起きない", result);
            TEST_CASE("2","ファイルが開かれている", file.is_open());
            TEST_CASE("3","interceptorがコールされている", TestSuite::isReached("interceptor"));
            TEST_CASE("4","interceptorの引数が正しい (file)", interceptor_file == &file);
            TEST_CASE("5","interceptorの引数が正しい", interceptor_filename.compare("textfile") == 0);

            string work;
            getline(file, work, '\n');
            TEST_CASE("4","読み出された内容が正しい", work.compare("text") == 0);

            file.close();
            remove("textfile");
        } END_CASE;

        BEGIN_CASE("5","interceptorがバイナリファイルを生成する") {
            TestSuite::clearCheckpoints();
            fstream file("binaryfile", ios::out|ios::binary);
            file.write("\x1", 1);
            file.close();

            bool result = false;
            try { openTextFile(file, "binaryfile", interceptor); } catch(Exception &) { result = true; }

            TEST_CASE("1","例外は起きる", result);
            TEST_CASE("2","ファイルが開かれていない", !file.is_open());
            TEST_CASE("3","interceptorがコールされている", TestSuite::isReached("interceptor"));
            TEST_CASE("4","interceptorの引数が正しい (file)", interceptor_file == &file);
            TEST_CASE("5","interceptorの引数が正しい", interceptor_filename.compare("binaryfile") == 0);

            file.close();
            remove("binaryfile");
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("hextodec","hextodec") {
        const char * letter = "0123456789abcdEFg";
        const char * work = letter;

        TEST_CASE("1", "切り出された値が正しい", hextodec(work, 2) == 1);
        TEST_CASE("2", "workが進んでいる", work == letter + 2);
        TEST_CASE("3", "切り出された値が正しい", hextodec(work, 4) == 0x2345);
        TEST_CASE("4", "workが進んでいる", work == letter + 6);
        TEST_CASE("5", "切り出された値が正しい", hextodec(work, 8) == 0x6789abcd);
        TEST_CASE("6", "workが進んでいる", work == letter + 14);
        TEST_CASE("7", "切り出された値が正しい", hextodec(work, 8) == 0xef);
        TEST_CASE("8", "workが'g'の位置でとまる", *work == 'g');
        TEST_CASE("9", "切り出された値が正しい", hextodec(work, 8) == 0);
        TEST_CASE("10", "workが'g'の位置でとまる", *work == 'g');
    } END_CASE;

    BEGIN_CASE("readGnuNmLine","readGnuNmLine") {
        fstream file("test", ios::out);
        file << "0804aab0 T _kernel_task_initialize\n0805d8a0 B _kernel_tcb_table\n\n0804e560 R _kernel_tinib_table\n         U getpid@@GLIBC_2.0\n";
        file.close();

        address_t address;
        string    attribute;
        string    symbolname;

        file.open("test", ios::in);
        BEGIN_CASE("1","普通のエントリが読める") {
            TEST_CASE("1","関数は成功する", readGnuNmLine(file, address, attribute, symbolname));
            TEST_CASE("2","addressは正しい", address == 0x0804aab0);
            TEST_CASE("3","attributeは正しい", attribute.compare("T") == 0);
            TEST_CASE("4","symbolnameは正しい", symbolname.compare("_kernel_task_initialize") == 0);
        } END_CASE;

        BEGIN_CASE("2","普通のエントリが読める (2)") {
            TEST_CASE("1","関数は成功する", readGnuNmLine(file, address, attribute, symbolname));
            TEST_CASE("2","addressは正しい", address == 0x0805d8a0);
            TEST_CASE("3","attributeは正しい", attribute.compare("B") == 0);
            TEST_CASE("4","symbolnameは正しい", symbolname.compare("_kernel_tcb_table") == 0);
        } END_CASE;

        BEGIN_CASE("3","空行を読み飛ばして次が読める") {
            TEST_CASE("1","関数は成功する", readGnuNmLine(file, address, attribute, symbolname));
            TEST_CASE("2","addressは正しい", address == 0x0804e560);
            TEST_CASE("3","attributeは正しい", attribute.compare("R") == 0);
            TEST_CASE("4","symbolnameは正しい", symbolname.compare("_kernel_tinib_table") == 0);
        } END_CASE;

        BEGIN_CASE("4","アドレスの無いエントリは無視する") {
            TEST_CASE("1","関数は失敗する", !readGnuNmLine(file, address, attribute, symbolname));
        } END_CASE;

        file.close();
        remove("test");
    } END_CASE;

    BEGIN_CASE("loadSymbols","loadSymbols") {
        fstream file("test", ios::out);
        file << "0804aab0 T _kernel_task_initialize\n0805d8a0 B _kernel_tcb_table\n\n0804e560 R _kernel_tinib_table\n         U getpid@@GLIBC_2.0\n";
        file.close();

        file.open("test",ios::in);

        FileContainerBinutilsImpl fcbi;
        fcbi.loadSymbols(file);

        TEST_CASE("1","読み込まれたエントリの数が正しい", fcbi.symbol_table.size() == 3);
        TEST_CASE("2","ファイルは閉じられている", !file.is_open());

        remove("test");
    } END_CASE;

    BEGIN_CASE("writeByte","writeByte") {
        FileContainerBinutilsImpl fcbi;

        BEGIN_CASE("1","存在しないページへの書き込み") {
            TEST_CASE("0", "[前提] contentsの要素数は0", fcbi.contents.size() == 0);
            fcbi.writeByte(0x100, 0);

            TEST_CASE("1","contentsの要素が増えている", fcbi.contents.size() == 1);

            const char * scope = fcbi.contents.find(MAKE_BASEADDRESS(0x100))->second + MAKE_OFFSETADDRESS(0x100);
            TEST_CASE("2","書き込まれている内容が正しい", *scope == 0);
        } END_CASE;

        BEGIN_CASE("2","存在するページへの書き込み (連続アクセス)") {
            fcbi.writeByte(0x100, 0xff);

            TEST_CASE("1","contentsの要素が増えていない", fcbi.contents.size() == 1);

            const char * scope = fcbi.contents.find(MAKE_BASEADDRESS(0x100))->second + MAKE_OFFSETADDRESS(0x100);
            TEST_CASE("2","書き込まれている内容が正しい", *scope == 0xff);
        } END_CASE;

        BEGIN_CASE("3","新しいページへの書き込み") {
            fcbi.writeByte(0x10000000, 0xff);

            TEST_CASE("1","contentsの要素が増えている", fcbi.contents.size() == 2);

            const char * scope = fcbi.contents.find(MAKE_BASEADDRESS(0x10000000))->second + MAKE_OFFSETADDRESS(0x10000000);
            TEST_CASE("2","書き込まれている内容が正しい", *scope == 0xff);
        } END_CASE;

        BEGIN_CASE("4","既存のページへのアクセス") {
            fcbi.writeByte(0x100, 0x0);

            TEST_CASE("1","contentsの要素が増えていない", fcbi.contents.size() == 2);

            const char * scope = fcbi.contents.find(MAKE_BASEADDRESS(0x100))->second + MAKE_OFFSETADDRESS(0x100);
            TEST_CASE("2","書き込まれている内容が正しい", *scope == 0x0);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("readRecord","readRecord") {
        fstream file("test",ios::out);
        file << "S315080480F42F6C69622F6C642D6C696E75782E736F98\n\nS315080480F42F6C69622F6C642D6C696E75782E736F98\nS308080481042E320005\nDUMMY\nS31808048108040000001000000001000000474E550056\nS31008048108040000001000000001000000474E550056\n";
        file.close();

        file.open("test",ios::in);
        BEGIN_CASE("1","正常ケース") {
            string work;
            bool result;
            bool exception = false;
            try { result = readRecord(file, work); } catch(Exception &) { exception = true; }
            TEST_CASE("1","例外は起こらない", !exception);
            TEST_CASE("2","関数はtrueを返す", result);
            TEST_CASE("3","読み出された内容が正しい", work.compare("S315080480F42F6C69622F6C642D6C696E75782E736F98") == 0);
        } END_CASE;

        BEGIN_CASE("2","正常ケース (空行の読み飛ばし)") {
            string work;
            bool result;
            bool exception = false;
            try { result = readRecord(file, work); } catch(Exception &) { exception = true; }
            TEST_CASE("1","例外は起こらない", !exception);
            TEST_CASE("2","関数はtrueを返す", result);
            TEST_CASE("3","読み出された内容が正しい", work.compare("S315080480F42F6C69622F6C642D6C696E75782E736F98") == 0);
        } END_CASE;

        BEGIN_CASE("3","チェックサムが誤っているエントリ") {
            string work;
            bool result;
            bool exception = false;
            try { result = readRecord(file, work); } catch(Exception &) { exception = true; }
            TEST_CASE("1","例外をおこす", exception);
        } END_CASE;

        BEGIN_CASE("4","先頭がSで始まらないエントリ") {
            string work;
            bool result;
            bool exception = false;
            try { result = readRecord(file, work); } catch(Exception &) { exception = true; }
            TEST_CASE("1","例外をおこす", exception);
        } END_CASE;

        BEGIN_CASE("5","指定された長さよりも長いエントリ") {
            string work;
            bool result;
            bool exception = false;
            try { result = readRecord(file, work); } catch(Exception &) { exception = true; }
            TEST_CASE("1","例外をおこす", exception);
        } END_CASE;

        BEGIN_CASE("6","指定された長さよりも短いエントリ") {
            string work;
            bool result;
            bool exception = false;
            try { result = readRecord(file, work); } catch(Exception &) { exception = true; }
            TEST_CASE("1","例外をおこす", exception);
        } END_CASE;

        BEGIN_CASE("7","ファイル終端") {
            string work;
            bool result;
            bool exception = false;
            try { result = readRecord(file, work); } catch(Exception &) { exception = true; }
            TEST_CASE("1","例外をおこさない", !exception);
            TEST_CASE("2","関数はfalseを返す", !result);
        } END_CASE;

        remove("test");
    } END_CASE;

    BEGIN_CASE("chopRecord","chopRecord") {
        BEGIN_CASE("1","S1レコード") {
            string src("S106080480F42F4A");

            chopRecord(src);
            TEST_CASE("1","値が正しい", src.compare("80F42F") == 0);
        } END_CASE;

        BEGIN_CASE("2","S2レコード") {
            string src("S206080480F42F4A");

            chopRecord(src);
            TEST_CASE("1","値が正しい", src.compare("F42F") == 0);
        } END_CASE;

        BEGIN_CASE("3","S3レコード") {
            string src("S306080480F42F4A");

            chopRecord(src);
            TEST_CASE("1","値が正しい", src.compare("2F") == 0);
        } END_CASE;

        BEGIN_CASE("4","S4レコード") {
            string src("S406080480F42F4A");

            chopRecord(src);
            TEST_CASE("1","値が正しい", src.compare("080480F42F") == 0);
        } END_CASE;

        BEGIN_CASE("5","S5レコード") {
            string src("S506080480F42F4A");

            chopRecord(src);
            TEST_CASE("1","値が正しい", src.compare("080480F42F") == 0);
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("loadDataContents/loadContents","loadDataContents/loadContents") {
        fstream file("test", ios::out);
        file << "S30D000000000123456789ABCDEF32\nS509FEDCBA9876543210BE";
        file.close();

        BEGIN_CASE("1","正常ケース") {
            FileContainerBinutilsImpl fcbi;

            file.open("test",ios::in);
            bool exception = false;
            try { fcbi.loadDataContents(file); } catch(...) { exception = true; }

            TEST_CASE("1","例外は起こらない", !exception);
            TEST_CASE("2","データが確保されている", fcbi.contents.size() == 1);
            TEST_CASE("3","ファイルは閉じられている", !file.is_open());
            BEGIN_CASE("4","格納した値が正しく読める") {
                unsigned int i;

                assert(sizeof(unsigned int) >= 4);
                i = 0;
                
                fcbi.loadContents(&i, 0, 4);
                TEST_CASE("1","1-4バイト目", i == 0x67452301);
                fcbi.loadContents(&i, 4, 4);
                TEST_CASE("1","5-8バイト目", i == 0xefcdab89);
                fcbi.loadContents(&i, 8, 4);
                TEST_CASE("1","9-12バイト目", i == 0x98badcfe);
                fcbi.loadContents(&i,12, 4);
                TEST_CASE("1","13-16バイト目", i == 0x10325476);
            } END_CASE;
        } END_CASE;
        remove("test");

        BEGIN_CASE("2","loadContentsで一度にページサイズを超える量を要求する") {
            FileContainerBinutilsImpl fcbi;
            unsigned int i;

            for(i=0;i<SIZE_LOADPAGE*2;++i)
                fcbi.writeByte(i, i);

            unsigned char * buffer = new unsigned char [SIZE_LOADPAGE * 2];
            fcbi.loadContents(buffer, 0, SIZE_LOADPAGE * 2);

            for(i=0;i<SIZE_LOADPAGE*2;++i)
                if(*(buffer + i) != (i & 0xff))
                    TEST_FAIL;

            delete [] buffer;
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("searchSymbolPrefix","searchSymbolPrefix") {

        BEGIN_CASE("1","プレフィクスがない") {
            FileContainerBinutilsImpl fcbi;
            fcbi.symbol_table.insert(pair<string, address_t>(MAGIC_SYMBOL, 0x100));

            fcbi.searchSymbolPrefix();
            if(!fcbi.symbol_prefix.empty())
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","変数名に\"_\"がつくタイプ") {
            FileContainerBinutilsImpl fcbi;
            fcbi.symbol_table.insert(pair<string, address_t>("_" MAGIC_SYMBOL, 0x100));

            fcbi.searchSymbolPrefix();
            if(fcbi.symbol_prefix.compare("_") != 0)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","\"__\"には反応しない") {
            FileContainerBinutilsImpl fcbi;
            fcbi.symbol_table.insert(pair<string, address_t>("__" MAGIC_SYMBOL, 0x100));

            fcbi.searchSymbolPrefix();
            if(!fcbi.symbol_prefix.empty())
                TEST_FAIL;
        } END_CASE;
            
        BEGIN_CASE("4","発見できない場合、変更しない") {
            FileContainerBinutilsImpl fcbi;
            fcbi.symbol_prefix.assign("test");

            fcbi.searchSymbolPrefix();
            if(fcbi.symbol_prefix.compare("test") != 0)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("searchByteOrder","searchByteOrder") {
        BEGIN_CASE("1","ホストと同じエンディアン") {
            FileContainerBinutilsImpl fcbi;
            unsigned int value = MAGIC_NUMBER;
            fcbi.symbol_table.insert(pair<string, address_t>(MAGIC_SYMBOL, 0x100));

            for(int i=0;i<4;i++)
                fcbi.writeByte(0x100 + i, *((char *)&value + i));

            fcbi.byteorder = UNKNOWN;
            fcbi.searchByteOrder();

            if(fcbi.byteorder != HOSTORDER)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","ホストと違うエンディアン") {
            FileContainerBinutilsImpl fcbi;
            unsigned int value = MAGIC_NUMBER;
            fcbi.symbol_table.insert(pair<string, address_t>(MAGIC_SYMBOL, 0x100));

            for(int i=0;i<4;i++)
                fcbi.writeByte(0x100 + i, *((char *)&value + (3 - i)));

            fcbi.byteorder = UNKNOWN;
            fcbi.searchByteOrder();

            if(fcbi.byteorder == HOSTORDER)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("3","シンボルが無い") {
            FileContainerBinutilsImpl fcbi;
            unsigned int value = MAGIC_NUMBER;

            for(int i=0;i<4;i++)
                fcbi.writeByte(0x100 + i, *((char *)&value + (3 - i)));

            fcbi.byteorder = UNKNOWN;
            fcbi.searchByteOrder();

            if(fcbi.byteorder != UNKNOWN)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("4","データが無い") {
            FileContainerBinutilsImpl fcbi;
            unsigned int value = MAGIC_NUMBER;
            fcbi.symbol_table.insert(pair<string, address_t>(MAGIC_SYMBOL, 0x100));

            fcbi.byteorder = UNKNOWN;
            fcbi.searchByteOrder();

            if(fcbi.byteorder != UNKNOWN)
                TEST_FAIL;
        } END_CASE;
    } END_CASE;

    chain.restoreContext();
}

#endif /* TESTSUITE */

#endif /* FILECONTAINER_BINUTILS || TESTSUITE */

