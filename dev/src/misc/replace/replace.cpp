/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "GenLoggingCLI.hpp"
#include "BootUtil.h"
#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include <fstream>
#include <assert.h>
#include <cctype>

using namespace std;
namespace bf = boost::filesystem;

namespace {
string footer = R"12(
  The old string, in the file, is replaced by the new string.
By default, an exact match is required. All writing
to disk is done in binary mode (normal for linux), so file 
written to disk will be in \n format.  All reading is done
in text format, so that \r\n on disk is read as \r\n on linux,
but as \n on Windows.
  If --rn is specified, the \r\n combination in the file is
identified with simply \n, for matching purposes, only.  
In this mode, every \r in the file must be followed by a \n.
  If --ws is specified, all consecutive white space is identitied
as a single character, both in the file and in the old string,
for matchging purposes, only.
  Only one of --rn and --ws may be specified.
  Note that, if the old string is empty, the file is simply
prepended by the new string.
)12";

string fileName;   // Name of file on disk
string fileString; // Contents of the disk file
string newString;

bool writeOutFile( int headLength,
		   int tailIndex ) {

  cout << "Matched " << fileName << endl;

  // Truncate file to just before the match
  {
    bf::path fileP( fileName.c_str() );
    bf::resize_file( fileP, headLength );
  }
  
  // Write the new string to the file
  ofstream ofs( fileName.c_str(),
		ios_base::app | ios::binary );
       
  if( !ofs ) {
    cerr << "Error reopening target file" << endl;
    return false;
  }
      
  ofs << newString;

  if( !ofs ) {
    cerr << "Error writing new string to target file" << endl;
    return false;
  }
  
  // Write the remainder of the file to the file.

  ofs << fileString.substr( tailIndex );
      
  if( !ofs ) {
    cerr << "Error writing tail to target file" << endl;
    return false;
  }

  return true;
}// writeOutFile
}

int main( int argc, char *argv[] )
{
  string oldString;
  bool combine_rn = false;
  bool combine_ws = false;
  
  CLI::App parser{"Replace one string with another -- first occurence"};
  {
    parser.footer( footer );
    
    CLI::Option* rnFlag =
    parser.add_flag( "--rn", combine_rn, "Combine \\r\\n to \\n: file only" );
    
    parser.add_flag( "--ws",
		     combine_ws,
		     "Combine white space to a single char: file and old string" )
      ->excludes( rnFlag );
    
    string oldStringFileName;
    string newStringFileName;

    parser.add_option( "FileName",
		       fileName,
		       "File to alter"
		       )
      ->required()->check( CLI::ExistingFile );

    CLI::Option* oldOpt =
      parser.add_option( "--os",
			 oldString,
			 "Old string"
			 )
      ->group( "Old string" );
    
    CLI::Option* newOpt =
      parser.add_option( "--ns",
			 newString,
			 "New string"
			 )
      ->group( "New string" );
   
    CLI::Option* oldFOpt =
      parser.add_option( "--osFile",
			 oldStringFileName,
			 "File consisting of old string"
			 )
      ->excludes( oldOpt )
      ->group( "Old string" );
   
    CLI::Option* newFOpt =
      parser.add_option( "--nsFile",
			 newStringFileName,
			 "File consisting of new string"
			 )
      ->excludes( newOpt )
      ->group( "New string" );
   
    CLI11_LOG_PARSE(parser, argc, argv);
   
    if( !( *oldOpt || *oldFOpt ) ) {
      cerr << "Old string not specified" << endl;
      return -1;
    }
     
    if( !( *newOpt || *newFOpt ) ) {
      cerr << "New string not specified" << endl;
      return -1;
    }

    if( *oldFOpt ) {
      ostringstream str;

      if( !GenWrite( str, oldStringFileName.c_str() ) ) {
	cerr << "Error reading old string from disk" << endl;
	return -1;
      }
      
      oldString = str.str();
    }
     
    if( *newFOpt ) {
      ostringstream str;

      if( !GenWrite( str, newStringFileName.c_str() ) ) {
	cerr << "Error reading new string from disk" << endl;
	return -1;
      }

      newString = str.str();
    }

    // Read the entire file into a string
    {
      ostringstream str;

      if( !GenWrite( str, fileName.c_str() ) ) {
	cerr << "Error reading file into string" << endl;
	return -1;
      }
     
      fileString = str.str();
    }
  }

  // Check for an empty old string.
  // Just means to prepend the new string to the file
  if( oldString.empty() ) {
    if( !writeOutFile( 0, 0 ) ) {
      cerr << "Error with empty old string" << endl;
      return -1;
    }

    return 0;
  }

  const char* const fileStr = fileString.c_str();
  const char* candidate = fileStr;
  const char* fcp = candidate;

  const char* const oldStr = oldString.c_str();
  const char* cp = oldStr; 

  while( '\0' != *candidate ) {
    if( '\0' == *cp ) {
      SLOG_DEBUG( "Match!" );
      
      if( !writeOutFile( candidate - fileStr,
			 fcp - fileStr ) ) {
	cerr << "Error with non-empty old string" << endl;
	return -1;
      }
      
      return 0;
    }

    char c = *fcp++;
    char old_c = *cp;

    if( combine_rn ) {
      if( '\r' == c ) {

	c = *fcp;
	
	if( '\0' == c ) {
	  cerr << "No char follows \\r" << endl;
	  return -1;
	}
	
	++fcp;

	if( '\n' != c ) {
	  cerr << "\\r not followed by \\n" << endl;
	  return -1;
	}
      }
    }
    
    if( combine_ws ) {
      if( isspace( c ) ) {
	c = ' ';
	while( isspace( *fcp ) ) {
	  ++fcp;
	}
      }
      if( isspace( old_c) ) {
	while( isspace( cp[1] ) ) {
	    ++cp;
	}
	old_c = ' ';
      }
    }

    SLOG_DEBUG( "c, old_c=" << "'" << c << "', '" << old_c << "'" );

    if( old_c == c ) {
      ++cp;
    }
    else {
      cp = oldStr;
      ++candidate;
      fcp = candidate;
    }
  }

  SLOG_DEBUG( fileName << ": No match" );
   
  return 0;
}// main
