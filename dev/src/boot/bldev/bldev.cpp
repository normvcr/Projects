/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <iostream>
#include <sstream>
#include <fstream>
//#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <CLI/CLI.hpp>
#include "bldev_Info.h"
#include "BootArch.h"
#include <iostream>
#include <memory>

namespace {

using namespace std;

// destDir and topSourceDir correspond in the two hierarchies.
// 
bool setupDir( const char* destDirName,
	       const char* topSourceDirName );

string footer
(
" Does two things in DestDir(and subdirs, if recursing):\n"
" -- Creates/updates defs.inc\n"
" -- Assembles a Makefile, if a Makefile.in is present.\n"
" defs.inc has the two lines:\n"
"     SOURCE_DIR=<full path source directory>\n"
"     CONFIG_PATH=<full path configuration file>\n"
" Ignoring a bad defs.inc is relevant only at the top dir.\n"
" DestDir defaults to the execution directory.\n"
" If configFile is not specified, it is taken from\n"
" defs.inc in DestDir or in the execution directory.\n"
" If SrcDir is not specified, it is set to\n"
" SOURCE_DIR from DestDir/defs.inc, if it exists.\n"
" If this file does not exist, SrcDir is set to DestDir.\n"
" SrcDir is relative to SOURCE_DIR in execDir/defs.inc,\n"
" if it exists, otherwise, SrcDir is relative to\n"
" execDir.\n"
" During recursion, at all directories except the top,\n"
" DestDir, SrcDir and ConfigDir are always specified,\n"
" so the rules for defaulting are relevant only at\n"
" the first (top) directory being configured.\n"
" A Makefile is assembled from these parts:\n"
" SOURCE_DIR := <full path to source folder>\n"
" Contents of Makefile.cfg\n"
" Inclusion of Platform.inc (from the same folder as Makefile.cfg)\n"
" Contents of Makefile.in (from the source folder)\n"
" Inclusion of Makefile.inc (from the folder above Makefile.cfg)\n"
 );

const string message0( "Run  bldev -h  for help" );

string message( const CLI::App *, const CLI::Error &e) {
  ostringstream s;
  s <<  "Error: " << e.get_name() << endl;

  s << message0 << endl;

  return s.str();
}// message



// These two variables are constant during the recursive processing.
static bool doRecursion = true;
static unique_ptr< CFG_Info > info( new CFG_Info );

}// anonymous
 
int main( int argc, char* argv[] )
{
  string configPathName;
  string incFilePathName;
  bool ignoreBadDefs =  false;
  string destDirName( "." );
  string srcDirName;


  CLI::App parser{"Create a build tree"};

  CLI::Option* my_flag = parser.add_flag( "--nr", "Don't do recursion" );

  parser.add_option( "--cfg", configPathName, "Configuration file." )
    ->type_name("confiFile");

  
  parser.add_flag( "--ign", ignoreBadDefs, "Ignore bad defs.inc." );

  parser.add_option( "destDir",
		     destDirName,
		     "Must be specified if srcDir is specified",
		     true );


  
  parser.add_option( "srcDir",
		     srcDirName,
		     "Can only be specified if destDir is specified" );

  parser.footer( footer );
  parser.failure_message( message );
  
  CLI11_PARSE(parser, argc, argv);
  doRecursion = !*my_flag;

  // All parameters used up?
  

  //---------- Read defs.inc, if needed, from destDir --------

  CFG_Defs defs; // May not be used.

  if( ( srcDirName.empty() ) || ( configPathName.empty() ) )
  {
    switch( defs.restore( destDirName.c_str() ) )
    {
      case GenStatusFailure:
      {
	if( srcDirName.empty() )
	{
	  srcDirName = destDirName;
	}

	break;
      }

      case GenStatusSuccess:
      {
	if( srcDirName.empty() )
	{
	  srcDirName = defs.sourceDirName_;
	}
	
	if( configPathName.empty() )
	{
	  configPathName = defs.configPathName_;
	}

	break;
      }

      default:
      {
	cerr << "bldev: Error defaulting to restore defs.restore(destDir)"
	     << endl;

	if( !ignoreBadDefs )
	{
	  cerr << message0 << endl;
	  return -1;
	}

	break;
      }
    }
  }

  //---------- Read defs.inc from execDir  --------

  if( (0 != mkdir( destDirName.c_str(), 
		   S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH )) 
    && (errno != EEXIST) )
  {
    perror( "bldev: Error from mkdir" );
    return -1;
  }

  unique_ptr< char[] > destAbsDirName( GenAbsDir( destDirName.c_str() ) );
  unique_ptr< char[] > srcAbsDirName;

  switch( defs.restore() )
  {
    case GenStatusFailure:
    {
      break;
    }

    case GenStatusSuccess:
    {
      srcAbsDirName.reset
	( GenAbsDir( srcDirName.c_str(), defs.sourceDirName_ ) );

      if( srcAbsDirName == NULL )
      {
	cerr << "bldev: Source subdir does not exist:\n"
	     << defs.sourceDirName_
	     << '/'
	     << srcDirName
	     << endl;
	return -1;
      }

      if( configPathName.empty() )
      {
	configPathName = defs.configPathName_;
      }

      break;
    }

    default:
    {
      cerr << "bldev: Error from defs.restore()"
	   << endl;

      if( !ignoreBadDefs )
      {
	cerr << message0 << endl;
	return -1;
      }

      break;
    }
  }

  if( srcAbsDirName == NULL )
  {
    srcAbsDirName.reset( GenAbsDir( srcDirName.c_str() ) );

    if( srcAbsDirName == NULL )
    {
      cerr << "bldev: Source dir does not exist:\n\""
	   << srcDirName << '"'
	   << endl;
      return -1;
    }
  }

  if( configPathName.empty() )
  {
    cerr << "bldev: configPath not known"
	 << endl;

    return -1;
  }

  //--------- Process the current directory ----------
  if( !info->setup( configPathName.c_str() ) )
  {
    cerr << "bldev: Error from CFG_Info:;setup with "
	 << configPathName
	 << endl;

    return -1;
  }

  if( !setupDir( destAbsDirName.get(), 
		 srcAbsDirName.get() ) )
  {
    cerr << "bldev: Error setting up top level dir "
	 << destDirName
	 << endl;

    return -1;
  }

  return 0;
}// main

