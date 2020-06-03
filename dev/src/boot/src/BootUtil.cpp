/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "BootUtil.h"
#include <string.h>
#include "BootArch.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std;

TRANSFER
char* GenStrNew( const char* str )
{
  if( str == NULL )
  {
    return NULL;
  }

  const int len = strlen(str) + 1;
  char* cp = new char[ len ];

  memcpy( cp, str, len );

  return cp;
}

TRANSFER
char* GenAbsPath( const char* pathName,
		  const char* workingDirName )
{
  if( nullptr == pathName ) {
    return nullptr;
  }

  boost::filesystem::path pathNamePath( pathName );

  //cout << "GEN pathNamePath= \"" << pathNamePath.string().c_str() << '"' << endl;

  boost::filesystem::path pathNameFile( pathNamePath.filename() );
  boost::filesystem::path pathNameParent( pathNamePath.parent_path() );
  
  // cout << "GEN pathNameParent= \"" << pathNameParent.string().c_str() << '"' << endl;

  unique_ptr< char [] > absPathNameParent
    ( GenAbsDir( pathNameParent.string().c_str(),
		 workingDirName ) );

  //cout << "GEN absPathNameParent= \"" << absPathNameParent << '"' << endl;
 

  if( nullptr == absPathNameParent.get() ) {
    return nullptr;
  }

  ostringstream os;
  os << absPathNameParent.get() << '/' << pathNameFile.string().c_str();

  return GenStrNew( os.str().c_str() );
}// GenAbsPath


TRANSFER char* GenAbsDir( const char* dirName,
			  const char* workingDirName )
{
  if( dirName == NULL )
  {
    return NULL;
  }

  std::unique_ptr< char, free_delete<char> > absStartDirName( getcwd( NULL, 0 ) );

  if( workingDirName != NULL )
  {
    if( 0 != chdir( workingDirName ) )
    {
      perror("GenAbsDir from chdir(workingDir)");
      return NULL;
    }
  }

  if( 0 != chdir( dirName ) )
  {
    perror("GenAbsDir from chdir( dirName )");
    cerr << "dirName= \"" << dirName << '"' << endl;
    return NULL;
  }

  unique_ptr< char, free_delete<char> > absDirName( getcwd( NULL, 0 ) );

  if( 0 != chdir( absStartDirName.get() ) )
  {
    perror("GenAbsDir from chdir(startDir)");
    return NULL;
  }

  for( char* cp = absDirName.get(); *cp != '\0'; ++cp )
  {
    if( *cp == '\\' )
    {
      *cp = '/';
    }
  }

  return GenStrNew( absDirName.get() );
}//GenAbsDir


bool GenWrite( const char* destFileName,
	       const char* srcFileName )
{
  ofstream destFile( destFileName, ios::binary );

  if( !destFile )
  {
    cerr << "GenWrite: Error opening destination file "
	 << destFileName
	 << endl;

    return false;
  }

  return GenWrite( destFile, srcFileName );
}// GenWrite( char*, char* )


bool GenWrite( ostream& destFile,
	       const char* srcFileName )
{
  ifstream srcFile( srcFileName, ios::in | ios_nocreate );

  if( !srcFile )
  {
    cerr << "GenWrite: Error opening to read "
	 << srcFileName
	 << endl;

    return false;
  }

  const int bufSize = 512;
  char buf[ bufSize ];

  while( srcFile.good() )
  {
    srcFile.read( buf, bufSize );
    const int readCount = srcFile.gcount();

    if( !destFile.write( buf, readCount ) )
    {
      cerr << "GenWrite: Error writing "
	   << readCount
	   << " bytes."
	   << endl;

      return false;
    }
  }

  if( !srcFile.eof() )
  {
    cerr << "GenWrite: Error reading from "
	 << srcFileName
	 << endl;

    return false;
  }

  return true;
}// GenWrite( ostream, char* )


