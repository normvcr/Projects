#ifndef GEN_IoOutput_H
#define GEN_IoOutput_H

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#if 0
This class performs the output to the outBase.h and outBase.cpp files, thus
separating the parsing of the input header file from the formatting
of these two dependent files.
#endif

#include <fstream>

class GEN_IoOutput
{
public:

  GEN_IoOutput( bool doOps ) :
    doOps_( doOps ),
    enumObjectCount_( 0 )
		{}

  virtual ~GEN_IoOutput(){}

  // Opens the two ofstreams, ofh_ and ofc_, 
  // and writes the first part of each output file.
  virtual
  bool initialize( const char* inFileName,  // Input header file
		   const char* outFileBaseName );

  // Paired with initialize
  virtual
  bool finalize( void );

  //----- Classes & Structs

  // Begin output for a new class/struct
  virtual
  bool initializeRecord( const char* tagName,
			 const char* recordName );

  // Paired with initializeRecord
  virtual
  bool finalizeRecord( const char* recordName );

  // Write the IO code for a field of the currrent class/struct
  virtual
  bool writeField( const char* fieldName,
		   const char* recordName );

  //----- Enums

  // Begin output for a new enum
  virtual
  bool initializeEnum( const char* recordName,
		       const char* integerName );
  virtual
  bool finalizeEnum( const char* recordName );

  virtual
  bool writeEnumField( const char* fieldName,
		       const char* fieldValue,
		       const std::string& recordName );

protected:
  // Writes only into the header file.
  // Take names spaces into account
  virtual
  bool forwardDeclare( const char* tagName,
		       const char* recordName,
		       const char* trailer = "" );

  bool doOps_;         // Create the insertion/extraction operators
  std::ofstream ofh_;  // For writing <outFileBaseName>_IO.h
  std::ofstream ofc_;  // For writing <outFileBaseName>_IO.cpp
  int enumObjectCount_;// To distinguish enum objects in the cpp file

public:
  bool forwardDeclare_ = true;
  std::string headerPrefix_;
};



class GEN_IoOutput2 : public GEN_IoOutput
{
public:
  // Creates the two output files _IO.h and _IO.cpp
  // This way, the non-existence of the _IO.h file will
  // not break the clang parser.  The _IO.h is usually
  // included in the .h file, for the convenience of the 
  // programmer.
  GEN_IoOutput2( bool doOps,
		 const std::string& outFileBaseName );

  // outFileBaseName is not needed, here, but for keeping the 
  // hierarchy, still hanging onto outFileBaseName
  bool initialize( const char* inFileName,  // Input header file
		   const char* outFileBaseName );

  bool finalize( void );

  bool initializeRecord( const char* tagName,
			 const char* recordName );

  bool finalizeRecord( const char* recordName );

  bool writeField( const char* fieldName,
		   const char* recordName );

  bool initializeEnum( const char* recordName,
		       const char* integerName );

  bool finalizeEnum( const char* recordName );

  bool writeEnumField( const char* fieldName,
		       const char* fieldValue,
		       const std::string& recordName );

private:
  std::string outFileBaseName_; 

};

#endif
