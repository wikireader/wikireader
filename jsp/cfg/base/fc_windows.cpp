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
 *  @(#) $Id: fc_windows.cpp,v 1.12 2003/12/20 06:51:58 takayuki Exp $
 */

#if (defined(FILECONTAINER_WINDOWS) || defined(TESTSUITE)) && defined(_MSC_VER)

#pragma warning(disable:4786)

#include "base/filecontainer.h"
#include <windows.h>
#include <imagehlp.h>
#include <string>

using namespace std;

namespace {

    class FileContainerWindowsImpl : public FileContainer
    {
    protected:
        HANDLE          process;
        LOADED_IMAGE    image;
        DWORD           base;

        inline bool isLoaded(void) const
        {   return base != 0;   }

    public:
        FileContainerWindowsImpl(void) throw();
        virtual ~FileContainerWindowsImpl(void) throw();

            /* インタフェース部 */
        virtual void        attachModule(const string & filename) throw(Exception);
        virtual void        loadContents(void * dest, address_t address, size_t size) throw(Exception);
        virtual address_t   getSymbolAddress(const string & symbol) throw(Exception);
        virtual std::string getArchitecture(void) throw();

        TESTSUITE_PROTOTYPE(main)
    };

    FileContainerWindowsImpl instance_of_FileContaienrWindowsImpl;

        /* コンストラクタ */
    FileContainerWindowsImpl::FileContainerWindowsImpl(void) throw()
        : process(NULL), image(), base(0)
    {}

        /* デストラクタ */
    FileContainerWindowsImpl::~FileContainerWindowsImpl(void) throw()
    {
        if(isLoaded()) {
            ::UnMapAndLoad(&image);
            ::SymUnloadModule(process, base);
            process = NULL;
            base    = 0;
        }
    }

        /* 対象モジュールの割付 */
    void FileContainerWindowsImpl::attachModule(const std::string & _filename) throw(Exception)
    {
        string filename(_filename);

        process = ::GetCurrentProcess();
        if(::SymInitialize( process , NULL, FALSE) == FALSE)
            ExceptionMessage("[Internal Error] ImageHelper API initialization failure","[内部エラー] 初期化に失敗しました (ImageHlp)").throwException();

        base = ::SymLoadModule(process, NULL, (PSTR)filename.c_str(), NULL, 0, 0);

        image.SizeOfImage = sizeof(LOADED_IMAGE);
        if(::MapAndLoad((PSTR)filename.c_str(), NULL, &image, FALSE, TRUE) == FALSE)
            ExceptionMessage("[Internel error] Module loading failure [%]","[内部エラー] モジュールの読み込みに失敗しました [%]") << filename << throwException;
    }

        /* 内容の取得 */
    void FileContainerWindowsImpl::loadContents(void * dest, address_t address, size_t size) throw(Exception)
    {
        PIMAGE_SECTION_HEADER header;
        unsigned int i;

        address -= base;
        for(i=0;i<image.NumberOfSections;i++) {
            header = image.Sections+i;
            if(address >= header->VirtualAddress && address < header->VirtualAddress + header->SizeOfRawData) {
                address -= header->VirtualAddress - header->PointerToRawData;
                ::CopyMemory(dest,image.MappedAddress + address,size);
                break;
            }
        }

        if(i == image.NumberOfSections)
            ExceptionMessage("[Internel error] Memory read with unmapped address","[内部エラー] マップされてないアドレスを使ってメモリリードが行われました").throwException();
    }

        /* シンボルアドレスの解決 */
    FileContainer::address_t FileContainerWindowsImpl::getSymbolAddress(const string & _symbol) throw(Exception)
    {
        FileContainer::address_t result = 0;
        string symbol(_symbol);
        IMAGEHLP_SYMBOL sym;

        if(process == NULL || base == 0)
            ExceptionMessage("Not initialized","初期化されてません").throwException();

        sym.SizeOfStruct = sizeof(sym);
        sym.MaxNameLength = 0;

        if(::SymGetSymFromName(process, (PSTR)symbol.c_str(), &sym) == TRUE)
            result = static_cast<FileContainer::address_t>(sym.Address);
     
        if(result == 0)
            ExceptionMessage("Unknown symbol [%]","不正なシンボル名 [%]") << symbol << throwException;

        return static_cast<FileContainer::address_t>(sym.Address);
    }

    string FileContainerWindowsImpl::getArchitecture(void) throw()
    {   return "Windows (Win32)";   }

}

//---------------------------------------------

#ifdef TESTSUITE
#include "base/coverage_undefs.h"

#pragma warning(disable:4311) //'reinterpret_cast' : ポインタを 'const int *__w64 ' から 'FileContainer::address_t' へ切り詰めます。

extern "C" const int FileContainerWindowsImplTestVariable = 0x01234567;
extern "C" const int _FileContainerWindowsImplTestVariableWithUnderbar = 0x89abcdef;

TESTSUITE(main, FileContainerWindowsImpl)
{
    PREDECESSOR("TFileContainer");

    SingletonBase::ContextChain chain;
    chain.saveContext<RuntimeObjectTable>();
    chain.renewInstance();

    BEGIN_CASE("attachModule","attachModule") {
        BEGIN_CASE("1","実行しているプログラムが開けるか") {
            FileContainerWindowsImpl fcwi;
            bool result = true;
            try { fcwi.attachModule(TestSuite::getProgName()); } catch(...) { result = false; }
            
            TEST_CASE("1", "例外は起きない", result);
        } END_CASE;

        BEGIN_CASE("2","存在しないファイル名で例外") {
            FileContainerWindowsImpl fcwi;
            bool result = false;
            try { fcwi.attachModule("..."); } catch(...) { result = true; }
            
            TEST_CASE("1", "例外が起きる", result);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("getSymbolAddress","getSymbolAddress") {
        FileContainerWindowsImpl fcwi;

        BEGIN_CASE("1","初期化していない状態で検索する") {
            bool result = false;
            try { fcwi.getSymbolAddress("FileContainerWindowsImplTestVariable"); } catch(...) { result = true; }
            TEST_CASE("1","例外がおきる", result);
        } END_CASE;

        fcwi.attachModule(TestSuite::getProgName());

        BEGIN_CASE("2","存在するシンボルを検索する") {
            FileContainer::address_t addr = 0;
            bool result = true;

            try { addr = fcwi.getSymbolAddress("FileContainerWindowsImplTestVariable"); } catch(...) { result = false; }

            TEST_CASE("1","例外は起きない", result);
            TEST_CASE("2","アドレスが正しい", addr == reinterpret_cast<FileContainer::address_t>(&FileContainerWindowsImplTestVariable));
        } END_CASE;

        BEGIN_CASE("3","余計な_を勝手に付加しない") {
            FileContainer::address_t addr = 0;
            bool result = false;

            try { addr = fcwi.getSymbolAddress("FileContainerWindowsImplTestVariableWithUnderbar"); } catch(...) { result = true; }

            TEST_CASE("1","例外が起きる", result);
            TEST_CASE("2","アドレスは0のまま", addr == 0);
        } END_CASE;

        BEGIN_CASE("4","存在しないシンボルを検索する") {
            FileContainer::address_t addr = 0;
            bool result = false;

            try { addr = fcwi.getSymbolAddress("____unknown____"); } catch(...) { result = true; }

            TEST_CASE("1","例外がおきる", result);
        } END_CASE;
    } END_CASE;

    BEGIN_CASE("loadContents","loadContents") {
        FileContainerWindowsImpl fcwi;
        fcwi.attachModule(TestSuite::getProgName());

        BEGIN_CASE("1","存在する変数を読み出す") {
            FileContainer::address_t addr;
            int i;

            addr = fcwi.getSymbolAddress("FileContainerWindowsImplTestVariable");
            bool result = true;
            try { fcwi.loadContents(&i, addr, sizeof(i)); } catch(...) { result = false; }

            TEST_CASE("1","例外は起きない", result);
            TEST_CASE("2","読み出された内容は正しい", i == FileContainerWindowsImplTestVariable);
        } END_CASE;

        BEGIN_CASE("2","存在しない変数を読み出す") {
            FileContainer::address_t addr;
            int i;

            addr = ~0;
            bool result = false;
            try { fcwi.loadContents(&i, addr, sizeof(i)); } catch(Exception &) { result = true; }

            TEST_CASE("1","例外が起こる", result);
        } END_CASE;

    } END_CASE;

    chain.restoreContext();
}

#endif /* TESTSUITE */

#endif /* FILECONTAINER_WINDOWS || TESTSUITE */

