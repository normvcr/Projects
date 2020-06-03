/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "GenLoggingUtil.hpp"

namespace ssrd_logging {

  namespace {
  // Not using iog_ code since logging is used in the
  // iogen utility, itself.
  const char* loggingStrings[] =
    { "TRACE", "DEBUG", "INFO", "NOTICE", "WARN",
      "ERROR", "CRITICAL", "ALERT", "EMERGENCY" };
  }
				    
    // The integer versions are used by the logging macros,
    // and should not otherwise be used.

    const char* getLoggingString( int level ) {
      if( (int) Level::lev_TRACE > level ) return "TRACE";
      if( (int) Level::lev_EMERGENCY < level ) return "EMERGENCY";
      
      return loggingStrings[ level ];
    }

    const char* getLoggingString( ::ssrd_logging::Level level ) {
      return getLoggingString( (int) level );
    }

  Level intToSsrdLevel( int level ) {
      if( (int) Level::lev_TRACE > level )
	return  Level::lev_TRACE;
      
      if( (int) Level::lev_EMERGENCY < level )
	return Level::lev_EMERGENCY;
      
      return (Level) level;
  }// intToSsrdLevel
}
