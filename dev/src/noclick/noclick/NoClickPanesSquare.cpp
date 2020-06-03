/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickPanesSquare.hpp"
#include "LeftClickPane.hpp"
#include "RightClickPane.hpp"
#include "DClickPane.hpp"
#include "DragPane.hpp"

namespace ssrd_mouse {

  extern int panesSideLength;

  NoClickPanesSquare::NoClickPanesSquare( int* order ) :
    NoClickPanes()
  {
    // In the standard order, left to right,
    // starting at top row:  LL, R, drag, L
    NoClickPane* panes[4]
      = { dClickPane_, rightClickPane_, dragPane_, leftClickPane_ };

    panes[ order[ 0 ] ]->setGeometry( 0,
				      0,
				      panesSideLength,
				      panesSideLength );
    
    panes[ order[ 1 ] ] ->setGeometry( panesSideLength,
				       0,
				       panesSideLength,
				       panesSideLength );

    panes[ order[ 2 ] ]->setGeometry( 0,
				      panesSideLength,
				      panesSideLength,
				      panesSideLength );
    
    panes[ order[ 3 ] ]->setGeometry( panesSideLength,
				      panesSideLength,
				      panesSideLength,
				      panesSideLength );
  }// NoClickPanesSquare
}
