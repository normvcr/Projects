/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickPane.hpp"
#include <QDebug>
#include <QPainter>
#include <QBitmap>

namespace ssrd_mouse {

  extern int panesSideLength;
  extern int panesBorderWidth;
  extern QColor panesBorderColor;
  extern double panesBorderOpacity;

  NoClickPane::~NoClickPane(){}
  
  NoClickPane::NoClickPane( QWidget *parent,
			    QColor& color,
			    int borderWidth ) :
    QLabel( "", parent ),
    pixmap_( panesSideLength, panesSideLength )
  {
    QColor borderColor = color;

    if( borderWidth < 0 ) {
      borderWidth *= -1;
      borderColor = panesBorderColor;
    }

    resize( panesSideLength, panesSideLength );

    Qt::WindowFlags flags =
      Qt::Widget
      | Qt::FramelessWindowHint
      | Qt::WindowStaysOnTopHint
      | Qt::WindowDoesNotAcceptFocus;
    
    setWindowFlags( flags );    

    if( 0 == borderWidth ) {
    pixmap_.fill( color );
    QPainter painter( &pixmap_ );
    painter.setPen( Qt::red );// ?? Does not work
    painter.setPen( Qt::DashLine );
    painter.drawRect( 0,
		      0,
		      pixmap_.width() - 1,
		      pixmap_.height() - 1 );
    } else {
      pixmap_.fill( borderColor );
    }
    setPixmap( pixmap_ );

    show();

    //qInfo() << "Left w h= " << width() << ' ' << height();

  }// NoClickPane::NoClickPane
  

}// ssrd_mouse
