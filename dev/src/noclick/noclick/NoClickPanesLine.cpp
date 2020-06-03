/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickPanesLine.hpp"
#include "LeftClickPane.hpp"
#include "RightClickPane.hpp"
#include "DClickPane.hpp"
#include "DragPane.hpp"

#include <QBitmap>
#include <QPainter>

namespace ssrd_mouse {

  extern int panesSideLength;
  extern int panesLineOffset;
  extern int panesBorderWidth;

  NoClickPanesLine::NoClickPanesLine( void ) :
    NoClickPanes( panesBorderWidth )
  {
    // The standard order, left to right:
    // LL, R, drag, L
    NoClickPane* panes[4]
      = { dragPane_, leftClickPane_, dClickPane_, rightClickPane_ };

    int borderWidth = panesBorderWidth;
    if( borderWidth < 0 ) {
      borderWidth *= -1;
    }

    if( 0 <= panesLineOffset ) {

      resize( 4 * panesSideLength,
	      panesSideLength + panesLineOffset );

      panes[0]->setGeometry( 0,
			     panesLineOffset,
			     panesSideLength,
			     panesSideLength );
    
      panes[1] ->setGeometry( panesSideLength,
			      0,
			      panesSideLength,
			      panesSideLength );

      panes[2]->setGeometry( 2*panesSideLength,
			     0,
			     panesSideLength,
			     panesSideLength );
    
      panes[3]->setGeometry( 3*panesSideLength,
			     panesLineOffset,
			     panesSideLength,
			     panesSideLength );

      // Set transparency mask for outside the 4 click panes.
      QBitmap bitmap( width(), height() );
      bitmap.fill(  Qt::color1 );
      QPainter painter( &bitmap );
      painter.fillRect( 0, 0,
			panesSideLength, panesLineOffset, Qt::color0 );
      painter.fillRect( 3*panesSideLength, 0,
			panesSideLength, panesLineOffset, Qt::color0 );
      painter.fillRect( panesSideLength, panesSideLength,
			2*panesSideLength, panesLineOffset,  Qt::color0 );

      // If using borders, set mask for inside the borders
      if( 0 < borderWidth ) {
	painter.fillRect( borderWidth,
			  borderWidth + panesLineOffset,
			  panesSideLength - 2*borderWidth,
			  panesSideLength - 2*borderWidth,
			  Qt::color0 );

	painter.fillRect( borderWidth + panesSideLength,
			  borderWidth,
			  panesSideLength - 2*borderWidth,
			  panesSideLength - 2*borderWidth,
			  Qt::color0 );

	painter.fillRect( borderWidth + 2*panesSideLength,
			  borderWidth,
			  panesSideLength - 2*borderWidth,
			  panesSideLength - 2*borderWidth,
			  Qt::color0 );

	painter.fillRect( borderWidth + 3*panesSideLength,
			  borderWidth + panesLineOffset,
			  panesSideLength - 2*borderWidth,
			  panesSideLength - 2*borderWidth,
			  Qt::color0 );
      }
      
      setMask( bitmap );
    }
    else { // 0 > paneLineOffset
     resize( 4 * panesSideLength,
	    panesSideLength - panesLineOffset );

     panes[0]->setGeometry( 0,
			     0,
			     panesSideLength,
			     panesSideLength );
    
      panes[1] ->setGeometry( panesSideLength,
			      -panesLineOffset,
			      panesSideLength,
			      panesSideLength );

      panes[2]->setGeometry( 2*panesSideLength,
			     -panesLineOffset,
			     panesSideLength,
			     panesSideLength );
    
      panes[3]->setGeometry( 3*panesSideLength,
			     0,
			     panesSideLength,
			     panesSideLength );
      
      // Set transparency mask for outside the 4 click panes.
      QBitmap bitmap( width(), height() );
      bitmap.fill(  Qt::color1 );
      QPainter painter( &bitmap );
      painter.fillRect( 0, panesSideLength,
			panesSideLength, -panesLineOffset, Qt::color0 );
      painter.fillRect( 3*panesSideLength, panesSideLength,
			panesSideLength, -panesLineOffset, Qt::color0 );
      painter.fillRect( panesSideLength, 0,
			2*panesSideLength, -panesLineOffset,  Qt::color0 );

      // If using borders, set mask for inside the borders
      if( 0 < borderWidth ) {
	painter.fillRect( borderWidth,
			  borderWidth,
			  panesSideLength - 2*borderWidth,
			  panesSideLength - 2*borderWidth,
			  Qt::color0 );

	painter.fillRect( borderWidth + panesSideLength,
			  borderWidth - panesLineOffset,
			  panesSideLength - 2*borderWidth,
			  panesSideLength - 2*borderWidth,
			  Qt::color0 );

	painter.fillRect( borderWidth + 2*panesSideLength,
			  borderWidth - panesLineOffset,
			  panesSideLength - 2*borderWidth,
			  panesSideLength - 2*borderWidth,
			  Qt::color0 );

	painter.fillRect( borderWidth + 3*panesSideLength,
			  borderWidth,
			  panesSideLength - 2*borderWidth,
			  panesSideLength - 2*borderWidth,
			  Qt::color0 );
      }


      setMask( bitmap );
    }
      

  }// NoClickPanesLine
}
