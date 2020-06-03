/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <iostream>
#include "BootParse.h"
#include <cctype>
#include <sstream>

using namespace std;

istream& operator>>( istream& is, const char* constStr )
{
  is >> ws;

  if( !get( is, constStr ) )
  {
    is.clear( is.rdstate() | ios::failbit );    
  }

  return is;
}

istream& operator>>( istream& is, const char& constChar )
{
  is >> ws;

  if( !get( is, constChar ) )
  {
    is.clear( is.rdstate() | ios::failbit );    
  }

  return is;
}


bool get( std::istream& is, const char* constStr )
{
  char c = '\0';

  while( *constStr != '\0' )
  {
    is.get( c );

    if( !is || (c != *constStr) )
    {
      is.putback( c );
      return false;
    }    

    ++constStr;
  }

  return true;  
}// get

bool get( std::istream& is, const char& constChar )
{
  char c = '\0';

  is.get( c );

  if( !is || (c != constChar) )
    {
      is.putback( c );
      return false;
    }    

  return true;  
}// get

bool find( istream& is, const char* token )
{
  if( '\0' == *token ) {
    return false;
  }

  while( 1 ) {
    if( !find( is, *token ) ) {
      break;
    }
  
    if( get( is, 1+token ) ) return true;
  }

  return false;
}// find(char*)

bool find( istream& is, const char c )
{
  while( 1 )
  {
    int retval = is.get();

    if( EOF == retval ) break;
    
    if( c == retval ) return true;
  }

  return false;
}// find(char)


istream& operator>>( istream& is, ostream& os )
{
  const int MYEOF = char_traits<char>::eof();
  // const int MYEOF = is::traits_type::eof;

  is >> ws;

  int ci;
  while( ((ci = is.get()) != MYEOF) && !isspace(ci) )
  {
    os << (char)ci;
  }

  return is;
}


TRANSFER
char* GEN_readString( istream& is )
{
  ostringstream str;

  is >> str;
  str << ends;

  return GenStrNew( str.str().data() );
}


char* GEN_findToken( char* buf, char*& afterLast )
{
  if( buf == NULL )
  {
    return NULL;
  }

  while( isspace( *buf ) )
  {
    ++buf;
  }

  if( *buf == '\0' )
  {
    return NULL;
  }

  afterLast = buf;

  while( !isspace( *afterLast ) )
  {
    ++afterLast;

    if( *afterLast == '\0' )
    {
      afterLast = NULL;
      return buf;
    }
  }

  *afterLast = '\0';
  ++afterLast;
  return buf;
}// GEN_findToken

bool isident( const char* str )
{
  if( !( isalpha( str[0] ) ||
	 '_' == str[0] ) )
  {
    cerr << "isident: str[0]= 0x" << hex << (int)str[0] << endl;

    return false;
  }

  for( char c = *++str; c != '\0'; c=*++str )
  {
    if( !( isalnum( c ) ||
	   '_' == c ) )
    {
      cerr << "isident:  c= 0x" << hex << (int)c << endl;

      return false;
    }
  }

  return true;
}// isident


void GEN_fb( std::ostream& os, int count )
{
  if( os )
  {
    while( 0 <= --count )
    {
      os << ' ';
    }
  }
}

