#ifndef BOOT_ARCH_H
#define BOOT_ARCH_H

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#define OS_IS_LINUX 0
#define OS_IS_WINDOWS 0

#if  defined(linux) || defined(__linux) || defined(__linux__)
  #undef OS_IS_LINUX
  #define OS_IS_LINUX 1
#endif
#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
  #undef OS_IS_WINDOWS
  #define OS_IS_WINDOWS 1
#endif


void sleepms( unsigned int ms );


#  include <ios>

//---------------------------------
#if defined( __MINGW32__ )
 typedef __int64 integer64;

#  include <sys/stat.h>
#  include <io.h>

#  define S_IRWXG S_IRWXU
#  define S_IROTH S_IRUSR
#  define S_IXOTH S_IXUSR

inline
int mkdir( const char* name,
	   mode_t )// mode )
{
  return mkdir( name );
}

namespace std
{
  static const ios::openmode ios_nocreate = ios::openmode(0);
};
#else

//-----------------------------------
#if defined(  __gnu_linux__ )
typedef __int64_t integer64;

#  include <sys/stat.h>
#  include <errno.h>
#  include <unistd.h>

namespace std
{
  static const ios::openmode ios_nocreate = ios::openmode(0);
};

//-----------------------------------
#else

#include <direct.h>
#include <stdio.h>

#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS 1
    #define restrict __restrict
#endif

int64_t getline(char** restrict line, size_t* restrict len, FILE* restrict fp);

inline
char *getcwd(char *buf, size_t size) {
  return _getcwd( buf, size );
}

inline
FILE *popen(
    const char *command,
    const char *mode
	    ) {
  return _popen( command, mode );
}

inline
int pclose(
FILE *stream
	   ) {
  return _pclose( stream );
}

typedef int mode_t;
const mode_t S_IRWXU = 0;
const mode_t S_IRWXG = 0;
//const mode_t S_IROTH = 0;
//const mode_t S_IXOTH = 0;

inline
int mkdir(const char *path, mode_t mode) {
  return _mkdir( path );
}


namespace std
{
  //static const ios::openmode ios_nocreate = ios::nocreate;
  static const ios::openmode ios_nocreate = ios::openmode(0);
}
;

#endif
#endif


#endif
