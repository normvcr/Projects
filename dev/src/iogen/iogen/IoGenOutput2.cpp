/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "IoGenOutput.h"
#include <memory>
#include <string>
#include <string.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include "GenLogging.h"



GEN_IoOutput2::GEN_IoOutput2( bool doOps,
			      const std::string& outFileBaseName ) :
  GEN_IoOutput( doOps ),
  outFileBaseName_( outFileBaseName )
{
  using namespace std;
  
  // Do the H file
  string outHFileName( outFileBaseName );
  {
    outHFileName += ".h";
    ofh_.open( outHFileName );

    if( !ofh_ )
    {
      SLOG_EXCEPTION
	( "IoOutput::initialize: Error opening ouput h file \""
	  << outHFileName << "'" );
    }
  }
  
  // Do the CPP file
  {
    string outCxxFileName( outFileBaseName );
    outCxxFileName += ".cpp";
    ofc_.open( outCxxFileName );
    
    if( !ofc_ )
    {
      SLOG_EXCEPTION
	( "IoOutput2::initialize: Error opening ouput cpp file \""
	  << outCxxFileName << "'" );
    }
    
    ofc_<< "#include \""
	<< outHFileName
	<< '\"' << endl;
    
    if( !ofc_ )
    {
      SLOG_EXCEPTION
	( "IoOutput2::initialize: Error writing inclusion of output"
	  );
    }
  }
}// GEN_IoOutput2

bool GEN_IoOutput2::initialize( const char* inFileName,
				const char* )
{
  using namespace std;

  boost::filesystem::path inFilePath( inFileName );

  // Do the H file
  {
    const char* slash = strrchr( outFileBaseName_.c_str(), '/' );
    if( nullptr == slash ) {
      slash = outFileBaseName_.c_str();
    }
    else {
      ++slash;
    }
    
    string inclGuardName( slash );
    inclGuardName += "_H";

    ofh_ << "#ifndef "
	 << inclGuardName
	 << endl;

    ofh_ << "#define "
	 << inclGuardName
	 << endl
	 << endl;

    ofh_ << "#include <iosfwd>"
	 << "\n#include \"GenArchiveIO.h\""
	 << endl;

    ofh_ << "#include \"GenArchiveWriteDirector.h\""
	 << "\n#include \"GenArchiveReadDirector.h\""
	 << endl;

    if( !forwardDeclare_ )
    {
      ofh_<< "#include \"";

      if( 0 < headerPrefix_.size() )
      {
	ofh_ << headerPrefix_ << '/';
      }

      ofh_  << inFilePath.filename().string().c_str()
	    << "\"" << endl;
    }

    if( !ofh_ )
    {
      SLOG_ALERT( "Error writing to output h file " );

      return false;
    }
  }// H file

  // Do the CPP file
  {
    ofc_ << "#include <iostream>" 
	   << endl;

    ofc_<< "#include \""
	<< inFilePath.filename().string()  // Otherwise includes quotes
	<< "\"" << endl;

    ofc_ << "#include \"GenArchiveIO.tpp\""
	 << endl;

    ofc_ << "#include \"GEN_Enum.h\""
	 << endl;

   if( !ofc_ )
   {
     SLOG_ALERT( "Error writing to output cpp file " );
     
     return false;
   }
  }// CPP file

  return true;
}// initialize


bool GEN_IoOutput2::finalize( void )
{
  using namespace std;

  // Complete the include guard
  ofh_ << "\n#endif" 
       << endl;

  ofh_.close();

  if( !ofh_ )
  {
    SLOG_ALERT( "GEN_IoOutput2::finalize: Error closing H file" );

    return false;
  }

  ofc_.close();

  if( !ofc_ )
  {
    SLOG_ALERT( "GEN_IoOutput2::finalize: Error closing CPP file" );

    return false;
  }

  return true;
}// finalize


