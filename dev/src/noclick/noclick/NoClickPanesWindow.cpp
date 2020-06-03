/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "BootUtil.h"
#include "AnchoredSpinBox.tpp"
#include "TriggeredSpinBox.tpp"
#include "NoClickPanesWindow.hpp"
#include "NoClickUtils.hpp"
#include "NoClick.hpp"
#include <QMouseEvent>
#include <QPushButton>
#include <QColorDialog>
#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QtMath>

namespace {
  QPushButton* borderColorLabel = nullptr;
  QColorDialog* borderColorDialog = nullptr;

  class BorderColorButton : public QPushButton {
    using QPushButton::QPushButton;

  protected:
    void mouseReleaseEvent(QMouseEvent *e) override;
  };

  void BorderColorButton::mouseReleaseEvent(QMouseEvent *e) {
    auto mouseEvent = (QMouseEvent*)e;

    switch( mouseEvent->button() ) {
    case Qt::RightButton: {
      borderColorDialog->hide();
      borderColorDialog->show();
      break;
    }
    default: {
      QPushButton::mouseReleaseEvent( e );
      break;
    }
    }
  }// BorderColorButton::mouseReleaseEvent
}

namespace ssrd_mouse {

  // On right side of window
  // Linear
  extern int panesLineOffset;
  extern QColor panesBorderColor;
  extern double panesBorderOpacity;
  extern int panesBorderWidth;

  // On left side of window
  // Click Panes (Suare and Linear)
  extern int hotPointOffset;  // Distance of hot point to click panes
  extern int panesSideLength;

  // Star
  extern int starOffset;  // Distance of hot point to star
  extern int starSideLength;
  extern double starAxisAngle;
  extern void (starAxisAngleTrigger)(double);

  void panesBorderOpacityTrigger( double val ) {
    panesBorderOpacity = val;
    NoClick::instance()->resetPanesSet();
    reset( borderColorLabel,
	   panesBorderColor,
	   panesBorderOpacity );
  }// panesBorderOpacityTrigger

