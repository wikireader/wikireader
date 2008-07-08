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
 *  @(#) $Id: directorymap.h,v 1.9 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/directorymap.h,v 1.9 2003/12/15 07:32:13 takayuki Exp $

#ifndef DIRECTORYMAP_H
#define DIRECTORYMAP_H

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif

#include "base/message.h"
#include "base/garbage.h"
#include "base/singleton.h"

  //MSVC6.0が <cstdio> してもstdに入れてくれないので
#include <stdio.h>

#include <string>
#include <map>
#include <iostream>

class Directory : public std::map<std::string, Directory *>
{
private:
    /*
     * Directoryクラス : 設計メモ
     *
     *   ・親子ノード間の関係
     *      生成 : 親が 子を直接いじってリンクを張る
     *      破棄 : 子が 親からの独立を依頼する (親から一方的に勘当しない)
     *
     *   ・NULLセーフ実装
     *      一部の関数は this != NULL を仮定しないで実装する (自分への利便性向上)
     *        - addChild
     *        - findChild, openChild (findNode()
     *        - erase(void)
     *        - getFirstChild, getLastChild, getNext, getPrev
     */

public:
    enum tagtype
    {
        UNKNOWN,
        POINTER,
        INTEGER,
        LITERAL,
        CONSTLITERAL,
        OBJECT
    };

    enum tagflag
    {
        NOTHING  = 0,
        DESTRUCT = 1,
        UNSIGNED = 2
    };

    enum tagmode
    {
        PTRPREFIX = 1
    };

protected:
    static int defaultflag;

    enum tagtype type;
    int flag;

    Directory * parent;
    std::map<std::string, Directory *>::iterator myself;

    union
    {
        void * pointer;
        long value;
        const char * const_literal;
        std::string * literal;
        class Garbage * instance;
    } content;

    Directory(const Directory &);
    void initialize(void);
    void clearContent(void);

    Directory * findNode(bool, const std::string &);
    Directory * findNode(bool automatic_creation, const char * key, va_list vl);

public:
    SINGLETON_CONSTRUCTOR(Directory) { initialize(); }

    Directory(void);
    Directory(int);
    Directory(long);
    Directory(const std::string &);
    Directory(void *);
    explicit Directory(const char *);
    explicit Directory(Garbage *);
    ~Directory(void);

    Directory & operator =(void *);
    Directory & operator =(long);
    Directory & operator =(const char *);
    Directory & operator =(const std::string &);
    Directory & operator =(Garbage *);

    bool operator == (enum tagtype cmptype)
    {   return type == cmptype;   };

    Directory * operator ()(const std::string & src)
    {   return findChild(src);  };
    Directory * operator ()(const char * src)
    {   return findChild(src);  };

    Directory & operator [](const std::string & src)
    {   return *openChild(src); };
    Directory & operator [](const char * src)
    {   return *openChild(std::string(src));    };

    enum tagtype getType(void) const { return type; };
    Directory *  getParent(void) const { return parent; };
    Directory *  getParent(int) const;
    Directory *  getNext(void) const;
    Directory *  getPrev(void) const;
    Directory *  getFirstChild(void) const;
    Directory *  getLastChild(void) const;
    Directory *  findChild(const std::string &);
    Directory *  findChild(const char *);
    Directory *  findChild(const char *, const char * , ... );
    Directory *  findDescandant(const std::string &, unsigned int = 0xffffffff) const;
    Directory *  openChild(const std::string &);
    Directory *  openChild(const char *);
    Directory *  openChild(const char *, const char *, ... );

    void * operator new(size_t);
    void * operator new(size_t, std::nothrow_t);
    void * operator * (void) const;

    operator const long (void) const;
    operator const unsigned long (void) const;
    operator const int (void) const;
    operator const unsigned int (void) const;
    operator const char (void) const;
    operator const unsigned char (void) const;
    operator const char * (void);
    operator const std::string & (void) const;
    operator const Garbage * (void) const;
    operator const void * (void) const;

    void * toPointer(const void * default_value = 0) const;
    long toInteger(const long default_value = 0) const;
    std::string toString(const std::string & default_value = "") const;

    bool operator == (int) const;
    bool operator == (const std::string &) const;
    bool operator == (const char *) const;

    template<class T>
    bool operator != (T src) const
    {   return !(this->operator ==(src));   };


    Directory * addChild(const std::string &, Directory * = 0);
    Directory * addChild(const char *, Directory * = 0);
    Directory * addChild(const std::string &, Directory &);
    Directory * addChild(const char *, Directory &);
    Directory * addChild(Directory &);
    Directory * addChild(Directory * = 0);

    void erase(void);
    iterator erase(iterator);
    void disconnect(void);
    void copyTo(Directory *, int = 0x7fffffff);
    void dropValue(void);
    std::map<std::string, Directory *>::size_type size(std::map<std::string, Directory *>::size_type = 0) const;

    const std::string getKey(void) const;
    bool changeKey(const std::string &);
    bool changeKey(const char *);

    void Load(std::istream *);
    void Store(std::ostream *);

    void drawTree(std::ostream * = &std::cerr, int = 0, std::string * = 0);
    void drawTree_byXML(std::ostream * = &std::cerr, int = 0);

    std::string format(const char *, int mode = 0);
    std::string format(const std::string &, int mode = 0);
};


//------

inline void Directory::initialize(void)
{
    parent = 0;
    type = UNKNOWN;
    flag = defaultflag;
    content.pointer = 0;
    defaultflag &= ~DESTRUCT;
}

inline Directory::Directory(void)
{   initialize();    }

inline Directory::Directory(long val)
{
    initialize();
    *this = val;
}

inline Directory::Directory(int val)
{
    initialize();
    *this = (long)val;
}

inline Directory::Directory(void * val)
{
    initialize();
    *this = val;
}

inline Directory::Directory(const std::string & val)
{
    initialize();
    *this = val;
}

inline Directory::Directory(const char * val)
{
    initialize();
    *this = val;
}

inline Directory::Directory(Garbage * val)
{
    initialize();
    *this = val;
}

inline Directory * Directory::addChild(const char * key, Directory * node)
{   return addChild(std::string(key), node);    }

inline Directory * Directory::addChild(const std::string & key, Directory & node)
{   return addChild(key, &node);    }

inline Directory * Directory::addChild(const char * key, Directory & node)
{   return addChild(std::string(key), &node);   }

inline Directory * Directory::addChild(Directory & node)
{   return addChild(&node); }

    //このaddChildで追加した要素は、一括削除するか、消してはいけない
inline Directory * Directory::addChild(Directory * node)
{
    char buffer[32];
    sprintf(buffer,"%03d", (int)size());
    return addChild(buffer, node);
}

inline bool Directory::changeKey(const char * key)
{   return changeKey(std::string(key)); }

inline const std::string Directory::getKey(void) const
{
    if(parent == 0)
        return std::string("/");
    return (*myself).first;
}

inline Directory::operator const unsigned long (void) const
{   return static_cast<const unsigned long>(this->operator const long());  }

inline Directory::operator const int (void) const
{   return static_cast<const int>(this->operator const long());  }

inline Directory::operator const unsigned int (void) const
{   return static_cast<const unsigned int>(this->operator const long());  }

inline Directory::operator const char (void) const
{   return static_cast<const char>(this->operator const long());  }

inline Directory::operator const unsigned char (void) const
{   return static_cast<const unsigned char>(this->operator const long());  }

inline Directory::operator const char * (void)
{
    if(type == CONSTLITERAL)
        return content.const_literal;
    if(type == LITERAL)
        return content.literal->c_str();
    ExceptionMessage("Bad cast exception raised","不正キャスト例外").throwException();
    return 0;
}

inline Directory::operator const std::string &(void) const
{
    if(type == LITERAL)
        return *content.literal;
    ExceptionMessage("Bad cast exception raised","不正キャスト例外").throwException();
    return *(std::string *)0;
}

inline Directory::operator const Garbage * (void) const
{
    if(type == OBJECT)
        return content.instance;
    ExceptionMessage("Bad cast exception raised","不正キャスト例外").throwException();
    return 0;
}

inline Directory::operator const void * (void) const
{   return **this;  }

inline Directory * Directory::findChild(const std::string & path)
{   return findNode(false, path);  }

inline Directory * Directory::openChild(const std::string & path)
{   return findNode(true, path);  }

inline Directory * Directory::getParent(int level) const
{
    const Directory * node = this;
    while(level-- > 0 && node != 0)
        node = node->parent;
    return const_cast<Directory *>(node);
}

inline Directory * Directory::getFirstChild(void) const
{
    if(this == 0 || size()== 0)
        return 0;
    return (*begin()).second;
}

inline Directory * Directory::getLastChild(void) const
{
    if(this == 0 || size()== 0)
        return 0;
    return (*rbegin()).second;
}

inline bool Directory::operator == (int src) const
{   return type == INTEGER && content.value == src; }

inline bool Directory::operator == (const std::string & src) const
{   return (type == LITERAL && src.compare(*content.literal) == 0) || (type == CONSTLITERAL && src.compare(content.const_literal) == 0);    }

inline bool Directory::operator == (const char * src) const
{   return (type == LITERAL && content.literal->compare(src) == 0) || (type == CONSTLITERAL && strcmp(content.const_literal, src)== 0); }

inline std::string Directory::format(const char * src, int mode)
{   return format(std::string(src), mode);  }

inline void Directory::dropValue(void)
{   clearContent();   }

#endif

