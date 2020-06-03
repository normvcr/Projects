/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include "bldev_Info.h"
#include "BootArch.h"
#include "BootParse.h"

using namespace std;


//---------- CFG_Info

const char CFG_Info::incFileBaseName_[] = "Makefile.inc";
const char CFG_Info::platFileBaseName_[] = "Platform.inc";

bool CFG_Info::setup(const char* pathName )
{
  configFileName_ = GenAbsPath( pathName );

  if( configFileName_ == NULL )
  {
    cerr << "CFG_Info::setup: Error from GenAbsPath"
	 << endl;
    return false;
  }

  //cout << "configFileName_= \"" << configFileName_ << '"' << endl;

  boost::filesystem::path pathNamePath( configFileName_ );

  //cout << "pathNamePath= \"" << pathNamePath.string().c_str() << '"' << endl;

  boost::filesystem::path pathNameP(  pathNamePath.parent_path() );
  
  //cout << "pathNamePathP= \"" << pathNameP.string().c_str() << '"' << endl;

  boost::filesystem::path pathNameParent( pathNameP.parent_path() );
  
  //cout << "pathNamePathParent= \"" << pathNameParent.string().c_str() << '"' << endl;

  // Platform.inc
  {
    ostringstream name;
    name << pathNameP.string().c_str()
	 << '/'
	 << platFileBaseName_;
  
    platFileName_ = GenStrNew( name.str().c_str() );
  }

  // Makefile.inc
  {
    ostringstream name;
    name << pathNameParent.string().c_str()
	 << '/'
	 << incFileBaseName_;

    incFileName_ = GenStrNew( name.str().c_str() );
  }

  // Sanity check

  ifstream ifs( configFileName_, ios::in | ios_nocreate );

  if( !ifs )
  {
    cerr << "Info::setup: Config file does not exist:\n"
	 << configFileName_
	 << endl;

    return false;
  }

  return true;
}// CFG_Info::setup


static const char source_dir[] = "SOURCE_DIR";
static const char config_path[] = "CONFIG_PATH";
static const char equals[] = "=";

//--------- CFG_Defs

const char CFG_Defs::fileBaseName_[] = "defs.inc";

GenStatus CFG_Defs::restore( const char* dirName )
{
  if( dirName == NULL )
  {
    dirName = ".";
  }

  ostringstream fileName;
  fileName << dirName
	   << '/'
	   << fileBaseName_;

  ifstream ifs( fileName.str().data(), ios::in | ios_nocreate );

  if( !ifs )
  {
    return GenStatusFailure;
  }

  ifs >> source_dir >> equals;
  sourceDirName_ = GEN_readString( ifs );

  ifs >> config_path >> equals;
  configPathName_ = GEN_readString( ifs );

  if( !ifs )
  {
    return GenStatusError;
  }

  return GenStatusSuccess;
}//  CFG_Defs::restore


bool CFG_Defs::archive( const char* srcDirName,
			const char* configPathName )
{
  ofstream ofs( CFG_Defs::fileBaseName_ );

  ofs << source_dir << ' '
      << equals //<< ' ' // No leading space in name
      << srcDirName
      << endl;

  ofs << config_path << ' '
      << equals //<< ' ' // No leading space in name
      << configPathName
      << endl;

  if( !ofs )
  {
    return false;
  }

  return true;
}// CFG_Defs::archive


