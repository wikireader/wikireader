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
 *  @(#) $Id: directorymap.cpp,v 1.12 2003/12/15 07:32:13 takayuki Exp $
 */


// $Header: /home/CVS/configurator/base/directorymap.cpp,v 1.12 2003/12/15 07:32:13 takayuki Exp $

/* MEMO:メモ書き
     Q: こういうクラスはテンプレートにしたほうがいいと思うが?
     A: ファイルに吐き出した後、読み出したときにどうやってクラス生成すべきかがわからない
         (クラスファクトリを作るのはやめたい)
        今と違う型を代入されたときの対処法がわからない
*/


#include "base/directorymap.h"
#include "base/message.h"
#include <stdarg.h>
#include <typeinfo>
#include <cassert>
#include <cstdio>

#ifdef _MSC_VER
  #pragma warning(disable:4786)
#endif

using namespace std;

int Directory::defaultflag = Directory::NOTHING;

Directory::Directory(const Directory & src)
{
    parent = 0;
    flag = defaultflag;
    defaultflag &= ~DESTRUCT;

    type = src.type;
    switch(type)
    {
    case LITERAL:
        content.literal = new string(*src.content.literal);
        break;
    default:
        content = src.content;
        break;
    }
}

Directory::~Directory(void)
{
    disconnect();                       //親との連結を解除
    map<string,Directory*>::clear();    //子ノードの削除
    clearContent();                         //自分の中身を削除する
}

void Directory::clearContent(void)
{
    switch(this->getType())
    {
    case LITERAL:
        delete content.literal;
        break;
    case OBJECT:
        delete content.instance;
        break;
    default:
        break;
    }

    type = UNKNOWN;
    content.pointer = 0;
}

Directory * Directory::findNode(bool automatic_creation, const string & path)
{
    string::size_type top, tail, length;
    string work;
    Directory::iterator scope;
    Directory * node = this;

    if(this == NULL)
        return NULL;

    if(path.empty())
        return this;

    length = path.length();
    top = 0;
    if(path[0] == '/')
    {
        while(node->getParent() != 0)
            node = node->getParent();
        if(path.size() == 1)
            return node;
        top = 1;
    }

    do {
        tail = path.find_first_of('/', top);
        if(tail == string::npos)
            work = path.substr(top);
        else
            work = path.substr(top, tail-top);

        if(work.compare(".") == 0 || work.compare("..") == 0)
        {
            if(work.size() > 1 && node->getParent() != 0)
                node = node->getParent();
        }else
        {
            scope = node->begin();
            while(scope != node->end())
            {
                if(work.compare((*scope).first) == 0)
                    break;
                ++ scope;
            }

            if(scope == node->end())
            {
                if(!automatic_creation)
                    return 0;
                node = node->addChild(work, new Directory);
            }else
                node = (*scope).second;
        }
        top = tail + 1;
    } while( tail != string::npos && top < length );

    return node;
}

Directory * Directory::findNode(bool automatic_creation, const char * key, va_list vl)
{
    Directory::iterator scope;
    Directory * node = this;

    if(this == NULL)
        return NULL;

    if(key == NULL)
        return this;

    if(*key == '/' && *(key+1) == '\x0')
    {
        while(node->getParent() != 0)
            node = node->getParent();
        if(vl == 0)
            return node;
        key = va_arg(vl, const char *);
    }

    do {
        if(strcmp(key,".") != 0)
        {
            if(strcmp(key,"..") == 0)
            {
                node = node->parent;
            }else
            {
                scope = node->begin();
                while(scope != node->end())
                {
                    if((*scope).first.compare(key) == 0)
                        break;
                    ++ scope;
                }

                if(scope == node->end())
                {
                    if(!automatic_creation)
                        return 0;
                    node = node->addChild(key, new Directory);
                }else
                    node = (*scope).second;
            }
        }
        if(vl != 0)
            key = va_arg(vl, const char *);
        else
            break;
    } while( key != 0 && node != 0);

    return node;
}

Directory & Directory::operator =(void * pointer)
{
    if(this->getType() != UNKNOWN && this->getType() != POINTER)
        clearContent();

    type = POINTER;
    content.pointer = pointer;
    return *this;
}

Directory & Directory::operator =(long value)
{
    if(this->getType() != UNKNOWN && this->getType() != INTEGER)
        clearContent();

    type = INTEGER;
    content.value = value;
    return *this;
}

Directory & Directory::operator =(const string & literal)
{
    if(this->getType() != UNKNOWN && this->getType() != LITERAL)
        clearContent();

    type = LITERAL;
    content.literal = new string(literal);

    return *this;
}

Directory & Directory::operator =(const char * constliteral)
{
    if(this->getType() != UNKNOWN && this->getType() != CONSTLITERAL)
        clearContent();

    type = CONSTLITERAL;
    content.const_literal = constliteral;
    return *this;
}

Directory & Directory::operator =(Garbage * instance)
{
    if(this->getType() != UNKNOWN)
        clearContent();

    type = OBJECT;
    content.instance = instance;
    return *this;
}

void * Directory::operator new(size_t sz)
{
    defaultflag |= DESTRUCT;
    return ::operator new(sz);
}

void * Directory::operator new(size_t sz, nothrow_t)
{
    defaultflag |= DESTRUCT;
    return ::operator new(sz, nothrow);
}

Directory::operator const long(void) const
{
    if( type == UNKNOWN )
        ExceptionMessage("Bad cast exception","不正キャスト例外") << throwException;
    return content.value;
}

void * Directory::operator * (void) const
{
    if( type == UNKNOWN )
        ExceptionMessage("Bad cast exception","不正キャスト例外") << throwException;
    return content.pointer;
}

Directory * Directory::addChild(const std::string & key, Directory * node)
{
    iterator scope;
    std::pair<iterator, bool> work;

    if(node == 0)
        node = new Directory;
    else
        if(node->parent != 0)
            node->disconnect();

    node->parent = this;
    if((scope = find(key)) != end())
    {
        Directory * old = (*scope).second;
        old->disconnect();
        old->erase();
    }

    work = insert(value_type(key, node));
    node->myself = work.first;

    return node;
}

Directory::iterator Directory::erase(iterator it)
{
    iterator result;
    Directory * scope = (*it).second;

        //戻り値の作成
    if((result = it) == begin())
      ++ result;
    else
      -- result;

        //外したノードの後始末
    if((scope->flag & DESTRUCT) != 0)
        delete scope;
    else
        scope->erase();

    return result;
}

void Directory::erase(void)
{
    iterator scope;

    if(this != NULL)
    {
        if(parent != 0)
        {
            parent->erase(myself);
        }else
        {
            while(!empty())
            {
                scope = begin();
                if((scope->second->flag & DESTRUCT) != 0)
                    delete scope->second;
                else
                    scope->second->erase();
            }
        }
    }
}

void Directory::disconnect(void)
{
    if(parent != 0)
    {
        parent->map<string,Directory*>::erase(myself);
        parent = 0;
    }
}

Directory * Directory::getNext(void) const
{
    if(parent == 0)
        return 0;

    iterator scope;
    scope = myself;
    ++ scope;
    if(scope == parent->end())
        return 0;

    return (*scope).second;
}

Directory * Directory::getPrev(void) const
{
    if(parent == 0 && myself == parent->begin())
        return 0;

    reverse_iterator scope;
    
    scope = parent->rbegin();
    while(scope != parent->rend() && (*scope).second != (*myself).second)
        ++ scope;

    ++ scope;
    return scope != parent->rend() ? (*scope).second : 0;
}

bool Directory::changeKey(const string & key)
{
    Directory * scope;

    if( key.size() == 0)
        return false;

    scope = parent;

    disconnect();
    scope->addChild(key, this);
    return true;
}


void Directory::drawTree(ostream * out, int level, string * link)
{
    iterator scope;
    iterator scope2;

    if(level == 0)
        link = new string;
    else
        *out << (*link).substr(0, (level-1)*3) << " +-";

    *out << '[' << getKey() << ']';
    switch(type)
    {
    case POINTER:
        out->setf(ios::hex);
        *out << " : PTR [" << content.pointer << "]";
        break;
    case INTEGER:
        out->setf(ios::dec);
        *out << " : INT [" << content.value << "]";
        break;
    case LITERAL:
        *out << " : STR [" << *content.literal << "]";
        break;
    case CONSTLITERAL:
        *out << " : CSTR[" << content.const_literal << "]";
        break;
    case OBJECT:
        {
            *out << " : OBJ";
            break;
        }
    case UNKNOWN:
        break;
    default:
        *out << "UNKNOWN";
    }
    *out << '\n';
    (*link) += " | ";
    scope = begin();
    while(scope != end())
    {
        scope2 = scope;
        ++ scope;

        if(scope == end())
            (*link)[level*3+1] = ' ';
        (*scope2).second->drawTree(out, level+1, link);
    }


    link->erase(level*3);
    if(level == 0)
        delete link;
}

static string escapeXMLLiterals(const string & src)
{
    int index;
    string::size_type pos;
    string result;
    const char   literal[4] ="&<>";
    const char * escape[3]  = {"&amp;","&lt;","&gt;"};

    result = src;
    for(index = 0; index < 3; index++)
    {
        pos = 0;
        while((pos = result.find_first_of(literal[index],pos)) != string::npos)
        {
            result.erase(pos,1);
            result.insert(pos, escape[index]);
            ++ pos;
        }
    }

    return result;
}

static string encloseAttributes(const string & src)
{
    if(src.find_first_of('"') != string::npos)
        return string("'") + src + "'";
    return string("\"") + src + "\"";
}

void Directory::drawTree_byXML(ostream * out, int level)
{
    iterator scope;

    if(level == 0)
        *out << "<?xml version='1.0' encoding='Shift_JIS' ?>\n<?xml-stylesheet type='text/xsl' href='basic.xsl' ?>\n";

    *out << "<node key=" << encloseAttributes(escapeXMLLiterals(getKey())) << " type='";

    switch(type)
    {
    case POINTER:
        out->setf(ios::hex);
        *out << "PTR'><value>" << content.pointer << "</value>";
        break;
    case INTEGER:
        out->setf(ios::dec);
        *out << "INT'><value>" << content.value << "</value>";
        break;
    case LITERAL:
        *out << "STR'><value>" << escapeXMLLiterals(*content.literal) << "</value>";
        break;
    case CONSTLITERAL:
        *out << "CSTR'><value>" << escapeXMLLiterals(content.const_literal) << "</value>";
        break;
    case OBJECT:
        *out << "OBJ'>";
        break;
    case UNKNOWN:
        *out << "'>";
        break;
    default:
        *out << "UNKNOWN'>";
    }
    *out << '\n';

    scope = begin();
    if(scope != end())
    {
        *out << "<child>\n";
        do{
            scope->second->drawTree_byXML(out, level+1);
            ++ scope;
        }while(scope != end());
        *out << "</child>\n";
    }
    *out << "</node>\n";
}
 
Directory * Directory::findChild(const char * key)
{
    string work(key);
    if(work.find_first_of('/') != string::npos)
        return findChild(work);
    return findNode(false, key, 0);
}

Directory * Directory::findChild(const char * key, const char * second, ... )
{
    va_list vl;
    va_start(vl, second);
    return findNode(false, key, 0)->findNode(false,second, vl);
}

Directory * Directory::openChild(const char * key)
{
    string work(key);
    if(work.find_first_of('/') != string::npos)
        return openChild(work);
    return findNode(true, key, 0);
}


Directory * Directory::openChild(const char * key, const char * second, ... )
{
    va_list vl;
    va_start(vl, second);
    return findNode(true, key, 0)->findNode(true, second, vl);
}

    //指定したキーを持つ子孫を探す。サーチ順は中順
Directory * Directory::findDescandant(const string & key, unsigned int level) const
{
    Directory::const_iterator scope;
    const Directory * node = this;

    if(empty())
        return 0;

        //子で探す
    scope = begin();
    while( scope != end() )
    {
        if((*scope).first.compare(key) == 0)
            return const_cast<Directory *>((*scope).second);
        ++ scope;
    }

    if(level > 0)
    {
        scope = begin();
        while( scope != end() )
        {
            if((node = (*scope).second->findDescandant(key, level-1)) != 0)
                return const_cast<Directory *>(node);
            ++ scope;
        }
    }
    return 0;
}

void Directory::copyTo(Directory * dest, int nest)
{
    Directory::iterator scope;
    Directory * node;

    if(this == NULL)
        return;

    assert(dest != NULL);

    node = dest;
    while(node != 0)
    {
        if(node == this)
             ExceptionMessage("CopyTo: dest must not be a descendant node.","CopyTo: 子孫ノードへのコピーはできません") << throwException;
        node = node->getParent();
    }

    scope = begin();
    while(scope != end())
    {
        node = dest->findChild((*scope).first);
        if(node != 0)
            node->erase();

        node = dest->addChild((*scope).first, new Directory(*(*scope).second));
        if(nest > 0)
            (*scope).second->copyTo(node, nest-1);

        ++ scope;
    }
}

void Directory::Store(ostream * out)
{
    int i;
    Directory * node;

    out->write((const char *)&type, sizeof(type));
    switch(type)
    {
    case INTEGER:
        out->write((const char *)&content.value, sizeof(content.value));
        break;
    case LITERAL:
        i = content.literal->size();
        out->write((const char *)&i, sizeof(int));
        out->write(content.literal->c_str(), i);
        break;
    case CONSTLITERAL:
        i = strlen(content.const_literal);
        out->write((const char *)&i, sizeof(int));
        out->write(content.const_literal, i);
        break;
    default:
        out->write((const char *)&content.pointer, sizeof(content.pointer));
    }
    i = size();
    out->write((const char *)&i, sizeof(int));
    for(node = getFirstChild(); node != 0; node = node->getNext())
    {
        const string & work = node->getKey();
        i = work.size();
        out->write((const char *)&i, sizeof(int));
        out->write(work.c_str(), i);
        node->Store(out);
    }
}

void Directory::Load(istream * in)
{
    int i;
    int count;
    char buffer[1024];

    in->read((char *)&type, sizeof(type));
    switch(type)
    {
    case INTEGER:
        in->read((char *)&content.value, sizeof(content.value));
        break;
    case CONSTLITERAL:
    case LITERAL:
        in->read((char *)&i, sizeof(int));
        in->read(buffer, i);
        buffer[i] = '\x0';
        *this = string(buffer);
        break;
    default:
        in->read((char *)&content.pointer, sizeof(content.pointer));
    }
    in->read((char *)&count, sizeof(int));
    while(count-- > 0)
    {
        in->read((char *)&i, sizeof(int));
        in->read(buffer, i);
        buffer[i] = '\x0';

        addChild(buffer)->Load(in);
    }
}

string Directory::toString(const string & default_value) const
{
    if(this == 0)
        return default_value;

    switch(type)
    {
    case POINTER:
    case OBJECT:
        {
            char buffer[256];
            sprintf(buffer,"%08lx", (long)content.pointer);
            return string(buffer);
        }
    case INTEGER:
        {
            char buffer[256];
            sprintf(buffer,"%ld", content.value);
            return string(buffer);
        }
    case CONSTLITERAL:
        return string(content.const_literal);
    case LITERAL:
        return string(*content.literal);
        default:
            return default_value;
    }

    return default_value;
}

void * Directory::toPointer(const void * default_value) const
{
    if(this == 0)
        return (void *)default_value;

    switch(type)
    {
    case INTEGER:
        return (void *)&content.value;
    case CONSTLITERAL:
        return (void *)content.const_literal;
    case LITERAL:
        return (void *)content.literal->c_str();
    case POINTER:
        return (void *)content.pointer;
    case OBJECT:
        return (void *)content.instance;
    default:
        return (void *)default_value;
    }

    return (void *)default_value;
}

long Directory::toInteger(const long default_value) const
{
    bool minus = false;
    long work;
    const char * str;

    if(this == 0)
        return default_value;

    switch(type)
    {
    case INTEGER:
        return content.value;

    case POINTER:
        return (long)content.pointer;

    case LITERAL:
    case CONSTLITERAL:
        if(type == CONSTLITERAL)
            str = content.const_literal;
        else
            str = content.literal->c_str();

        if(*str == '-')
        {
            minus = true;
            str ++;
        }

        if(*str == '0')
        {
            str ++;
            if(*str == 'x' || *str == 'X')
            {
                if(sscanf(str+1, "%ux", (int *)&work) == 0)
                    return default_value;
            }else
                if(*str != '\x0')
                {
                    if(sscanf(str, "%o", (int *)&work) == 0)
                        return default_value;
                }else
                    return 0;
        }else
            if(sscanf(str, "%d", (int *)&work) == 0)
                return default_value;

        if(minus)
            work = -work;
        return work;
    default:
        return default_value;
    }

    return default_value;
}

static string::size_type find_corresponding_parenthesis(const string & target, string::size_type pos = 0, char left = '(', char right = ')')
{
    int nest;

    nest = 1;
    do {
        if(target[pos] == left)
        {
            ++ nest;
        }else
        {
            if(target[pos] == right)
            {
                -- nest;
                if(nest == 0)
                    return static_cast<int>(pos);
            }
        }
        ++ pos;
    }while(pos < target.size());

    return string::npos;
}


string Directory::format(const string & fmt, int mode)
{
    Directory * node;
    string work;
    string key;
    string default_value;
    string::size_type top,tail;
    string::size_type pos;
    int i;

    default_value.assign("(null)");
    tail = 0;
    while((top = fmt.find_first_of('$', tail)) != string::npos)
    {
        if(top != tail)
            work += fmt.substr(tail, top - tail);

        switch(fmt[top+1])
        {
        case '$':
            tail = top+2;
            work += '$';
            break;

        case '@':
            work += (*myself).first;
            tail = top + 2;
            break;

        case '(':
            top += 2;
            i = find_corresponding_parenthesis(fmt, top);
            key = fmt.substr(top, i - top);

            if(key.find_first_of('$') != string::npos)
                key = format(key,mode);

            pos = key.find_first_of(',');
            if(pos != string::npos)
            {
                default_value = key.substr(pos+1);
                key.erase(pos);
            }

            node = findNode(false,key);
            if((mode & PTRPREFIX) != 0 && (node != NULL && node->type == POINTER))
                work += "0x";
            work += node->toString(default_value);

            tail = i+1;
            break;
        }
    }

    work += fmt.substr(tail);

    return work;
}

map<std::string, Directory *>::size_type Directory::size(map<string, Directory *>::size_type defval) const
{
    size_type      i;
    const_iterator scope;

    if(this == NULL)
        return defval;

    i = 0;
    scope = begin();
    while(scope != end())
        ++ i, ++ scope;

    return i;
}

