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
 *  @(#) $Id: fc_bfd.cpp,v 1.8 2003/12/15 07:32:13 takayuki Exp $
 */


#include "base/filecontainer.h"

#include "base/message.h"

#include "bfd.h"
#include "libiberty.h"

#include <string>
#include <map>

using namespace std;


/*
 *  BFDを使ったファイルコンテナクラス
 */
class filecontainer_BFD : public filecontainer
{
protected:
    bfd * object;                           //オブジェクト
    map<string, asymbol *> symbol_table;    //検索用ハッシュもどき
    asymbol ** symbol_container;

public:
    filecontainer_BFD(void)  throw();
    ~filecontainer_BFD(void) throw();

    virtual bool attach_module(const char *);
    virtual bool change_endian(void * target, unsigned int size);
    virtual bool load_contents(void * dest, unsigned long address, unsigned int size);
    virtual unsigned long get_symbol_address(const char *);
    virtual const char * get_architecture(void);

} Instance_of_filecontainer_BFD;

filecontainer_BFD::filecontainer_BFD(void) throw()
{
    bfd_init();

    object = NULL;
    symbol_table.clear();
    symbol_container = NULL;
    instance = this;
}

filecontainer_BFD::~filecontainer_BFD(void) throw()
{
    if(object != NULL)
        bfd_close(object);
}

/*
 *  attach_module : 対象モジュールをアタッチする
 */
bool filecontainer_BFD::attach_module(const char * filename)
{
    char ** target_list;
    asymbol ** symbols;
    asymbol *  sym;
    int num_syms;
    boolean result;
    int i;

    if(object != NULL)
        bfd_close(object);
    symbol_table.clear();

        //モジュールオープン (読込用)
    object = bfd_openr(filename, "default");

        //ターゲット解決
    target_list = (char **)bfd_target_list();
    result = bfd_check_format_matches(object, bfd_object, &target_list);
    if(result == 0)
        ExceptionMessage("Internel error: BFD could not recognize the target file format.","内部エラー: BFDはファイルの読み出しに失敗しました").throwException();

        //シンボルのハッシュもどき作成
    symbols = (asymbol **)xmalloc( bfd_get_symtab_upper_bound(object) );
    num_syms = bfd_canonicalize_symtab(object, symbols);

    for(i=0;i<num_syms;i++)
    {
        sym = *(symbols+i);
        if(sym != NULL && sym->name != NULL && *(sym->name) != '\x0')
            symbol_table[string(sym->name)] = sym;
    }

    symbol_container = symbols;
    return true;
}

bool filecontainer_BFD::change_endian(void * target, unsigned int size)
{
        //ホストはリトルと仮定
    enum bfd_endian host_endian = BFD_ENDIAN_LITTLE;

    char * top, * tail;

    if(object == NULL)
        return false;

    if(object->xvec->byteorder == BFD_ENDIAN_UNKNOWN)
        return false;

/*
    unsigned int __work = 0x1;
    if( *(char *)__work == 0)
        host_endian = BFD_ENDIAN_BIG;
*/

    if(object->xvec->byteorder == host_endian)
        return true;

    /*
     *  メインループ : bswapしないで、素直に書く
     */

    top  = (char *)target;
    tail = (char *)target+size -1;

    while(top < tail)
    {
        *top ^= *tail, *tail ^= *top, *top ^= *tail;
        top ++;
        tail --;
    }

    return true;
}


bool filecontainer_BFD::load_contents(void * dest, unsigned long address, unsigned int size)
{
    struct sec * section;

    if(object == 0)
        return false;

        //対象アドレスを保有するセクションを探す
    section = object->sections;
    while(section != 0)
    {
        if(address - (unsigned long)section->vma <= section->_raw_size 
            && (section->flags & (SEC_ALLOC|SEC_HAS_CONTENTS)) == (SEC_ALLOC|SEC_HAS_CONTENTS))
        {
                //読み出し
            bfd_get_section_contents(object, section, dest, address - (unsigned long)section->vma, size);
            return true;
        }
        section = section->next;
    }

        //どこにもない
    ExceptionMessage("Internel error: Memory read with unmapped address","内部エラー; マップされてないアドレスを使ってメモリリードが行われました").throwException();

    return false;
}


/*
 * get_symbol_address : シンボル名からアドレス値を取得する
 */
unsigned long filecontainer_BFD::get_symbol_address(const char * symbol)
{
    map<string, asymbol *>::iterator scope;
    string symbol_name;

    if(object == 0)
        ExceptionMessage("Not initialized","初期化されてません").throwException();

        //シンボル名を生成する ("_"とかの処理)
    if(object->xvec->symbol_leading_char != '\x0')
        symbol_name += object->xvec->symbol_leading_char;
    symbol_name += symbol;

    scope = symbol_table.find(symbol_name);
    if(scope == symbol_table.end())
        return 0;
        //Exception("Internal error: Unknown symbol [%s]","内部エラー: 不明なシンボル [%s]").format(symbol_name.c_str());

        //Address = セクション内オフセット値 + セクションのVMA
    return (*scope).second->value + (*scope).second->section->vma;
}

/*
 * get_architecture : アーキテクチャ名の取得
 */
const char * filecontainer_BFD::get_architecture(void)
{
    if(object == NULL)
        return "Unknown";

        //とりあえずターゲット名を持ってアーキテクチャ名にしておく。
        // #どうせバナーにしか使ってないしね 今のところ
    return object->xvec->name;
}

