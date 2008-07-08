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
 *  @(#) $Id: filecontainer.cpp,v 1.11 2003/12/20 06:51:58 takayuki Exp $
 */

#ifdef _MSC_VER
#pragma warning(disable:4786) //デバッグ文字列を255文字に切り詰めた
#endif

#include "base/filecontainer.h"
#include <memory.h>
#include <fstream>

using namespace std;

namespace {
        /* 符号なし10進文字列のパース */
    char * parse_uint(char * src, unsigned int * dest, unsigned int default_value = 0)
    {
        int result = 0;

        if(*src < '0' || *src > '9') {
            *dest = default_value;
            return src;
        }

        do {
            result = result * 10 + *(src++) - '0';
        } while(*src >= '0' && *src <='9');

        *dest = result;
        return src;
    }
}

    /* 変数情報の読み込み */
void FileContainer::attachInfo(const string & filename) throw(Exception)
{
    char    buffer[256];
    fstream file;

    char * work;
    struct tagVariableInfo info;

    memset(&info, 0, sizeof(info));

    file.open(filename.c_str(), ios::in);
    if(!file.is_open()) {
        ExceptionMessage("Structure information file(%) was not found. ","構造体情報ファイル(%)が見つかりません ") << filename << throwException;
        return;
    }

    while(!file.eof())
    {
        file.get(buffer, 256);

        if(!file.fail()) {
            switch(buffer[0])
            {
            case 's':
                {
                    unsigned int address = 0;
                    unsigned int size    = 0;

                    work = buffer+1;
                    while(*work != ',' && *work != '\x0')
                        work++;
                    if(*work != '\x0') {
                        *work = '\x0';
                        work = parse_uint(work+1,&size);
                        work = parse_uint(work+1,&address);
                    }

                    info.address = static_cast<address_t>(address);
                    info.size    = static_cast<size_t>(size);
                    variableinfo[string(buffer+1)] = info;
                    break;
                }

            case 'd':   //識別子名は無視
                break;

            default:
                ExceptionMessage("Unknown information type identifier found - [%]","未知の型識別子 - [%]") << buffer[0] << throwException;
            }
        }
        else
            file.clear();

        file.get();
    }

    file.close();
}

    /* 変数情報の取得 */
struct FileContainer::tagVariableInfo FileContainer::getVariableInfo(const string & name) throw(Exception)
{
    map<string, tagVariableInfo>::const_iterator scope;

    scope = variableinfo.find(name);
    if(scope == variableinfo.end()) {
        ExceptionMessage("Operation against the unknwon symbol [%] was rejected.","不明なシンボル名 [%] の情報を取得しようとしています") << name << throwException;

        struct tagVariableInfo dummy;
        memset(&dummy, 0, sizeof(dummy));
        return dummy;
    }

    return scope->second;
}


    /* コンストラクタ (シンボル名を指定して初期化) */
TargetVariableBase::TargetVariableBase(const std::string & sym) throw() 
    : address(0), size(0), offset(0), structure_size(0), loaded(false)
{
    try{
        FileContainer * fc = FileContainer::getInstance();
        address = fc->getSymbolAddress(sym);
        size    = fc->getVariableInfo(sym).size;
        structure_size = size;
    }
    catch(...) {
        address = 0;
        size    = 0;
    }
}

    /* コンストラクタ (シンボル名, データ長を指定して初期化) */
TargetVariableBase::TargetVariableBase(const string & sym, size_t _size) throw() 
    : address(0), size(_size), offset(0), structure_size(_size), loaded(false)
{
    try{
        FileContainer * fc = FileContainer::getInstance();
        address = fc->getSymbolAddress(sym);
    }
    catch(...) {
        address = 0;
        size    = 0;
    }
}

    /* コンストラクタ (シンボル名、メンバ名を指定して初期化) */
TargetVariableBase::TargetVariableBase(const string & sym, const string & sz) throw() 
    : address(0), size(0), offset(0), structure_size(0), loaded(false)
{
    try{
        FileContainer * fc = FileContainer::getInstance();
        address = fc->getSymbolAddress(sym);                                //構造体の先頭アドレス
        size    = fc->getVariableInfo(sz).size;                             //メンバ変数の長さ
        offset  = static_cast<size_t>(fc->getVariableInfo(sz).address);     //メンバ変数のオフセット
        structure_size = fc->getVariableInfo(sym).size;                     //構造体の長さ
    }
    catch(...) {
        address = 0;
        size    = 0;
    }
}

    /* コピーコンストラクタ */
TargetVariableBase::TargetVariableBase(const TargetVariableBase & src) throw()
    : address(0), size(0), offset(0), structure_size(0), loaded(false)
{
    if(src.isValid()) {
        address        = src.address;
        size           = src.size;
        offset         = src.offset;   
        structure_size = src.structure_size;
        loaded         = false;
    }
}


    /* オブジェクト内容の取得 */
void TargetVariableBase::loadContent(void * dest, size_t dest_size) throw(Exception)
{
    char * buffer = 0;

    try {
        if(isValid()) {
            buffer = new char[getSize()];
            FileContainer * fc = FileContainer::getInstance();

            fc->loadContents(buffer, getAddress() + getOffset(), getSize());
            if(fc->getByteOrder() != FileContainer::HOSTORDER)
                changeEndian(buffer, getSize());

            if(size < dest_size)
                memset(dest, 0, dest_size);
            else if(size > dest_size)
                ExceptionMessage("[TargetVariableBase::loadContent] Destination storage is too small to store the contents.","[TargetVariableBase::loadContent] 出力先バッファのサイズが小さすぎます").throwException();
            memcpy(dest, buffer, getSize());

            delete [] buffer;
            loaded = true;
        }
        else {
            ExceptionMessage("loadContent was performed with an illegal target variable.","不正なターゲット変数情報でloadContentが呼ばれました").throwException();
        }
    }
    catch(bad_alloc &) {
        ExceptionMessage("No available memory [%B]","メモリ不足です [%B]") << (unsigned int)getSize() << throwException;
    }
    catch(Exception &) {
        if(buffer)
            delete [] buffer;
        throw;
    }
}

    /* バイトオーダの変更 */
void TargetVariableBase::changeEndian(char * buffer, size_t size) throw()
{
    char * tail = buffer + size - 1;

    while(buffer < tail) {
        char temporary = *buffer;
        *buffer = *tail;
        *tail   = temporary;

        ++ buffer;
        -- tail;
    }
}



//---------------------------------------------

#ifdef TESTSUITE
#include "base/coverage_undefs.h"

#include <cctype>
#define _isprint(x) isprint(x)

class TFileContainer : public FileContainer
{
public:
    TFileContainer(void) throw() {}
    virtual ~TFileContainer(void) throw() {}

    string module;
    void attachModule(const std::string & _module) throw(Exception) 
    {
        CHECKPOINT("attachModule"); 
        module = _module;
    }

    void *    dest;
    address_t addr;
    size_t    sz;
    void loadContents(void * _dest, address_t _addr, size_t _sz) throw(Exception) 
    {
        CHECKPOINT("loadContents");
        dest = _dest;
        addr = _addr;
        sz   = _sz;
        memset(_dest, ~0, _sz);
    }

    string symbol;
    address_t getSymbolAddress(const std::string & _symbol) throw(Exception) 
    {
        CHECKPOINT("getSymbolAddress");
        symbol = _symbol;
        return 1; 
    }

    std::string getArchitecture(void) throw(Exception)
    {   return "TFileContainer";   }

    TESTSUITE_PROTOTYPE(main)
};

TESTSUITE(main, TFileContainer)
{
    PREDECESSOR("SingletonBase");
    PREDECESSOR("RuntimeObjectTable");

    SingletonBase::ContextChain chain;
    chain.saveContext<RuntimeObjectTable>();
    chain.renewInstance();

    BEGIN_CASE("getInstance","getInstance") {
        BEGIN_CASE("1","何も登録されていないときのgetInstanceで例外") {
            bool result = false;
            try { FileContainer::getInstance(); } catch(Exception &) { result = true; }
            if(!result)
                TEST_FAIL;
        } END_CASE;

        BEGIN_CASE("2","getInstanceでインスタンスが返る") {
            TFileContainer fc;
            FileContainer * instance;
            bool result = true;
            try { instance = FileContainer::getInstance(); } catch(...) { result = false; }
            TEST_CASE("1","例外はおきない", result);
            TEST_CASE("2","取得できるインスタンスが正しい", instance == &fc);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("parse_uint","parse_uint") {
        BEGIN_CASE("1","数字を切り出せる") {
            unsigned int dest;
            char * src = "123";
            char * result = parse_uint(src, &dest);

            TEST_CASE("1","resultが最後に来ている", result == src + 3);
            TEST_CASE("2","値は正しい", dest == 123);
        } END_CASE;

        BEGIN_CASE("2","数字で無い文字を与えるとデフォルト値が返る") {
            unsigned int dest;
            char * src = "abc";
            char * result = parse_uint(src, &dest, 1234567);

            TEST_CASE("1","resultは進んでいない", result == src);
            TEST_CASE("2","デフォルト値が返る", dest == 1234567);
        } END_CASE;

        BEGIN_CASE("3","デリミタでとまる") {
            unsigned int dest;
            char * src = "321,123";
            char * result = parse_uint(src, &dest);

            TEST_CASE("1","resultは進んでいない", result == src + 3);
            TEST_CASE("2","デフォルト値が返る", dest == 321);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("attachInfo","attachInfo") {
        BEGIN_CASE("1","普通に処理をさせる") {
            fstream file("dummy",ios::out);
            file << "s_kernel_tinib_table,32,0\nstask_initialization_block::texrtn,4,28\n";
            file.close();

            TFileContainer fc;

            bool result = true;
            try { fc.attachInfo("dummy"); } catch(...) { result = false; }

            TEST_CASE("1","例外はおきない", result);

            map<string, struct tagVariableInfo>::iterator scope = fc.variableinfo.find("_kernel_tinib_table");
            TEST_CASE("2","要素が追加されている (tinib)", scope != fc.variableinfo.end());
            TEST_CASE("3","値が正しい (tinib)", scope->second.size == 32);

            scope = fc.variableinfo.find("task_initialization_block::texrtn");
            TEST_CASE("4","要素が追加されている (texrtn)", scope != fc.variableinfo.end());
            TEST_CASE("5","値が正しい (texrtn.size)", scope->second.size == 4);
            TEST_CASE("6","値が正しい (texrtn.address)", scope->second.address == 28);
        } END_CASE;
        remove("dummy");

        BEGIN_CASE("2","識別文字 'd' を無視する") {
            fstream file("dummy",ios::out);
            file << "ddummy,0\ndtask_initialization_block::texrtn,4,28\n";
            file.close();

            TFileContainer fc;

            bool result = true;
            try { fc.attachInfo("dummy"); } catch(...) { result = false; }

            TEST_CASE("1","例外はおきない", result);
            TEST_CASE("2","要素は追加されていない", fc.variableinfo.size() == 0);
        } END_CASE;
        remove("dummy");

        BEGIN_CASE("3","空行を無視できる") {
            fstream file("dummy",ios::out);
            file << "s_kernel_tinib_table,32,0\n\nstask_initialization_block::texrtn,4,28\n";
            file.close();

            TFileContainer fc;

            bool result = true;
            try { fc.attachInfo("dummy"); } catch(...) { result = false; }

            TEST_CASE("1","例外はおきない", result);

            map<string, struct tagVariableInfo>::iterator scope = fc.variableinfo.find("_kernel_tinib_table");
            TEST_CASE("2","要素が追加されている (tinib)", scope != fc.variableinfo.end());
            TEST_CASE("3","値が正しい (tinib)", scope->second.size == 32);

            scope = fc.variableinfo.find("task_initialization_block::texrtn");
            TEST_CASE("4","要素が追加されている (texrtn)", scope != fc.variableinfo.end());
            TEST_CASE("5","値が正しい (texrtn.size)", scope->second.size == 4);
            TEST_CASE("6","値が正しい (texrtn.address)", scope->second.address == 28);
        } END_CASE;
        remove("dummy");

        BEGIN_CASE("4","'s'と'd'以外をはじく") {
            char c;
            char msg[2] = {0, 0};

            for(c=0;c<128;++c) {
                if(isprint(c) && (c != 's' && c != 'd')) {
                    fstream file("dummy",ios::out);
                    file << c;
                    file << "dummy,0\n";
                    file.close();

                    TFileContainer fc;

                    bool result = false;
                    try { fc.attachInfo("dummy"); } catch(...) { result = true; }

                    msg[0] = c;
                    TEST_CASE_("1","例外がおきる", result, string(msg));
                    TEST_CASE_("2","要素は追加されていない", fc.variableinfo.size() == 0, string(msg));
                }
            }
        } END_CASE;
        remove("dummy");
    } END_CASE;

    chain.restoreContext();
}

class TTargetVariable : public TargetVariable<int>
{
    TESTSUITE_PROTOTYPE(main)

        /*
         * コンストラクタ (TargetVariable<int>に回送)
         */
    inline TTargetVariable(FileContainer::address_t addr) throw()
        : TargetVariable<int>(addr)
    {}
    
    inline TTargetVariable(FileContainer::address_t addr, size_t sz) throw()
        : TargetVariable<int>(addr, sz)
    {}

    inline TTargetVariable(FileContainer::address_t addr, size_t sz, size_t ofs, size_t ssz) throw()
        : TargetVariable<int>(addr, sz, ofs, ssz)
    {}

    inline TTargetVariable(const std::string & sym) throw()
        : TargetVariable<int>(sym)
    {}

    inline TTargetVariable(const std::string & sym, const std::string & sz) throw()
        : TargetVariable<int>(sym, sz)
    {}

        /* デストラクタ (特に何もしない) */
    virtual ~TTargetVariable(void) throw() 
    {}
};

TESTSUITE(main, TTargetVariable)
{
    PREDECESSOR("SingletonBase");
    PREDECESSOR("TFileContainer");
    PREDECESSOR("RuntimeObjectTable");

    SingletonBase::ContextChain chain;
    chain.saveContext<RuntimeObjectTable>();
    chain.renewInstance();

    TFileContainer fc;
    fstream file("dummy",ios::out);
    file << "s_test,4,0\ns_kernel_tinib_table,32,0\nstask_initialization_block::texrtn,4,28\n";
    file.close();

    fc.attachInfo("dummy");

    BEGIN_CASE("constructor","コンストラクタ") {
        BEGIN_CASE("1", "(address_t, size_t)") {
            TTargetVariable tv(1, 2);

            TEST_CASE("1","変数は有効", tv.isValid());
            TEST_CASE("2","アドレスは正しい", tv.getAddress() == 1);
            TEST_CASE("3","サイズは正しい", tv.getSize() == 2);
            TEST_CASE("4","オフセットは0", tv.getOffset() == 0);
            TEST_CASE("5","構造体サイズは変数サイズと一緒", tv.getStructureSize() == tv.getSize());
        } END_CASE;

        BEGIN_CASE("2","(address_t, size_t, size_t, size_t)") {
            TTargetVariable tv(1,2,3,4);

            TEST_CASE("1","変数は有効", tv.isValid());
            TEST_CASE("2","アドレスは正しい", tv.getAddress() == 1);
            TEST_CASE("3","サイズは正しい", tv.getSize() == 2);
            TEST_CASE("4","オフセットは正しい", tv.getOffset() == 3);
            TEST_CASE("5","構造体サイズは正しい", tv.getStructureSize() == 4);
        } END_CASE;

        BEGIN_CASE("3","(string)") {
            TestSuite::clearCheckpoints();

            TTargetVariable tv("_test");

            TEST_CASE("1","変数は有効", tv.isValid());
            TEST_CASE("2","getSymbolAddressがコールされている", TestSuite::isReached("getSymbolAddress"));
            TEST_CASE("3","getSymbolAddressの引数が正しい", fc.symbol.compare("_test") == 0);
            TEST_CASE("3","アドレスは正しい", tv.getAddress() == 1);
            TEST_CASE("4","サイズは正しい", tv.getSize() == 4);
            TEST_CASE("5","オフセットは正しい", tv.getOffset() == 0);
            TEST_CASE("6","構造体サイズは正しい", tv.getStructureSize() == 4);
        } END_CASE;

        BEGIN_CASE("4","(string,string)") {
            TestSuite::clearCheckpoints();

            TTargetVariable tv("_kernel_tinib_table", "task_initialization_block::texrtn");

            TEST_CASE("1","変数は有効", tv.isValid());
            TEST_CASE("2","getSymbolAddressがコールされている", TestSuite::isReached("getSymbolAddress"));
            TEST_CASE("3","getSymbolAddressの引数が正しい", fc.symbol.compare("_kernel_tinib_table") == 0);
            TEST_CASE("3","アドレスは正しい", tv.getAddress() == 1);
            TEST_CASE("4","サイズは正しい", tv.getSize() == 4);
            TEST_CASE("5","オフセットは正しい", tv.getOffset() == 28);
            TEST_CASE("6","構造体サイズは正しい", tv.getStructureSize() == 32);
        } END_CASE;

        BEGIN_CASE("5","無効なケース") {
            TEST_CASE("1","アドレス0の変数は無効 (1)", !TTargetVariable(0,4).isValid());
            TEST_CASE("2","アドレス0の変数は無効 (2)", !TTargetVariable(0,4,4,4).isValid());
            TEST_CASE("3","サイズ0の変数は無効 (1)", !TTargetVariable(4,0).isValid());
            TEST_CASE("4","サイズ0の変数は無効 (2)", !TTargetVariable(4,0,4,4).isValid());
            TEST_CASE("5","構造体サイズ0の変数は無効", !TTargetVariable(4,4,4,0).isValid());

            TEST_CASE("6","存在しないシンボル名の変数も無効(1)", !TTargetVariable("unknown").isValid());
            TEST_CASE("7","存在しないシンボル名の変数も無効(2)", !TTargetVariable("unknown","task_initialization_block::texrtn").isValid());
            TEST_CASE("8","存在しないメンバ名の変数も無効", !TTargetVariable("_kernel_tinib_table","unknown").isValid());

            TEST_CASE("10","格納領域が不足している変数", !TTargetVariable("_kernel_tinib_table").isValid());
            TEST_CASE("11","無効な引数をもらった場合", !TTargetVariable(*((TTargetVariable *)0)).isValid());
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("loadContents", "loadContents") {
        BEGIN_CASE("1","通常起動") {
            int i;
            TTargetVariable tv(0x100);

            bool result = true;
            try { tv.loadContent(&i, sizeof(int)); } catch(...) { result = false; }

            TEST_CASE("1","例外は起こらない", result);
            TEST_CASE("2","中身は~0でフィルされている", i == ~0);
        } END_CASE;

        BEGIN_CASE("2","元よりも大きな型で受ける") {
            struct { int first; int second; } i;
            TTargetVariable tv(0x100);

            bool result = true;
            try { tv.loadContent(&i, sizeof(i)); } catch(...) { result = false; }

            TEST_CASE("1","例外は起こらない", result);
            TEST_CASE("2","中身は~0でフィルされている", i.first == ~0);
            TEST_CASE("3","残りは0でフィルされている", i.second == 0);
        } END_CASE;

        BEGIN_CASE("3","無効な要素に対して実行する") {
            TTargetVariable tv(0, 0, 0, 0);
            int i;

            bool result = false;
            try{ tv.loadContent(&i, sizeof(i)); } catch(...) { result = true; }
            TEST_CASE("1","例外が起きる", result);
        } END_CASE;

        BEGIN_CASE("4","元よりも小さな型で受ける") {
            char i;
            TTargetVariable tv(0x100);

            bool result = false;
            try { tv.loadContent(&i, sizeof(i)); } catch(...) { result = true; }

            TEST_CASE("1","例外が起こる", result);
        } END_CASE;

    } END_CASE;

    BEGIN_CASE("pointer_operators","ポインタ風の演算") {
        TargetVariable<int> tv(0x100);
        TargetVariable<int> tvl(0x100);

        *tvl;

        TEST_CASE("0","[前提] tv:アドレスは0x100", tv.getAddress() == 0x100);
        TEST_CASE("0","[前提] tv:サイズは4", tv.getSize() == 4);
        TEST_CASE("0","[前提] tv:構造体サイズも4", tv.getStructureSize() == 4);
        TEST_CASE("0","[前提] tv:読出し未了", !tv.isLoaded());
        TEST_CASE("0","[前提] tvl:アドレスは0x100", tvl.getAddress() == 0x100);
        TEST_CASE("0","[前提] tvl:サイズは4", tvl.getSize() == 4);
        TEST_CASE("0","[前提] tvl:構造体サイズも4", tvl.getStructureSize() == 4);
        TEST_CASE("0","[前提] tvl:読出し完了", tvl.isLoaded());

        BEGIN_CASE("1","operator +") {
            TestSuite::clearCheckpoints();

            TargetVariable<int> work = tv + 0x10;
            TEST_CASE("1","workのアドレスは0x140", work.getAddress() == 0x100 + sizeof(int) * 0x10);
            TEST_CASE("2","workは読み込まれていない", !work.isLoaded());
        } END_CASE;

        BEGIN_CASE("2","operator -") {
            TestSuite::clearCheckpoints();

            TargetVariable<int> work = tv - 0x10;
            TEST_CASE("1","workのアドレスは0xc0", work.getAddress() == 0x100 - sizeof(int) * 0x10);
            TEST_CASE("2","workは読み込まれていない", !work.isLoaded());
        } END_CASE;

        BEGIN_CASE("3","operator ++") {
            TestSuite::clearCheckpoints();

            TargetVariable<int> work(tvl);

            TargetVariable<int> result = (++ work);

            TEST_CASE("1","workのアドレスは0x104", work.getAddress() == 0x100 + sizeof(int));
            TEST_CASE("2","work == result", work == result);
            TEST_CASE("3","読み込み未了状態", !result.isLoaded());
        } END_CASE;

        BEGIN_CASE("4","operator ++(int)") {
            TestSuite::clearCheckpoints();

            TargetVariable<int> work(tvl);

            TargetVariable<int> result = (work++);

            TEST_CASE("1","workのアドレスは0x104", work.getAddress() == 0x100 + sizeof(int));
            TEST_CASE("2","resultのアドレスは0x100", result.getAddress() == 0x100);
            TEST_CASE("3","読み込み完了状態", result.isLoaded());
        } END_CASE;

        BEGIN_CASE("5","operator --") {
            TestSuite::clearCheckpoints();

            TargetVariable<int> work(tvl);

            TargetVariable<int> result = (-- work);

            TEST_CASE("1","workのアドレスは0x9c", work.getAddress() == 0x100 - sizeof(int));
            TEST_CASE("2","work == result", work == result);
            TEST_CASE("3","読み込み未了状態", !result.isLoaded());
        } END_CASE;

        BEGIN_CASE("6","operator --(int)") {
            TestSuite::clearCheckpoints();

            TargetVariable<int> work(tvl);

            TargetVariable<int> result = (work--);

            TEST_CASE("1","workのアドレスは0x9c", work.getAddress() == 0x100 - sizeof(int));
            TEST_CASE("2","resultのアドレスは0x100", result.getAddress() == 0x100);
            TEST_CASE("3","読み込み完了状態", result.isLoaded());
        } END_CASE;

        BEGIN_CASE("7","operator +=") {
            TestSuite::clearCheckpoints();

            TargetVariable<int> work(tvl);

            work += 0x10;

            TEST_CASE("1","workのアドレスは0x140", work.getAddress() == 0x100 + sizeof(int) * 0x10);
            TEST_CASE("2","読み込み未了状態", !work.isLoaded());
        } END_CASE;

        BEGIN_CASE("8","operator -=") {
            TestSuite::clearCheckpoints();

            TargetVariable<int> work(tvl);

            work -= 0x10;

            TEST_CASE("1","workのアドレスは0xc0", work.getAddress() == 0x100 - sizeof(int) * 0x10);
            TEST_CASE("2","読み込み未了状態", !work.isLoaded());
        } END_CASE;

        BEGIN_CASE("9", "operator &") {
            TEST_CASE("1","アドレスが抜ける", &tv == 0x100);
            TEST_CASE("2","NULLインスタンスには0が返る", &(*((TargetVariable<int> *)0)) == 0);
        } END_CASE;

        BEGIN_CASE("10", "operator *") {
            TestSuite::clearCheckpoints();
            TargetVariable<int> work(0x100);

            const void * result = &(*work);

            TEST_CASE("1","loadContentsが実行されている", TestSuite::isReached("loadContents"));
            TEST_CASE("2","引数が正しい", fc.addr == 0x100);
            TEST_CASE("3","引数が正しい", fc.sz == sizeof(int));

            TestSuite::clearCheckpoints();
            *work;

            TEST_CASE("4","loadContentsは実行されない", !TestSuite::isReached("loadContents"));
        } END_CASE;

        BEGIN_CASE("11","operator ==/!=") {
            TEST_CASE("1","アドレスが一致すればOK", tv == TTargetVariable(tv.getAddress()));
            TEST_CASE("2","NULLインスタンスとの比較もできる", tv != *((TTargetVariable *)0));
        } END_CASE;

        BEGIN_CASE("cvr","カバレッジ対策") {
            
            (*((TTargetVariable *)0)) ++;

        } END_CASE;

    } END_CASE;

    chain.restoreContext();
}


#endif

