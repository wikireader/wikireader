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
 *  @(#) $Id: jsp_check.cpp,v 1.25 2005/07/06 01:29:21 honda Exp $
 */

// $Header: /home/CVS/configurator/jsp/jsp_check.cpp,v 1.25 2005/07/06 01:29:21 honda Exp $

#include "base/defs.h"
#include "jsp/jsp_defs.h"
#include "base/message.h"
#include "base/component.h"
#include "base/filecontainer.h"

#include <fstream>
#include <iomanip>

class ConfigurationChecker : public Component
{
public:

    enum tagCheckLevel
    {
        UNKNOWN    = 0,
        LAZY       = 1,     /* 致命的 (行き過ぎ改造防止用)  */
        STANDARD   = 2,     /* ITRON仕様の範囲 (改造を許容) */
        TOPPERS    = 4,     /* TOPPERS/JSPの範囲内          */
        RESTRICTED = 8,     /* 片っ端から捕まえる           */

        NORMAL     = 8
    };

protected:
    enum tagCheckLevel current_level;

    unsigned int error_count;
    std::string banner;

    void set_banner(Directory &, Formatter , const char *, int);
    void notify(enum tagCheckLevel, Formatter , bool = true);

    bool check_taskblock(Directory &, FileContainer *);
    bool check_semaphoreblock(Directory &, FileContainer *);
    bool check_eventflagblock(Directory &, FileContainer *);
    bool check_dataqueueblock(Directory &, FileContainer *);
    bool check_mailboxblock(Directory &, FileContainer *);
    bool check_fixed_memorypoolblock(Directory &, FileContainer *);
    bool check_cyclic_handlerblock(Directory &, FileContainer *);
    bool check_interrupt_handlerblock(Directory &, FileContainer *);
    bool check_exception_handlerblock(Directory &, FileContainer *);

    virtual void parseOption(Directory &);
    virtual void body(Directory &);

public:
    ConfigurationChecker(void) throw();
    ~ConfigurationChecker(void) throw() {}
};

//------------------------------------------------------
using namespace std;

static ConfigurationChecker  instance_of_ConfigurationChecker;

//------------------------------------------------------
    //ターゲットの同名の型よりも大きな型の定義
    // (注) 値比較, 演算が可能な型であること

typedef          int  DT_INT;
typedef unsigned int  DT_UINT;
typedef unsigned long DT_FP;
typedef unsigned long DT_VP_INT;
typedef unsigned long DT_VP;
typedef          long DT_RELTIM;

//------------------------------------------------------

ConfigurationChecker::ConfigurationChecker(void) throw()
{
    setBanner("--- TOPPERS/JSP Configuration Checker (ver 2.4) ---");
}

void ConfigurationChecker::set_banner(Directory & container, Formatter object, const char * type, int id)
{
    Directory * node;
    char buffer[32];

    banner = string("    ") + object.str() + " : ";

    sprintf(buffer, "id = %d", id);

    node = container.findChild(OBJECTTREE, type, NULL)->getFirstChild();
    while(node != 0 && node->toInteger() != id)
        node = node->getNext();

    if( node != 0 ) {
        banner += node->getKey() + " (" + buffer + ") ";

        node = node->findChild("position");
        if(node != 0)
            banner += string("at ") + node->toString();
    }
    else
        banner += buffer;

    if(VerboseMessage::getVerbose())
    {
        cout << banner << endl;
        banner.erase();
    }
}

void ConfigurationChecker::notify(enum tagCheckLevel level, Formatter msg, bool error)
{
    if((level & current_level) != 0)
    {

        if(!banner.empty())
        {
            cout << banner << endl;
            banner.erase();
        }

        cout << "      ";

        if(error)
        {
            cout << Message("[ Error ] ","[エラー] ");
            error_count ++;
        }else
            cout << Message("[Warning] ","[ 警告 ] ");
        cout << msg << endl;
    }
}

    /*
     * タスクオブジェクトに関するエラー検出
     */

bool ConfigurationChecker::check_taskblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int maxpri;
    unsigned int minpri;
    unsigned int old_error_count = error_count;

    TargetVariable<unsigned int> _kernel_tmax_tskid("_kernel_tmax_tskid");

    Message object("Task","タスク");

    if(!_kernel_tmax_tskid.isValid())
        ExceptionMessage(
            "Internal error: Unknown symbol (Probably, Symbol table was stripped)",
            "内部エラー: 不正なシンボル名 (実行形式がシンボルテーブルを含んでない可能性があります)").throwException();

    if(*_kernel_tmax_tskid < 1)
    {
        notify(RESTRICTED,
            Message("  [Task] : No tasks created\n","  [タスク] : タスクオブジェクトがありません\n"));
        return true;
    }

    TargetVariable<DT_UINT> tskatr("_kernel_tinib_table", "task_initialization_block::tskatr");
    TargetVariable<DT_FP>   task("_kernel_tinib_table", "task_initialization_block::task");
    TargetVariable<DT_INT>  ipriority("_kernel_tinib_table", "task_initialization_block::ipriority");
    TargetVariable<DT_UINT> texatr("_kernel_tinib_table", "task_initialization_block::texatr");
    TargetVariable<DT_UINT> stksz("_kernel_tinib_table", "task_initialization_block::stksz");

    maxpri = container->getVariableInfo("TMAX_TPRI").value;
    minpri = container->getVariableInfo("TMIN_TPRI").value;

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n") << object << *_kernel_tmax_tskid;
    for(id = 1; id <= *_kernel_tmax_tskid; id++)
    {
        set_banner(parameter, object, TASK, id);

            /*
             *  属性チェック
             */

            // 属性値が TA_HLNG|TA_ASM|TA_ACT 以外の値をとっている
        if((*tskatr & ~0x3) != 0)
            notify( STANDARD,
                Message("Illegal task attribute (It should be ((TA_HLNG||TA_ASM)|TA_ACT))",
                        "不正なタスク属性 ((TA_HLNG||TA_ASM)|TA_ACT)以外"));

            // 属性値に TA_ASM が含まれている
        if((*tskatr & 0x1) != 0)
            notify( RESTRICTED,
                Message("TA_ASM specified as task attribute takes no effect.",
                        "タスク属性にTA_ASMが指定されている"));

            /* 起動番地が0 */
        if(*task == 0)
            notify(RESTRICTED,
                Message("The address of task routine is equal to zero.",
                        "開始番地に0が設定されています"));

            /*
             *  優先度チェック
             */

            // 最大優先度と最小優先度の設定がおかしい
        if(maxpri < minpri)
            notify(LAZY,
                Message("Illegal Priority Settings found (TMAX_TPRI(%) < TMIN_TPRI).",
                        "初期優先度が最低優先度(%)を超えている") << minpri);

            // 優先度の範囲が[最小優先度, 最大優先度]の範囲を超えている
        if(*ipriority > (signed)(maxpri - minpri))
            notify(TOPPERS,
                Message("Initial priority is greater than maximum priority (%).",
                        "初期優先度が最大優先度(%)を超えている") << maxpri);
        if(*ipriority < 0)
            notify(STANDARD,
                Message("Initial priority is less than the minimum priority (%).",
                        "初期優先度が最低優先度(%)を下回る") << minpri);

            /*
             *   タスク例外属性チェック
             */

            // 属性値がTA_HLNG or TA_ASMでない
        if((*texatr & ~0x3) != 0)
            notify(STANDARD,
                Message("Task exception routine has an illegal attribute specifier.",
                        "タスク例外に無効な属性(TA_HLNG,TA_ASM以外) が設定されています"));

            // 属性値に TA_ASM が含まれている
        if((*texatr & 0x1) != 0)
            notify( RESTRICTED,
                Message("TA_ASM, specified as texatr, does not always take effect.",
                        "タスク例外にTA_ASMが指定されています"));

            /*
             *   スタックチェック
             */

            // スタックサイズが0
        if(*stksz == 0)
            notify(RESTRICTED,
                Message("Stack size is equal to zero.",
                        "スタックサイズに0が設定されています"));

            // スタックの番地が0
        if(*stksz == 0)
            notify(RESTRICTED,
                Message("The address of task stack is equal to zero.",
                        "スタック開始番地に0が設定されています"));

        ++ tskatr, ++ task, ++ ipriority, ++ texatr, ++ stksz;
    }

    return old_error_count == error_count;
}

