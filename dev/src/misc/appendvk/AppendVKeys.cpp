/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <CLI/CLI.hpp>
#include <string>
#include <string.h>
#include <fstream>


using namespace std;
//using namespace ssrd_logging;

int main( int argc, char *argv[] )
{
  //setLoggingLevel( LoggingLevel::INFO );

  string outFileName;
  string enumPrefix;

  CLI::App parser{"Append alpha-numeric ascii codes"};
  {
    parser.add_option( "-o,--outFile",
		       outFileName,
		       "Contains list of  enum = value"
		       )
      ->required();

    parser.add_option( "-e,--eprefix",
		       enumPrefix,
		       "Prefix for enums e.g. VKEY_"
		       )
      ->required();

    parser.footer(
R"(Append  
    VKEY_* = value,
to the file
for * = 0...9, and * = A...Z
where, value = the ascii value of * .
 )" );

    CLI11_PARSE(parser, argc, argv);
  }

  ofstream ofs( outFileName.c_str(),
		ios_base::app );

  if( !ofs ) {
    cerr << "Error opening to append " << outFileName << endl;
    return -1;
  }

  // Numeric

  for( char ind = '0'; ind <= '9'; ++ind ) {
    ofs << ",\n"
	<< enumPrefix
	<< ind
	<< " = 0x"
	<< uppercase << hex << int( ind )
      ;
  }

  for( char ind = 'A'; ind <= 'Z'; ++ind ) {
    ofs << ",\n"
	<< enumPrefix
	<< ind
	<< " = 0x"
	<<  int( ind )
      ;
  }

    return 0;
}// main

