
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "GenLogging.h"
#include "IoGenOutput.h"
#include <memory>
#include <string>
#include <iostream>
#include <boost/filesystem.hpp>


bool GEN_IoOutput::initialize( const char* inFileName,
			       const char* outFileBaseName )
{
  using namespace std;

  // Do the H file
  string outHFileName( outFileBaseName );
  {
    outHFileName += ".h";
    ofh_.open( outHFileName );

    if( !ofh_ )
    {
      SLOG_ERROR( "IoOutput::initialize: Error opening ouput h file \""
		  << outHFileName << "'" );
      return false;
    }

    string inclGuardName( outFileBaseName );
    inclGuardName += "_H";

    ofh_ << "#ifndef "
	 << inclGuardName
	 << endl;

    ofh_ << "#define "
	 << inclGuardName
	 << endl
	 << endl;

    ofh_ << "#include <iosfwd>"
	 << "\n#include \"GEN_IoInterface.h\""
	 << endl;

    if( !ofh_ )
    {
      SLOG_ALERT( "Error writing to output h file "
		  << outHFileName );

      return false;
    }
  }// H file

  // Do the CPP file
  {
    string outCxxFileName( outFileBaseName );
    outCxxFileName += ".cpp";
    ofc_.open( outCxxFileName );

    if( !ofc_ )
    {
      SLOG_ERROR( "IoOutput::initialize: Error opening ouput cpp file \""
		  << outCxxFileName << "'" );
      return false;
    }

    ofc_ << "#include <iostream>" 
	   << endl;

    ofc_ << "using namespace std;" 
	   << endl;

    ofc_<< "#include \""
	<< outHFileName
	<< '\"' << endl;

    boost::filesystem::path inFilePath( inFileName );

    ofc_<< "#include \""
	<< inFilePath.filename().string()  // Otherwise includes quotes
	<< "\"" << endl;

    ofc_ << "#include \"GEN_IoInterface.tpp\""
	 << endl;

    ofc_ << "#include \"GEN_Enum.h\""
	 << endl;

   if( !ofc_ )
    {
      SLOG_ALERT( "Error writing to output cpp file "
		  << outCxxFileName );

      return false;
    }
  }// CPP file

  return true;
}// initialize


bool GEN_IoOutput::finalize( void )
{
  using namespace std;

  // Complete the include guard
  ofh_ << "\n#endif" 
       << endl;

  ofh_.close();

  if( !ofh_ )
  {
    SLOG_ALERT( "GEN_IoOutput::finalize: Error closing H file" );

    return false;
  }

  ofc_.close();

  if( !ofc_ )
  {
    SLOG_ALERT( "GEN_IoOutput::finalize: Error closing CPP file" );

    return false;
  }

  return true;
}// finalize