bool ConfigurationChecker::check_semaphoreblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int old_error_count = error_count;

    Message object("Semaphore","セマフォ");

    TargetVariable<DT_UINT> _kernel_tmax_semid("_kernel_tmax_semid");
    if(*_kernel_tmax_semid < 1)
        return true;

    TargetVariable<DT_UINT> sematr("_kernel_seminib_table","semaphore_initialization_block::sematr");
    TargetVariable<DT_UINT> maxsem("_kernel_seminib_table","semaphore_initialization_block::maxsem");
    TargetVariable<DT_UINT> isemcnt("_kernel_seminib_table","semaphore_initialization_block::isemcnt");

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n")
        << object << *_kernel_tmax_semid;

    for(id = 1; id <= *_kernel_tmax_semid; id++)
    {
        set_banner(parameter, object, SEMAPHORE, id);

            //attribute validation check
        if((*sematr & ~0x1) != 0)
            notify(STANDARD,
                Message("Illegal attribute (It should be (TA_TFIFO||TA_TPRI)).",
                        "(TA_TFIFO||TA_TPRI)以外の属性が指定されている"));

            //maxcnt < isemcnt
        if(*maxsem < *isemcnt)
            notify(STANDARD,
                Message("Initial count[%] is greater than the maximum count[%] of this semaphore",
                        "初期値[%]が最大値[%]を超えている") << *isemcnt << *maxsem);

        if(*maxsem == 0)
            notify(STANDARD,
                Message("Maximum count must be greater than zero.",
                        "セマフォの最大カウントは1以上でなければいけません"));

        ++ sematr, ++ maxsem, ++ isemcnt;
    }

    return old_error_count == error_count;
}


