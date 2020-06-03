/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <iostream>
#include <string>
#include <unordered_set>
#include <clang/Basic/Version.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Attr.h>
#include <clang-c/Index.h>
#include "IoGenOutput.h"
#include "GenLoggingCLI.hpp"

#define DEBUGX 0

#if 0
Got off the ground using
http://llvm.org/devmtg/2010-11/Gregor-libclang.pdf
#endif

struct IoOutputPlus
{
  GEN_IoOutput* ioOutput = nullptr;
  std::string recordName;
};

static bool useTypesSet = false;
static std::unordered_set<std::string> typesSet;
static bool restrictToMainFile = true;
static bool restrictToTopLevel = true; // Do not do sub types


static
CXChildVisitResult doEnumMembers( CXCursor cursor, 
				  CXCursor parent, 
				  CXClientData client_data )
{
  using namespace std;

  const clang::Decl* cursorDecl = 
    static_cast< const clang::Decl* >( cursor.data[0] );

  assert( nullptr != cursorDecl );
    
  clang::Decl::Kind declKind = cursorDecl->getKind();
#if 0
  cout << "Decl kind= " << declKind
       << ", name= " << cursorDecl->getDeclKindName() << endl;
#endif
  
  if( clang::Decl::Kind::EnumConstant != declKind )
  {
    return CXChildVisit_Continue;
    // return CXChildVisit_Recurse;    
  }

  const clang::EnumConstantDecl* enumDecl =
    static_cast< const clang::EnumConstantDecl* >( cursorDecl );

  assert( nullptr != enumDecl );

  string fieldName = enumDecl->getName().data();
  //  llvm::APSInt // Signed integer.  Use radix 10.
  const string strVal = enumDecl->getInitVal().toString(10);

  // SLOG_INFO( strVal << " = " << fieldName << "," );

  IoOutputPlus* iop = reinterpret_cast< IoOutputPlus* >( client_data );
  GEN_IoOutput* io = iop->ioOutput;

  if( !io->writeEnumField( fieldName.c_str(),
			   strVal.c_str(),
			   iop->recordName ) )
  {
    SLOG_ERROR( "doEnumMembers: Error writing enum field" );

    return CXChildVisit_Break;
  }

  return CXChildVisit_Continue;    
}// doEnumMembers

static
bool processEnum( CXCursor cursor,
		  IoOutputPlus* iop )
{
  using namespace std;

  GEN_IoOutput* io = iop->ioOutput;

  SLOG_NOTICE( "Processing enum " << iop->recordName );

  CXType integerType = clang_getEnumDeclIntegerType( cursor );
  assert( CXType_Invalid != integerType.kind );
  CXString integerName = clang_getTypeSpelling( integerType );

  if( !io->initializeEnum( iop->recordName.c_str(),
			   clang_getCString( integerName ) ) )
  {
    SLOG_ALERT( "processEnum: Error from initializeEnum" );

    return false;
  }
  clang_disposeString( integerName );

  unsigned retval = clang_visitChildren
    ( cursor,
      doEnumMembers, // CXCursorVisitor visitor
      iop );        // void* CXClientData

  if( 0 != retval )
  {
    SLOG_ALERT( "processEnum: Visiting broken by child" );
    return false;
  }

  if( !io->finalizeEnum( iop->recordName.c_str() ) )
  {
    SLOG_ALERT( "processEnum: Error from finaalizeEnum" );

    return false;
  }

  return true;
}// processEnum
		  

