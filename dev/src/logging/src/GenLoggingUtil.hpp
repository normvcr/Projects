#ifndef GenLoggingUtil_HPP
#define GenLoggingUtil_HPP
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <exception>
#include <string>

namespace ssrd_logging {

  // These are syslog strings, plus TRACE.
  // The enum values are not the same as the syslog values.
  enum class Level
    { // Values must be sequential upwards from 0
     lev_TRACE = 0,
     lev_DEBUG,
     lev_INFO,
     lev_NOTICE,
     lev_WARN,        // Potentially problematic
     lev_ERROR,       // Problematic
     lev_CRITICAL,    // Requires action, at some time
     lev_ALERT,       // Requires immediate action
     lev_EMERGENCY    // System unusable
    };

  const char* getLoggingString( Level level );

  // Clamps at the low and high ends
  const char* getLoggingString( int level );
  Level intToSsrdLevel( int level );

  class Exception : public std::exception {
  public:
    Exception( const char* msg ) :
      msg_( msg )
    {}

    const char* what( void ) {
      return msg_.c_str();
    }
    
  private:
    const std::string msg_;
  };
    
}

#endif