bool ConfigurationChecker::check_eventflagblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int old_error_count = error_count;

    Message object("Event flag","イベントフラグ");

    TargetVariable<DT_UINT> _kernel_tmax_flgid("_kernel_tmax_flgid");
    if(*_kernel_tmax_flgid < 1)
        return true;

    TargetVariable<DT_UINT> flgatr("_kernel_flginib_table","eventflag_initialization_block::flgatr");

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n")
         << object << *_kernel_tmax_flgid;

    for(id = 1; id <= *_kernel_tmax_flgid; id++)
    {
        set_banner(parameter, object, EVENTFLAG, id);
            
            //attribute validation check
        if((*flgatr & ~0x7) != 0)
            notify(STANDARD,
                Message("Illegal attribute value [0x%]",
                        "おかしな属性値 [0x%]") << setbase(16) << (*flgatr & ~0x7));

        if((*flgatr & 0x2) != 0)
            notify(TOPPERS,     //依存部で直らないのでRESTRICTEDにしない
                Message("Attribute TA_WMUL is not supported in current version.",
                        "TA_WMULはサポート外"));
    
        ++ flgatr;
    }

    return old_error_count == error_count;
}


bool ConfigurationChecker::check_dataqueueblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int old_error_count = error_count;

    Message object("Data queue","データキュー");

    TargetVariable<DT_UINT> _kernel_tmax_dtqid("_kernel_tmax_dtqid");
    if(*_kernel_tmax_dtqid < 1)
        return true;

    TargetVariable<DT_UINT> dtqatr("_kernel_dtqinib_table", "dataqueue_initialization_block::dtqatr");
    TargetVariable<DT_UINT> dtqcnt("_kernel_dtqinib_table", "dataqueue_initialization_block::dtqcnt");
    TargetVariable<DT_VP_INT> dtq("_kernel_dtqinib_table", "dataqueue_initialization_block::dtq");

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n")
        << object << *_kernel_tmax_dtqid;

    for(id = 1; id <= *_kernel_tmax_dtqid; id++)
    {
        set_banner(parameter, object, DATAQUEUE, id);

            //attribute validation check
        if((*dtqatr & ~0x1) != 0)
            notify(STANDARD,
                Message("Illegal attribute value [0x%]",
                        "おかしな属性値 [0x%]") << setbase(16) << (*dtqatr & ~0x1));

        if(*dtqcnt != 0 && *dtq == 0)
            notify(TOPPERS,
                Message("Dataqueue buffer should not be NULL", "データキューのバッファがNULL値"));

        ++ dtqatr, ++ dtqcnt, ++ dtq;
    }

    return old_error_count == error_count;
}