// client_data = IoOutputPlus*
static
CXChildVisitResult doDataMembers( CXCursor cursor, 
				  CXCursor parent, 
				  CXClientData client_data )
{
  using namespace std;

  // TODO: On clang version upgrade, change this to
  //       cxcursor::getCursorDecl( cursor )
  const clang::Decl* cursorDecl = 
    static_cast< const clang::Decl* >( cursor.data[0] );

  assert( nullptr != cursorDecl );

  clang::Decl::Kind declKind = cursorDecl->getKind();
#if 0
  cout << "Decl kind= " << declKind
       << ", name= " << cursorDecl->getDeclKindName() << endl;
#endif

  if( clang::Decl::Kind::Field != declKind )
  {
    return CXChildVisit_Continue;
    // return CXChildVisit_Recurse;    
  }

  // The cursor refers to a data field

  const clang::NamedDecl* namedCursorDecl =
    static_cast< const clang::NamedDecl* >( cursorDecl );

  assert( nullptr != namedCursorDecl );

  string fieldName = namedCursorDecl->getName().data();
  //string fieldName = namedCursorDecl->getQualifiedNameAsString().data();

  //SLOG_INFO( "Name= " << fieldName );

#if 0
  {
    const clang::AttrVec& vec = cursorDecl->getAttrs();
    for( clang::Decl::attr_iterator it = cursorDecl->attr_begin();
	 it != cursorDecl->attr_end();
	 ++it )
      {
	const clang::Attr* attr = *it;

	SLOG_DEBUG( "Attr kind: " << attr->getKind() );
      }
    SLOG_INFO( "Num attributes= " << vec.size() );
  }
#endif

  IoOutputPlus* iop = reinterpret_cast< IoOutputPlus* >( client_data );

  if( !iop->ioOutput->writeField( fieldName.c_str(),
				  iop->recordName.c_str() ) )
  {
    SLOG_ALERT( "doDataMembers: Error from writeField" );

    return CXChildVisit_Break;
  }

#if 0
  CXString String = clang_getCursorUSR( cursor );
  cout << "USR= " << clang_getCString( String ) << endl;
  clang_disposeString( String );
#endif

  // Proceed to next sibling
  return CXChildVisit_Continue;
}// doDataMembers


