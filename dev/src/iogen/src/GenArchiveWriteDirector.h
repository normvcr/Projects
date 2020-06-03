#ifndef GenArchiveWriteDirector_H
#define GenArchiveWriteDirector_H

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "GenArchiveDirector.h"

namespace GenArchive 
{
  class WriteDirector : public Director
  {
  private:
  public:
    int indentWidth_ = 0;               // spaces
    std::ostream& os_;
  
    void incrementIndentLevel ( void );
    void decrementIndentLevel ( void );
    void setIndentLevel( int level) { indentWidth_ = level * indentUnit(); }

    template< typename T >
    using IoRefrType = const T&;

    WriteDirector( std::ostream& os );

    // Wwrite the spaces to os_
    bool indent( void );

    std::ostream& sss( void ) { return os_; }

    void setFail( void ) { os_.setstate( std::ios::failbit ); }

    bool fail( void ) const { return os_.fail(); }
  };

}

#endif
