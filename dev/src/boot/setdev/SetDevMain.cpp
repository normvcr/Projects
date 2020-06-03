/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "BootArch.h"
#include "bldev_Info.h"
#include <boost/filesystem.hpp>
#include <CLI/CLI.hpp>
#include <string>
#include <algorithm> // Replace characters in string
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main( int argc, char *argv[] )
{
  string tagName;
  string devTop;
  string bgName( "FFFFFF" );
  string fgName( "000000" );

  bool overwriteFlag = false;
  bool noExecuteFlag = false;

  CLI::App parser{"Create script to initialize development"};
  {
    CLI::Option* devTopOpt =
    parser.add_option( "-d,--devTop",
		       devTop,
		       "Name of top development directory"
		       );

    parser.add_option( "--bg",
		       bgName,
		       "Colour of the command window background",
		       true )
      ->needs( devTopOpt )->group("Colours");
    
    parser.add_option( "--fg",
		       fgName,
		       "Color of the command window foreground",
		       true )
      ->needs( devTopOpt )->group("Colours");
    
    parser.add_option( "tag",
		       tagName,
		       "Short name for the development environment"
		       )
      ->required();

    parser.add_flag( "-f, --force",
		     overwriteFlag,
		     "OK to overwrite script if it exists" );

    parser.add_flag( "-n, --noexec",
		     noExecuteFlag,
		     "Don't execute script after creation" );


    parser.footer(
R"(
The PLATBIN environment variable must be defined.
It is a folder to hold the initialization scripts.
The PLATBIN folder should be in the PATH variable.

The DEVTOP environmnet variable must NOT be defined.
Development scripts/environments must be created/invoked from a
non-development environment.

If devTop or colour is specified, the corresponding script is 
assumed not to exist, and it is created:
     $PLATBIN/setdev_tag
This script is executable, and creates a command window where:
-- DEVTOP is defined
-- $DEVTOP is in the PATH variable

If devTop is not specified, the script is assumed to exist,
and is invoked.  The initialization scripts may also be invoked 
directly from the command line, without using this setdev utility.

Each colour is specified as RRGGBB = 6 hex digits 
)" );

    CLI11_PARSE(parser, argc, argv);
  }

  const char* platbin = getenv( "PLATBIN" );

  if( nullptr == platbin ) {
    cerr << "PLATBIN is not defined in the environment"
	 << endl;

    return -1;
  }
  
  if( nullptr != getenv( "DEVTOP" ) ) {
    cerr << "DEVTOP is already defined in the environment"
	 << endl;

    return -4;
  }

#if OS_IS_LINUX
  const string extName;
#else
  const string extName( ".bat" );
#endif

  const string scriptName = platbin + ( "/setdev_" + tagName + extName );

  cout << "Working with script: " << scriptName << endl;

  namespace bf = boost::filesystem;

  bf::path scriptP( scriptName );

  // If the script exists, invoke it.
  if( bf::exists( scriptP ) &&
      !( overwriteFlag && (0 < parser.count( "-d" ) ) ) ){
    cout << "Invoking development environment" << endl;

    if( (0 < parser.count( "--fg" )) ||
	(0 < parser.count( "--bg" )) ||
	(0 < parser.count( "-d" )) ) {
      cerr << "Cannot invoke with --fg, --bg or -d flags on command line"
	   << endl;

      return -5;
    }

    int retval = system( scriptName.c_str() );

    cout << "retval= " << retval<< endl;
  }
  else {
    // devTop is a required command line parameter,
    // and should not be defined in the current environment.

    if( 0 == parser.count( "-d" ) ) {
      cerr << "Need to specify -d devTop" << endl;
      return -3;
    }

    FILE* ofs = fopen( scriptName.c_str(), "w" );

    if( nullptr == ofs ) {
      cerr << "Error creating script file" << endl;
      return -2;
    }

    // Convert devTop to an absolute file name
    bf::path devTopAbs = bf::canonical( devTop );

    std::string devTopAbsStr( devTopAbs.string() );

    cout << "DEVTOP()= " << devTopAbsStr << endl;

    CFG_Defs defs;

    if( GenStatusSuccess != defs.restore( devTop.c_str() ) ) {
      cerr << "Error restoring defs.inc in "
	   << devTop
	   << endl;
      
      return -3;
    }

    // The defs.inc has been succesfully restored.
    // In particular, the devTop folder exists

    bf::path configFileP( defs.configPathName_ );
    bf::path projInitFileP( configFileP.parent_path().string() +
			    "/projectInit_" + tagName + extName );
    bf::path platInitFileP( configFileP.parent_path().string() +
			    "/platformInit" + extName );

#if OS_IS_LINUX

    const char* contents =
      R"(#!/bin/sh

if [ ! -z $DEVTOP ]
then
   echo Environment variable DEVTOP already exists.  Exiting ...
   exit
fi

export DEVTOP=%s
export DEVINIT=%s
export PLATINIT=%s

export PATH=$DEVTOP/bin:$PATH
cd $DEVTOP

if [ -f $DEVINIT ]; then
  . $DEVINIT
else
  echo No project initialization file

  if [ -f $PLATINIT ]; then
    . $PLATINIT
  else
    echo No platfrom initialization file
  fi
fi

unset DEVINIT
unset PLATINIT

xterm -xrm 'XTerm.vt100.allowTitleOps: false' -T %s -bg \#%s -fg \#%s &
)";

    fprintf( ofs, contents,
	     devTopAbsStr.c_str(),
	     projInitFileP.string().c_str(),
	     platInitFileP.string().c_str(),
	     tagName.c_str(),
	     bgName.c_str(),
	     fgName.c_str() );
    
#else // Windows ----------------
    std::replace( devTopAbsStr.begin(),
		  devTopAbsStr.end(),
		  '\\',
		  '/' );

    cout << "DEVTOP(/)= " << devTopAbsStr << endl;
    
    string bgR( bgName.substr(0, 2) );
    string bgG( bgName.substr(2, 2) );
    string bgB( bgName.substr(4, 2) );

    string fgR( fgName.substr(0, 2) );
    string fgG( fgName.substr(2, 2) );
    string fgB( fgName.substr(4, 2) );

    const char* contents =
      R"(@echo off
if defined DEVTOP (
echo Environment variable DEVTOP already exists.  Exiting ...
goto:eof
)
set DEVTOP=%s
set DEVINIT=%s
set PLATINIT=%s

@set oldPATH=%%PATH%%
@set PATH=%%DEVTOP%%/bin;%%PATH%%

if exist %%DEVINIT%% (
  call %%DEVINIT%%
) else (
  echo No project initialization file

  if exist %%PLATINIT%% (
    call %%PLATINIT%%
  ) else (
    echo No platform initialization file
  )
)
@set PLATINIT=
@set DEVINIT=

start "%s" /D "%%DEVTOP%%" cmd /t:01 /k "setpalette %s %s %s %s %s %s"

@REM Don't change the callers environment
@set PATH=%%oldPATH%%
@set DEVTOP=
:eof
)";
    fprintf( ofs, contents,
	     devTopAbsStr.c_str(),
	     projInitFileP.string().c_str(),
	     platInitFileP.string().c_str(),
	     tagName.c_str(),
	     bgR.c_str(),
	     bgG.c_str(),
	     bgB.c_str(),
	     fgR.c_str(),
	     fgG.c_str(),
	     fgB.c_str()
	     );

#endif

    fclose( ofs );

    bf::permissions
      ( scriptP,
	bf::perms::owner_all |
	bf::perms::group_exe |
	bf::perms::others_exe );

    if( !noExecuteFlag ) {
      int retval = system( scriptName.c_str() );
      
      cout << "system() retval= " << retval<< endl;
    }
  }


  return 0;
}// main

