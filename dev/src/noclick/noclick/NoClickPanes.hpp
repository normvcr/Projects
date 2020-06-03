#ifndef NOCLICK_PANES_HPP
#define NOCLICK_PANES_HPP
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
This class contains the 4 click pannels.
 */

#include <QLabel>


namespace ssrd_mouse {

  class LeftClickPane;
  class RightClickPane;
  class DClickPane;
  class DragPane;
  class NoClickPane;
  class NoClick;

  // mainWindow is not a parent for any of
  // the pane widgets, nor the Panes
  // encompassing window.
  // arrangentOrder points to a permutation of
  // 0,1,2,3, which indicates the standard order
  // of the 4 panes:
  // 
  // In the standard order, left to right,
  // starting at top row:  D, R, d, L
  // D = double click, d = drag
  class NoClickPanes : public QLabel {
  public:
    NoClickPanes( int borderWidth = 0 );
    
    DragPane* dragPane( void ) const {
      return dragPane_;
    }
    
  protected:
    LeftClickPane* leftClickPane_ = nullptr;
    RightClickPane* rightClickPane_ = nullptr;
    DClickPane* dClickPane_ = nullptr;
    DragPane* dragPane_ = nullptr;
  };

}
#endif

    
