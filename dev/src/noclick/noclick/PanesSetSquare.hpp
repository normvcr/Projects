#ifndef NOCLICK_PanesSetSquare_HPP
#define NOCLICK_PanesSetSquare_HPP
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "PanesSet.hpp"
#include <QPoint>

class QLabel;

namespace ssrd_mouse {

  class PanesSetSquare : public PanesSet {

  public:
    PanesSetSquare( void );
    virtual ~PanesSetSquare();

    virtual PaneID paneOf( const QPoint& pt );

    virtual void shutDown( void );
    
    virtual QLabel* starPane( void );
    
    virtual QLabel* clickPanes( void );
 
    virtual void adjustPanes( const QPoint& loc,
			      bool forStarPane = true );

    PanesType type( void ) const {
      return SquarePanesType;
    }

  private:
    // The order of the 5 panes is:
    // Natural, Flipped Horizontal, Flipped Vertical, Rotated pi,
    // followed by the Star pane.
    QLabel*  panesArray_[5];
    int currentPanesIndex_ = 0;

    // The distance of the hot point to the visible pane
    int offsets_[5];

    // The direction of the pane relative to the hot point.
    QPoint offsetVectors_[4] =
      { QPoint(-1,-1), QPoint(1, -1), QPoint(-1, 1), QPoint(1,1) };

    // Delta shifts to positon the ulc of the panes.
    QPoint widthVectors_[4] =
      { QPoint(-1,-1), QPoint(0, -1), QPoint(-1, 0), QPoint(0,0) };
  };

}

#endif
