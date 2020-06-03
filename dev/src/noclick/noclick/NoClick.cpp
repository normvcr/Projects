/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <QGuiApplication>
#include <QApplication>
#include <QBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QCursor>
#include <QScreen>
#include <QList>
#include <QDebug>
#include <QVector>
#include <QVector2D>
#include <QtMath>
#include <QAction>
#include <QCloseEvent>
#include <QSettings>
#include <QStatusBar>
#include "NoClick.hpp"
#include "GenArchiveIO.tpp"
#include "DragPane.hpp"
#include "NoClickPanesSquare.hpp"
#include "NoClickActive.hpp"
#include "NoClickSettings.hpp"
#include "NoClickKbd.hpp"
#include "NoClickDetailsWindow.hpp"
#include "NoClickPanesWindow.hpp"
#include "PanesSetSquare.hpp"
#include "PanesSetLine.hpp"

#include "BootUtil.h"

#include <stdlib.h>
#include <memory>
#include <iostream>
#include <sstream>
using namespace std;

#include "BootArch.h"
#if OS_IS_LINUX
#include <QWebView>
#else
#include <QWebEngineView>
#endif


namespace ssrd_mouse {
  NoClickKbd* noClickKeyboard = nullptr;
  
    // The settings used by this source file
  extern int labelInterval;
  extern int actionInterval;
  extern int mouseInterval;
  extern QColor dragStageColor;
  extern double starAxisAngle;
  extern int dragType;
  extern bool hotCursorShow;
  extern PanesType panesType;

  extern int starStationaryTimeCutoff;
  extern int starStationaryTimeAppear;
  extern int panesMovingTimeCutoff;
  extern int mouseStationaryThreshold;
  extern int dragStopDelay;
  extern int dragStopDelay2;  
}


namespace {
  // Is set to false when using line configuration
  bool useStarPane = true;

  // Depends on whether a 1-Stage or 2-Stage drag is in progress
  int currentDragStopDelay = 0;

  //------ Set by constructor
  double cosStarAxisAngle = 0.0;

  QPoint dragStartPoint;

  ssrd_mouse::NoClick* thisInstance = nullptr;
}


namespace ssrd_mouse {
  
  void starAxisAngleTrigger( double valDegrees ) {
    starAxisAngle = qDegreesToRadians( valDegrees );
    cosStarAxisAngle = qCos( starAxisAngle );
  }
  
  void NoClick::startPositionTimer( void ) {
    timer_->start( mouseInterval );
  }
  void NoClick::startActionTimer( void ) {
    timer2_->start( actionInterval );
  }

  NoClick::~NoClick(){
    disconnect( timer_.get(),   // QObject::disconnect
		&QTimer::timeout,
		this,
		&NoClick::checkMouse );

    disconnect( timer2_.get(),  
		&QTimer::timeout,
		this,
		&NoClick::checkAction );

    disconnect( radio1_,
		&QRadioButton::toggled,
		this,
		&NoClick::setDrag1 );
	     
    disconnect( radio2_,
		&QRadioButton::toggled,
		this,
		&NoClick::setDrag2 );

    disconnect( exitAction_,
		&QAction::triggered,
		this,
		&NoClick::triggerExit );
      
    disconnect( settingsAction_,
		&QAction::triggered,
		this,
		&NoClick::toggleSettingsHelpViewer );
      
    disconnect( aboutAction_,
		&QAction::triggered,
		this,
		&NoClick::toggleAboutHelpViewer );

    disconnect( noClickKeyboard,
		&NoClickKbd::kbdInterrupt,
		this,
		&NoClick::kbdInterrupt );


    DeletePointer( settingsHelpViewer_ );
    DeletePointer( aboutHelpViewer_ );
    //DeletePointer( kbd_ );
    DeletePointer( radio1_ );
    DeletePointer( radio2_ );
    DeletePointer( panesSet_ );
  }// NoClick::~NoClick
  

