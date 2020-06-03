/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "BootArch.h"
#include "GenLogging.h"
#include <CLI/CLI.hpp>
#include <string>
#include <string.h>
#include <stdio.h>

using namespace std;
//using namespace ssrd_logging;

int main( int argc, char *argv[] )
{
  //setLoggingLevel( LoggingLevel::INFO );

  string inFileName;
  string outFileName;
  string macroPrefix;
  string enumPrefix;
  bool createTwoFiles = false;

  CLI::App parser{"Convert macros to enum"};
  {
    parser.add_option( "-i, --inFile",
		       inFileName,
		       "File containing macros" )
      ->required();
    
    parser.add_option( "-o,--outFile",
		       outFileName,
		       "Contains list of  enum = value"
		       )
      ->required();

    parser.add_option( "-m,--mprefix",
		       macroPrefix,
		       "Prefix for macros to convert"
		       )
      ->required();

    parser.add_option( "-e,--eprefix",
		       enumPrefix,
		       "Replacement prefix for enums"
		       )
      ->required();

    parser.add_flag( "-2",
		     createTwoFiles,
		     "Create 2 output files" );
    

    parser.footer(
R"(Converts all macros that begin with the macro prefix.
The input file is scanned by the gnu C++ preprocessor, 
so the macros do not need to be directly in the input file.
Each  enum=value  line, except the last, is ended by a comma.

The value may actually be a macro, rather than an integer.
With the -2 option, two output files are created, with 0 & 1
appended to the output file name.  The 0-file is identical to 
the standard output file (without the -2 flag), and the 1-file
defines an array of { char*, enum }, which can be used to compile
into an executable to write out the values all as numbers, as is
done by the inc2inc executable.
 )" );

    CLI11_PARSE(parser, argc, argv);
  }


  string cmd( "g++ -E -dD -I. ");
  cmd += inFileName;

  FILE* inp = popen( cmd.c_str(), "r" );
  if( nullptr == inp ) {
    cerr << "Error crating pipe \"" << cmd << '"' << endl;
    return -1;
  }

  string outFileName_1;
  FILE* outp_1 = nullptr;

  if( createTwoFiles ) {
    outFileName_1 = outFileName + "1";
    outFileName += "0";
    
    outp_1 = fopen( outFileName_1.c_str(), "w" );
    
    if( nullptr == outp_1 ) {
      cerr << "Error creating file " << outFileName_1 << endl;
      return -1;
    }
  }

  FILE* outp = fopen( outFileName.c_str(), "w" );

  if( nullptr == outp ) {
    cerr << "Error creating file " << outFileName << endl;
    return -1;
  }

  char* linePtr = nullptr;
  size_t lineLen = 0;

  string head( "#define " );
  head += macroPrefix;

  char var[1000];

  const string fmt0 = "%s%s =%s";
  const string fmt1 = ",\n" + fmt0;

  const string fmt0_1 = "{\"%s%s\", %s}"; 
  const string fmt1_1 = ",\n" + fmt0_1;

  set< long int > valSet;

  while( 0 <= getline( &linePtr, &lineLen, inp ) ) {
    static const char* fmt = fmt0.c_str();
    static const char* fmt_1 = fmt0_1.c_str();

    // Remove any trailing newline
    {
      const int lineLen = strlen( linePtr );
      if( '\n' == linePtr[ lineLen - 1 ] ) {
	linePtr[ lineLen - 1 ] = '\0';
      }
    }
    
    //fprintf( stdout, "%s", linePtr );

    if( 0 != strncmp( head.c_str(), linePtr, head.size() ) ) continue;

    //fprintf( stdout, "%s", linePtr + head.size() );

    if( 1 != sscanf( linePtr + head.size(), "%s",  var ) ) {
      cerr << "sscanf error reading macro name" << endl;
      return -1;
    }

    // Writing out the entire tail, in case the value
    // contains white space
    const char* tail = strstr( linePtr + head.size(),
			       var );

    const char* valStr = tail + strlen( var );

    // Check that valStr is not all white space
    {
      char* valStr2 = strdup( valStr );

      if( nullptr == strtok( valStr2, " \t\n" ) ) {
	cerr << "Skipping valueless macro: "
	     << linePtr << endl;
	continue;
      }
      free( valStr2 );
    }
    
    if( 0 > fprintf( outp,
		     fmt,
		     enumPrefix.c_str(),
		     var,
		     valStr ) ) {
      cerr << "meconv: Error writing to "
	   << outFileName
	   << endl;

      return -1;
    }

    if( createTwoFiles ) {
      if( 0 > fprintf( outp_1,
		       fmt_1,
		       enumPrefix.c_str(),
		       var,
		       valStr ) ) {
	cerr << "meconv: Error writing to "
	     << outFileName_1
	     << endl;

	return -1;
      }
    }
 
    fmt = fmt1.c_str();
    fmt_1 = fmt1_1.c_str();
  }

  if( nullptr != linePtr ) free( linePtr );

  fclose( outp );

  // Returns the status of the piped command
  const int retval = pclose( inp );
  cout << "pclose= " << retval << endl;

  // Returning 256 is interpereted as 0 by make
  return ( retval ? -1 : 0 );
}// main

