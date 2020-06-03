#ifndef GenLogging_H
#define GenLogging_H
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "GenLoggingUtil.hpp"


#if defined(SSRD_LOGGING)

#define SSRD_A GenLogging_
#define SSRD_B BOOST_PP_CAT(SSRD_A,SSRD_LOGGING)
#define SSRD_LOGGING_Include BOOST_PP_STRINGIZE(SSRD_B)

// Includes GenLogging_TAG
// where TAG = SSRD_LOGGING
// Omitting header file extension, because could not
// get preprocessor to accept it
#include SSRD_LOGGING_Include

#undef SSRD_LOGGING_Include
#undef SSRD_B
#undef SSRD_A

#else
// No logger is specified

// The macro interface
#define SLOG_TRACE( expr );
#define SLOG_DEBUG( expr );
#define SLOG_INFO( expr );
#define SLOG_NOTICE( expr );
#define SLOG_WARN( expr );
#define SLOG_ERROR( expr );
#define SLOG_CRITICAL( expr );
#define SLOG_ALERT( expr );
#define SLOG_EMERGENCY( expr );

//Posts EXCEPTION log msg, then throws exception
#define SLOG_EXCEPTION( expr );
 
#define SLOG_SET_LEVEL( level );


#endif


#endif