  void  NoClick::endDrag( void ) {
    hideAll();

    if( mouseIsStationary() ) {
      action_= StationaryEmpty;
    }
    else {
      action_ = MovingEmpty;
    }

    showActionMessage();

    sendMouseUp(1, dragStartPoint );

    QPalette palette = radio2_->palette();
    palette.setColor(QPalette::Window, Qt::white );
    radio2_->setPalette(palette);
    radio1_->setPalette(palette);
    //    clearFocus();
  }// NoClick::endDrag

  
  void NoClick::showActionMessage( void ) {

    switch( action_ ) {
    case Drag0:
      statusBar()->showMessage( "Stage-1 Drag" );
      break;
    case EDrag0:
      statusBar()->showMessage( "Stage-2 Drag" );
      break;
     case MovingEmpty:
      statusBar()->showMessage( "Moving Empty" );
      break;

    case StationaryEmpty:
      statusBar()->showMessage( "Stationary Empty" );
      break;
    case MovingPanes:
      statusBar()->showMessage( "Moving Panes" );
      break;
    case StationaryStar:
      statusBar()->showMessage( "Stationary Star" );
      break;

    case StationaryEmptyB:
      statusBar()->showMessage( "Stationary Empty in Stage-2 Drag" );
      break;
    case MovingPanesB:
      statusBar()->showMessage( "Moving Panes in Stage-2 Drag" );
      break;
    case StationaryStarB:
      statusBar()->showMessage( "Stationary Star in Stage-2 Drag" );
      break;

    default:
      break; // Too fleeting
    }

    static Action lastAction = action_;
    static int count = 0;

    if( lastAction == action_ ) {
      ++count;
    } else {
      //qInfo() << count << ", Action= " << toString( action_ ).c_str();
      lastAction = action_;
      count = 1;
    }
    
  }// NoClick::showActionMessage


  
  bool NoClick::mouseIsStationary( void ) {
    return stationaryDuration_ > mouseStationaryThreshold;
  }// NoClick::mouseIsStationary


  // 4 different configurations of click boxes, plus
  // 1 '*' 
  NoClick::NoClick( void ) :
    hotCursor_( this ),
    settingsHelpViewer_( new NoClickViewer() ),
    aboutHelpViewer_( new QLabel ),
    panesSet_( nullptr ),
    timer_( new QTimer( this ) ),
    timer2_( new QTimer( this ) ),
    radio1_(  new QRadioButton( "&1-Stage Drag" ) ),
    radio2_( new QRadioButton( "&2-Stage Drag" ) ) {

    assert( thisInstance == nullptr );
    thisInstance = this;

    cosStarAxisAngle = qCos( starAxisAngle );

    noClickKeyboard = new NoClickKbd();

    connect( noClickKeyboard, &NoClickKbdBase::kbdInterrupt,
	     this, &NoClick::kbdInterrupt );

    noClickKeyboard->start();

    qInfo() << "Panes type= " << toString( panesType ).c_str() << endl;
    setPanesSet( panesType );

    
    //------------ Set up OS communication
    QPoint ulDesktop(0,0);  // Expect stay at (0,0)
    QPoint lrDesktop(0,0);

    for( int ind = 0; ind < PanesSet::screenList.size(); ++ind ) {
      QRect qr = PanesSet::screenList[ ind ]->geometry();
      
      ulDesktop.rx() = std::min( ulDesktop.x(), qr.x() );
      ulDesktop.ry() = std::min( ulDesktop.y(), qr.y() );
      lrDesktop.rx() = std::max( lrDesktop.x(), qr.right() );
      lrDesktop.ry() = std::max( lrDesktop.y(), qr.bottom() );
    }
    qInfo() << "ulDesktop= " << ulDesktop;
    qInfo() << "lrDesktop" << lrDesktop;
    
    eventOS_.reset
      ( new ssrd_event::EventOS( ulDesktop.x(),
				 ulDesktop.y(),
				 lrDesktop.x() - ulDesktop.x(),
				 lrDesktop.y() - ulDesktop.y() ) );
    
    //---------- Populate the face of the main window
    //QGridLayout* gridLayout = new QGridLayout();


     //------- Radio buttons to choose drag type

    // Allow changing background colors
    radio1_->setBackgroundRole( QPalette::Window );
    radio1_->setAutoFillBackground(true);
    radio2_->setBackgroundRole( QPalette::Window );
    radio2_->setAutoFillBackground(true);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(radio1_  );
    vbox->addWidget( radio2_ );

    if( 1 == dragType ) {
      radio1_->setChecked(true);
    }
    else {
      radio2_->setChecked(true);
    }

    connect( radio1_,
	     &QRadioButton::toggled,
	     this,
	     &NoClick::setDrag1 );
	     
    connect( radio2_,
	     &QRadioButton::toggled,
	     this,
	     &NoClick::setDrag2 );
		     
    radioBox_ = new QGroupBox( "Drag Type",
			       nullptr );

    radioBox_->setAlignment( Qt::AlignHCenter | Qt::AlignBottom );

    radioBox_->setLayout(vbox);
    //gridLayout->addWidget( groupBox, 1, 1 );

   
    //---------- Set up hot cursor

    hotCursor_.setWindowFlags( Qt::Window 
			       | Qt::FramelessWindowHint
    			       | Qt::WindowStaysOnTopHint
			       | Qt::WindowDoesNotAcceptFocus );

    hotCursor_.setAttribute( Qt::WA_TranslucentBackground, true );

    //----------- Set up timers    
    connect( timer_.get(),   // QObject::connect
	     &QTimer::timeout,
	     this,
	     &NoClick::checkMouse );

    connect( timer2_.get(),  
	     &QTimer::timeout,
	     this,
	     &NoClick::checkAction );

    startPositionTimer();
    startActionTimer();

    //qInfo() << "Timer interval: " << mouseInterval;

    //-----------------------
    QMainWindow* mainW = createMainWindow();
    mainW->show();
  }// NoClick::NoClick


