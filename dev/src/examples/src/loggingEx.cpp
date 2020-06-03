/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// Set this macro to Boost or Simple to specify the logging library
// The default is set in config/Makefile.inc
//#define SSRD_LOGGING Boost

// Undefine the macro to remove all logging calls
//#undef SSRD_LOGGING

#include "GenLoggingCLI.hpp"

// Use the --logLevel flag to control which log messages
// get written to the screen.
int main( int argc, char *argv[] )
{
  CLI::App parser{"Example SLOG logging"};
  {
    // Add command line flags, as needed ...

    // Finished specifying all the flags
    CLI11_LOG_PARSE(parser, argc, argv);

    // Post-process the command-line flags ...
  }

  SLOG_TRACE( "A trace message" );
  SLOG_DEBUG( "A debug message" );
  SLOG_INFO( "A info message" );
  SLOG_NOTICE( "A notice message" );
  SLOG_WARN( "A warning message" );
  SLOG_ERROR( "An error message" );
  SLOG_CRITICAL( "A critical message" );
  SLOG_ALERT( "An alert message" );
  SLOG_EMERGENCY( "An emergency message" );

  try {
  SLOG_EXCEPTION( "An exception message" );
  }
  catch( ssrd_logging::Exception& e ) {
    std::cout << "Caught message: "
	      << e.what()
	      << std::endl;
  }

  std::cout << "Leaving main" << std::endl;
  return 0;
}//
