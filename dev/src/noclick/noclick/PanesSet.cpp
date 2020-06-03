/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "PanesSet.hpp"
#include <QGuiApplication>
#include <QLabel>
#include <QList>
#include <QScreen>
#include <QRect>
#include <QDebug>

#include "NoClick.hpp"

#include <string.h>

namespace ssrd_mouse {

  extern int starSideLength;
  extern QColor starColor;
  extern double starOpacity;


  //=================  static stuff

  const char* toString( PanesType pt ) {
    switch( pt ) {
    case LinePanesType: return "Line";
    }

    return "Square";
  }// toString

  template<>
  PanesType fromString<PanesType>( const char* str ) {
    if( 0 == strcmp( str, "LinePanesType" ) ) return LinePanesType;

    return SquarePanesType;
  }// fromString


  QList< QScreen* > PanesSet::screenList;

  void PanesSet::initialize( void ) {
    if( screenList.empty() ) {
      screenList = QGuiApplication::screens();
    }
  }// 

  int  PanesSet::getScreenIndex( const QPoint& point ) {
    for( int ind = 0; ind < screenList.size(); ++ind ) {
      QRect qr = screenList[ ind ]->geometry();

      if( ( qr.x() <= point.x() ) &&
	  ( point.x() <= qr.right() ) &&
	  ( qr.y() <= point.y() ) &&
	  ( point.y() <= qr.bottom() ) )
	return ind;
    }

    return -1;
  }// getScreenIndex
  

  QLabel* PanesSet::createStarPane( void ) {
    QLabel* sm = new QLabel();
    sm->resize( starSideLength, starSideLength );

    //qInfo() << "Star size= " << starSideLength;
    //qInfo() << "Star color= " << starColor.name();

    Qt::WindowFlags flags =
      Qt::Tool
      | Qt::FramelessWindowHint
      | Qt::WindowStaysOnTopHint
      | Qt::WindowDoesNotAcceptFocus
      ;

    sm->setWindowFlags( flags );

    sm->setFocusPolicy( Qt::NoFocus );
    sm->setAttribute(Qt::WA_TransparentForMouseEvents);

    QPixmap pm( starSideLength, starSideLength );
    pm.fill( starColor );

    // qInfo() << "Star color= " << starColor.name();
    
    sm->setPixmap( pm );
    sm->setWindowOpacity( starOpacity );
    return sm;
  }// createStarPane
  

  //============================================
 

  PanesSet::PanesSet( void ) {
    initialize();
  }// PanesSet::PanesSet

  
  PanesSet::~PanesSet() {
    screenList.clear();
  }// PanesSet::~PanesSet



}