  bool NoClick::sendMouseMove( void ) {
    QPoint hotloc;
    getHotPoint( hotloc );

    return eventOS_->sendMouseMove( hotloc.x(),
				    hotloc.y() );
  }// NoClick:sendMouseMove


  bool NoClick::sendMouseDoubleClick( void ) {
    QPoint hotloc;
    getHotPoint( hotloc );

    return eventOS_->sendMouseDClick( hotloc.x(),
				      hotloc.y() );
  }// NoClick:sendMouseDoubleClick


  bool NoClick::sendMouseClick( int buttonNumber )  {
    QPoint hotloc;
    getHotPoint( hotloc );

    // qInfo() << hotloc;

    return eventOS_->sendMouseClick( buttonNumber,
				     hotloc.x(),
				     hotloc.y() );
  }// NoClick::sendMouseClick


  bool NoClick::sendMouseDown( int buttonNumber )  {
    QPoint hotloc;
    getHotPoint( hotloc );

    return eventOS_->sendMouseDown( buttonNumber,
				   hotloc.x(),
				   hotloc.y() );
  }// NoClick::sendMouseDown


  bool NoClick::sendMouseUp( int buttonNumber, const QPoint& pt ) {
    return eventOS_->sendMouseUp( buttonNumber,
				 pt.x(),
				  pt.y() );
  }// NoClick::sendMouseUp


  bool NoClick::sendMouseUp( int buttonNumber )  {
    QPoint pt;
    getHotPoint( pt );

    return sendMouseUp( buttonNumber, pt );
  }// NoClick::sendMouseUp


  void NoClick::showApparatus( bool flag ) {
      showApparatus_ = flag;
  }//NoClick::showApparatus


  void NoClick::hideAll( void ) {
    currentPanes()->hide();
    hotCursor_.hide();
  }// NoClick::hideAll
  

  void NoClick::setDrag1( void ) {
    if( radio1_->isChecked() ) {
      //      qInfo() << "Setting Drag1";
      dragType = 1;
    }
  }// NoClick::setDrag1


  void NoClick::setDrag2( void ) {
    if( radio2_->isChecked() ) {
      //     qInfo() << "Setting Drag2";
      dragType = 2;
    }
  }// NoClick::setDrag2
  

