/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef NOCLICK_HOT_CURSOR_HPP
#define NOCLICK_HOT_CURSOR_HPP

#include <QLabel>

namespace ssrd_mouse {

  class NoClickHotCursor : public QLabel {
  private:
    QPixmap pixmap_;   // The cursor that is displayed.

  public:
    // Creates cursor
    NoClickHotCursor( QWidget* parent = nullptr,
		      Qt::WindowFlags f = Qt::WindowFlags() );
    
    int h( void ) const { return pixmap_.height(); }
    int w( void ) const { return pixmap_.width(); }

    // Repositions the mask to indicate the location of the hot point.
    void setHotPoint( const QPoint& hotloc );

    void getHotPoint( QPoint& hotloc ) const;
  };

}

#endif

