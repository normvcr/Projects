/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <type_traits>
#include "KbdVkeyHeader.h"

enum SSRD_Enum {
		#include "KbdCodes.inc0"
};

typedef std::underlying_type<SSRD_Enum>::type IntegerType;

struct SSRD_Struct {
  const char* name;
  IntegerType value;
} SSRD_array[] = {
  #include "KbdCodes.inc1"
};

#include <iostream>
#include <fstream>

#include "GenLogging.h"

using namespace std;

int main( int argc, char *argv[] )
{
  if( 1 != argc ) {
    cerr << "Usage: inc2inc\n"
	 << "Creates KbdCodes.inc as output.\n"
	 << "Expects KbdCodes.inc0, KbdCodes.inc1 and KbdVkeyHeader.h\n"
	 << "for inclusion."
	 << endl;

    return 1;
  }

  const char* fileName =  "KbdCodes.inc";
  ofstream ofs( fileName );

  if( !ofs ) {
    cerr << "Error creating output file "
	 << fileName
	 << endl;

    return -1;
  }
  
  const int numEntries = sizeof( SSRD_array ) / sizeof( SSRD_Struct );

  for( int ind = 0; ind < numEntries; ++ind ) {
    if( 0 < ind ) ofs << ",\n";

    const SSRD_Struct& s = SSRD_array[ ind ];

    ofs << s.name << " = " << s.value;
  }
  
  return 0;
}// main
