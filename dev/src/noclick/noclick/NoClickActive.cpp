/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <QApplication>
#include <QDebug>
#include <QString>
#include <QMouseEvent>
#include "BootUtil.h"
#include "NoClick.hpp"
#include "NoClickActive.hpp"
#include "ActiveHotKey.hpp"
#include "GenArchiveIO.tpp"
#include "KbdCodes.hpp"


namespace ssrd_mouse {

  extern int hotKeyModifiers;
  extern int hotKeyKey;

  namespace {

    ActiveHotKey* hkWidget = nullptr;

    const QString modifierStrings[8] =
      {
       "",
       "Shift ",
       "Ctl ",
       "Shift-Ctl ",
       "Alt ",
       "Shift-Alt ",
       "Ctl-Alt ",
       "Shift-Ctl-Alt " };
  }

  extern bool labelAllowHover;

  NoClickActive::NoClickActive( const QString &text,
				QWidget *parent,
				Qt::WindowFlags f  ) :
    NoClickLabel( text, parent, f )
  {
    hkWidget = new ActiveHotKey();
    stationaryAction( false );
   
  }// NoClickActive::NoClickActive
  
  
  NoClickActive::~NoClickActive( void )
  {
   DeletePointer( hkWidget );
  }

  void NoClickActive::stationaryAction( bool toggle ) {
    //qInfo() << "NoClickActive::stationaryAction";

    QString tag( "" );

    if( hotKeyModifiers >= 0 ) {
      tag = modifierStrings[ hotKeyModifiers ] +
	( toString( ssrd_kbd::KbdCodes(hotKeyKey) ).c_str() +
	  strlen( "VKEY_" ) );
    }

    if( text()[0] == 'S' ) {
      if( toggle ) {
	showingApparatus_ = true;
	setText( "Hide: " + tag );
	mainWindow()->showApparatus( true );
      }
      else {
	setText( "Show: " + tag );
      }
    }
    else {
      if( toggle ) {
	showingApparatus_ = false;
	setText( "Show: " + tag );
	mainWindow()->showApparatus( false );
      }
      else {
	setText( "Hide: " + tag );
      }
    }
  }// NoClickActive::stationaryAction
  

  void NoClickActive::mousePressEvent(QMouseEvent *event) {
    if( event->button() == Qt::RightButton ) {
      hkWidget->show();
      hkWidget->move( event->screenPos().x(), event->screenPos().y() );
      hkWidget->raise();
      event->accept();
    }
    else {
      QPushButton::mousePressEvent( event );
    }
  }// NoClickActive::mousePressEvent
  
 
  void NoClickActive::closeEvent(QCloseEvent *event) {
    hkWidget->close();
  }// NoClickActive::closeEvent
  

}