bool ConfigurationChecker::check_mailboxblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int old_error_count = error_count;

    Message object("Mailbox","メールボックス");

    TargetVariable<DT_UINT> _kernel_tmax_mbxid("_kernel_tmax_mbxid");
    if(*_kernel_tmax_mbxid < 1)
        return true;
    
    TargetVariable<DT_UINT> mbxatr("_kernel_mbxinib_table","mailbox_initialization_block::mbxatr");
    TargetVariable<DT_INT>  maxmpri("_kernel_mbxinib_table","mailbox_initialization_block::maxmpri");

    DT_INT maxpri = container->getVariableInfo("TMAX_MPRI").value;
    DT_INT minpri = container->getVariableInfo("TMIN_MPRI").value;

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n") << object << *_kernel_tmax_mbxid;
    for(id = 1; id <= *_kernel_tmax_mbxid; id++)
    {
        set_banner(parameter, object, MAILBOX, id);

            //attribute validation check
        if((*mbxatr & ~0x3) != 0)
            notify(STANDARD,
                Message("Illegal attribute value [0x%]",
                        "おかしな属性値 [0x%]") << setbase(16) << (*mbxatr & ~0x3));
    
            //mailbox message priority check
        if(*maxmpri < 0)
            notify(STANDARD,
                Message("Priority must not be a negative number.","優先度が負値"));

        if(*maxmpri < minpri)
            notify(STANDARD,
                Message("Message priority should be greater than or equal to %.",
                        "メッセージ優先度は%以上でなければいけません") << minpri);

        if(*maxmpri > maxpri)
            notify(STANDARD,
                Message("Message priority should be less than or equal to %.",
                        "メッセージ優先度は%以下でなければいけません") << maxpri);

        ++ mbxatr, ++ maxmpri;
    }

    return old_error_count == error_count;
}

bool ConfigurationChecker::check_fixed_memorypoolblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int old_error_count = error_count;

    Message object("Fixed size memory pool","固定長メモリプール");

    TargetVariable<DT_UINT> _kernel_tmax_mpfid("_kernel_tmax_mpfid");
    if(*_kernel_tmax_mpfid < 1)
        return true;

    TargetVariable<DT_UINT> mpfatr("_kernel_mpfinib_table", "fixed_memorypool_initialization_block::mpfatr");
    TargetVariable<DT_UINT> limit ("_kernel_mpfinib_table", "fixed_memorypool_initialization_block::limit");
    TargetVariable<DT_VP>   mpf   ("_kernel_mpfinib_table", "fixed_memorypool_initialization_block::mpf");
    TargetVariable<DT_UINT> blksz ("_kernel_mpfinib_table", "fixed_memorypool_initialization_block::blksz");

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n") << object << *_kernel_tmax_mpfid;
    for(id = 1; id <= *_kernel_tmax_mpfid; id++)
    {
        set_banner(parameter, object, FIXEDSIZEMEMORYPOOL, id);

            //attribute validation check
        if((*mpfatr & ~0x1) != 0)
            notify(STANDARD,
                Message("Illegal attribute value [0x%]","おかしな属性値 [0x%]") << (*mpfatr & ~0x1));

            //ブロック数が0
        if(*mpf == *limit)
            notify(STANDARD,
                Message("blkcnt should be a non-zero value.","ブロック数が0です"));

            //ブロックサイズが0
        if(*blksz == 0)
            notify(STANDARD,
                Message("blksz should be a non-zero value.","ブロックサイズが0です"));

            //バッファアドレスが0
        if(*mpf == 0)
            notify(TOPPERS,
                Message("buffer address is a NULL pointer.","バッファアドレスがNULLポインタになっています"));

        ++ mpfatr, ++ limit, ++ mpf, ++ blksz;
    }

    return old_error_count == error_count;
}


