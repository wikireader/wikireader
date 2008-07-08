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
 *  @(#) $Id: coverage.cpp,v 1.5 2003/12/15 07:32:13 takayuki Exp $
 */

#include "base/coverage_defs.h"
#include "base/coverage_undefs.h"

#include <iostream>
#include <iomanip>

using namespace std;

#if defined(COVERAGE)
/*
 *   簡単なカバレッジチェック
 */

    //フォーマット済みファイル位置情報の取得
string Coverage::Location::getDetails(void) const
{
    string result;

    result = string(filename) + ":" + String(lineno);
    if(*additional != '\x0')
        result += string("(") + additional + ")";

    return result;
}



    //要素の全削除
Coverage::BranchMap::~BranchMap(void) throw()
{
    iterator scope;

    scope = begin();
    while(scope != end()) {
        delete scope->second;
        ++ scope;
    }

    clear();
}

    //要素の登録
Coverage::BranchBase::BranchBase(const Location & location) throw()
{
    BranchMap * bmap = Singleton<BranchMap>::getInstance();
    NewBranchList * blist = Singleton<NewBranchList>::getInstance();

    (*bmap)[location] = this;
    blist->push_back(this);
}

    //locationに一致する要素の取得
Coverage::BranchBase * Coverage::BranchBase::find(const Location & location) throw()
{
    BranchMap * bmap = Singleton<BranchMap>::getInstance();
    BranchMap::iterator scope;
    BranchBase * result = 0;

    scope = bmap->find(location);
    if(scope != bmap->end())
        result = scope->second;

    return result;
}

    //BranchIfコンストラクタ
Coverage::If::If(const Location & location) throw() : BranchBase(location), true_case(false), false_case(false)
{}

    //ifの正当性判定 (成立/不成立の両方が起っている)
bool Coverage::If::checkValidity(void) const throw()
{   return true_case && false_case;   }

    //通過情報の取得
string Coverage::If::getDetails(void) const throw()
{
    string result;

    if(true_case)
        result += "true";

    if(false_case) {
        if(true_case)
            result += "/";
        result += "false";
    }

    return result;
}

    //if分岐のチェック
bool Coverage::If::branch(const Location & location, bool expression) throw()
{
    If * node = dynamic_cast<If *>(find(location));
    if(node == 0)
        node = new(nothrow) If(location);

    if(node != 0) {
        if(expression)
            node->true_case = true;
        else
            node->false_case = true;
    }
    else
        cerr << "[Coverage::Branch] Memory allocation error!\n";

    return expression;
}


    //Whileコンストラクタ
Coverage::While::While(const Location & location) throw() : BranchBase(location), valid(false)
{}

    //Whileの正当性判定 (少なくとも一回はループの中をまわっている)
bool Coverage::While::checkValidity(void) const throw()
{   return valid;   }

    //通過情報の取得
string Coverage::While::getDetails(void) const throw()
{
    string result;

    if(valid)
        result = "valid";
    else
        result = "invalid";

    return result;
}

    //while分岐のチェック (少なくとも一回はループの中をまわっている)
bool Coverage::While::branch(const Location & location, bool expression) throw()
{
    While * node = dynamic_cast<While *>(find(location));
    if(node == 0)
        node = new(nothrow) While(location);

    if(node != 0) {
        if(expression)
            node->valid = true;
    }
    else
        cerr << "[Coverage::Branch] Memory allocation error!\n";

    return expression;
}


    //Switchコンストラクタ
Coverage::Switch::Switch(const Location & location) throw() : BranchBase(location)
{}

    //Switchの正当性判定 (通過した要素だけを覚えておく (後々判定も入れたい))
bool Coverage::Switch::checkValidity(void) const throw()
{   return true;   }

    //通過情報の取得
string Coverage::Switch::getDetails(void) const throw()
{
    stringstream buf;
    set<int>::const_iterator scope;

    scope = checkpoint.begin();
    while(scope != checkpoint.end()) {
        buf << *scope;

        ++ scope;
        if(scope != checkpoint.end())
            buf << ", ";
    }
        
    return buf.str();
}

    //Switch分岐のチェック
void Coverage::Switch::_branch(const Location & location, int expression) throw()
{
    Switch * node = dynamic_cast<Switch *>(find(location));
    if(node == 0)
        node = new(nothrow) Switch(location);

    if(node != 0) {
        if(expression)
            node->checkpoint.insert(expression);
    }
    else
        cerr << "[Coverage::Branch] Memory allocation error!\n";
}

    //全ての項目を表示
void Coverage::printCoverage(ostream & out)
{
    BranchMap * bmap = Singleton<BranchMap>::getInstance();
    BranchMap::iterator scope;

    unsigned long cases = 0;
    unsigned long fails = 0;

    scope = bmap->begin();
    while(scope != bmap->end()) {
        ++ cases;

        if(scope->second->checkValidity())
            out << "Success : ";
        else {
            out << "Failure : ";
            ++ fails;
        }
        out << scope->first.getDetails() << ' ' << scope->second->getDetails() << '\n';

        ++ scope;
    }

    out << fails << " fails in " << cases << " cases (" << setprecision(2) << (fails * 100.0 / cases) << "%)\n";
}

    //全て通過したかどうかのチェック
bool Coverage::checkValidity(void)
{
    bool result = true;

    NewBranchList * blist = Singleton<NewBranchList>::getInstance();
    NewBranchList::iterator scope;

    scope = blist->begin();
    while(scope != blist->end()) {
        if(!(*scope)->checkValidity())
            result = false;
        ++ scope;
    }

    blist->clear();
    return result;
}
    //名称の取得
string Coverage::getBranchName(BranchBase * node)
{
        //一致する要素の検索
    BranchMap * bmap = Singleton<BranchMap>::getInstance();
    BranchMap::iterator scope;

    scope = bmap->begin();
    while(scope != bmap->end()) {
        if(scope->second == node)
            break;
        ++ scope;
    }

    //assert(scope != bmap->end());
    
    return scope->first.getDetails();
}

#endif /* COVERAGE */

