/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "PanesSetLine.hpp"
#include "NoClickPanesLine.hpp"
#include "BootUtil.h"
#include <QDebug>
#include <QRect>
#include <QScreen>
#include <QCoreApplication>

namespace ssrd_mouse {
  extern int panesSideLength;
  extern int panesLineOffset;
  extern int hotPointOffset;
  extern int starOffset;
  extern double panesOpacity;
  extern double panesBorderOpacity;
  extern int panesBorderWidth;

  namespace {
  QRect panesRect;
  }
  
  PanesSetLine::PanesSetLine( void ) {
    panesRect.setX( 0 );
    panesRect.setY( 0 );
    panesRect.setWidth( 4*panesSideLength );
    panesRect.setHeight(panesLineOffset + panesSideLength );

    // Standard click boxes are drag, Left, Double, Right:
    
    for( int index = 0; index < 2; ++index ) {
      NoClickPanesLine* ncp
	= new NoClickPanesLine( );

      if( 0 == panesBorderWidth ) {
	ncp->setWindowOpacity( panesOpacity );
      } else {
	ncp->setWindowOpacity( panesBorderOpacity );
      }

      panesArray_[index] = ncp;
      panesLineOffset *= -1;
    }
    
    panesArray_[2] = createStarPane();
  }// anesSetSquare::PanesSetSquare
  

  PanesSetLine::~PanesSetLine() {
   for( int ind = 0; ind < 3; ++ind ) {
     DeletePointer( panesArray_[ind] );
    }
  }// PanesSetLine::~PanesSetLine
  

  void PanesSetLine::shutDown( void ) {
   for( int ind = 0; ind < 3; ++ind ) {
     QLabel* ptr = panesArray_[ind];

     if( nullptr != ptr ) {
       ptr->close();
    }
   }
  }// PanesSetLine::shutDown


  QLabel* PanesSetLine::starPane( void ) {
    return panesArray_[ 2 ];
  }
  

  QLabel* PanesSetLine::clickPanes( void ) {
    return panesArray_[ currentPanesIndex_ ];
  }


  PaneID
  PanesSetLine::paneOf( const QPoint& pt ) {
    QPoint panesMouse
      = clickPanes()->mapFromGlobal( pt );

    if( panesRect.contains( panesMouse ) ) {
      QPoint loc( panesMouse );
      
      // Convert flipped coordinates to standard
      if(  1 == currentPanesIndex_ ) {
	loc.ry() = panesSideLength + panesLineOffset - panesMouse.y();
      }

      if( 2*panesSideLength > loc.x() ) {
	
	if( panesSideLength > loc.x() ) {
	  if( panesLineOffset <= loc.y() ) {
	    return DragPaneID;
	  }
	}
	else if( panesSideLength > loc.y() ) {
	  return LeftClickPaneID;
	}
	
      } else {

	if( 3*panesSideLength <= loc.x() ) {

	  if( panesLineOffset <= loc.y() ) {
	    return RightClickPaneID;
	  }
	}
	else if( panesSideLength > loc.y() ) {
	  return DClickPaneID;
	  }
      }
      
    }// point in bounding box
      
    return NoPaneID;
  }// PanesSetSquare::paneOf
  
  
  void PanesSetLine::adjustPanes( const QPoint& pt,
				  bool forStarPane ) {
    
    //qInfo() << "PanesSetLine::adjustPanes";

    const int sindex = PanesSet::getScreenIndex( pt );

    if( 0 > sindex ) {
      qCritical()
	<< "PanesSetSquare::adjustPanes: "
	<< pt << " not in desktop";
      
      QCoreApplication::exit( -1 );
      return;
    }
   
    QRect screenRect = screenList[ sindex ]->geometry();

    QPoint ulc
      ( pt.x() - 2*panesSideLength,
	pt.y() - panesSideLength - hotPointOffset );

    // Horizontal position
    {
      if( ulc.x() < screenRect.x() ) {
	ulc.rx() = pt.x() + hotPointOffset;
      }
      else if( ulc.x() + 4 * panesSideLength >= screenRect.width() ) {
	ulc.rx() = pt.x() - hotPointOffset - 4 * panesSideLength;
      }
    }

    int panesIndex = -1;

    // Vertical position and panesIndex
    {
      if( ulc.y() < screenRect.y() ) {
	panesIndex = 1;

	ulc.ry() = pt.y() + hotPointOffset - panesLineOffset;
      }
      else {
	panesIndex = 0;

	if( ulc.y() + panesSideLength + panesLineOffset
	    >= screenRect.height() ) {
	  ulc.ry() = screenRect.y() - panesSideLength - panesLineOffset;
	}
      }
    }
      
    if( forStarPane ) {
      ulc.rx() = ( ulc.x() + panesRect.width()/2 );

      ulc.ry() =  ulc.y() + panesSideLength/2;
      if( 1 == panesIndex ) {
	ulc.ry() += panesLineOffset;
      }
      
      panesIndex = 2;
    }

    // Avoid flashing
    if( currentPanesIndex_ != panesIndex ) {
      clickPanes()->hide();
      currentPanesIndex_ = panesIndex;
    }    
     
    clickPanes()->move( ulc );
    clickPanes()->show();
    clickPanes()->raise();
  }// PanesSetLine::adjustPanes

}