bool GEN_IoOutput::initializeRecord( const char* tagName,
				     const char* recordName )
{
  using namespace std;
  SLOG_NOTICE( "Initializing record: " 
	       << tagName << ' ' 
	       << recordName );

  // Write to header file

  //------ Forward declaration --------
  // Take names spaces into account

#if 0
  // This ignores namesspaces
  ofh_ << '\n' << tagName << ' ' << recordName << ';'
       << endl;
#else
  ofh_ << '\n';
  int namespaceCount = 0;
  const char* cpOld = recordName;
  for( const char* cp  = strstr( cpOld, "::" );
       nullptr != cp;
       cp  = strstr( cpOld, "::" ) )
  {
    SLOG_DEBUG( "cpOld= " << cpOld << endl
		<< "cp= " << cp );

    ofh_ << "namespace " << string( cpOld, cp ) << "{\n";
    cpOld = cp + 2;
    ++namespaceCount;
  }

  ofh_ << '\n' << tagName << ' ' << cpOld << ';' << flush;

  for( int ind = namespaceCount; ind > 0; --ind )
  {
    ofh_ << " }";
  }
  ofh_ << endl;
  
#endif
  

  ofh_ << "\nextern template\n " 
       << "bool GenIoArchive( std::ostream& os, const "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool GenIoRestore( std::istream& is, "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n " 
       << "bool GenIoArchive( const char* fileName, const "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool GenIoRestore( const char* fileName, "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "\ntemplate< class SSS >\n "
       << "bool GEN_IoCopy( SSS& ois, "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool GEN_IoCopy( std::ostream&, "
       << recordName
       << "& var );"
       << endl;

  ofh_ << "extern template\n "
       << "bool GEN_IoCopy( std::istream&, "
       << recordName
       << "& var );"
       << endl;

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
    << "\ntemplate< class SSS >\n"
    << "bool GEN_IoCopy( SSS& sss, "
    << recordName
    << "& var )\n"
    << '{' << endl;

  ofc_  
    << "  GEN_IoCopy( sss, \"{\" );\n"
    << "  GEN_IoWriteOnly( sss, \"\\n\" );\n"
    << endl;
 
  if( !ofc_ )
  {
    SLOG_ALERT( "initializeRecord: Error writing to source file" );

    return false;
  }
  
  return true;
}// initializeRecord

bool GEN_IoOutput::finalizeRecord( const char* recordName )
{
  using namespace std;

  ofc_ 
    << "  return GEN_IoCopy( sss, \"}\" )"
    << endl;

  ofc_ << "  &&!sss.fail();\n"
       << "} // GEN_IoCopy( "
       << recordName
       << " )"
       << endl;

  ofc_ 
    << "\ntemplate bool GEN_IoCopy( std::ostream& os, "
    << recordName
    << "& var );"
    << endl;

  ofc_ 
    << "template bool GEN_IoCopy( std::istream& is, "
    << recordName
    << "& var );"
    << endl;

  ofc_ 
    << "\ntemplate bool GenIoArchive( ostream& os, const "
    << recordName
    << "& var );"
    << endl;

  ofc_ 
    << "template bool GenIoRestore( istream& is, "
    << recordName
    << "& var );"
    << endl;

  ofc_
    << "template bool GenIoArchive( const char* fileName, const "
    << recordName
    << "& var );"
    << endl;

  ofc_
    << "template bool GenIoRestore( const char* fileName, "
    << recordName
    << "& var );"
    << endl;

  if( doOps_ )
  {
    ofc_ 
      << "\nostream& operator<<( ostream& os, const "
      << recordName
      << "& var )\n"
      << "{\n"
      << "  if( !GenIoArchive( os, var ) )\n"
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
      << "  if( !GenIoRestore( is, var ) )\n"
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

bool GEN_IoOutput::writeField( const char* fieldName,
			       const char* recordName )
{
  using namespace std;

  ofc_ 
    << "  if( !GEN_IoCopy( sss, var."
    << fieldName
    << ", \""
    << fieldName
    << "\" ) )"
    << endl;

  ofc_
    << "  {\n"
    << "    cerr << \"GEN_IoCopy<"
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

bool GEN_IoOutput::initializeEnum( const char* recordName,
				   const char* )
{
  using namespace std;

 ofc_ 
   << "\ntemplate<>\n"
   << "const vector< GEN_Enum< "
   << recordName
   << " >::ValString > GEN_Enum< "
   << recordName
   << " >::vsVec_ = {\n" << flush;

 return !ofc_.fail();
}// initializeEnum

bool GEN_IoOutput::finalizeEnum( const char* recordName )
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

  return !ofc_.fail();
}// finalizeEnum

bool GEN_IoOutput::writeEnumField( const char* fieldName,
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

bool GEN_IoOutput::forwardDeclare( const char* tagName,
				   const char* recordName,
				   const char* trailer )
{
  if( !forwardDeclare_ ) return true;

  using namespace std;

  ofh_ << '\n';
  int namespaceCount = 0;
  const char* cpOld = recordName;
  for( const char* cp  = strstr( cpOld, "::" );
       nullptr != cp;
       cp  = strstr( cpOld, "::" ) )
  {
    SLOG_DEBUG( "cpOld= " << cpOld << endl
		<< "cp= " << cp );

    ofh_ << "namespace " << string( cpOld, cp ) << "{\n";
    cpOld = cp + 2;
    ++namespaceCount;
  }

  ofh_ << tagName 
       << ' ' << cpOld 
       << ' ' << trailer
       << ';' << flush;

  for( int ind = namespaceCount; ind > 0; --ind )
  {
    ofh_ << " }";
  }
  ofh_ << endl;
  
  return !ofh_.fail();
}// forwardDeclare
