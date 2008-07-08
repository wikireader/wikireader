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
 *  @(#) $Id: coverage_defs.h,v 1.4 2003/12/15 07:32:13 takayuki Exp $
 */

/*
    #ifdef関係
    coverage_defsは、undefsで解除したあとに読み込む場合があるので、
        パートをいくつかに分割している.
        - COVERAGE      : カバレッジ機能を使用するかどうか
        - COVERAGE_H    : coverage.cppに関連する部分で、多重定義されては困るもの
        - COVERAGE_DEFS : カバレッジに関連する部分で、coverage_undefs.hでundefでき、もう一度coverage_defs.hを読んでも大丈夫なもの
*/


#ifdef COVERAGE

#include "base/singleton.h"

    /* ヘッダ */
#include <string>
#include <map>
#include <set>
#include <iostream>


#ifndef COVERAGE_H
#define COVERAGE_H
/*
 *   簡単なカバレッジチェック
 */

class Coverage {
public:
        /* 場所を保持するクラス */
    class Location {
    protected:
        const char *  filename;
        unsigned int  lineno;
        const char *  additional;

    public:
        Location(const char * _filename, unsigned int _lineno, const char * _additional = "") : filename(_filename), lineno(_lineno), additional(_additional) {}
        Location(const Location & src) : filename(src.filename), lineno(src.lineno), additional(src.additional) {}

        inline Location & operator = (const Location & right)
        {
            filename   = right.filename;
            lineno     = right.lineno;
            additional = right.additional;

            return *this;
        }

        inline bool operator == (const Location & right) const
        {   return lineno == right.lineno && std::string(filename).compare(right.filename) == 0 && std::string(additional).compare(right.additional) == 0;   }

        inline bool operator < (const Location & right) const
        {
            if(lineno < right.lineno)
                return true;
            if(std::string(filename).compare(right.filename) < 0)
                return true;

            return false;
        }

        inline std::string getFilename(void) const
        {   return std::string(filename);   }

        inline unsigned int getLineno(void) const
        {   return lineno;   }

        inline std::string getAdditional(void) const
        {   return std::string(additional);   }

            //フォーマット済みファイル位置情報の取得 filename:lineno(additional)
        std::string getDetails(void) const;
    };

        //分岐カバレッジ判定のベースクラス
    class BranchBase {
    protected:
        BranchBase(const Location & location) throw();   //要素の登録
        static BranchBase * find(const Location & location) throw();     //locationに一致する要素の取得

    public:
        virtual ~BranchBase(void) throw() {}                        //デストラクタ
        virtual bool checkValidity(void) const throw(...) = 0;      //正当性判定
        virtual std::string getDetails(void) const throw(...) = 0;  //データ表示
    };

        //if-statementに引っかかるクラス
    class If : public BranchBase {
    protected:
        bool true_case;
        bool false_case;

        If(const Location & location) throw(); //branch経由で無いと生成させない

    public:
        virtual bool checkValidity(void) const throw();         //正当性の判定
        virtual std::string getDetails(void) const throw();     //データ表示

        static bool branch(const Location & location, bool expression) throw();
    };
    
        //while-statementに引っかかるクラス
    class While : public BranchBase {
    protected:
        bool valid;

        While(const Location & location) throw();
    public:
        virtual bool checkValidity(void) const throw();         //正当性の判定
        virtual std::string getDetails(void) const throw();     //データ表示

        static bool branch(const Location & location, bool expression) throw();
    };

        //switch-statementに引っかかるクラス
    class Switch : public BranchBase {
    protected:
        std::set<int> checkpoint;

        Switch(const Location & location) throw();
        static void _branch(const Location & location, int expression) throw();

    public:
        virtual bool checkValidity(void) const throw();         //正当性の判定
        virtual std::string getDetails(void) const throw();     //データ表示

        template<class T>
        static T branch(const Location & location, T expression) throw()
        {
            _branch(location, (int)expression);
            return expression;
        }
    };


protected:
    class BranchMap : public std::map<Location, BranchBase *> {
    public:
        SINGLETON_CONSTRUCTOR(BranchMap) {}
        ~BranchMap(void) throw();
    
    };
    class NewBranchList : public std::list<BranchBase *>
    {   public: SINGLETON_CONSTRUCTOR(NewBranchList) {}   };

        //項目の表示
    static std::string getBranchName(BranchBase * node);

public:
        //全て通過したかどうかのチェック
    static bool checkValidity(void);

        //全ての項目を表示
    static void printCoverage(std::ostream & out);

};

#endif /* COVERAGE_H */


#ifndef COVERAGE_DEFS
#define COVERAGE_DEFS

#define if(x)    i##f(Coverage::If::branch(Coverage::Location(__FILE__, __LINE__, "if"), (x) ? true : false))
#define while(x) w##hile(Coverage::While::branch(Coverage::Location(__FILE__, __LINE__, "while"), (x) ? true : false))
#define switch(x) s##witch(Coverage::Switch::branch(Coverage::Location(__FILE__, __LINE__, "switch"), (x)))

#endif  /* COVERAGE_DEFS */

#else

#include <iostream>

#ifndef COVERAGE_H
#define COVERAGE_H
class Coverage {
public:
    static bool checkValidity(void) { return true; }
    static void printCoverage(std::ostream &) {}
};
#endif /* COVERAGE_H */

#endif

