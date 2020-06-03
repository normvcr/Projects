#ifndef GENPARSE_H
#define GENPARSE_H
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <iosfwd>
#include <iomanip>
#include "BootUtil.h"

// Skips white space, then matches character for character
// the contents of the string with the stream.
std::istream& operator>>( std::istream& is, const char* constStr );
std::istream& operator>>( std::istream& is, const char& constChar );

namespace GenArchive
{
  // Without this, the compiler can complain about missing
  // the operator>>(istream&, const char*), when calling it
  // from within this namespace.
#if 0
  inline
    std::istream& operator>>( std::istream& is, const char* ccp ) {
    return ::operator>>( is, ccp ); }    
#else
  using ::operator>>;//( std::istream&, const char* );
#endif
}

// Matches character for character
// the contents of the string with the stream.
// Returns true if a match is found.
// Only matched characters are removed from the stream
// Leading white space is NOT skipped.
bool get( std::istream& is, const char* constStr );
bool get( std::istream& is, const char& constChar );

// Finds and extracts the first occurence of token.
// Assumes token does not have any new line characters.
// Stream is positioned right after the token (or character)
bool find( std::istream& is, const char* token );
bool find( std::istream& is, const char c );

// Skips ws, extracts contiguous string to os, stopping at ws or EOF.
// A null character is not inserted.
std::istream& operator>>( std::istream& is, std::ostream& os );

// Returns a nul-terminated string.
TRANSFER
char* GEN_readString( std::istream& is );

// Returns NULL if buf is NULL or no token found.
// Returns only non-empty tokens.
// afterLast is set to 
//    first char past the nul byte of the found token, or
//    nul, if no bytes left in the string.
char* GEN_findToken( char* buf, char*& afterLast );

// Returns true if str is a valid C identifier:
// 1st char alpha or '_'
// Remaining chars alphanum or '_'
bool isident( const char* str );

////////////// fillblanks(int) manipulator ////////////////////
// os << fillblanks(3)
// puts 3 blanks into the ostream, os.

struct _FillBlanks
{ 
  int count_;

  _FillBlanks( int N ) :
  count_( N )
  {}
};

inline
_FillBlanks fillblanks( int count ) { return _FillBlanks( count ); }

// Puts count spaces into os.
void GEN_fb( std::ostream& os, int count );

template<typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>& 
operator<<(std::basic_ostream<_CharT, _Traits>& os, _FillBlanks N )
{ 
  GEN_fb( os, N.count_ );
  return os; 
}
///////////////////////////////////////////////////////////////////


#endif


