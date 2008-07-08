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
 *  @(#) $Id: jsp_checkscript.cpp,v 1.18 2005/07/27 10:26:30 honda Exp $
 */

// $Header: /home/CVS/configurator/jsp/jsp_checkscript.cpp,v 1.18 2005/07/27 10:26:30 honda Exp $

#include "jsp/jsp_defs.h"
#include "jsp/jsp_common.h"

#include <string>
#include <iostream>

#include "base/component.h"
#include "base/mpstrstream.h"

using namespace std;
using namespace ToppersJsp;

class CheckScriptGenerator : public Component
{
protected:
    virtual void parseOption(Directory & container);
    virtual void body(Directory & container);
    
    void insertMagic(void) throw(Exception);

public:
    CheckScriptGenerator(void) : Component(FINALIZE) {};
} instance_of_CheckScriptGenerator;

void CheckScriptGenerator::parseOption(Directory & container)
{
    MultipartStream * stream;
    string work;

    if(findOption("h","help"))
    {
        cerr << '\n' << Message(
            "Configuration Checker Script Generation Option\n"
            "  -c, --check  : Generate a checker script\n",
            "カーネル構成チェックスクリプト生成用オプション\n"
            "  -c, --check  : チェックスクリプトを生成します\n");
        return;
    }

    if(findOption("c","check",&work))
    {
        if(work.empty())
            work.assign("kernel_chk.c");

        stream = new MultipartStream(work);
        stream->createPart("header")
               .createPart("body")
               .createPart("footer");

        container["/file/kernel_chk"] = stream;

        checkOption("ci","checker-macro");
        activateComponent();
    }
}

static void createScriptEntry(Directory & container, MultipartStream * out, const char * objname, const char * member, bool mode = true)
{
    string work;
    string inib;
    Directory * scope;
    int i, j;
    
    (*out) << container.format("\n\t\t/* $@ */\n\n");

    if(mode)
    {
        scope = container.getFirstChild();
        while(scope != 0)
        {
            work = scope->getKey();
            if(work[0] < '0' || work[1] > '9')
                (*out) << "\tOBJECT(" << container.getKey() << '_' << scope->toInteger() << ',' << work << ");\n";

            scope = scope->getNext();
        }

        (*out) << container.format("\tEVAR(ID,_kernel_tmax_$(/prefix/$@/id)id);\n");
    }else
    {
        i = 0;
        scope = container.getFirstChild();
        while(scope != 0)
        {
            work = scope->getKey();
            (*out) << "\tOBJECT(" << container.getKey() << '_' << (i++) << ',' << scope->getKey() << ");\n";
            scope = scope->getNext();
        }

        (*out) << container.format("\tEVAR(ID,_kernel_tnum_$(/prefix/$@/id)no);\n");
    }

    (*out) << container.format("\tEVAR($(/prefix/$@/SID)INIB,_kernel_$(/prefix/$@/sid)inib_table);\n");

    work.assign(member);
    i = 0;
    j = work.find_first_of(',');
    if(objname == NULL)
        inib = container.getKey();
    else
        inib = objname;
    
    do {
        (*out) << "\tMEMBER(" << inib << "_initialization_block," << work.substr(i, j-i) << ");\n";

        i = j + 1;
        j = work.find_first_of(',', i);
    } while(i != string::npos+1);
    (*out) << '\n';
}

inline void createScriptEntry(Directory & container, MultipartStream * out, const char * member)
{   createScriptEntry(container, out, NULL, member, true);  }

void CheckScriptGenerator::insertMagic(void) throw(Exception)
{
    KernelCfg * out = dynamic_cast<KernelCfg *>(RuntimeObjectTable::getInstance(typeid(KernelCfg)));
    if(out != 0) {
        out->movePart("others");
        out->createPart("checkscript");
        (*out) << Message("\t/* Variables for kernel checker */\n", "\t/* カーネルチェッカ用変数 */\n");
        (*out) << "const UW _checker_magic_number = 0x01234567;\n\n";
    }
}

void CheckScriptGenerator::body(Directory & container)
{
    MultipartStream * out;

    out = reinterpret_cast<MultipartStream *>(container["/file/kernel_chk"].toPointer());

    out->movePart("header") <<
        "#include \"jsp_kernel.h\"\n"
        "#include \"logtask.h\"\n"
        "#include \"timer.h\"\n\n";


	string work;
	if(findOption("ci","checker-macro",&work)) {

			/* 中身が空でなかったらincludeをするが，空ならincludeすら吐かない */
		if(!work.empty())
			(*out) << "#include \"" << work << "\"\n\n";
	}
	else {
		(*out) <<	"#define OBJECT(x,y) __asm(\"d\" #x \",\" #y \"@\");\n"
			        "#define MEMBER(x,y) __asm(\"s\" #x \"::\" #y \",(%0),(%1)@\" ::\\\n"
			        "     \"i\"(sizeof(((struct x *)0)->y)), \"i\"(&((struct x *)0)->y));\n"
			        "#define VAR(x) __asm(\"s\" #x \",(%0),(0)@\" :: \"i\"(sizeof(x)));\n"
			        "#define EVAR(x,y) __asm(\"s\" #y \",(%0),(0)@\" :: \"i\"(sizeof(x)));\n"
			        "#define SVAR(x) __asm(\"s\" #x \",(%0),(0)@\" :: \"i\"(sizeof(x[0])));\n"
			        "#define DEFS(x) __asm(\"s\" #x \",(%0),(0)@\" :: \"i\"((unsigned long)x));\n\n";
	}

    (*out) <<	"#include \"queue.h\"\n\n"
		        "#include \"task.h\"\n"
		        "#include \"semaphore.h\"\n"
		        "#include \"eventflag.h\"\n"
		        "#include \"dataqueue.h\"\n"
		        "#include \"mailbox.h\"\n"
		        "#include \"mempfix.h\"\n"
		        "#include \"cyclic.h\"\n"
		        "#include \"../kernel/exception.h\"\n"
		        "#include \"interrupt.h\"\n"
		        "#include \"wait.h\"\n\n"
		        "void checker_function(void)\n{\n";

    out->movePart("footer") << "}\n";

    out->movePart("body") <<
        "\tDEFS(TMAX_TPRI);\n\tDEFS(TMIN_TPRI);\n\n"
        "\tDEFS(TMAX_MPRI);\n\tDEFS(TMIN_MPRI);\n\n"
//      "\tDEFS(TMAX_MAXSEM);\n\n"
        "\tDEFS(TMAX_RELTIM);\n\n"
        "\tMEMBER(queue,next);\n\tMEMBER(queue,prev);\n\n";

    createScriptEntry(container[OBJECTTREE "/" TASK],      out, "tskatr,exinf,task,ipriority,stksz,stk,texatr,texrtn");
    createScriptEntry(container[OBJECTTREE "/" SEMAPHORE], out, "sematr,isemcnt,maxsem");
    createScriptEntry(container[OBJECTTREE "/" EVENTFLAG], out, "flgatr,iflgptn");
    createScriptEntry(container[OBJECTTREE "/" DATAQUEUE], out, "dtqatr,dtqcnt,dtq");
    createScriptEntry(container[OBJECTTREE "/" MAILBOX], out, "mbxatr,maxmpri");
    createScriptEntry(container[OBJECTTREE "/" FIXEDSIZEMEMORYPOOL], out, "fixed_memorypool", "mpfatr,blksz,mpf,limit");
    createScriptEntry(container[OBJECTTREE "/" CYCLICHANDLER], out, "cyclic_handler", "cycatr,exinf,cychdr,cyctim,cycphs");
    createScriptEntry(container[OBJECTTREE "/" INTERRUPTHANDLER], out, "interrupt_handler", "inhno,inhatr,inthdr", false);
    createScriptEntry(container[OBJECTTREE "/" EXCEPTIONHANDLER], out, "cpu_exception_handler", "excno,excatr,exchdr", false);

    insertMagic();

    VerboseMessage("Configuration check script generation was finished successfully.\n","カーネル構成チェックスクリプトファイルを出力しました\n");
}

