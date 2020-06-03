/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "GenLogging_Boost"
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

using namespace boost::log::trivial;

namespace ssrd_logging {
  namespace Boost {

    severity_level
    ssrdToBoostLevel( ::ssrd_logging::Level sl ) {
      switch( sl ) {
      case  ::ssrd_logging::Level::lev_TRACE: {
	return trace;
	break;
      }
      case ::ssrd_logging::Level::lev_DEBUG: {
	return debug;
	break;
      }
      case::ssrd_logging::Level:: lev_INFO: {
	return info;
      	break;
      }
      case ::ssrd_logging::Level::lev_NOTICE: {
	return info;
	break;
      }
      case ::ssrd_logging::Level::lev_WARN: {
	return warning;
	break;
      }
      case ::ssrd_logging::Level::lev_ERROR: {
	return error;
	break;
      }
      case ::ssrd_logging::Level::lev_CRITICAL: {
	return error;
	break;
      }
      case ::ssrd_logging::Level::lev_ALERT: {
	return error;
	break;
      }
      }

      // Compiler happier with final return statement 
      return fatal; 
    }// ssrdToBoostLevel


    void setLoggingLevel( ::ssrd_logging::Level level ) {
      const severity_level bl = ssrdToBoostLevel( level );
      
      boost::log::core::get()->set_filter( severity >= bl );
    }

  }
}
