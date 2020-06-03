/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickHotCursor.hpp"
#include <QPainter>
#include <QDebug>
#include <QBitmap>

namespace ssrd_mouse {

  extern int hotCursorSemiWidth;
  extern int hotCursorInset;
  extern bool hotCursorShow;
  extern QColor hotCursorColor;

  NoClickHotCursor::NoClickHotCursor( QWidget* parent,
				      Qt::WindowFlags f ) :
    QLabel( parent, f ),
    pixmap_( 1 + 2*hotCursorSemiWidth,
	     1 + 2*hotCursorSemiWidth ) 
  {
    stackUnder( parent );

    //qInfo() << pixmap_.size();
    //qInfo() << hotCursorColor.name();

    pixmap_.fill( Qt::white );
    
    // The bitmap is a cross-hair.
    {
      QPainter painter( &pixmap_ );

      const int inset = hotCursorInset;

      // Leave the centre open so that the
      // cursor is not blocked by the crosshairs.
      painter.setPen( hotCursorColor );
      painter.drawLine( w()/2, 0, w()/2, h()/2-inset );
      painter.drawLine( w()/2, h()/2+inset, w()/2, h() );
      painter.drawLine( 0, h()/2, w()/2-inset, h()/2 );
      painter.drawLine( w()/2+inset, h()/2, w(), h()/2 );
    }

    QBitmap bitmap = pixmap_.createMaskFromColor( Qt::white,
						  Qt::MaskInColor );

    pixmap_.setMask( bitmap );
    setPixmap( pixmap_ );
#if 0
    qInfo() << "Hot Cursor has alpha: " << pixmap_.hasAlpha();
    qInfo() << "Hot Cursor has alpha channel: "
	    << pixmap_.hasAlphaChannel();
#endif     
  }// NoClickHotCursor::NoClickHotCursor
  

  void NoClickHotCursor::getHotPoint( QPoint& hotloc ) const {
    hotloc.setX( x() + w()/2 );
    hotloc.setY( y() + h()/2 );
  }// NoClickHotCursor::getHotPoint
  

  void NoClickHotCursor::setHotPoint( const QPoint& hotloc ) {

    setGeometry( hotloc.x() - w()/2,
		 hotloc.y() - h()/2,
		 w(),
		 h() );

    //if( hotCursorShow ) show();
  }// NoClickHotCursor:setHotPoint
  
}
