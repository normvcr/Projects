/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "PanesSetSquare.hpp"
#include "NoClickPanesSquare.hpp"
#include "BootUtil.h"
#include <QDebug>
#include <QRect>
#include <QScreen>
#include <QCoreApplication>

namespace ssrd_mouse {
  extern int panesSideLength;
  extern int hotPointOffset;
  extern int starOffset;
  extern double panesOpacity;
  extern double panesBorderOpacity;
  extern int panesBorderWidth;

  namespace {
  QRect panesRect;
  }
  
  PanesSetSquare::PanesSetSquare( void ) {
    offsets_[0] = hotPointOffset;
    offsets_[1] = hotPointOffset;
    offsets_[2] = hotPointOffset;
    offsets_[3] = hotPointOffset;
    offsets_[4] = starOffset;
    
    panesRect.setX( 0 );
    panesRect.setY( 0 );
    panesRect.setWidth( 2*panesSideLength );
    panesRect.setHeight(2* panesSideLength );

    // Standard click boxes are Double, Right, drag, Left:
    // DR
    // dL
    // Flipped horizontally:
    // RD
    // Ld
    // Flipped vertically:
    // dLg
    // DR
    // Rotated:
    // Ld
    // RD
    //                       D  R  d  L
    int orderings[4][4] = { {0, 1, 2, 3},    // standard layout
			    {1, 0, 3, 2},    // left edge of screen
			    {2, 3, 0, 1},    // top edge of screen
			    {3, 2, 1, 0} };  // UL of screen

    for( int index = 0; index < 4; ++index ) {
      NoClickPanesSquare* ncp
	= new NoClickPanesSquare( orderings[index] );

      //qInfo() << "PanesSetSquare: opacity= " << panesOpacity;

      ncp->setWindowOpacity( panesOpacity );

      ncp->setGeometry( 0,
			0,
			2 * panesSideLength,
			2 * panesSideLength );
      
      panesArray_[index] = ncp;
    }
    panesArray_[4] = createStarPane();
  }// anesSetSquare::PanesSetSquare
  

  PanesSetSquare::~PanesSetSquare() {
   for( int ind = 0; ind < 5; ++ind ) {
     DeletePointer( panesArray_[ind] );
    }
  }// PanesSetSquare::~PanesSetSquare
  

  void PanesSetSquare::shutDown( void ) {
   for( int ind = 0; ind < 5; ++ind ) {
     QLabel* ptr = panesArray_[ind];

     if( nullptr != ptr ) {
       ptr->close();
    }
   }
  }// PanesSetSquare::shutDown


  QLabel* PanesSetSquare::starPane( void ) {
    return panesArray_[ 4 ];
  }
  

  QLabel* PanesSetSquare::clickPanes( void ) {
    return panesArray_[ currentPanesIndex_ ];
  }


  PaneID
  PanesSetSquare::paneOf( const QPoint& pt ) {
    QPoint panesMouse
      = clickPanes()->mapFromGlobal( pt );
    
    if( panesRect.contains( panesMouse ) ) {
      
      // The sub-indices are 0 or 1
      int subIndexX = panesMouse.x() > panesSideLength;
      int subIndexY = panesMouse.y() > panesSideLength;
#if 0
      qInfo() << "before subindices= "
	      << subIndexX << ' '
	      << subIndexY;
#endif
      // Account for reflections
      if( currentPanesIndex_ & 1 ) {
	subIndexX = 1 - subIndexX;
      }
      if( currentPanesIndex_ & 2 ) {
	subIndexY = 1 - subIndexY;
      }
#if 0
      qInfo() << "after subindices= "
	      << subIndexX << ' '
	      << subIndexY;
#endif
      const int subIndex = subIndexX + 2*subIndexY;
      
      return PaneID(subIndex);
    }
    
    return NoPaneID;
  }// PanesSetSquare::paneOf
  
  
  void PanesSetSquare::adjustPanes( const QPoint& pt,
				    bool forStarPane ) {

    //qInfo() << "PanesSetSquare::adjustPanes";
    //qInfo() << "pt= " << pt;
    
    const int sindex = PanesSet::getScreenIndex( pt );

    if( 0 > sindex ) {
      qCritical()
	<< "PanesSetSquare::adjustPanes: "
	<< pt << " not in desktop";
      
      QCoreApplication::exit( -1 );
      return;
    }
   
    QRect screenRect = screenList[ sindex ]->geometry();
    //qInfo() << "screenRect= " << screenRect;
    
    QPoint ulc
      ( pt.x() - 2*panesSideLength - hotPointOffset,
	pt.y() - 2*panesSideLength - hotPointOffset );
    //qInfo() << "ulc= " << ulc;

    // Gives an index from 0 to 3.
    // 0 = Natural
    // 1 = Flipped horizontally
    // 2 = Flipped vertically
    // 3 = Rotated pi
    const int geomIndex
      = ( ulc.x() < screenRect.x() )
      + 2*( ulc.y() < screenRect.y() );

    const int panesIndex
      = ( forStarPane ? 4 : geomIndex );

    // Avoid flashing
    if( currentPanesIndex_ != panesIndex ) {
      clickPanes()->hide();
      currentPanesIndex_ = panesIndex;
    }    
     
    clickPanes()->move
      ( pt
	+ offsets_[panesIndex] * offsetVectors_[geomIndex]
	+ clickPanes()->height() * widthVectors_[geomIndex] );

    //clickPanes()->show();
    clickPanes()->setVisible(true);
    
    //clickPanes()->raise();

    // qInfo() << "Focus= " << clickPanes()->hasFocus();
  }// PanesSetSquare::adjustPanes

}
