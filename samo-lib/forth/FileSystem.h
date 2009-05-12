// interface to file system for Forth programs

//	Copyright 2009 Christopher Hall <hsw@openmoko.com>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in
//     the documentation and/or other materials provided with the
//     distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#if !defined(_FILE_SYSTEM_H_)
#define _FILE_SYSTEM_H_ 1

#include "forth.h"

void FileSystem_initialise(void);
void FileSystem_CloseAll(void);

Forth_ReturnType FileSystem_rename(const Forth_PointerType OldFilename, Forth_CellType OldLength,
				   const Forth_PointerType NewFilename, Forth_CellType NewLength);
Forth_ReturnType FileSystem_delete(const Forth_PointerType filename, Forth_CellType length);
Forth_ReturnType FileSystem_create(const Forth_PointerType filename, Forth_CellType length, Forth_CellType fam);

Forth_ReturnType FileSystem_open(const Forth_PointerType filename, Forth_CellType length, Forth_CellType fam);
Forth_ReturnType FileSystem_close(Forth_CellType handle);
Forth_ReturnType FileSystem_read(Forth_CellType handle, void *buffer, Forth_CellType length);
Forth_ReturnType FileSystem_write(Forth_CellType handle, void *buffer, Forth_CellType length);
Forth_ReturnType FileSystem_sync(Forth_CellType handle);
Forth_ReturnType FileSystem_lseek(Forth_CellType handle, Forth_CellType pos);
Forth_ReturnType FileSystem_ltell(Forth_CellType handle, Forth_CellType pos);
Forth_ReturnType FileSystem_lsize(Forth_CellType handle, Forth_CellType pos);

Forth_CellType FileSystem_ReadOnly(void);
Forth_CellType FileSystem_ReadWrite(void);
Forth_CellType FileSystem_WriteOnly(void);
Forth_CellType FileSystem_bin(Forth_CellType fam);

Forth_ReturnType FileSystem_OpenDirectory(const Forth_PointerType directoryname, Forth_CellType length);
Forth_ReturnType FileSystem_CloseDirectory(Forth_CellType handle);
Forth_ReturnType FileSystem_ReadDirectory(Forth_CellType handle, void *buffer, Forth_CellType length);


Forth_ReturnType FileSystem_AbsoluteRead(Forth_CellType sector, void *buffer, Forth_CellType count);
Forth_ReturnType FileSystem_AbsoluteWrite(Forth_CellType sector, const void *buffer, Forth_CellType count);


#endif