bool GEN_IoOutput2::initializeRecord( const char* tagName,
				      const char* recordName )
{
  using namespace std;
#if 1
  cout << "Initializing record: " 
	       << tagName << ' ' 
       << recordName << endl;
#endif

  // Write to header file

  if( !forwardDeclare( tagName, recordName ) )
  {
    SLOG_ALERT( "Forward declaring " 
		<< tagName << ' ' << recordName );

    return false;
  }
  
  ofh_ << "\nnamespace GenArchive\n{" << endl;

  ofh_ << "extern template\n " 
       << "bool write( std::ostream& os, const "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool read( std::istream& is, "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n " 
       << "bool write( const char* fileName, const "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool read( const char* fileName, "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "\ntemplate< class RWD >\n "
       << "bool copy( RWD& rwd, "
       << "typename RWD::template IoRefrType< " 
       << recordName
       << " > var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool copy( WriteDirector& wd, const "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool copy( ReadDirector& rd, "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "}" << endl; // Close off GenArchive namespace

  if( doOps_ )
  {
    ofh_ << "\n"
	 << "std::ostream& operator<<( std::ostream& os, const "
	 << recordName
	 << "& var );"
	 << endl;
      
    ofh_ << "std::istream& operator>>( std::istream& is, "
	 << recordName
	 << "& var );"
	 << endl;
  }

  if( !ofh_ )
  {
    SLOG_ALERT( "initializeRecord: Error writing to header file" );

    return false;
  }

  // Write to source file

  ofc_
    << "\nusing namespace std;\n" 
    << "\nnamespace GenArchive\n{" << endl
    << "template< class RWD >\n"
    << "bool copy( RWD& rwd, "
    << "typename RWD::template IoRefrType< " 
    << recordName
    << " > var )\n"
    << '{' << endl;

  ofc_  
    << "  readWrite( rwd, \"{\" );\n"
    << "  writeOnly( rwd, \"\\n\" );\n"
    << endl;
 
  if( !ofc_ )
  {
    SLOG_ALERT( "initializeRecord: Error writing to source file" );

    return false;
  }
  
  return true;
}// initializeRecord

bool GEN_IoOutput2::finalizeRecord( const char* recordName )
{
  using namespace std;

  ofc_ 
    << "  rwd.indent();\n"
    << endl;

  ofc_ 
    << "  return\n"
    << "  readWrite( rwd, \"}\" )"
    << endl;

  ofc_ 
    << "  && !rwd.sss().fail();\n"
    << "} // copy( "
    << recordName
    << " )"
    << endl;

  ofc_ 
    << "\ntemplate bool copy( WriteDirector& wd, const "
    << recordName
    << "& var );"
    << endl;

  ofc_ 
    << "template bool copy( ReadDirector& rd, "
    << recordName
    << "& var );"
    << endl;

  ofc_ 
    << "\ntemplate bool write( ostream& os, const "
    << recordName
    << "& var );"
    << endl;

  ofc_ 
    << "template bool read( istream& is, "
    << recordName
    << "& var );"
    << endl;

  ofc_
    << "template bool write( const char* fileName, const "
    << recordName
    << "& var );"
    << endl;

  ofc_
    << "template bool read( const char* fileName, "
    << recordName
    << "& var );"
    << endl;

  ofc_
    << "}" << endl;  // Close namespace GenArchive

  if( doOps_ )
  {
    ofc_ 
      << "\nostream& operator<<( ostream& os, const "
      << recordName
      << "& var )\n"
      << "{\n"
      << "  if( !GenArchive::write( os, var ) )\n"
      << "  {\n"
      << "    os.setstate( ios::failbit );\n"
      << "  }\n"
      << "  return os;\n"
      << "}// operator<<("
      << recordName
      << ")"
      << endl;

    ofc_ 
      << "\nistream& operator>>( istream& is, "
      << recordName
      << "& var )\n"
      << "{\n"
      << "  if( !GenArchive::read( is, var ) )\n"
      << "  {\n"
      << "    is.setstate( ios::failbit );\n"
      << "  }\n"
      << "  return is;\n"
      << "}// operator>>("
      << recordName
      << ")"
      << endl;    
  }

  if( !ofc_ )
  {
    SLOG_ALERT( "finalizeRecord: Error writing to source file" );

    return false;
  }

  return true;
}// finalizeRecord

bool GEN_IoOutput2::writeField( const char* fieldName,
			       const char* recordName )
{
  using namespace std;

  ofc_ 
    << "  if( !copy( rwd, var."
    << fieldName
    << ", \""
    << fieldName
    << "\" ) )"
    << endl;

  ofc_
    << "  {\n"
    << "    cerr << \"copy<"
    << recordName
    << ">: Error at member name: "
    << fieldName << "\"\n"
    << "         << endl;"
    << endl;

  ofc_
    << "\n    return false;\n"
    << "  }\n"
    << endl;
  
  if( !ofc_ )
  {
    SLOG_ALERT( "writeField: Error writing to source file" );

    return false;
  }

  return true;
}// writeField

bool GEN_IoOutput2::initializeEnum( const char* recordName,
				    const char* integerName )
{
  using namespace std;

#if 0
  const char* tagName = "enum";

  // Does not work.  Forward declaring requires the underlying
  // integer type, which may be different for gcc and clang.
  // Moreover, declaring explicitly with the integer type 
  // conflicts with the implict enum declaration
  //
  string trailer( ": " );
  trailer += integerName;

  if( !forwardDeclare( tagName, 
		       recordName,
		       trailer.c_str() ) )
  {
    SLOG_ALERT( "Forward declaring " 
		<< tagName << ' ' << recordName );

    return false;
  }

  ofh_ << "\nnamespace GenArchive{" << endl;
  
  ofh_ << "\nextern template\n "
       << "bool copy( WriteDirector& wd, const "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool copy( ReadDirector& rd, "
       << recordName
       << "& var );"
       << endl;
#endif

 ofc_ 
   << "\ntemplate<>\n"
   << "const std::vector< GEN_Enum< "
   << recordName
   << " >::ValString > GEN_Enum< "
   << recordName
   << " >::vsVec_ = {\n" << flush;

 return !ofc_.fail();
}// initializeEnum

bool GEN_IoOutput2::finalizeEnum( const char* recordName )
{
  using namespace std;


  ofc_ << "\n};" << endl;
  
  ofc_ 
    << "template<>\n"
    << "typename GEN_Enum< "
    << recordName
    << " >::bm_type GEN_Enum< "
    << recordName
    << " >::bm_ = {};" << endl;

  ofc_ 
    << "static GEN_Enum< "
    << recordName 
    << " > enumObject_" 
    << enumObjectCount_++
    << ';'
    << endl;


  ofc_ << "\nnamespace GenArchive{" << endl;

  ofc_ 
    << "template bool copy( WriteDirector& wd, const "
    << recordName
    << "& var );"
    << endl;

  ofc_ 
    << "template bool copy( ReadDirector& rd, "
    << recordName
    << "& var );"
    << endl;

  ofc_ 
    << "template bool copy( WriteDirector& wd, const "
    << recordName
    << "& var, const char* memb );"
    << endl;

  ofc_ 
    << "template bool copy( ReadDirector& rd, "
    << recordName
    << "& var, const char* memb );"
    << endl;

  // Close off namespace GenArchive
  ofc_ << "}" << endl;

  return !ofc_.fail();
}// finalizeEnum

bool GEN_IoOutput2::writeEnumField( const char* fieldName,
				   const char* fieldValue,
				   const std::string& recordName )
{
  using namespace std;

  ofc_
    << '{'
    << fieldValue
    << ", \""
    << fieldName
    << "\"}, ";

    return !ofc_.fail();
}// writeEnumField