// client_data is GEN_IoOutput*
static
CXChildVisitResult doClassStruct( CXCursor cursor, 
				  CXCursor parent, 
				  CXClientData client_data )
{
  using namespace std;

  CXSourceLocation location = clang_getCursorLocation( cursor );
  const bool isFromMainFile = clang_Location_isFromMainFile( location );
  CXType cursorType = clang_getCursorType( cursor );

  // 1=UnexposedDecl -- is not tagged as from the main file
  // 2=StructDecl, 4=ClassDecl, 5=EnumDecl, 20=TypedefDecl
  // 300=TranslationUnit -- occurs only as a parent
  CXCursorKind cursorKind = clang_getCursorKind( cursor );
   //107=Typedef, 106=Enum
   const CXTypeKind typeKind = cursorType.kind;

#if 0
  {
    CXString String = clang_getTypeSpelling( cursorType );
    cout << "\nCursor kind " << cursorKind
	 << ": " << clang_getCString( String ) << endl;
    clang_disposeString( String );
  }

  /*
    0 CXLanguage_Invalid 	
    1 CXLanguage_C 	
    2 CXLanguage_ObjC 	
    3 CXLanguage_CPlusPlus 	
   */
  CXLanguageKind languageKind = clang_getCursorLanguage( cursor );
  cout << "languageKind= " << languageKind << endl;

  {
    CXFile file;
    clang_getSpellingLocation( location, &file, NULL, NULL, NULL );
    CXString fileName = clang_getFileName( file );
    cout << "File name= " << clang_getCString( fileName ) 
	 << ", isFromMainFile= " << isFromMainFile
	 << endl;
  }

#endif

   const bool isCursorDefinition = clang_isCursorDefinition( cursor );

#if 0
   cout << "\nisFromMainFile= " << isFromMainFile << endl;
   cout << "typeKind= " << typeKind << endl;
   cout << "isCursorDefinition= " << isCursorDefinition << endl;
   {
     CXString typeName = clang_getTypeSpelling( cursorType );
     cout << "typeName= " << clang_getCString( typeName ) << endl;
     clang_disposeString( typeName );
     cout << "cursorKind= " << cursorKind << endl;
   }
#endif

  if( restrictToMainFile && 
      ! ( (CXCursor_UnexposedDecl == cursorKind) || 
	  isFromMainFile )  )  
  {
    // Go to my next child
    return CXChildVisit_Continue;
  }

  CXString StringTypeSpelling = clang_getTypeSpelling( cursorType );

  GEN_IoOutput* ioOutput = 
    reinterpret_cast< GEN_IoOutput* >( client_data );

  if( CXCursor_Namespace == cursorKind )
  {
    //CXString StringCursorSpelling = clang_getCursorSpelling( cursor );

    // Keep going in the namespace
    //ioOutput->namespaceName_ = clang_getCString( StringCursorSpelling );

    // cout << "namespaceName_= " << ioOutput->namespaceName_ << endl;

    //clang_disposeString( StringCursorSpelling );
    clang_disposeString( StringTypeSpelling );
    return CXChildVisit_Recurse;
  }

  if( CXCursor_UnexposedDecl == cursorKind )
  {
    // Dig into extern "C", anonymous typedefs
    clang_disposeString( StringTypeSpelling );
    return CXChildVisit_Recurse;
  }

#if 0
  if( CXCursor_TypedefDecl == cursorKind )
  {
    // Keep going in
    clang_disposeString( StringTypeSpelling );
    return CXChildVisit_Recurse;
  }
#endif

#if 0
   cout << "\nisFromMainFile= " << isFromMainFile << endl;
   cout << "typeKind= " << typeKind << endl;
   cout << "isCursorDefinition= " << isCursorDefinition << endl;
   {
     CXString typeName = clang_getTypeSpelling( cursorType );
     cout << "typeName= " << clang_getCString( typeName ) << endl;
     clang_disposeString( typeName );
   }
#endif

  if( !isCursorDefinition )    
  {
    // Go to my next child
    //return CXChildVisit_Recurse ;
    clang_disposeString( StringTypeSpelling );
    return CXChildVisit_Continue;
  }

#if DEBUGX
   cout << "typeKind= " << typeKind << endl;
   {
     CXString typeName = clang_getTypeSpelling( cursorType );
     cout << "typeName= " << clang_getCString( typeName ) << endl;
     clang_disposeString( typeName );
   }
#endif


  // This is a definition

#if 0
  {
    CXString StringTypeKind =  clang_getTypeKindSpelling( typeKind );
    cout << "typeSpelling= " << clang_getCString( StringTypeSpelling ) << endl;
    cout << "typeKindSpelling= " << clang_getCString( StringTypeKind ) << endl;
    clang_disposeString( StringTypeKind );
  }
#endif

  IoOutputPlus iop;
  iop.ioOutput = ioOutput;
  iop.recordName = clang_getCString( StringTypeSpelling );



   if( useTypesSet )
   {
     if( 0 == typesSet.erase( clang_getCString( StringTypeSpelling ) ) )
     {
       if( !restrictToTopLevel &&
	 ( ( CXCursor_StructDecl == cursorKind ) ||
	   ( CXCursor_ClassDecl == cursorKind ) ) )
       {
	 //cout << "Entering struct/class..." << endl;
	 clang_disposeString( StringTypeSpelling );
	 return CXChildVisit_Recurse;
       }

       clang_disposeString( StringTypeSpelling );
       return CXChildVisit_Continue;
     }
   }

   CXFile file;
   clang_getSpellingLocation( location, &file, NULL, NULL, NULL );
   CXString fileName = clang_getFileName( file );

   SLOG_DEBUG( "\n(2)File name= " << clang_getCString( fileName ) << endl
	       << "typeKind= " << typeKind );

  //  cout << "\nCursor kind " << cursorKind
  //       << ": " << clang_getCString( String ) << endl;

#if 0
   {
     CXCursorKind parentCursorKind = clang_getCursorKind( parent );
     CXType parentCursorType = clang_getCursorType( parent );
     CXString String = clang_getTypeSpelling( parentCursorType );
     
     cout << "Parent Cursor kind " << parentCursorKind
	  << ": " << clang_getCString( String ) 
	  << ", MainFile= " << isFromMainFile
	  << endl;

     clang_disposeString( String );
   }
#endif

  string tagName;

  switch( cursorKind )
  {
  case CXCursor_StructDecl:
    {
      tagName = "struct";
      break;
    }

  case CXCursor_ClassDecl:
    {
      tagName = "class";
      break;
    }

  case CXCursor_EnumDecl:
    {
      if( !processEnum( cursor,
		        &iop ) )
      {
	SLOG_ALERT( "doClassStruct: Error from processEnum " );

	clang_disposeString( StringTypeSpelling );
	return CXChildVisit_Break;
      }

      clang_disposeString( StringTypeSpelling );
     return CXChildVisit_Continue;
    }
 
  default:
    {
      // Not class, struct or enum 
      clang_disposeString( StringTypeSpelling );
      return CXChildVisit_Continue;
    }
  }

  if( !ioOutput->initializeRecord( tagName.c_str(),
				   iop.recordName.c_str() ) )
  {
    SLOG_ALERT( "doClassStruct: Error from initializeRecord " );

    clang_disposeString( StringTypeSpelling );
    return CXChildVisit_Break;
  }

  unsigned retval = clang_visitChildren
    ( cursor,
      doDataMembers, // CXCursorVisitor visitor
      &iop );       // void* CXClientData
  
  if( 0 != retval )
  {
    SLOG_CRITICAL( "doClassStruct: Visiting broken by child" );

    // Terminate the traversal
    clang_disposeString( StringTypeSpelling );
    return CXChildVisit_Break;
  }

  if( !ioOutput->finalizeRecord(  clang_getCString( StringTypeSpelling ) ) )
  {
    SLOG_ALERT( "doClassStruct: Error from finalizeRecord " );

    clang_disposeString( StringTypeSpelling );
    return CXChildVisit_Break;
  }

  clang_disposeString( StringTypeSpelling );

  // Go to my next sibling
  return CXChildVisit_Continue;
}// doClassStruct