  void NoClick::checkAction( void ) {
    //cout << "In checkAction" << endl;

    if( !showApparatus_ ) {
      hideAll();
      return;
    }

    qDebug() << "State/Action= " << action_;

    int retval = 1;

    switch( action_ ) {

      // These 4 states are handled in checkMouse, so that
      // the red star is more responsive.
    case MovingEmpty:
    case StationaryStar:
    case StationaryStarB:
    case EDrag0:
      break;
	
    case StationaryEmpty:
      if( !mouseIsStationary() ) {
	action_ = MovingEmpty;
      }
      break;
	
    case StationaryEmptyB:
      if( !mouseIsStationary() ) {
	action_ = EDrag0;
      }
      break;
	
    case MovingPanesB:
    case MovingPanes:
      if( movingDuration_ > panesMovingTimeCutoff ) {
	hideAll();

	if( action_ == MovingPanesB ) {
	  action_ = EDrag0;
	}
	else {
	  action_ = MovingEmpty;
	}
	break;
      }
      movingDuration_ += timer2_->interval();
      if( mouseIsStationary() ) {
	QPoint pt = QCursor::pos();
	
	PaneID paneID = panesSet_->paneOf( pt );
	
	//qInfo() << "PaneID= " << paneID;
	
	if( NoPaneID != paneID ) {
	  
	  hideAll();
	  
	  if( action_ == MovingPanesB ) {
	    //	    if( DragPaneID == paneID  ) {
	      stationaryDuration_ = 0;
	      
	      QPalette palette = radio2_->palette();
	      palette.setColor(QPalette::Window, Qt::white );
	      radio2_->setPalette(palette);
	      palette.setColor(QPalette::Window, dragStageColor );
	      radio1_->setPalette(palette);
	      
	      retval = sendMouseMove();	      
	      action_ = Drag0;
	      //	    }
#if 0
	    else{ // Stationary, not in Drag
	      action_ = StationaryEmptyB;
	    }
#endif
	  }
	  else // MovingPanes
	    switch( paneID ) {
	    case DClickPaneID: 
	      action_ = DoubleClick;
	      break;
	    case RightClickPaneID: 
	      action_ = RightClick;
	      break;
	    case DragPaneID: 
	      action_ = ( dragType==1 ? Drag : EDrag );
	      break;
	    case LeftClickPaneID: 
	      action_ = LeftClick;
	      break;
	    }
	}
	else {// Stationary, not in the panes
	  hideAll();
	  action_ =
	    ( action_ == MovingPanes ? StationaryEmpty : StationaryEmptyB );
	}
      }
      break;
      
    case LeftClick:
      retval = sendMouseClick( 1 ); 
      recentPoint_ = QCursor::pos();//Make sure mouse is deemed stationary
      action_ = StationaryEmpty;
      break;
      
    case MiddleClick:
      retval = sendMouseClick( 2 );
      recentPoint_ = QCursor::pos();
      action_ = StationaryEmpty;
      break;
	  
    case RightClick:
      retval = sendMouseClick( 3 );
      recentPoint_ = QCursor::pos();
      action_ = StationaryEmpty;
      break;
	  
    case DoubleClick:
      retval = sendMouseDoubleClick();
      recentPoint_ = QCursor::pos();
      action_ = StationaryEmpty;
      break;
	  
    case Drag: {
      getHotPoint( dragStartPoint );
      //      setFocus( Qt::OtherFocusReason );
      retval = sendMouseDown( 1 );
      action_ = Drag0;
      currentDragStopDelay = dragStopDelay;
      noClickKeyboard->setState( KeyboardState::Drag );
      
      QPalette palette = radio1_->palette();
      palette.setColor(QPalette::Window, dragStageColor );
      radio1_->setPalette(palette);
      break;
    }
      
    case Drag0:
      if( stationaryDuration_ > currentDragStopDelay ) {
	noClickKeyboard->setState( KeyboardState::Normal );

	hotCursor_.setHotPoint( recentPoint_ );
	retval = sendMouseUp( 1 );
	QApplication::beep();
	
	recentPoint_ = QCursor::pos();
	action_ = StationaryEmpty;

	QPalette palette = radio1_->palette();
	palette.setColor(QPalette::Window, Qt::white );
	radio1_->setPalette(palette);
      }
      break;
	  
    case EDrag:
      getHotPoint( dragStartPoint );
      //      setFocus( Qt::OtherFocusReason );
      //qInfo() << "NoClick::checkAction: In EDrag";
      //      radio2_->setStyleSheet( "background-color: blue" );

      QPalette palette = radio2_->palette();
      palette.setColor(QPalette::Window, dragStageColor );
      radio2_->setPalette(palette);
      
      retval = sendMouseDown( 1 );
      action_ = EDrag0;
      currentDragStopDelay = dragStopDelay2;
      noClickKeyboard->setState( KeyboardState::Drag );
      break;
	  
    }// switch
	
    showActionMessage();
    
    if( !retval) {
      cerr << "ClickWindows::checkAction: Error from Action"
	   << endl;
	  
      exit(-1);
    }
  }// NoClick::checkAction
  

