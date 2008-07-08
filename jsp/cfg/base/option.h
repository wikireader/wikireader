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
 *  @(#) $Id: option.h,v 1.5 2003/12/20 06:51:58 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/option.h,v 1.5 2003/12/20 06:51:58 takayuki Exp $

#ifndef OPTION_H
#define OPTION_H

#include "base/testsuite.h"
#include "base/singleton.h"

#include <string>
#include <vector>
#include <map>

#define DEFAULT_PARAMETER "#default"

using namespace std;

/*
 *   オプションパラメータを管理するクラス
 */
class OptionParameter
{
public:
    class OptionItem : public std::string
    {
    protected:
        bool                      checked;  //使用したかどうか
        std::vector<std::string>  param;    //オプション

    public:
            //コンストラクタ
        OptionItem(void) throw() : string(DEFAULT_PARAMETER), checked(false) {}
        OptionItem(const std::string & name) throw() : string(name), checked(false) {}
        OptionItem(const OptionItem & src) throw() : string(src), checked(src.checked), param(src.param) {}

            //オプションパラメータを解析してOptionItemを生成する
        static bool createItem(OptionItem & item, string & argv);

            //別のOptionItemが持つパラメータを自分に追加する
        void mergeItem(const OptionItem & src);

            //パラメータの追加
        inline void addParameter(const std::string & src) throw()
        {   param.push_back(src);   }

            //パラメータ数の取得
        inline size_t countParameter(void) const throw()
        {   return param.size();    }

            //パラメータを持っているかどうか
        inline bool hasParameter(void) const throw()
        {   return countParameter() != 0;   }

            //位置を指定してパラメータを参照 (無効な位置だと空文字)
        inline std::string operator[](std::vector<std::string>::size_type offset) const throw()
        {
            string result;

            if(offset < countParameter())
                result = param[offset];

            return result;
        }

            //このオプションを使用したことを記す (OptionParameterを介しての利用のみなので "_" をつけておく)
        inline void _check(void) throw()
        {   checked = true;   }

            //このオプションに触ったかどうか
        inline bool isChecked(void) const throw()
        {   return checked;   }

            //有効かどうか (ななしは無効)
        inline bool isValid(void) const throw()
        {   return !empty();   }

            //無効なオプションアイテムの生成
        static OptionItem createInvalidItem(void) throw();

            //全てのパラメータ結合したものを取得
        std::string getParameters(std::string punctuator = " ") const throw();

            //パラメータの個数をチェック
        bool checkParameterCount(size_t count, bool allow_zero = false) const throw();

        TESTSUITE_PROTOTYPE(main)
    };

protected:
        //オプションを格納する変数
    std::map<std::string, OptionItem> container;    //set::begin()はconst_iteratorしか返さないので仕方なくmapに変える (実装を考えれば当然か...)

        //プログラム名称
    std::string program_name;

        //コマンドラインに渡された引数そのもの
    std::string cmdline;    

        //デフォルトコンストラクタ (テスト用)
    OptionParameter(void) throw() {}

        //要素の検出
    std::map<std::string, OptionItem>::iterator _find(const std::string & name, bool check = true) throw();

public:

        //シングルトンコンストラクタ
    SINGLETON_CONSTRUCTOR(OptionParameter) throw() {}

        //オプションパラメータのパース
    bool parseOption(int _argc, char const * const * _argv, char const * _default = NULL) throw();

        //全てのオプションが使用されたかどうかの確認
    bool validateOption(void) const throw();

        //使用されなかったオプション列を取得
    std::string getInvalidOptions(void) const throw();

        //アイテムの数を数える
    inline size_t countItem(void) const throw()
    {   return container.size();   }

        //オプションアイテムの参照
    OptionItem get(const std::string & name, bool check = true) throw();
    
    inline OptionItem operator [] (const std::string & name) throw()
    {   return get(name);   }
    
    inline OptionItem operator [] (const char * name) throw()
    {   return get(string(name));   }

        //オプションの有無の検出
    inline bool find(std::string name) throw()
    {   return get(name).isValid(); }


        //参照マークをつける
    inline bool check(const std::string & name) throw()
    {   return _find(name) != container.end();   }

    inline bool check(const char * name) throw()
    {   return check(string(name));   }

        //プログラム名を取得する
    inline const std::string & getProgramName(void) const throw()
    {   return program_name;    }

        //引数列をそのまま取得する (プログラム名は含まず)
    inline const std::string & getCommandLine(void) const throw()
    {   return cmdline;   }

    TESTSUITE_PROTOTYPE(main)
};

    /* システム内で単一のOptionParameterを取るための関数 */
inline OptionParameter & getOptionParameter(void) throw()
{   return *Singleton<OptionParameter>::getInstance();   }

#endif


