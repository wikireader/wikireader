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
 *  @(#) $Id: filecontainer.h,v 1.8 2003/12/15 07:32:13 takayuki Exp $
 */

// $Header: /home/CVS/configurator/base/filecontainer.h,v 1.8 2003/12/15 07:32:13 takayuki Exp $

#ifndef FILECONTAINER_H
#define FILECONTAINER_H

#ifdef _MSC_VER
#pragma warning(disable:4786) //デバッグ情報を255文字に切り詰めました
#endif

#include "testsuite.h"

#include <string>
#include <map>

#include "base/except.h"
#include "base/message.h"
#include "base/collection.h"

class FileContainer : public RuntimeObject
{
public:
    typedef unsigned long address_t;

    struct tagVariableInfo {
        address_t  address;
        union {
            size_t     size;
            int        value;
        };
    };

    enum tagByteOrder { LITTLE, BIG, HOSTORDER=LITTLE, UNKNOWN };   /* UNKNOWNはテスト用 */

protected:
    enum tagByteOrder byteorder;

    std::map<std::string, struct tagVariableInfo> variableinfo;

    FileContainer(void) throw() : byteorder(HOSTORDER) {}
    virtual ~FileContainer(void) throw() {}

public:
        /* インタフェース部 */
    virtual void                    attachModule(const std::string & filename) throw(Exception) = 0;
    virtual void                    loadContents(void * dest, address_t address, size_t size) throw(Exception) = 0;
    virtual address_t               getSymbolAddress(const std::string & symbol) throw(Exception) = 0;
    virtual std::string             getArchitecture(void) throw(Exception) = 0;

    virtual struct tagVariableInfo  getVariableInfo(const std::string & name) throw(Exception);
    virtual void                    attachInfo(const std::string & filename) throw(Exception);

        /* 登録されているコンテナの呼び出し */
    static inline FileContainer * getInstance(void) throw(Exception)
    {
        FileContainer * result;
        RuntimeObjectTable::getInstance(&result);
        if(result == 0)
            ExceptionMessage("[Internal error] Filecontainer has no instance.","[内部エラー] FileContainerのインスタンスがありません").throwException();
        return result;
    }

    enum tagByteOrder getByteOrder(void) const
    {   return byteorder;   }
};

class TargetVariableBase
{
protected:
    FileContainer::address_t address;               //変数のアドレス
    size_t                   size;                  //変数のサイズ
    size_t                   offset;                //属する構造体の先頭からのオフセット
    size_t                   structure_size;        //属する構造体の大きさ
    bool                     loaded;                //値が読出し済みであることを保持するフラグ

    inline TargetVariableBase(FileContainer::address_t _address, size_t _size) throw()
        : address(_address), size(_size), offset(0), structure_size(_size), loaded(false)
    {}

    inline TargetVariableBase(FileContainer::address_t addr, size_t sz, size_t ofs, size_t ssz) throw()
        : address(addr), size(sz), offset(ofs), structure_size(ssz), loaded(false)
    {}
    
    TargetVariableBase(const std::string & sym) throw();
    TargetVariableBase(const std::string & sym, size_t _size) throw();
    TargetVariableBase(const std::string & sym, const std::string & sz) throw();
    TargetVariableBase(const TargetVariableBase & src) throw();

    virtual ~TargetVariableBase(void) throw() {}

        /* 内容の取得 */
    void loadContent(void * dest, size_t dest_size) throw(Exception);

        /* エンディアン変換 */
    void changeEndian(char * buffer, size_t size) throw();

        /* アドレスの移動 (ついでにloadedを下げる) */
    inline void setAddress(int offset) throw()
    {
        if(isValid()) {
            address += offset;
            loaded   = false;
        }
    }

public:
    inline bool isValid(void) const throw()
    {   return this != 0 && address != 0 && size != 0 && structure_size != 0;   }

    inline size_t getSize(void) const throw()
    {   return this != 0 ? size : 0;   }

    inline size_t getOffset(void) const throw()
    {   return this != 0 ? offset : 0;   }

    inline size_t getStructureSize(void) const throw()
    {   return this != 0 ? structure_size : 0;   }

    inline FileContainer::address_t getAddress(void) const throw()
    {   return this != 0 ? address : 0;   }

    inline bool isLoaded(void) const throw()
    {   return this != 0 ? loaded : false;   }

        /* 基本的な操作 */
    inline bool operator == (const TargetVariableBase & right) const throw()
    {   return isValid() && right.isValid() && address == right.address;   }

    inline bool operator != (const TargetVariableBase & right) const throw()
    {   return ! operator ==(right);   }

    inline FileContainer::address_t operator & (void) const throw()
    {   return isValid() ? address + offset : 0;   }

    inline size_t sizeOf(void) const throw()
    {   return isValid() ? size : 0;   }
};

template<class T>
class TargetVariable : public TargetVariableBase
{
protected:
    T    entity;

public:
        /*
         * コンストラクタ (TargetVariableBaseに回送)
         */
    inline TargetVariable(FileContainer::address_t addr) throw()
        : TargetVariableBase(addr, sizeof(T)), entity() 
    {}

    inline TargetVariable(FileContainer::address_t addr, size_t sz) throw()
        : TargetVariableBase(addr, sz), entity() 
    {}

    inline TargetVariable(FileContainer::address_t addr, size_t sz, size_t ofs, size_t ssz) throw()
        : TargetVariableBase(addr, sz, ofs, ssz), entity() 
    {}

    inline TargetVariable(const std::string & sym) throw()
        : TargetVariableBase(sym), entity()
    {}

    inline TargetVariable(const std::string & sym, const std::string & sz) throw()
        : TargetVariableBase(sym, sz), entity() 
    {}

    inline TargetVariable(const TargetVariable<T> & src) throw()
        : TargetVariableBase(src), entity()
    {
        if(isValid() && src.isValid()) {
            loaded = src.loaded;
            entity = src.entity;
        }
    }

        /* デストラクタ (特に何もしない) */
    virtual ~TargetVariable(void) throw() 
    {}

        /* 正当判定に格納に十分なサイズがあるかどうかを追加しておく */
    inline bool isValid(void) const
    {   return TargetVariableBase::isValid() && (sizeof(T) >= getSize());   }

        /*
         * ポインタ風の動作をするオペレータ群
         */
    inline TargetVariable<T> offsetInBytes(int offset) const throw()
    {   return TargetVariable<T>(getAddress() + offset, getSize(), getOffset(), getStructureSize());   }

    inline TargetVariable<T> operator + (int index) const throw()
    {   return offsetInBytes(index * static_cast<int>(getStructureSize()));   }

    inline TargetVariable<T> operator - (int index) const throw()
    {   return offsetInBytes(- index * static_cast<int>(getStructureSize()));   }

    inline TargetVariable<T> operator ++ (int) throw()
    {
        TargetVariable<T> result(*this);
        ++ (*this);
        return result;
    }

    inline TargetVariable<T> operator -- (int) throw()
    {
        TargetVariable<T> result(*this);
        -- (*this);
        return result;
    }

    inline TargetVariable<T> & operator ++ (void) throw()
    {
        setAddress(static_cast<int>(getStructureSize()));
        return *this;
    }

    inline TargetVariable<T> & operator -- (void) throw()
    {  
        setAddress(-static_cast<int>(getStructureSize()));   
        return *this;
    }

    inline TargetVariable<T> & operator += (int sz) throw()
    {   
        setAddress(static_cast<int>(getStructureSize()) * sz);   
        return *this;
    }

    inline TargetVariable<T> & operator -= (int sz) throw()
    {   
        setAddress(-static_cast<int>(getStructureSize()) * sz);   
        return *this;
    }

    inline const T & operator * (void) throw(Exception)
    {
        if(!isLoaded())
            loadContent(&entity, sizeof(T));
        return entity;
    }

    inline T operator [] (int index) const throw(Exception)
    {   return * TargetVariable<T>(getAddress() + index * getStructureSize(), getSize(), getOffset(), getStructureSize());   }
};

#endif

