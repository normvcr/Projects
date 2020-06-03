/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
  Atomic "copy" methods for various data types.
 */

#include <stdint.h>
#include "GenArchiveIO.h"
#include "GEN_IoUtil.h"
#include "BootParse.h"
#include "GenArchiveWriteDirector.h"
#include "GenArchiveReadDirector.h"
#include "GenLogging.h"

namespace GenArchive 
{

#define T int
#include "GenArchiveCopy.inc"
#undef T

#define T short int
#include "GenArchiveCopy.inc"
#undef T

#define T long int
#include "GenArchiveCopy.inc"
#undef T

#define T unsigned int
#include "GenArchiveCopy.inc"
#undef T

#define T uintptr_t
#include "GenArchiveCopy.inc"
#undef T

#define T float
#include "GenArchiveCopy.inc"
#undef T

#define T double
#include "GenArchiveCopy.inc"
#undef T

  //----------- char --------------------
  static const char quote = '\'';

  bool copy( WriteDirector& wd, const char& val )
  {
    std::ostream& os = wd.os_;

    os << quote;

    switch( val )
    {
      case '\n':
      {
	os << "\\n";
	break;
      }

      case '\\':
      case '\'':
      {
	os << '\\' << val;
	break;
      }

      default:
	os << val;
    }

    os << quote;

    return !wd.fail();
  }

  bool copy( ReadDirector& rd, char& val )
  {
    std::istream& is = rd.is_;

    is >> skipComments >> quote;

    if( !is )
    {
      SLOG_ALERT( "copy(ReadDirector&,char&): Error getting first quote" );
      return false;
    }

    if( !is.get( val ) )
    {
      SLOG_ALERT( "copy(ReadDirector&,char&): Error reading value" );
      return false;
    }

    if( '\'' == val )
    {
      SLOG_ALERT( "copy(ReadDirector&,char&): Empty char" );
      rd.setFail();
      return false;
    }

    if( '\\' == val )
    {
      if( !is.get( val ) )
      {
	SLOG_ALERT( "copy(ReadDirector&,char&): 2 Error reading value" );
	return false;
      }

      switch( val )
      {
	case 'n':
	{
	  val = '\n';
	  break;
	}

	// Literal
	case '\\':
	case '\'':
	{
	  break;
	}

	default:
	  SLOG_ALERT( "copy(ReadDirector&,char&): Bad char escape: <" 
		      << val << '>' );
	  rd.setFail();
	  return false;
      }
    }

    if( !get( is, quote ) )
    {
      SLOG_ALERT( "copy(ReadDirector&,char&): Error closing quote" );
      rd.setFail();
      return false;
    }

    return !rd.fail();
  }

  //-------------------------------------

  // These two interfaces were templatized for overloading reasons.
#if 0
  bool copy( WriteDirector& wd, const char* val ) 
  {
    wd.os_ << val;  
    return !wd.os_.fail(); 
  }

  bool copy( ReadDirector& rd, const char* var ) 
  {
    rd.is_ >> skipComments >> var; 
    return !rd.is_.fail(); 
  }
#endif

  bool copy( WriteDirector& wd, const std::string& var )
  {
    std::ostream& os = wd.os_;

    os << stringDelim_;
    const int len = var.length();

    for( int ind = 0; ind < len; ++ind )
    {
      const char c = var[ ind ];

      switch( c )
      {
	case '\n':
	{
	  os << "\\n";
	  break;
	}

	case stringDelim_:
	case stringEscape_:
	{
	  os << stringEscape_;
	  // fall through to default
	}
	default:
	{
	  os << c;
	}
      }
    }

    os << stringDelim_;

    return !wd.fail();
  }//write(string)

  bool copy( ReadDirector& rd, std::string& var )
  {
    std::istream& is = rd.is_;

    var.clear();

    is >> skipComments >> stringDelim_;

    char c = '\0';
    while( 1 )
    {
      if( !is.get( c ) )
      {
	return false;
      }

      if( stringDelim_ == c )
      {
	return true;
      }

      if( stringEscape_ == c )
      {
	if( !is.get( c ) )
	{
	  return false;
	}

	switch( c )
	{
	  case 'n':
	  {
	    var += '\n';
	    continue;
	  }

	  // Literal
	  case '\\':
	  case '\"':
	  {
	    break;
	  }

	  default:
	    SLOG_ALERT( "copy(ReadDirector&,string&): Bad char escape: <" 
			<< c << '>' );
	    rd.setFail();
	    return false;
	}
      }

      var += c;
    }

    return !rd.fail();
  }// read(string)
}