int main( int argc, char *argv[] )
{
  using namespace std;

  string inFileName;
  bool doOps = false; // Insertion/extraction operators
  string headerPrefix;
  string outBaseName;

  vector< string > tailVec;
  unique_ptr< const char* []> argvTail;

  CLI::App parser{"Generate IO source files"};
  {
    parser.add_option( "-i, --inFile",
		       inFileName,
		       "File containing structs, classes & enums" )
      ->required();

    // Is negated after parsing.
    parser.add_flag( "-n,--noOps",
		     doOps,
		     "Don't generate insertion/extraction operators" );

    // Is negated after parsing.
    CLI::Option* allOption =
    parser.add_flag( "-a,--all",
		     restrictToMainFile,
		     "Don't restrict to the main source file" );

    CLI::Option* topOption =
      parser.add_option( "-s,--subtypes",
			 headerPrefix,
	   "Sub-types too.  \nBy design, only works with -t flag(s)."
	   "\nThis switch replaces forward declaration \nby header inclusion"
	   " for all types, \nnot just sub-types.",
			 true // Display default value
		     )
      ->type_name( "IncludePrefix" );
    

    parser.add_option( "-o,--outBase",
		       outBaseName,
		       "Creates outBase.h and outBase.cpp"
		       )
      ->required()->type_name( "outBase" );

    vector<string> typesVec;

    parser.add_option( "-t,--type",
		       typesVec,
		       "Specific types to process only" );

    parser.add_option( "tail",
		       tailVec,
		       "The remaining arguments, passed to clang parser" );
 
    CLI11_LOG_PARSE(parser, argc, argv);
   
    doOps ^= 1;
    restrictToMainFile = !*allOption;
    restrictToTopLevel = !*topOption;
    typesSet.insert( typesVec.begin(), typesVec.end() );
    useTypesSet = ( 0 < typesVec.size() );

    SLOG_INFO( "tail vec length= " << tailVec.size() );
    argvTail.reset( new const char* [ 1 + tailVec.size() ] );
    
    for( int ind = 0; ind <  tailVec.size(); ++ind ) {
      argvTail[ ind ] = tailVec[ind].c_str();
    }
    argvTail[ tailVec.size() ] = nullptr;
  }

  using namespace std;

  SLOG_INFO( "restrictToMainFile= " << restrictToMainFile );
  SLOG_INFO( "restrictToTopLevel= " << restrictToTopLevel );
  SLOG_INFO( "useTypesSet= " << useTypesSet );
  

  if( !restrictToTopLevel && !useTypesSet )
  {
    SLOG_ERROR( "Processing sub-types (-s) must be accompanied by"
		" spcecifying which types (-t)" );
    return -1;
  }

  SLOG_INFO( "doOps= " << doOps );


  // Create the output pcrocessor
  // This creates the output h and cpp files.
  unique_ptr< GEN_IoOutput > ioOutputPtr;

  ioOutputPtr.reset( new GEN_IoOutput2( doOps, outBaseName.c_str() ) );

  GEN_IoOutput& ioOutput = *ioOutputPtr; // Convenience

  ///// Get clang version
  {
    string fullVersion = clang::getClangFullVersion();
    string revision = clang::getClangRevision();

    SLOG_INFO( "Clang full version= \"" << fullVersion << "\""
	       << "\nClang revision= \"" << revision << "\"" );
  }
  
  ///// Parse translation unit

  // Provides a shared context for creating translation units. 
  CXIndex Index = clang_createIndex
    ( 1,    // int excludeDeclarationsFromPCH
      1 );  // int displayDiagnostics 


  SLOG_INFO( "IoGenMain: parsing..." );

  // TODO: On clang upgrade, use the function call that
  //       returns an error code
  CXTranslationUnit TU;
  CXErrorCode errorCode  = clang_parseTranslationUnit2
    ( Index, 
      inFileName.c_str(),                // const char* source file name
      argvTail.get(), //args,
      tailVec.size(), //numArgs,        // compiler command line args
      nullptr, 0,                        // Unsaved files
      CXTranslationUnit_SkipFunctionBodies,    // unsigned int Options
      &TU );

  SLOG_INFO( "IoGenMain: Finished parsing.  Error code= "
	     << errorCode );

  if( nullptr == TU )
  {
    SLOG_ERROR( "Error from clang_parseTranslationUnit, parsing file "
		<< '"' << inFileName << '"' );
    return -1;
  }

  const int numDiagnostics = clang_getNumDiagnostics(TU);
  SLOG_INFO( "There are " << numDiagnostics << " diagnostics" );

  int numErrors = 0;

  for( int I = 0; I < numDiagnostics; ++I) 
  {
    CXDiagnostic Diag = clang_getDiagnostic( TU, I );

    CXDiagnosticSeverity severity = clang_getDiagnosticSeverity( Diag );

    numErrors += ( severity >= CXDiagnostic_Error );

    CXString String = 
      clang_formatDiagnostic( Diag,
			      clang_defaultDiagnosticDisplayOptions() );
    fprintf( stderr, "%s\n", clang_getCString( String ) );
    clang_disposeString( String );
    clang_disposeDiagnostic( Diag );
  }

  if( 0 < numErrors )
  {
    SLOG_NOTICE(  "**** There are " << numErrors 
		  << " compilation errors *****" );

    return -2;
  }

  ///// Initialize the output process
  // This can only be done after clang parses the source code,
  // as the two output files will not be correct c++ right after
  // initialization.

  ioOutput.forwardDeclare_ = restrictToTopLevel;
  ioOutput.headerPrefix_ = headerPrefix;

  // 2nd parameter not being  used. 
  // Was used during construction.
  if( !ioOutput.initialize( inFileName.c_str(), "" ) )
  {
    SLOG_ERROR( "main: Error from IoOutput::initialize()" );
    return -1;
  }  

  ///// Process structs, classes & enums defined in this source file
  CXCursor cursor = clang_getTranslationUnitCursor( TU );

  unsigned retval = clang_visitChildren
    ( cursor,
      doClassStruct, // CXCursorVisitor visitor
      &ioOutput );     // void* CXClientData

  if( 0 != retval )
  {
    SLOG_ERROR( "main: Visiting broken by child" );
    return -1;
  }

  if( !ioOutput.finalize() )
  {
    SLOG_ERROR( "main: Error from IoOutput::finalize()" );
    return -1;
  }  


  ///// Clean up
  clang_disposeTranslationUnit( TU );
  clang_disposeIndex( Index );

  return 0;
}// main
