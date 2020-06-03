/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef CFG_INFO_H
#define CFG_INFO_H

#include "BootUtil.h"

struct CFG_Info
{
  char* configFileName_ = nullptr;
  char* incFileName_ = nullptr;
  char* platFileName_ = nullptr;

  CFG_Info( void )
    {}

  ~CFG_Info() {
    DeleteArray( configFileName_ );
    DeleteArray( incFileName_); 
    DeleteArray( platFileName_);
  }

  bool setup( const char* pathName );

  static const char incFileBaseName_[];
  static const char platFileBaseName_[];

 private:
  CFG_Info( const CFG_Info& other );
  CFG_Info& operator=( const CFG_Info& other );
};

struct CFG_Defs
{
  char* sourceDirName_;
  char* configPathName_;

  CFG_Defs( void ) :
    sourceDirName_( NULL ),
    configPathName_( NULL )
    {}

  ~CFG_Defs() {
    DeleteArray( sourceDirName_ );
    DeleteArray( configPathName_ ); }

  /*
    A NULL dirName is replaced by "." .
    Failure is returned if defs.inc does not open.
    Error is returned if defs.inc does not parse.
    Otherwise, Success is returned.
   */
  GenStatus restore( const char* dirName = NULL );

  static
  bool archive( const char* srcDirName,
		const char* configPathName );

  static
  const char fileBaseName_[];

 private:
  CFG_Defs( const CFG_Defs& other );
  CFG_Defs& operator=( const CFG_Defs& other );
};


#endif