bool ConfigurationChecker::check_cyclic_handlerblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int old_error_count = error_count;

    Message object("Cyclic handler","周期ハンドラ");

    TargetVariable<DT_UINT> _kernel_tmax_cycid("_kernel_tmax_cycid");
    if(*_kernel_tmax_cycid < 1)
        return true;

    DT_RELTIM maxreltim = container->getVariableInfo("TMAX_RELTIM").value;
    TargetVariable<DT_UINT> cycatr("_kernel_cycinib_table", "cyclic_handler_initialization_block::cycatr");
    TargetVariable<DT_RELTIM> cyctim("_kernel_cycinib_table", "cyclic_handler_initialization_block::cyctim");
    TargetVariable<DT_RELTIM> cycphs("_kernel_cycinib_table", "cyclic_handler_initialization_block::cycphs");

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n") << object << *_kernel_tmax_cycid;
    for(id = 1; id <= *_kernel_tmax_cycid; id++)
    {
        set_banner(parameter, object, CYCLICHANDLER, id);

            //attribute validation check
        if((*cycatr & ~0x7) != 0)
            notify(STANDARD,
                Message("Illegal attribute value [0x%]","おかしな属性値 [0x%]") << (*cycatr & ~0x1));

        if((*cycatr & 0x4) != 0)
            notify(TOPPERS,     //非依存部なのでRESTRICTEDにしない
                Message("TA_PHS is not supported in this kernel.","TA_PHSはサポート外"));

            // 属性値に TA_ASM が含まれている
        if((*cycatr & 0x1) != 0)
            notify( RESTRICTED,
                Message("TOPPERS/JSP Kernel never minds the flag 'TA_ASM'.",
                        "TOPPERS/JSPカーネルの全ての機種依存部でTA_ASMをサポートするとは限らない"));

            //RELTIMでの表現範囲内にあるかどうかのチェック
        if(*cyctim > maxreltim)
            notify(STANDARD,
                Message("The cyclic object has a period (%) that exceeds the maximum period (%)",
                        "起動周期(%)が表現可能な相対時間の範囲(%)を超えています") << *cyctim << maxreltim);

            //起動周期が0でないことのチェック
        if(*cyctim == 0)
            notify(STANDARD,
                Message("The cyclic object has a ZERO period.",
                        "起動周期が0になっています"));

        if(*cycphs > maxreltim)
            notify(STANDARD,
                Message("The cyclic object has a initial delay (%) that exceeds the maximum period (%)",
                        "起動位相(%)が表現可能な相対時間の範囲(%)を超えています") << *cycphs << maxreltim);

        ++ cycatr, ++ cyctim, ++ cycphs;
    }

    return old_error_count == error_count;
}

