#ifndef GenArchiveDirector_H
#define GenArchiveDirector_H

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <iostream>
#include <string>

namespace GenArchive 
{
  // This is a convenient way for having these constants
  // available publicly

  static constexpr char nullObjChar_ = '*';
  static constexpr char indentFill_ = ' ';
  static constexpr char stringDelim_ = '"';
  static constexpr char stringEscape_ = '\\';

  static constexpr const char* statementBegin_ = "";    // INDENT begin
  static constexpr const char* statementJoin_ = "=";
  static constexpr const char* statementEnd_ = ";";

  static constexpr const char* compositeBegin_ = "{\n";
  static constexpr const char* compositeEnd_ = "}";     // INDENT  end

  static constexpr int scientificPrecision_ = 16;

  // This enum describes the format of the file/stream being
  // read or written.  
  enum class FormatType { 
    labeled,   // Class label, followed by scoped
      scoped,  // Leading '{' and closing '}'; flat in between
      flat,    // Without the label and the leading and closing braces
  };

  class Manager3;

  // The base class for the WriteDirector and ReadDirector.
  // The directors carry state that is needed for archiving:
  // e.g. Indentation and Handling shared pointers
  // To consider:  This base class is not needed.  The functionality
  // for the directors has largely been moved to non-member functions,
  // to overcome technial problems related to template method matching.
  class Director
  {
  private:
    // INDENT = integer multiple of the indentUnit_
    // INDENT is adjusted recursively during the archive process
    const static int indentUnit_;                // spaces    

    FormatType fmt_ = FormatType::labeled;

  public:
    Director( void );

    void setFormatType( FormatType fmt ) { fmt_ = fmt; }
    FormatType formatType( void ) const { return fmt_; }
    
    static int indentUnit( void );

    void setManager( Manager3* mgr );

    Manager3* mgr_ = nullptr;  // Do not delete
  };

}

#endif
