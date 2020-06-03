/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef NOCLICK_PANE_HPP
#define NOCLICK_PANE_HPP



#include <QLabel>
#include <QTimer>
#include <memory>

/*
  Abstract base class for panes that are activated 
  by the mouse being stationary inside the pane.

  The click occurs over the simulated mouse cursor.
  The simulated mouse cursor is always visible for
  these actions.
*/

namespace QT {
  class QColor;
}

namespace ssrd_mouse {

  class NoClick;

  class NoClickPane : public QLabel {
  public:

  private:

    NoClick* mainWindow_ = nullptr; // Not the parent
    QPixmap pixmap_;
   
  public:
    NoClickPane( QWidget *parent,
		 QColor& color,
		 int borderWidth = 0 );

    virtual ~NoClickPane();
  };


}
#endif