bool ConfigurationChecker::check_interrupt_handlerblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int old_error_count = error_count;

    Message object("Interrupt handler","割込みハンドラ");

    TargetVariable<DT_UINT> _kernel_tnum_inhno("_kernel_tnum_inhno");
    if(*_kernel_tnum_inhno == 0)
        return true;

    TargetVariable<DT_UINT> inhatr("_kernel_inhinib_table", "interrupt_handler_initialization_block::inhatr");
    TargetVariable<DT_FP>   inthdr("_kernel_inhinib_table", "interrupt_handler_initialization_block::inthdr");

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n") << object << *_kernel_tnum_inhno;
    for(id = 0; id < *_kernel_tnum_inhno; id++)
    {
        set_banner(parameter, object, INTERRUPTHANDLER, id);

            //attribute validation check
        if((*inhatr & 0x1) != 0)
            notify(STANDARD,
                Message("The attribute can take only TA_HLNG|TA_ASM",
                        "TA_HLNG|TA_ASM以外の属性は設定できません"));

            // 属性値に TA_ASM が含まれている
        if((*inhatr & 0x1) != 0)
            notify(RESTRICTED,
                Message("TOPPERS/JSP Kernel never minds the flag 'TA_ASM'.",
                        "TA_ASMが使用されている"));

            // 起動番地チェック
        if(*inthdr == 0)
            notify(RESTRICTED,
                Message("NULL pointer is specified as an inthdr address.",
                        "割込みハンドラの番地がNULLです"));

        ++ inhatr, ++ inthdr;
    }

    return old_error_count == error_count;
}

bool ConfigurationChecker::check_exception_handlerblock(Directory & parameter, FileContainer * container)
{
    unsigned int id;
    unsigned int old_error_count = error_count;

    Message object("Exception handler","例外ハンドラ");

    TargetVariable<DT_UINT> _kernel_tnum_excno("_kernel_tnum_excno");
    if(*_kernel_tnum_excno == 0)
        return true;

    TargetVariable<DT_UINT> excatr("_kernel_excinib_table", "cpu_exception_handler_initialization_block::excatr");
    TargetVariable<DT_FP>   exchdr("_kernel_excinib_table", "cpu_exception_handler_initialization_block::exchdr");

    VerboseMessage("% object : % items\n","%オブジェクト : % 個\n") << object << *_kernel_tnum_excno;
    for(id = 0; id < *_kernel_tnum_excno; id++)
    {
        set_banner(parameter, object, EXCEPTIONHANDLER, id);

            //attribute validation check
        if((*excatr & 0x1) != 0)
            notify(STANDARD,
                Message("The attribute can take only TA_HLNG|TA_ASM",
                        "TA_HLNG|TA_ASM以外の属性は設定できません"));

            // 属性値に TA_ASM が含まれている
        if((*excatr & 0x1) != 0)
            notify(RESTRICTED,
                Message("TOPPERS/JSP Kernel never minds the flag 'TA_ASM'.",
                        "TOPPERS/JSPカーネルの全ての機種依存部でTA_ASMをサポートするとは限らない"));

            // 起動番地チェック
        if(*exchdr == 0)
            notify(RESTRICTED,
                Message("NULL pointer is specified as an exchdr address.",
                        "例外ハンドラの番地がNULLです"));

        ++ excatr, ++ exchdr;
    }

    return old_error_count == error_count;
}

//------------------------------------------------------