  void panesSideLengthTrigger( int val ) {
    panesSideLength = val;
    NoClick::instance()->resetPanesSet();
  }
  void hotPointOffsetTrigger( int val ) {
    hotPointOffset = val;
    NoClick::instance()->resetPanesSet();
  }
  void starOffsetTrigger( int val ) {
    starOffset = val;
    NoClick::instance()->resetPanesSet();
  }
  void starSideLengthTrigger( int val ) {
    starSideLength = val;
    NoClick::instance()->resetPanesSet();
  }
  void panesLineOffsetTrigger( int val ) {
    panesLineOffset = val;
    NoClick::instance()->resetPanesSet();
  }
  void panesBorderWidthTrigger( int val ) {
    panesBorderWidth = val;
    NoClick::instance()->resetPanesSet();
  }


  
  NoClickPanesWindow::NoClickPanesWindow( QAction* panesAction ) :
    panesAction_( panesAction ),
    gridLayout_( new QGridLayout() )
  {
    borderColorDialog = new QColorDialog;
    borderColorDialog->setCurrentColor( panesBorderColor );
    connect( borderColorDialog,
	     &QColorDialog::currentColorChanged,
	     this,
	     &NoClickPanesWindow::borderColorChanged );

    setLayout( gridLayout_ );

    //---------------- All click panes
    {
      auto formLayout = new QFormLayout;

      QGroupBox* panesBox =
	new QGroupBox( "Click Panes", this );
      panesBox->setAlignment( Qt::AlignCenter );
      panesBox->setLayout( formLayout );
      panesBox->setToolTip(
R"(Settings that apply to all configurations of panes)" );

      //Hot Point offset
      {
	auto sb = ssrd_gui::CreateTriggeredSpinBox( hotPointOffset,
						    hotPointOffsetTrigger );
	hideToolTip( sb );

	formLayout->addRow( "Hot Point offset [pix]", sb );
	formLayout->labelForField( sb )->setToolTip(
R"(The distance of the Click Panes from the Hot Point)" );
      }

      //Panes side length
      {
	auto sb = ssrd_gui::CreateTriggeredSpinBox( panesSideLength,
						    panesSideLengthTrigger );
	hideToolTip( sb );

	formLayout->addRow( "Pane side length [pix]", sb );
	formLayout->labelForField( sb )->setToolTip(
R"(The length of each side of a click pane)" );
	}

      gridLayout_->addWidget( panesBox, 0, 0 );
    }

     //------------------- Star
    {
      auto formLayout = new QFormLayout;

      QGroupBox* starBox =
	new QGroupBox( "Star", this );
      starBox->setAlignment( Qt::AlignCenter );
      starBox->setLayout( formLayout );
      starBox->setToolTip(
R"(Settings that apply to the Star)" );

      //Hot Point offset
      {
	auto sb = ssrd_gui::CreateTriggeredSpinBox( starOffset,
						    starOffsetTrigger);
	hideToolTip( sb );	

	formLayout->addRow( "Hot Point offset [pix]", sb );
	formLayout->labelForField( sb )->	setToolTip(
R"(The distance of the Star from the Hot Point)" );
      }
      
      //Star side length
      {
	auto sb = ssrd_gui::CreateTriggeredSpinBox( starSideLength,
						    starSideLengthTrigger );

	hideToolTip( sb );

	formLayout->addRow( "Star side length [pix]", sb );
	formLayout->labelForField( sb )->	setToolTip(
R"(The length of each side of the Star)" );
      }
      
      //Star axis angle
      {
	auto sb =
	  ssrd_gui::CreateTriggeredSpinBox( qRadiansToDegrees( starAxisAngle ),
					    starAxisAngleTrigger );
	hideToolTip( sb );	

	formLayout->addRow( "Star axis angle [deg]", sb );
	formLayout->labelForField( sb )->	setToolTip(
R"(The half angle of the cone toward the Click Panes,
based at the Star)" );
      }

      gridLayout_->addWidget( starBox, 1, 0 );
    }

    //---------- Linear
    {
      auto formLayout = new QFormLayout;

      QGroupBox* linearBox =
	new QGroupBox( "Linear Click Panes: Drag Left Double Right", this );
      linearBox->setAlignment( Qt::AlignCenter );
      linearBox->setLayout( formLayout );
      linearBox->setToolTip(
R"(Settings that apply to Linear Click Panes, only)" );

      // offset
      {
	qInfo() << "panesLineOffset= " << panesLineOffset;

	auto sb = ssrd_gui::CreateTriggeredSpinBox( panesLineOffset,
						    panesLineOffsetTrigger );
	hideToolTip( sb );	

	formLayout->addRow( "Stagger [pix]", sb );
	formLayout->labelForField( sb )->setToolTip(
R"(The amount the two centre panes are offset from the two end panes)" );
      }

      // border width
      {
	auto sb = ssrd_gui::CreateTriggeredSpinBox( abs( panesBorderWidth ),
						    panesBorderWidthTrigger );
	hideToolTip( sb );

	formLayout->addRow( "Border Width [pix]", sb );

	formLayout->labelForField( sb )->setToolTip(
R"(The width of the border around each linear pane)" );
      }

      // border colour
      {
	borderColorLabel = new BorderColorButton;

	reset( borderColorLabel,
	       panesBorderColor,
	       panesBorderOpacity );

	if( 0 > panesBorderWidth ) {
	  borderColorLabel->setText("X");
	} else {
	  borderColorLabel->setText("");
	}

	formLayout->addRow( "Border Color", borderColorLabel );

	hideToolTip( borderColorLabel );       
				     
	formLayout->labelForField( borderColorLabel )->setToolTip(
R"(The color of all the linear pane boundaries,
when this color button is checked)" );
	
	connect( borderColorLabel,
		 &QAbstractButton::clicked,
		 this,
		 &NoClickPanesWindow::borderColorButtonClicked );
      }

      // border opacity
      {
	auto sb = ssrd_gui::CreateTriggeredSpinBox( panesBorderOpacity,
						    panesBorderOpacityTrigger );
	hideToolTip( sb );	
	
	formLayout->addRow( "Border Opacity", sb );
	
	formLayout->labelForField( sb )->setToolTip(
R"(The opacity of the border around each linear pane)" );
      }

      gridLayout_->addWidget( linearBox, 0, 1, 2, 1 );
    }

    connect( panesAction_,
	     &QAction::triggered,
	     this,
	     &NoClickPanesWindow::makeVisible );

  }// NoClickPanesWindow::NoClickPanesWindow

  
  NoClickPanesWindow::~NoClickPanesWindow() {

    disconnect( borderColorLabel,
		&QAbstractButton::clicked,
		this,
		&NoClickPanesWindow::borderColorButtonClicked );
     
    disconnect( panesAction_,
		&QAction::triggered,
		this,
		&NoClickPanesWindow::makeVisible );

  }// ~NoClickDetailsWindow
  
  
  void NoClickPanesWindow::makeVisible( bool checked ) {
    QPoint p = pos();
    hide();
    move( p );
    show();
    setWindowState(Qt::WindowState::WindowActive);
  }// makeVisible

  void NoClickPanesWindow::borderColorButtonClicked( bool checked ) {
    if( borderColorLabel->text()[0] == '\0' ) {
      panesBorderWidth = -abs( panesBorderWidth );
      borderColorLabel->setText("X");
    }
    else {
      panesBorderWidth = abs( panesBorderWidth );
      borderColorLabel->setText("");
    }

    NoClick::instance()->resetPanesSet();
  }// borderColorButtonClicked

  
  void NoClickPanesWindow::borderColorChanged( const QColor& color ) {
    panesBorderColor = color;

    reset( borderColorLabel,
	   color,
	   panesBorderOpacity );

    if( borderColorLabel->text()[0] != '\0' ) {
      NoClick::instance()->resetPanesSet();
    }
  }// NoClickPanesWindow::borderColorChanged
  
  void NoClickPanesWindow::closeEvent( QCloseEvent* ev ) {
    borderColorDialog->close();
  }


}// ssrd_mouse