  void NoClick::checkMouse( void ) {
    QPoint pt = QCursor::pos();

    //qInfo() << "\ncheckMouse pt= " << pt;

    if( pt == recentPoint_ ) {
      stationaryDuration_ += timer_->interval();

      if( (action_ == MovingEmpty) &&
	  (stationaryDuration_ > starStationaryTimeAppear) ) {

	if( useStarPane ) {
	  action_ = StationaryStar;
	} else {
	  action_ = MovingPanes;
	  movingDuration_ = 0;
	  stationaryDuration_ = 0;
	}

	showPanes( useStarPane );
      }
      else if( ( action_ == StationaryStar ) &&
	       ( stationaryDuration_ > starStationaryTimeCutoff ) ) {
	starPane()->hide();
	hotCursor_.hide();
	action_ = StationaryEmpty;
      }
      else if( action_ == EDrag0 ) {
	if( stationaryDuration_ > starStationaryTimeAppear ) {
	  
	  if( useStarPane ) {
	    action_ = StationaryStarB;
	  } else {
	    action_ = MovingPanesB;
	    movingDuration_ = 0;
	    stationaryDuration_ = 0;
	  }
	  
	  showPanes( useStarPane );
	}
      }
      else if( (action_ == StationaryStarB) &&
	       (stationaryDuration_ > starStationaryTimeCutoff) ) {
	  starPane()->hide();
	  hotCursor_.hide();
	  recentPoint_ = QCursor::pos();
	  action_ = StationaryEmptyB;
      }
    }
    else { // Moving
      if( (action_ == StationaryStar)
	  || (action_ == StationaryStarB) ) {
	// Check whether the Star pane is being displayed.
	// If so, we check if we should hide it.
	  /*            	
        star     pt
  	   \ a  /
 	 v0 \  /v1
	     \/
	recent Pt
        
	We remove the star pane if the angle, a, between 
	v0 and v1, is less than the starAxisAngle.
	  */

	QVector2D v0( starPane()->pos() - recentPoint_ );
	QVector2D v1( pt - recentPoint_ );

	if( ( QVector2D::dotProduct( v0, v1 ) >
	      cosStarAxisAngle * v0.length() * v1.length() ) ) {
	  
	  showPanes( false );
	  movingDuration_ = 0;
	  action_ =
	    ( action_==StationaryStar
	      ? MovingPanes
	      : MovingPanesB );
	}
	else { // Big angle
	  starPane()->hide();
	  hotCursor_.hide();
	  action_ =
	    (  action_==StationaryStar ? MovingEmpty : EDrag0 );
	}
      }// StationaryStar
      
      recentPoint_ = pt;
      stationaryDuration_ = 0;
    }// Moving

    showActionMessage();
  }// NoClick::checkMouse

  
  void NoClick::setPanesSet( PanesType pt ) {
    const bool oldShow = showingApparatus();
    showApparatus( false );
    DeletePointer( panesSet_ );
    if( pt == LinePanesType ) {
      panesSet_ = new PanesSetLine();
      panesType = LinePanesType;
      useStarPane = false;
    }
    else {
      panesSet_ = new PanesSetSquare();
      panesType = SquarePanesType;
      useStarPane = true;
    }
    showApparatus( oldShow );
  }// NoClick::setPanesSet


  void NoClick::showPanes( bool allowStarPane ) {
    //qInfo() << "Kbd state= " << (int)noClickKeyboard->state();
    //qInfo() << "showPanes, showStar= " << allowStarPane;

    if( !showingApparatus() ) return;

    QPoint pt = recentPoint_;

    qDebug() << "pt= " << pt;

    if( allowStarPane || !useStarPane ) {
      hotCursor_.setHotPoint( pt );
    }

    panesSet_->adjustPanes( pt,
			    allowStarPane );

    currentPanes()->clearFocus();

    if( hotCursorShow ) {
      hotCursor_.show();
      hotCursor_.raise();
    }
    //currentPanes()->activateWindow();
  }// NoClick::showPanes


  void NoClick::closeEvent(QCloseEvent *event) {
    // QT bug?
    static int numCalled = 0;
    if (numCalled++ > 0) return;

    showApparatus( false );

    settingsHelpViewer_->close();
    aboutHelpViewer_->close();
    hotCursor_.close();
    showHideW_->close();
    detailsWindow_->close();
    panesWindow_->close();
    panesSet_->shutDown();

    shutDownMainWindow();

    qInfo() << "In NoClick::closeEvent";
    ssrd_mouse::writeSettings();
    if( nullptr != event ) event->accept();
  }// NoClick::closeEvent
  

  void NoClick::kbdInterrupt( int flag ) {
    // qInfo() << "NoClick::kbdInterrupt flag= "  << (int)flag;

    switch( flag ) {
    case NoClickKbdResult::EndDrag:
      dragStartPoint = QCursor::pos();
      hotCursor_.setHotPoint( dragStartPoint );
      // Fall through
    case NoClickKbdResult::CancelDrag:
      //sleepms(1000);
      endDrag();
      break;
      
    case NoClickKbdResult::Error:
      qCritical() << "Keyboard failure.  Closing down";
      close();
      break;

    case NoClickKbdResult::ToggleShow:
      showHideW_->stationaryAction();
      break;      
    }
  }// NoClick::kbdInterrupt
  

  // Static
  NoClick* NoClick::instance( void ) {
    return thisInstance;
  }// NoClick::instance
  

  void NoClick::refreshShowHide( void) {
    showHideW_->stationaryAction( false );
  }// NoClick::refreshShowHide
 
  void NoClick::restartShowHideIfRunning( void ) {
    showHideW_->restartTimerIfRunning();
  }// NoClick::restartShowHideIfRunning

}// namespace ssrd_mouse