void ConfigurationChecker::parseOption(Directory & parameter)
{
    string loadmodule;
    string work;

    if(findOption("h","help"))
    {
        cout << endl << Message(
            "Configuration checker - option\n"
            "  -m, --module=filename : Specify the load module (essential option)\n"
            "  -cs, --script=filename  : Specify the checker script file\n"
            "  -cl, --checklevel=level : Specify one of the check levels below \n"
            "    l(azy)       : Minimum check will be performed.\n"
            "    s(tandard)   : includes some ITRON Standard check items.\n"
            "    t(oppers)    : checks whether it meets TOPPERS/JSP restrictions\n"
            "    r(estricted) : All of check items will be performed.\n",
            "コンフィギュレーションチェッカ - オプション\n"
            "  -m, --module=ファイル名 : ロードモジュール名を指定します (必須項目)\n"
            "  -cs, --script=ファイル名  : チェッカスクリプトを指定します\n"
            "  -cl, --checklevel=level : Specify one of the check levels below \n"
            "    l(azy)       : 最小限のチェックのみを行います\n"
            "    s(tandard)   : ITRON仕様の範囲でチェックを行います\n"
            "    t(oppers)    : TOPPERS/JSPカーネルの制限を満たすことを確認します\n"
            "    r(estricted) : 機種依存部を含め全てのチェック項目を実施します\n");
        cout << endl 
             << Message("Supported architecture : ", "対応アーキテクチャ : ")
             << FileContainer::getInstance()->getArchitecture()
             << endl;
        return;
    }

    if(findOption("m","module",&loadmodule) || findOption(DEFAULT_PARAMETER,NULL,&loadmodule))
    {
        if(findOption("s","source"))
            ExceptionMessage("Configuration checker can not execute while Configurator executes","コンフィギュレータとチェッカは同時に起動できません").throwException();

        parameter["/file/loadmodule"] = loadmodule;
        activateComponent();
    }

    if(!findOption("cs","script",&work))
        work = loadmodule.substr(0,loadmodule.find_last_of('.')) + ".chk";
    parameter["/file/checkerscript"] = work;

    work.erase();
    if(findOption("obj","load-object",&work))
    {
        if(work.empty())
            work.assign("kernel_obj.dat");

        fstream f(work.c_str(), ios::in|ios::binary);
        if(f.is_open())
        {
            parameter["/object"].Load(&f);
            f.close();
        }else
            ExceptionMessage(" Failed to open the file '%' for storing object definitions"," オブジェクト情報を格納するためのファイル(%)が開けません") << work << throwException;
    }

    if(findOption("cl","checklevel",&work))
    {
        current_level = UNKNOWN;

        if(work.compare("lazy") == 0 || work[0] == 'l')
            current_level = LAZY;
        if(work.compare("standard") == 0 || work[0] == 's')
            current_level = STANDARD;
        if(work.compare("toppers") == 0 || work[0] == 't')
            current_level = TOPPERS;
        if(work.compare("restricted") == 0 || work[0] == 'r')
            current_level = RESTRICTED;

        if(current_level == UNKNOWN)
            ExceptionMessage(" Unknown check level [%] specified"," 無効なチェックレベル指定 [%]") << work << throwException;
    }else
        current_level = NORMAL;

    checkOption("cpu", "cpu");
    checkOption("system", "system");
}

void ConfigurationChecker::body(Directory & parameter)
{
    FileContainer * container;
    bool result = true;

        /* より優先度の高いエラーも対象に */
    current_level = static_cast<enum tagCheckLevel>(static_cast<int>(current_level) * 2 - 1);

    container = FileContainer::getInstance();
    container->attachInfo(parameter["/file/checkerscript"].toString());
    container->attachModule(parameter["/file/loadmodule"].toString());

    if(VerboseMessage::getVerbose())
    {
        cout << Message("  Target architecture : ","  ターゲットアーキテクチャ : ")
             << container->getArchitecture() << endl;
    }

    error_count = 0;
    result &= check_taskblock(parameter,container);
    result &= check_semaphoreblock(parameter,container);
    result &= check_eventflagblock(parameter,container);
    result &= check_dataqueueblock(parameter,container);
    result &= check_mailboxblock(parameter,container);
    result &= check_fixed_memorypoolblock(parameter,container);
    result &= check_cyclic_handlerblock(parameter,container);
    result &= check_interrupt_handlerblock(parameter,container);
    result &= check_exception_handlerblock(parameter,container);

    if(!result)
        ExceptionMessage("Total % errors found in current configuration.\n","全部で%個のエラーが検出されました\n") << error_count << throwException;

    VerboseMessage("No error found in current configuration\n","構成に異常はありませんでした\n");
}

