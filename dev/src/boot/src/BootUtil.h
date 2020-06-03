/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef GenUtil_H
#define GenUtil_H


#include <stdlib.h> // size_t, calloc
#include <iostream>

// For use with unique_ptr with malloc'd memory
template< class T >
struct free_delete {

  void operator()(T* ptr) const {
    free( ptr );
  }

};

enum GenStatus
{
  GenStatusSuccess,
  GenStatusFailure, // e.g. no more characters left to get from file
  GenStatusError    // e.g. permissions problem
};

// Memory ownership is changing hands.
#define TRANSFER

template< class T >
inline
void DeletePointer( T*& ptr )
{
  delete ptr;
  ptr = NULL;
}

template< class T >
inline
void DeleteArray( T*& ptr )
{
  delete [] ptr;
  ptr = NULL;
}

// Do this as template, otherwise the compiler
// generates a temporary for "ptr" when the
// prototype GenFree( void*& ptr ) is used.
template< class T >
inline
void FreePointer( T*& ptr )
{
  if( NULL != ptr )
  {
    free( ptr );
    ptr = NULL;
  }
}

inline
void* GenCalloc( size_t count, size_t size )
{
  if( (count == 0) || (size == 0) )
  {
    return NULL;
  }

  return calloc( count, size );
}

TRANSFER
char* GenStrNew( const char* str );

// Returns the absolute path to the given dirName
// relative to the workingDirName. 
// If workingDirName is NULL, then cwd is used for it.  
// Returns NULL if dirName or workingDirName
// is not an accessible directory for chdir(),
// or if dirName is NULL.
TRANSFER char* GenAbsDir( const char* dirName,
		       const char* workingDirName = NULL );

// As GenAbsDir, but cannot assume the path is a dir
TRANSFER char* GenAbsPath( const char* pathName,
			   const char* workingDirName = NULL );

bool GenWrite( std::ostream& destFile,
	       const char* srcFileName );

bool GenWrite( const char* destFileName,
	       const char* srcFileName );
#endif
