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
 *  @(#) $Id: jsp_parser.cpp,v 1.60 2004/09/09 19:22:41 takayuki Exp $
 */

// $Header: /home/CVS/configurator/jsp/jsp_parser.cpp,v 1.60 2004/09/09 19:22:41 takayuki Exp $

#include <stdarg.h>

#include "jsp/jsp_defs.h"
#include "base/parser.h"
#include "base/mpstrstream.h"
#include "jsp/jsp_common.h"

#include <set>
#include <map>
#include <cctype>
#include <algorithm>
#include <iomanip>

using namespace ToppersJsp;
using namespace std;

class CoreParser : public ParserComponent
{
protected:
    virtual void parseOption(Directory &);
            void outputContainer(Directory & container) throw();
            void assignID(Directory & container) throw();

public:
    CoreParser(void);
    virtual bool parse(Parser & p, Directory & container);
} instance_of_Parser;

CoreParser::CoreParser(void)
{
    setBanner("=== TOPPERS/JSP Kernel Configurator ver.9.4 (for JSP rel 1.4) ===");
}

void CoreParser::parseOption(Directory & container)
{
    ParserComponent::parseOption(container);

    if(findOption("h","help"))
    {
        cerr << Message(
            "  -obj, --dump-object=filename : Dump the object tree into the file specified\n"
            "  -ao, --assign-order=order : Specify the order of automatic ID assignment\n"
            "    You can use three terms below as ordering rule.\n"
            "     alphabetic (in alphabetic order)\n"
            "     fcfs       (in arrival order [as default])\n"
            "     reverse    (reverse the order)\n"
            "  -var, --variable-id : Prepare ID-variable for storing its identifier number\n",
            "  -obj, --dump-object=ファイル名 : 指定したファイルにオブジェクト情報を出力します\n"
            "  -ao, --assign-order=順序 : 自動ID割当の割当順序を指定します\n"
            "    割当順序は次の3つの組合せで指定します.\n"
            "     alphabetic (ABC順), fcfs (宣言順 [デフォルト]), reverse (逆順)\n"
            "  -var, --variable-id : ID番号を格納する変数を用意します");
        return;
    }

    checkOption("D","dump");
    checkOption("cpu","cpu");
    checkOption("system","system");
    checkOption("var","variable-id");
}

namespace {
	int displayHandler(Directory & container, const char * category, const char * format)
	{
	    Directory * node  = 0;
	    Directory * scope = 0;

	    node = container.findChild(OBJECTTREE,category,NULL);
	    if(node == 0 || node->size() == 0)
		return 0;

	    VerboseMessage("Handler assignment list [%]\n","ハンドラ割付表 [%]\n") << category;

	    scope = node->getFirstChild();
	    while(scope != 0)
	    {
		VerboseMessage::getStream() << scope->format(format);
		scope = scope->getNext();
	    }

	    return node->size();
	}

	//マクロ化されたtoupperを関数化する (STL-algorithm用)
	int toupper_function(int c)
	{ return toupper(c); }
}
/*
 *  outputContainer - デバッグ時 or ダンプ指定時にオブジェクト情報を所定のファイル形式で出力する
 */
void CoreParser::outputContainer(Directory & container) throw()
{
    try {

        if(findOption("D","dump") || findOption("debug","debug")) {
            fstream file("container.txt", ios::out);
            container.drawTree(&file);
            file.close();
        }

        if(findOption("Dx","dump-xml") || findOption("debug","debug")) {
            fstream file("container.xml", ios::out);
            container.drawTree_byXML(&file);
            file.close();
        }
    }
    catch (...) {
    }
}

/*
 *  assignID - オブジェクトに対してIDを割り付ける
 *             取り込んだオブジェクト名をユーザに提示する
 */
void CoreParser::assignID(Directory & container) throw()
{
    multimap<int, const char *> sorter;
    multimap<int, const char *>::iterator scope;
    enum Common::tagAssignmentOrder order = Common::FCFS;

    sorter.insert(pair<int, const char *>(container[OBJECTTREE "/" TASK].size(), TASK));
    sorter.insert(pair<int, const char *>(container[OBJECTTREE "/" SEMAPHORE].size(), SEMAPHORE));
    sorter.insert(pair<int, const char *>(container[OBJECTTREE "/" EVENTFLAG].size(), EVENTFLAG));
    sorter.insert(pair<int, const char *>(container[OBJECTTREE "/" DATAQUEUE].size(), DATAQUEUE));
    sorter.insert(pair<int, const char *>(container[OBJECTTREE "/" MAILBOX].size(), MAILBOX));
    sorter.insert(pair<int, const char *>(container[OBJECTTREE "/" FIXEDSIZEMEMORYPOOL].size(), FIXEDSIZEMEMORYPOOL));
    sorter.insert(pair<int, const char *>(container[OBJECTTREE "/" CYCLICHANDLER].size(), CYCLICHANDLER));

    order = Common::parseOrder(getOption("ao", "assign-order"));
    scope = sorter.begin();
    while(scope != sorter.end())
    {
        if( (*scope).first != 0 )
            Common::assignID(container, (*scope).second, OBJECTTREE, order);
        ++ scope;
    }

    displayHandler(container, INTERRUPTHANDLER, "  $@ : $(inthdr)\n");
    displayHandler(container, EXCEPTIONHANDLER, "  $@ : $(exchdr)\n");
    displayHandler(container, INITIALIZER, "  $@ : $(inirtn)($(exinf))\n");
    displayHandler(container, TERMINATOR, "  $@ : $(terrtn)($(exinf))\n");
}


bool CoreParser::parse(Parser & p, Directory & container)
{
    Token token;
    string work;
    bool isParseErrorOccured;

    if(findOption("cpu","cpu", &work))
        container["/cpu"] = work;
    
    try{
        isParseErrorOccured = false;
        do {
            p.getToken(token);
            if(token == Token::IDENTIFIER)
            {
                    /* エラーが起きても極力続けたいので、
                        起きたことだけ覚えておいてどんどん続ける */
                try 
                {   this->parseStaticAPI(p, container, token);  }
                catch(...)
                {   isParseErrorOccured = true;                 }

            }
        }while(token != Token::EOS);

        p.setLogBuffer(NULL);

        if(isParseErrorOccured)
            ExceptionMessage(ExceptionMessage::FATAL,"The configuration process was aborted due to occurence of parse error","構文解析に失敗したため処理を中断します").throwException();

        if(container[OBJECTTREE "/" TASK].size() == 0)
            ExceptionMessage("Kernel requires one or more task objects.","タスクオブジェクトがありません").throwException();

            /* ID割付 & オブジェクト名表示 */
        assignID(container);

            /* オブジェクト情報をファイルに出力 */
        outputContainer(container);

        return true;
    }
    catch(Exception &)
    {
            /* 例外を投げなおす前に 現在のオブジェクト情報を格納する */
        outputContainer(container);
        throw;
    }
}

/* ======================================================================== */

DECLARE_DIRECTIVE(assigner,"assign_id")
{
    Token name;
    Token value;

    p.getToken(name, Token::IDENTIFIER);
    p.getToken(value, Token::INTEGER);

    if(value.value <= 0)
        ExceptionMessage("Cannot assign an ID number less or equal to 0.","0以下のID番号を設定することはできません").throwException();

    *container.openChild("/","identifier",name.c_str(),NULL) = value.value;
}

DECLARE_DIRECTIVE(parameter,"parameter")
{
    Token token;
    string key;
    string::size_type i,j;

    do {
        p.getToken(token);
        if(!(token == Token::STRINGLITERAL))
            ExceptionMessage("parameter pragma requires additional parameter put in '\"...\"'","parameterプラグマには\"...\"で括られた追加パラメータが必要です").throwException();

        token.chopLiteral();

        if(token[0] == '-')
        {
            i = token.find_first_of('=');
            j = token.find_first_not_of('-');
            if(i == j || j == string::npos)
                ExceptionMessage(ExceptionMessage::FATAL,"Wrong option [%]","不正なオプション [%]") << token << throwException;

            if(i != string::npos)
            {
                key = DEFAULT_PARAMETER;
                *container.openChild(string("/Parameter/") + token.substr(1, i-1)) = token.substr(i+1);
            }
            else
            {
                key = token.substr(1);
                *container.openChild(string("/Parameter/") + key) = string("");
            }
        }else
            *container.openChild(string("/Parameter/") + key) = token;

        p.getToken(token);
    } while(token.compare(",") == 0);

    p.putBack(token);
}


/* ======================================================================== */
class ConfigurationFileGenerator : public Component
{
protected:
    bool backward_compatible;
    bool libstyle_include;
    bool restrict_zero;
    bool def_prototype;

    enum tagDefinition { HEADER=1, TNUM=2, BUFFER=4, DEFINITION=8, CONTROLBLOCK=16, SHORT_ID=32, TNUMNO=64, INIT=128, PROTOTYPE=256, INITLIST=512 };
    void createObjectDefinition(MultipartStream *, Directory &, int, ...);

    virtual void parseOption(Directory &);
    virtual void body(Directory &);
    virtual void onFatalExit(Directory &);

public:
    ConfigurationFileGenerator(void) : Component(CODEGENERATOR) {};
} instance_of_Configurator;



namespace {

        //ファイル名の取得
    string get_filename(string category, string _default) throw(Exception)
    {
        string result(_default);
        OptionParameter::OptionItem item = getOptionParameter()[category];

        if(item.isValid()) {
            if(item.countParameter() == 1)
                result = item[0];
            else
                ExceptionMessage("-% should have just ONE filename.","-%オプションに指定できるファイルは1つのみ") << category << throwException;
        }

        if(result.empty())
            ExceptionMessage("-% has illegal file name.","-%オプションに渡されたファイル名は不正") << category << throwException;
        
        return result;
    }

        // カーネル構成ファイルの生成
    KernelCfg * createKernelCfg(void)
    {
        KernelCfg * cfg;

        cfg = KernelCfg::createInstance(get_filename("kernel_cfg","kernel_cfg.c"));
        cfg->disableOutput();       //処理が完了するまで出力させない

            //基本的なオブジェクトの格納場所はここで指定しておく
        cfg->createPart("header")
            .createPart("include")
            .createPart(IDENTIFIER_VARIABLE)
            .createPart(TASK)
            .createPart(SEMAPHORE)
            .createPart(EVENTFLAG)
            .createPart(DATAQUEUE)
            .createPart(MAILBOX)
            .createPart(FIXEDSIZEMEMORYPOOL)
            .createPart(CYCLICHANDLER)
            .createPart(INTERRUPTHANDLER)
            .createPart(EXCEPTIONHANDLER)
            .createPart(INITIALIZER)
            .createPart(OBJECT_INITIALIZER)
            .createPart("others");

            //カーネル構成ファイル生成イベントの実行
        KernelCfg::CreationEvent ev;
        ev.file = cfg;
        Event<KernelCfg::CreationEvent>::raise(ev);

            //エラーチェック
        if(ev.file == 0)
            ExceptionMessage("Internal error : kernel_cfg event handler eliminates the instance.","内部エラー : kernel_cfgのハンドラがインスタンスを削除した") << throwException;
        if(RuntimeObjectTable::getInstance(typeid(KernelCfg)) != ev.file)
            ExceptionMessage("Internal error : kernel_cfg event handler creates the illegal instance.","内部エラー : kernel_cfgのハンドラが不正なインスタンスを生成した") << throwException;

        return ev.file; //cfgではダメ
    }

        //ファイル名から衝突回避用の名前を生成する
    string convert_filename_to_definition(string filename)
    {
        string result;
        string::iterator scope;

        scope = filename.begin();
        while(scope != filename.end()) {
            if(isalnum(*scope))
                result += static_cast<char>(toupper(*scope));
            else
                result += '_';
            ++ scope;
        }

        return result;
    }

        //自動ID割付結果ファイルの生成
    KernelID * createKernelID(void)
    {
        KernelID * id = 0;

        id = KernelID::createInstance(get_filename("kernel_id","kernel_id.h"));
        id->disableOutput();

            //基本的なオブジェクトの格納場所はここで指定しておく
        id-> createPart("__header__")
            .createPart("body")
            .createPart("__footer__");

            //ファイル生成イベントの実行
        KernelID::CreationEvent ev;
        ev.file = id;
        Event<KernelID::CreationEvent>::raise(ev);

            //エラーチェック
        if(ev.file == 0)
            ExceptionMessage("Internal error : kernel_id event handler eliminates the instance.","内部エラー : kernel_idのハンドラがインスタンスを削除した") << throwException;
        if(RuntimeObjectTable::getInstance(typeid(KernelID)) != ev.file)
            ExceptionMessage("Internal error : kernel_id event handler creates the illegal instance.","内部エラー : kernel_idのハンドラが不正なインスタンスを生成した") << throwException;

            //多重インクルード回避用定義の追加
        string defname = convert_filename_to_definition(id->getFilename());
        id->movePart("__header__");
        (*id)   // << get_agreement(id->getFilename())      //文言の付与
                << "#ifndef " << defname << '\n'        //#ifndef KERNELID_H
                << "#define " << defname << "\n\n";     //#define KERNELID_H
        
        id->movePart("__footer__");
        (*id)   << "\n#endif /* " << defname << " */\n\n";          //#endif /* KERNELID_H */

        return ev.file; //idではダメ
    }

        //登録されている文字の最大長を得る
    unsigned int countLongestSymbolName(const std::map<std::string, int> & identifiers) throw()
    {
        unsigned int max_length = 0;
        std::map<std::string, int>::const_iterator scope;

            // もっとも長い名前を持つシンボル名を検索
        scope = identifiers.begin();
        while(scope != identifiers.end()) {
            if(max_length < scope->first.size())
                max_length = scope->first.size();
            ++ scope;
        }

        return max_length;
    }

        //自動ID割付結果出力 (これまでとの互換モード)
    void serializeIdentifiers_defineStyle(KernelID * out, std::map<std::string, int> & identifier_pool)
    {
        std::map<std::string, int>::const_iterator scope;
        string::size_type max_length;

        (*out) << "\t/* object identifier deifnition */\n\n";

            // もっとも長い名前を持つシンボル名を検索
        max_length = countLongestSymbolName(identifier_pool);

        scope = identifier_pool.begin();
        while(scope != identifier_pool.end()) {
            (*out)  << "#define " << scope->first 
                    << string(max_length - scope->first.size() + 4, ' ')    //値をそろえる
                    << setw(3)
                    << scope->second << '\n';
            ++ scope;
        }
    }

        //自動ID割付結果出力 (定数変数格納方式)
    void serializeIdentifiers_variableStyle(KernelID * id, std::map<std::string, int> & identifier) throw(Exception)
    {
        std::map<std::string, int>::const_iterator scope;
        string::size_type max_length;
        KernelCfg * cfg = getRuntimeObjectInstance(KernelCfg);

            /* kernel_id.hに出力 */

        (*id) << "#ifdef __cplusplus\n"
                 "#define KERNELID_EXPORT extern \"C\"\n"
                 "#else\n"
                 "#define KERNELID_EXPORT extern\n"
                 "#endif\n\n";

            // もっとも長い名前を持つシンボル名を検索
        max_length = countLongestSymbolName(identifier);

        scope = identifier.begin();
        while(scope != identifier.end()) {
            (*id) << "KERNELID_EXPORT const int "
                  << scope->first
                  << ";\n";

            ++ scope;
        }

        (*id) << "\n#undef KERNELID_EXPORT\n\n";

            /* kernel_cfg.cに定数定義を出力 */

        cfg->movePart(IDENTIFIER_VARIABLE);
        (*cfg) << "\t/* Identifier storage variables */\n";
        scope = identifier.begin();
        while(scope != identifier.end()) {
            (*cfg) << "const int " 
                  << scope->first
                  << string(max_length - scope->first.size() + 4, ' ')
                  << " = "
                  << scope->second << ";\n";
            ++ scope;
        }
    }

        //自動ID割付結果出力 (併用方式 - (toppers-dev 28))
    void serializeIdentifiers_blendedStyle(KernelID * id, std::map<std::string, int> & identifier) throw(Exception)
    {
        /* kernel_id.hへの出力は従来と同じ */
        serializeIdentifiers_defineStyle(id, identifier);

            /* kernel_cfg.cに定数定義を出力 */

        std::map<std::string, int>::const_iterator scope;
        string::size_type max_length;
        KernelCfg * cfg = getRuntimeObjectInstance(KernelCfg);

        max_length = countLongestSymbolName(identifier);

        cfg->movePart(IDENTIFIER_VARIABLE);
        (*cfg) << "\t/* Identifier storage variables */\n";
        scope = identifier.begin();
        while(scope != identifier.end()) {
            (*cfg) << "const int " 
                  << scope->first
                  << "_id"
                  << string(max_length - scope->first.size() + 1, ' ')
                  << " = "
                  << scope->second << ";\n";
            ++ scope;
        }
    }

        //自動ID割付結果の出力
    void serializeIdentifiers(bool varid_style = false) throw(Exception)
    {
        KernelID * out = getRuntimeObjectInstance(KernelID);
        std::map<std::string, int> identifier_pool;

            //識別子プールを生成
        {
            Directory * scope = Singleton<Directory>::getInstance()->findChild("/identifier")->getFirstChild();
            while(scope != 0)
            {
                identifier_pool[scope->getKey()] = scope->toInteger();
                scope = scope->getNext();
            }
        }

            //割付結果の出力
        out->movePart("body");
        if(varid_style) {
            serializeIdentifiers_blendedStyle(out, identifier_pool);
//            serializeIdentifiers_variableStyle(out, identifier_pool);
        }
        else {
            serializeIdentifiers_defineStyle(out, identifier_pool);
        }

        out->enableOutput();
    }

    void serializeConfiguration_headerPart(KernelCfg & out) throw(Exception)
    {
        const bool backward_compatible = getOptionParameter()["1.3"].isValid();
        
            //頭の部分の出力
        out.movePart("header");

        out // << get_agreement(out.getFilename())          //文言の添付
            << "/* Configured with ["                   //コンフィギュレーションオプションを出力しておく
            << getOptionParameter().getCommandLine() 
            << "] */\n\n";

            //1.4以降はkernel_cfg.hを使用
        if(!backward_compatible)
            out << "#include " << conv_includefile("kernel_cfg.h") << '\n';

            //kernel_id.hのinclude (kernel_idはダブルクォートでくくる)
        out << "#include \"" << dynamic_cast<KernelID *>(RuntimeObjectTable::getInstance(typeid(KernelID)))->getFilename() <<"\"\n" 
            << '\n';


            //CFG_INT/EXCHDR_ENTRY
            // Q: "<<"があったりなかったりするのはなぜ? A: 2行目から5行目まではelseブロックで出力している内容とまったく同じ。こうすればコンパイラによってはシュリンクされて一つになるし、可読性も悪くならない。
        if(backward_compatible) {
            out << "#if TKERNEL_PRVER >= 0x1040\n"
                << "#error \"This configuration file has no compatibility with"
                << " TOPPERS/JSP rel 1.4 or later version.\"\n"
                   "#elif TKERNEL_PRVER >= 0x1011\n"
                << "#define CFG_INTHDR_ENTRY(inthdr) INTHDR_ENTRY(inthdr)\n"
                   "#define CFG_EXCHDR_ENTRY(exchdr) EXCHDR_ENTRY(exchdr)\n"
                   "#define CFG_INT_ENTRY(inthdr) INT_ENTRY(inthdr)\n"
                   "#define CFG_EXC_ENTRY(exchdr) EXC_ENTRY(exchdr)\n"
                << "#else\n"
                << "#define CFG_INTHDR_ENTRY(inthdr) INTHDR_ENTRY(ENTRY(inthdr), inthdr)\n"
                   "#define CFG_EXCHDR_ENTRY(exchdr) EXCHDR_ENTRY(ENTRY(exchdr), exchdr)\n"
                   "#define CFG_INT_ENTRY(inthdr) ENTRY(inthdr)\n"
                   "#define CFG_EXC_ENTRY(exchdr) ENTRY(exchdr)\n"
                << "#endif\n\n";
        }
        else {
            out << "#if TKERNEL_PRVER >= 0x1040\n"
                << "#define CFG_INTHDR_ENTRY(inthdr) INTHDR_ENTRY(inthdr)\n"
                   "#define CFG_EXCHDR_ENTRY(exchdr) EXCHDR_ENTRY(exchdr)\n"
                   "#define CFG_INT_ENTRY(inthdr) INT_ENTRY(inthdr)\n"
                   "#define CFG_EXC_ENTRY(exchdr) EXC_ENTRY(exchdr)\n"
                << "#else\n"
                << "#error \"This configuration file has no compatibility with"
                << " TOPPERS/JSP rel 1.3 or earlier.\"\n"
                << "#endif\n\n";
        }

            //__EMPTY_LABELマクロの出力
        if(!getOptionParameter()["z"].isValid()) {
            
            string zeromacro;

            if(getOptionParameter()["ZERO"].isValid())
                zeromacro = getOptionParameter()["ZERO"][0];
            else
                zeromacro.assign("x y[0]");

            out <<  "#ifndef __EMPTY_LABEL\n"
                    "#define __EMPTY_LABEL(x,y) " << zeromacro << "\n"
                    "#endif\n\n";
        }

            /* PRIDのチェック */
        out << "#if TKERNEL_PRID != 0x0001u /* TOPPERS/JSP */\n"
               "#error \"You can not use this configuration file without TOPPERS/JSP\"\n"
               "#endif\n\n";
    }
}

void ConfigurationFileGenerator::parseOption(Directory & parameter)
{
    string work;

    if(findOption("h","help"))
    {
        cerr << endl << Message(
            "Kernel configuration file generator\n"
            "  -id, --kernel_id=filename  : Specify the name of ID assignment file\n"
            "  -cfg,--kernel_cfg=filename : Specify the name of kernel configuration file\n"
            "  -il, --include-libstyle    : Use #include <...> style for kernel headers\n"
            "  -oproto, --output-prototype: Output task prototype definitions in kernel_cfg\n"
            "  -z, --nonzero              : Do not output __EMPTY_LABEL macro and related definitions\n"
            "  -1.3  : Generates kernel_cfg.c for TOPPERS/JSP rel 1.3\n",
            "カーネル構成ファイルの生成\n"
            "  -id, --kernel_id=ファイル名  : ID割当ファイルの名前を指定します\n"
            "  -cfg,--kernel_cfg=ファイル名 : カーネル構成ファイルの名前を指定します\n"
            "  -il, --include-libstyle      : カーネルのヘッダファイルを<...>の形で出力します\n"
            "  -oproto, --output-prototype  : タスク本体などの定義をkernel_cfg.cに生成します\n"
            "  -z, --nonzero                : __EMPTY_LABELマクロの使用を制限します\n"
            "  -1.3  : TOPPERS/JSP rel 1.3 用のコンフィギュレーションファイルを生成します\n");
        return;
    }

    checkOption("cfg","cfg");
    checkOption("id","id");

        /*
         *  カーネルコンフィギュレーション結果のファイル (kernel_cfg.c) の初期設定
         */

    createKernelCfg();

        /*
         *  ID割付結果のファイル (kernel_id.h) の初期設定
         */

    createKernelID();

        /* その他オプション */
    restrict_zero = findOption("z","zero");
    def_prototype = findOption("oproto","output-prototype");
    libstyle_include = findOption("il","include-libstyle");
    backward_compatible = findOption("1.3","1.3");
    
    checkOption("mcfg","minimize-cfg");
    checkOption("ZERO","ZERO");
    checkOption("obj", "dump-object");
    checkOption("ext");

    activateComponent();
}

void ConfigurationFileGenerator::onFatalExit(Directory & container)
{
        //例外で異常終了したら、ファイルを生成しない
    dynamic_cast<MultipartStream *>(RuntimeObjectTable::getInstance(typeid(KernelCfg)))->disableOutput();
    dynamic_cast<MultipartStream *>(RuntimeObjectTable::getInstance(typeid(KernelID)))->disableOutput();
}

void ConfigurationFileGenerator::createObjectDefinition(MultipartStream * out, Directory & container, int flag, ...)
{
    Directory * scope;
    const char * buffer;
    string work;

    map<int, Directory *> sorter;
    map<int, Directory *>::iterator psorter;

    va_list vl;
    int i;

    Directory & id = container[string(PREFIX "/") + container.getKey().c_str()];

    if(container.size() == 0 && findOption("mcfg","minimize-cfg"))
    {
        if(!getOptionParameter().find("ext"))
            ExceptionMessage("Use of the extended option --minimize-cfg. Please use -ext option together.","--minimizeオプションは拡張機能です。使用するには-extを指定してください。").throwException();
        return;
    }
    va_start(vl, flag);


        //必要な情報の作成
    work = string(va_arg(vl, const char *));
    id["id"]  = work;
    transform(work.begin(), work.end(), work.begin(), toupper_function);
    id["ID"]  = work;
    

    if((flag & SHORT_ID) != 0)
    {
        work = string(va_arg(vl, const char *));
        id["sid"]  = work;
	for(i=0; i<(signed)work.size(); i++) {
	  if (work[i] >= 'a' && work[i] <= 'z')
	    work[i] = work[i] - 'a' + 'A';
	}
        id["SID"]  = work;
    }else
    {
        id["sid"] = id["id"].toString();
        id["SID"] = id["ID"].toString();
    }

    if(!backward_compatible)
        id["symbolprefix"] = "_kernel_";

    work = container.getKey();
    id["name"] = work;
    out->movePart(work);

        //オブジェクト初期化関数を追加
    if((flag & INIT) != 0 && container.size() != 0)
        container["/" OBJECT_INITIALIZER].addChild(new Directory(work));

    (*out) << "\n\t/* " << (Message("Object initializer [%]","オブジェクト初期化ブロック [%]") << work) << " */\n\n";

    if(((flag & HEADER) != 0) && backward_compatible)
        (*out) << "#include " << conv_includefile(work) << "\n\n";

    if((flag & TNUM) != 0)
    {
        (*out) << id.format("#define TNUM_$(ID)ID ") << container.size() << "\n\n";
        (*out) << id.format("const ID $(symbolprefix,)tmax_$(id)id = (TMIN_$(ID)ID + TNUM_$(ID)ID - 1);\n\n");
    }

    if((flag & TNUMNO) != 0)
    {
        (*out) << id.format("#define TNUM_$(ID)NO ") << container.size() << "\n\n";
        (*out) << id.format("const UINT $(symbolprefix,)tnum_$(id)no = TNUM_$(ID)NO;\n\n");
    }

        /* ID順にソート (この時点で連番が確定しているはずなので，チェックはしない) */
    if((flag & (BUFFER|DEFINITION)) != 0)
    {
        const bool is_identifier = (flag & TNUMNO) == 0;

        i = 0;
        scope = container.getFirstChild();
        while(scope != 0)
        {
            if(is_identifier)
                sorter[scope->toInteger(i++)] = scope;
            else
                sorter[i++] = scope;

            scope = scope->getNext();
        }
    }

        //プロトタイプ定義の出力
    if((flag & PROTOTYPE) != 0)
    {
        buffer = va_arg(vl, const char *);

        if(def_prototype)
        {
            set<string> func;
            set<string>::iterator pfunc;

            scope = container.getFirstChild();
            while(scope != NULL)
            {
                func.insert(scope->format(buffer));
                scope = scope->getNext();
            }

            pfunc = func.begin();
            while(pfunc != func.end())
            {
                (*out) << (*pfunc) << '\n';
                ++ pfunc;
            }
        }
    }

        //バッファの作成
    if((flag & BUFFER) != 0 && container.size() != 0)
    {
        buffer = va_arg(vl, const char *);
        psorter = sorter.begin();
        while(psorter != sorter.end())
        {
            (*out) << (*psorter).second->format(buffer) << '\n';
            ++ psorter;
        }
        (*out) << '\n';
    }

        //定義ブロック(xINIB)の出力
    if((flag & DEFINITION) != 0)
    {
        buffer = va_arg(vl, const char *);
        if(container.size() != 0)
        {
            (*out) << id.format("const $(SID)INIB $(symbolprefix,)$(sid)inib_table[TNUM_$(ID)") << ( (flag & TNUMNO) != 0  ? "NO" : "ID") << "] = {\n";
            psorter = sorter.begin();
            while(psorter != sorter.end())
            {
                (*out) << "\t{" << (*psorter).second->format(buffer) << '}';
                ++ psorter;
                if(psorter != sorter.end())
                    (*out) << ',';
                (*out) << '\n';
            }
            (*out) << "};\n\n";
        }else
            if( !restrict_zero )
                (*out) << id.format("__EMPTY_LABEL(const $(SID)INIB, $(symbolprefix,)$(sid)inib_table);\n");
    }

    if((flag & INITLIST) != 0)
    {
        Directory * node;

            /* 宣言順でID番号をソート */
        sorter.clear();
        node = container.getFirstChild();
        while(node != NULL)
        {
            sorter.insert(pair<int,Directory*>(node->findChild("#order")->toInteger(), node));
            node = node->getNext();
        }

        *out << id.format("const ID $(symbolprefix,)$(sid)order_table[TNUM_$(ID)ID] = {");
        psorter = sorter.begin();
        while(psorter != sorter.end())
        {
            *out << psorter->second->toInteger();
            ++ psorter;
            if(psorter != sorter.end())
                *out << ',';
        }
        *out << "};\n\n";
    }

        //制御ブロック(xCB)の出力
    if((flag & CONTROLBLOCK) != 0)
    {
        if(container.size() != 0)
            (*out) << id.format("$(SID)CB $(symbolprefix,)$(sid)cb_table[TNUM_$(ID)ID];\n\n");
        else
            if( !restrict_zero )
                (*out) << id.format("__EMPTY_LABEL($(SID)CB, $(symbolprefix,)$(sid)cb_table);\n\n");
    }
}

void ConfigurationFileGenerator::body(Directory & container)
{
    Directory * scope;
    KernelCfg * out;
    string work;
    const bool varid_style = findOption("var","variable-id");

        /* kernel_id.hの出力 */
    serializeIdentifiers(varid_style);

        /* kernel_cfg.cの出力 */
    out = dynamic_cast<KernelCfg *>(RuntimeObjectTable::getInstance(typeid(KernelCfg)));
    if(out == 0) {
        ExceptionMessage("[Internal error] The stream buffer of kernel_cfg disappeared.","[内部エラー] ファイルストリームが開けません (kernel_cfg.c)").throwException();
        return;
    }

        /* 頭の定義部分の作成 */
    serializeConfiguration_headerPart(*out);

        /* INCLUDE */
    out->movePart(INCLUDEFILE);
    scope = container(OBJECTTREE "/" INCLUDEFILE)->getFirstChild();
    (*out) << "\t/* " << Message("User specified include files","ユーザ定義のインクルードファイル") << "*/\n";
    while(scope != 0)
    {
        (*out) << "#include " << scope->toString() << '\n';
        scope = scope->getNext();
    }
    (*out) << '\n';

        /* TASK */
    work.assign("$(tskatr), (VP_INT)($(exinf)), (FP)($(task)), INT_PRIORITY($(itskpri)), __TROUND_STK_UNIT($(stksz)), __stack_$@, $(texatr), (FP)($(texrtn))");
    if(container["/cpu"].toString().compare("nios32") == 0)
        work += ", $(hi_limit), $(lo_limit)";
    createObjectDefinition(out, container[OBJECTTREE "/" TASK], HEADER|TNUM|SHORT_ID|BUFFER|DEFINITION|CONTROLBLOCK|INIT|PROTOTYPE|INITLIST,"tsk", "t", "void $(task)(VP_INT exinf);", "static __STK_UNIT __stack_$@[__TCOUNT_STK_UNIT($(stksz))];",work.c_str());

        /* SEMAPHORE */
    createObjectDefinition(out, container[OBJECTTREE "/" SEMAPHORE], HEADER|TNUM|DEFINITION|CONTROLBLOCK|INIT,"sem", "$(sematr), $(isemcnt), $(maxsem)");

        /* EVENTFLAG */
    createObjectDefinition(out, container[OBJECTTREE "/" EVENTFLAG], HEADER|TNUM|DEFINITION|CONTROLBLOCK|INIT,"flg", "$(flgatr), $(iflgptn)");

        /* DATAQUEUE */
    createObjectDefinition(out, container[OBJECTTREE "/" DATAQUEUE], HEADER|TNUM|BUFFER|DEFINITION|CONTROLBLOCK|INIT,"dtq", "#if ($(dtqcnt)) > 0\n  static VP_INT __dataqueue_$@[$(dtqcnt)];\n#else\n  #define __dataqueue_$@ NULL\n#endif","$(dtqatr), $(dtqcnt), __dataqueue_$@");

        /* MAILBOX */
    createObjectDefinition(out, container[OBJECTTREE "/" MAILBOX], HEADER|TNUM|DEFINITION|CONTROLBLOCK|INIT,"mbx","$(mbxatr), $(maxmpri)");

        /* FIXEDSIZEMEMORYPOOL */
    createObjectDefinition(out, container[OBJECTTREE "/" FIXEDSIZEMEMORYPOOL], HEADER|TNUM|BUFFER|DEFINITION|CONTROLBLOCK|INIT,"mpf","static __MPF_UNIT __fixedsize_memorypool_$@[__TCOUNT_MPF_UNIT($(blksz)) * (($(blkcnt)))];","$(mpfatr), __TROUND_MPF_UNIT($(blksz)), __fixedsize_memorypool_$@, (VP)((VB *)__fixedsize_memorypool_$@ + sizeof(__fixedsize_memorypool_$@))");

        /* CYCLICHANDLER */
    createObjectDefinition(out, container[OBJECTTREE "/" CYCLICHANDLER], HEADER|TNUM|DEFINITION|CONTROLBLOCK|INIT|PROTOTYPE,"cyc","void $(cychdr)(VP_INT exinf);","$(cycatr),$(exinf),(FP)($(cychdr)),$(cyctim),$(cycphs)");

        /* INTERRUPTHANDLER */
    createObjectDefinition(out, container[OBJECTTREE "/" INTERRUPTHANDLER], HEADER|BUFFER|TNUMNO|DEFINITION|INIT|PROTOTYPE,"inh","void $(inthdr)(void);","CFG_INTHDR_ENTRY($(inthdr));","$@,$(inhatr),(FP)CFG_INT_ENTRY($(inthdr))");

        /* EXCEPTIONHANDLER */
    createObjectDefinition(out, container[OBJECTTREE "/" EXCEPTIONHANDLER], HEADER|BUFFER|TNUMNO|DEFINITION|INIT|PROTOTYPE,"exc","void $(exchdr)(VP p_excinf);","CFG_EXCHDR_ENTRY($(exchdr));","$@,$(excatr),(FP)CFG_EXC_ENTRY($(exchdr))");


        /* オブジェクト初期化ルーチン */
    out->movePart(OBJECT_INITIALIZER);
    (*out) << "\t/* " << Message("Object initialization routine","オブジェクト初期化ルーチン") << " */\n\n";
    (*out) << "void\n" << conv_kernelobject("object_initialize") << "(void)\n{\n";
    scope = container["/" OBJECT_INITIALIZER].getFirstChild();
    while(scope != 0)
    {
        (*out) << '\t' << conv_kernelobject(scope->toString() + "_initialize") << "();\n";
        scope = scope->getNext();
    }
    (*out) << "}\n\n";


        /* 初期化ハンドラ */
    out->movePart(INITIALIZER);
    (*out) << "\t/* " << Message("Initialization handler","初期化ハンドラ起動ルーチン") << " */\n\n";
    (*out) << "void\n" << conv_kernelobject("call_inirtn") << "(void)\n{\n";
    scope = container[OBJECTTREE "/" INITIALIZER].getFirstChild();
    while(scope != 0)
    {
        (*out) << scope->format("\t$(inirtn)( (VP_INT)($(exinf)) );\n");
        scope = scope->getNext();
    }
    (*out) << "}\n\n";

        /* 終了ハンドラ */
    scope = container[OBJECTTREE "/"  TERMINATOR].getLastChild();
    if(!backward_compatible) {
        (*out) << "void\n" << conv_kernelobject("call_terrtn") << "(void)\n{\n";
        while(scope != 0)
        {
            (*out) << scope->format("\t$(terrtn)( (VP_INT)($(exinf)) );\n");
            scope = scope->getPrev();
        }
        (*out) << "}\n\n";
    }
    else {
        if(scope->size() != 0)
            ExceptionMessage("VATT_TER is not supported for TOPPERS/JSP rel 1.3 or earlier.",
                             "VATT_TERはTOPPERS/JSP rel 1.3以前ではサポートされません").throwException();
    }

    out->movePart("others");
    (*out) << "TMEVTN   " << conv_kernelobject("tmevt_heap") << "[TNUM_TSKID + TNUM_CYCID];\n\n";

    container["/object_initializer"].erase();

    string filename;
    if(findOption("obj","dump-object",&filename))
    {
        if(filename.empty())
            filename.assign("kernel_obj.dat");

        fstream f(filename.c_str(),ios::out|ios::binary);
        if(f.is_open())
        {
            container[OBJECTTREE].Store(&f);
            f.close();
        }else
            ExceptionMessage(" Failed to open the file % for storing object definitions"," オブジェクト情報を格納するためのファイル(%)が開けません") << filename << throwException;
    }

    out->enableOutput();
}


/* ======================================================================== */

