/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// Adapted from
// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

namespace {

int checkHex( const char* val, const unsigned long maxVal ) {
  switch( strlen( val ) ) {
  case 1: if( 0xF != maxVal ) {
      cerr << "1 hex digit needs max of 0xF" << endl;
      return -3;
    }
    break;
  case 2: if( 0xFf != maxVal ){
      cerr << "2 hex digits needs max of 0xFF" << endl;
      return -4;
    }
    break;
  default: return -5;
  }
  
  char* cp = nullptr;

  const unsigned long val2 = strtoul( val, &cp, 16 );

  if( '\0' != *cp ) {
    cerr << "Improper converion of " << val << endl;
    return -1;
  }

  if( val2 > maxVal ) {
    cerr << "Value " << val << ' ' << val2 << " too large" << endl;
    return -2;
  }

  return val2;
}// checkHex

}

int main( int argc, char* argv[] )
{
  if( 7 != argc ) {
    cerr << "setpalette  bgRR bgGG bgBB  fgRR fgGG fgBB\n"
	 << "R, G and B are single hex digits"
	 << endl;

    return -1;
  }
  
  const int bgRR = checkHex( argv[1], 0xFF );
  const int bgGG = checkHex( argv[2], 0xFF );
  const int bgBB = checkHex( argv[3], 0xFF );
  const int fgRR = checkHex( argv[4], 0xFF );
  const int fgGG = checkHex( argv[5], 0xFF );
  const int fgBB = checkHex( argv[6], 0xFF );

  cout << "bgRGB fgRGB= "
       << bgRR << ' '
       << bgGG << ' '
       << bgBB << ' '
       << fgRR << ' '
       << fgGG << ' '
       << fgBB
       << endl;   

  if( 
      0 >> bgRR ||
      0 >> bgGG ||
      0 >> bgBB ||
      0 >> fgRR ||
      0 >> fgGG ||
      0 >> fgBB
      ) {
    cerr << "Bad input" << endl;
    return -1;
  }
  
  // Set output mode to handle virtual terminal sequences
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
    {
      cerr << "Error from GetStdHandle" << endl;
      return GetLastError();
    }

  CONSOLE_SCREEN_BUFFER_INFOEX bufEx;
  memset( &bufEx, 0, sizeof( bufEx ) );
  bufEx.cbSize =  sizeof( bufEx );

  if( ! GetConsoleScreenBufferInfoEx( hOut,
				      &bufEx ) ) {
    int retval = GetLastError();
    cerr << "Error from getConsoleScreenBufferInfoEx: "
	 << retval
	 << endl;
    return retval;
   }
  
  COLORREF* colors = &bufEx.ColorTable[0];

  colors[ 0 ] = RGB( bgRR, bgGG, bgBB );
  colors[ 1 ] = RGB( fgRR, fgGG, fgBB );

  if( ! SetConsoleScreenBufferInfoEx( hOut,
				      &bufEx ) ) {
      cerr << "Error from SetConsoleScreenBufferInfoEx" << endl;
      return GetLastError();
   }

  return 0;
}// main
