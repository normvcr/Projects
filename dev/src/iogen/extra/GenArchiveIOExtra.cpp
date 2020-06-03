/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "GenArchiveIO.h"
#include "GenArchiveBool.h"
#include "GenArchiveWriteDirector.h"
#include "GenArchiveReadDirector.h"

namespace GenArchive 
{

  //----------- bool ---------------------
  template<>
  bool copy( WriteDirector& rd, 
	     const bool& var, 
	     const char* memb )
  {
    return copy( rd, var ? True : False, memb );
  }

  bool copy( WriteDirector& wd, const bool& val )
  {
    return copy( wd, val ? True : False );
  }

  bool copy( ReadDirector& rd, bool& val )
  {
    Boolean val2;
    const bool retval = copy( rd, val2 );

    val = ( val2 == True );
    return retval;
  }

}