namespace {

bool setupDir( const char* destDirName,
	       const char* topSourceDirName )
{
  using namespace std;
  // Handle the current directory.

  cout << "Building "
       << destDirName
       << endl;

  unique_ptr< char[] > topDestDirName( getcwd( NULL, 0 ) );

  if( (0 != mkdir( destDirName, 
		   S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH )) 
    && (errno != EEXIST) )
  {
    perror( "setupDir: Error from mkdir" );
    return false;
  }

  if( 0 != chdir( destDirName ) )
  {
    perror( "setupDir: Error from chdir" );
    cerr << "\nfor destDirName= "
	 << destDirName
	 << endl;
    return false;
  }

  // Write defs.inc
  //
  if( !CFG_Defs::archive( topSourceDirName,
			  info->configFileName_ ) )
  {
    cerr << "setupDir: Error from CFG_Defs::archive"
	 << endl;

    return false;
  }

  // Generate Makefile, if Makefile.in is present.
  //
  unique_ptr< char[] > makefileInName;
  {
    ostringstream name;
    name << topSourceDirName
	 << '/'
	 << "Makefile.in"
	 << ends;
    makefileInName.reset( GenStrNew( name.str().data() ) );
  }

  struct stat BUF;
  if( ( 0 == stat( makefileInName.get(), &BUF ) ) 
      && S_ISREG( BUF.st_mode ) ) {
    
    // Makefile.in exists, so process it.

    static const char makefileName[] = "Makefile";
    ofstream makefile( makefileName );

    makefile << "SOURCE_DIR := "
	     << topSourceDirName
	     << endl;

    if( !makefile )
      {
	cerr << "setupdir: Error writing SOURCE_DIR into Makefile"
	     << endl;
	return false;
      }

    if( !GenWrite( makefile, info->configFileName_ ) )
      {
	cerr << "setupdir: Error appending Makefile.cfg\n"
	     << endl;
	return false;
      }
    
    makefile << "\ninclude " <<  info->platFileName_ << endl;

    if( !GenWrite( makefile, makefileInName.get() ) )
      {
	cerr << "setupdir: Error appending Makefile.in\n"
	     << endl;
	return false;
      }
    
    makefile << "\ninclude " <<  info->incFileName_ << endl;
    
    if( !makefile )
      {
	cerr << "setupdir: Error writing Makefile"
	     << endl;
	return false;
      }
  }// Handling Makefile

  if( doRecursion )
  {
    DIR* topSourceDir = opendir( topSourceDirName );

    if( topSourceDir == NULL )
    {
      perror( "bldev: from opendir" );
      return false;
    }

    struct dirent* ep = NULL;

    while( NULL != (ep = readdir( topSourceDir ) ) )
    {
      // Skip all entries beginning with a period.
      if( ep->d_name[0] == '.' )
      {
	continue;
      }

      // Skip all entries beginning with "CVS".
      if( 0 == strncmp( ep->d_name, "CVS", 3 ) )
      {
	continue;
      }

      unique_ptr< char[] > sourceDirName;
      {
	ostringstream name;
	name << topSourceDirName
	     << '/'
	     << ep->d_name
	     << ends;
      
	sourceDirName.reset( GenStrNew( name.str().data() ) );
      }

      struct stat BUF;
      if( 0 != stat( sourceDirName.get(), &BUF ) )
      {
	perror( "bldev: from stat for source dir name" );
	cout << "sourceDirName= " << sourceDirName.get() << endl;
	return false;
      }

      if( !S_ISDIR( BUF.st_mode ) )
      {
	continue;
      }

      if( !setupDir( ep->d_name,
		     sourceDirName.get() ) )
      {
	cerr << "setupDir: Error at sourceDirName"
	     << endl;
	return false;
      }
    }
  
    if( 0 != closedir( topSourceDir ) )
    {
      perror( "bldev: from closedir" );
      return false;
    }
  }

  if( 0 != chdir( topDestDirName.get() ) )
  {
    perror( "setupDir: Error from chdir to top dest dir" );
    return false;
  }

  return true;
}// setupDir

}// anonymous
