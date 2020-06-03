/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "AnchoredSpinBox.tpp"
#include "TriggeredSpinBox.tpp"
#include "NoClickDetailsWindow.hpp"
#include <QAction>
#include <QLabel>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QFormLayout>
#include "NoClick.hpp"

namespace ssrd_mouse {
  extern int mouseInterval;
  extern int actionInterval;
  extern int labelInterval;
  
  extern int panesMovingTimeCutoff;
  //extern int panesStationaryTimeCutoff;
  extern int starStationaryTimeCutoff;
  extern int starStationaryTimeAppear;
  extern int mouseStationaryThreshold;
  extern int dragStopDelay;
  extern int dragStopDelay2;

  void mouseIntervalTrigger( int val ) {
    mouseInterval = val;
    NoClick::instance()->startPositionTimer();
  }
  
  void actionIntervalTrigger( int val ) {
    actionInterval = val;
    NoClick::instance()->startActionTimer();
  }
  
  void labelIntervalTrigger( int val ) {
    labelInterval = val;
    NoClick::instance()->restartShowHideIfRunning();
  }
  
  extern int dragType;

  NoClickDetailsWindow::NoClickDetailsWindow( QAction* detailsAction ) :
    detailsAction_( detailsAction ),
    gridLayout_( new QGridLayout() )
  {
    setLayout( gridLayout_ );
    setSizePolicy
	( QSizePolicy( QSizePolicy::Minimum,
		       QSizePolicy::Minimum,
		       QSizePolicy::DefaultType ) );

    //-------- Samplers
    {
      auto formLayout = new QFormLayout;

      QGroupBox* samplersBox =
	new QGroupBox( "Periods [ms]", this );
      samplersBox->setAlignment( Qt::AlignCenter );
      samplersBox->setLayout( formLayout );

      samplersBox->setToolTip(
R"(The periods (inverse frequencies) for sampling aspects
of the NoClick system)" );

      //Position
      auto sb = ssrd_gui::CreateTriggeredSpinBox( mouseInterval,
						  mouseIntervalTrigger );
       formLayout->addRow( "Position", sb );

      formLayout->labelForField( sb )->	setToolTip(
R"(The period for querying the mouse position.  
Also the period for dealing with actions involving the red star, 
for the sake of responsiveness.)" );
      
      // Action
      sb = ssrd_gui::CreateTriggeredSpinBox( actionInterval,
					     actionIntervalTrigger );
      formLayout->addRow( "Action", sb );

      formLayout->labelForField( sb )->	setToolTip(
R"(The period at which actions, not involving the Red Star, 
are handled.)"  );

      // Button
      sb = ssrd_gui::CreateTriggeredSpinBox( labelInterval,
					    labelIntervalTrigger );
      formLayout->addRow( "Hover", sb );
      
      formLayout->labelForField( sb )->	setToolTip(
R"(The period at which the button Hide/Show
is checked for the mouse being stationary in the button, 
when hovering is allowed.)"  );

      
      gridLayout_->addWidget( samplersBox, 0, 0, 1, 1 );
    }

    //-- Intervals
    {
      auto formLayout = new QFormLayout;
      QGroupBox* otherBox = new QGroupBox( "Durations [ms]", this );
      otherBox->setAlignment( Qt::AlignCenter );
      otherBox->setLayout( formLayout );

      otherBox->setToolTip(
R"(Time durations that effect state change)" );

      // Star stationary hide
      {
	auto sb =
	  ssrd_gui::CreateAnchoredSpinBox( starStationaryTimeCutoff );
      
	formLayout->addRow( "Star stationary hide", sb );

	formLayout->labelForField( sb )->setToolTip(
R"(How long the mouse must be stationary
for the Red Star to disappear.)" );
      }
             
      // Star stationary appear
      {
	auto sb =
	  ssrd_gui::CreateAnchoredSpinBox( starStationaryTimeAppear );
      
	formLayout->addRow( "Star stationary appear", sb );

	formLayout->labelForField( sb )->setToolTip(
R"(How long the mouse must be stationary
for the Red Star to appear.)" );
      }      
        
      // Apparatus moving hide
      {
	auto sb =
	  ssrd_gui::CreateAnchoredSpinBox( panesMovingTimeCutoff );
	
	formLayout->addRow( "Apparatus moving hide", sb );

	formLayout->labelForField( sb )->setToolTip(
R"(The time interval, while the mouse is moving,
after which the Panes and Star will be hidden.)" );
      }
      
      // Mouse stationary
      {
	auto sb =
	  ssrd_gui::CreateAnchoredSpinBox( mouseStationaryThreshold );
      
	formLayout->addRow( "Mouse staationary", sb );

	formLayout->labelForField( sb )->setToolTip(
R"(How long the mouse must be stationary, 
for the algorithms to deem the mouse to be stationary.)" );
      }
      
      // Drag stop delay
      {
	auto sb = ssrd_gui::CreateAnchoredSpinBox( dragStopDelay );

	formLayout->addRow( "Drag stop delay", sb );

	formLayout->labelForField( sb )->setToolTip(
R"(How long the mouse must be stationary for a 1-stage drag to end)" );
      }
      
        
      // Drag stop delay2
      {
	auto sb = ssrd_gui::CreateAnchoredSpinBox( dragStopDelay2 );

	formLayout->addRow( "Drag stop delay2", sb );

	formLayout->labelForField( sb )->setToolTip(
R"(How long the mouse must be stationary for the 
2nd stage of a 2-stage drag to end)" );
      }
      
        
      gridLayout_->addWidget( otherBox, 0, 1, 2, 1 );
    }

    connect( detailsAction_,
	     &QAction::triggered,
	     this,
	     &NoClickDetailsWindow::makeVisible );
  }// NoClickDetailsWindow
  

  NoClickDetailsWindow::~NoClickDetailsWindow() {
    disconnect( detailsAction_,
		&QAction::triggered,
		this,
		&NoClickDetailsWindow::makeVisible );

  }// ~NoClickDetailsWindow


  void NoClickDetailsWindow::makeVisible( bool checked ) {
    QPoint p = pos();
    hide();
    move( p );
    show();
    setWindowState(Qt::WindowState::WindowActive);
}// makeVisible

  void NoClickDetailsWindow::closeEvent(QCloseEvent *event) {
  }// closeEvent
  
}
