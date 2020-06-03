/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickLabel.hpp"
#include "NoClick.hpp"
#include <QCloseEvent>
#include <QDebug>
#include <memory>
using namespace std;

namespace ssrd_mouse {

  extern int labelInterval;
  extern bool labelAllowHover;
  
  NoClickLabel::NoClickLabel( const QString &text,
			      QWidget *parent,
			      Qt::WindowFlags f  ) :
    //    QLabel( text, parent, f ),
    QPushButton( text, parent ),
    timer_( new QTimer( this ) )
  {
    //setWindowFlags( f );
    setCheckable( false );
    
    connect( timer_.get(),   // QObject::connect
	     &QTimer::timeout,
	     this,
	     &NoClickLabel::checkStationary );
    
    connect( static_cast< QAbstractButton* >( this ),
	     &QAbstractButton::clicked,
	     this,
	     &NoClickLabel::clicked );
  }// NoClickLabel::NoClickLabel
  

  NoClickLabel::~NoClickLabel() {
    disconnect( timer_.get(),   // QObject::disconnect
		&QTimer::timeout,
		this,
		&NoClickLabel::checkStationary );
    
    disconnect( static_cast< QAbstractButton* >( this ),
		&QAbstractButton::clicked,
		this,
		&NoClickLabel::clicked );
  }// NoClickLabel::~NoClickLabel
  

  void NoClickLabel::checkStationary( void ) {
    if( mainWindow()->mouseIsStationary() &&
	labelAllowHover ) {
      haltTimer(); // Stability
      stationaryAction();
    }
  }// NoClickLabel::checkStationary
  

  void NoClickLabel::haltTimer( void ) {
    timer_->start( 1<<30 );  // milliseconds
  }// NoClickLabel::haltTimer
  
  
  void NoClickLabel::startTimer( void ) {
    timer_->start( labelInterval );  // milliseconds
  }// NoClickLabel::startTimer
  
  
  void NoClickLabel::restartTimerIfRunning( void ) {
    if( timer_->isActive() ) {
      startTimer(); // Pulls in new value of labelInterval
    }
  }// NoClickLabel::reStartTimerIfRunning
  
  
  void NoClickLabel::enterEvent( QEvent *event ) {
    event->accept();
    showingApparatus_ = mainWindow()->showingApparatus();

    if( !showingApparatus_ && labelAllowHover ) {
       startTimer();  // milliseconds
    }

  }// NoClickLabel::enterEvent


  void NoClickLabel::leaveEvent( QEvent *event ) {
    event->accept();

    if( labelAllowHover ) {
      haltTimer();
      mainWindow()->showApparatus( showingApparatus_ );
    }
    
  }// NoClickLabel::leaveEvent

  
  void NoClickLabel::clicked( bool checked ) {
    stationaryAction();
  }// NoClickLabel::clicked
  

  NoClick* NoClickLabel::mainWindow( void ) {
    return NoClick::instance();
  }
  

}// ssrd_mouse
