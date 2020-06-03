/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "AnchoredSpinBox.tpp"
#include "NoClick.hpp"
#include "NoClickUtils.hpp"
#include "NoClickActive.hpp"
#include "BootUtil.h"
#include "NoClickDetailsWindow.hpp"
#include "NoClickPanesWindow.hpp"
#include "QMainWindow"
#include "PanesSet.hpp"
#include "QCoreApplication"
#include "QGridLayout"
#include "QGroupBox"
#include <QPainter>
#include <QMenuBar>
#include "NoClickQMenu.hpp"
#include <QMouseEvent>
#include <QTextStream>
#include <QRadioButton>
#include <QPushButton>
#include <QSettings>
#include <QColorDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QtMath>

#include "BootArch.h"
#if OS_IS_LINUX
#include <QWebView>
#else
#include <QWebEngineView>
#endif

  
namespace ssrd_mouse {
  extern PanesType panesType;
  extern double panesOpacity;
  extern double starOpacity;

  extern QColor panesLeftClickColor;
  extern QColor panesRightClickColor;
  extern QColor panesDClickColor;
  extern QColor panesDragColor;
  extern QColor starColor;

  extern bool labelAllowHover;

namespace {
  class ColoursBoxWidget* coloursBox_ = nullptr;
  
  QDoubleSpinBox* starOpacityField = nullptr;
  QPushButton* starLabel = nullptr;
  QColorDialog* starColorDialog = nullptr;
  
  QDoubleSpinBox* panesOpacityField = nullptr;


  QGroupBox* panesTypeBox_ = nullptr;
  QRadioButton* squareButton = nullptr;
  QRadioButton* lineButton = nullptr;
  QPushButton* hoverButton = nullptr;

  

  class ColoursBoxWidget : public QGroupBox {
  public:
    ~ColoursBoxWidget();
    ColoursBoxWidget( const QString &title, QWidget *parent = nullptr);

    void reset( void );

  private:

    QPushButton* dclickLabel_ = nullptr;
    QPushButton* rclickLabel_ = nullptr;
    QPushButton* dragLabel_ = nullptr;
    QPushButton* lclickLabel_ = nullptr;

    QColorDialog* dclickColorDialog_ = nullptr;
    QColorDialog* rclickColorDialog_ = nullptr;
    QColorDialog* dragColorDialog_ = nullptr;
    QColorDialog* lclickColorDialog_ = nullptr;

    void resetPanesSet( void ) {
      return NoClick::instance()->resetPanesSet();
    }

    void dclickLabelClicked(bool checked ) {
      dclickColorDialog_->setCurrentColor( panesDClickColor );
      //      dclickColorDialog_->setGeometry( 500, 500, width(), height() );
      dclickColorDialog_->show();      
     }
    void dclickColored( const QColor& color) {
      panesDClickColor = color;
      ssrd_mouse::reset( dclickLabel_, color, panesOpacity );
      resetPanesSet();
    }
    
    void lclickLabelClicked(bool checked ) {
      lclickColorDialog_->setCurrentColor( panesLeftClickColor );
      lclickColorDialog_->show();      
    }
    void lclickColored( const QColor& color) {
      panesLeftClickColor = color;
      ssrd_mouse::reset( lclickLabel_, color, panesOpacity );
      resetPanesSet();
    }
    
    void rclickLabelClicked(bool checked ) {
      rclickColorDialog_->setCurrentColor( panesRightClickColor );
      rclickColorDialog_->show();      
    }
    void rclickColored( const QColor& color) {
      panesRightClickColor = color;
      ssrd_mouse::reset( rclickLabel_, color, panesOpacity );
      resetPanesSet();
    }
    
    void dragLabelClicked(bool checked ) {
      dragColorDialog_->setCurrentColor( panesDragColor );
      dragColorDialog_->show();      
    }
    void dragColored( const QColor& color) {
      panesDragColor = color;
      ssrd_mouse::reset( dragLabel_, color, panesOpacity );
      resetPanesSet();
    }
  };

  ColoursBoxWidget::ColoursBoxWidget( const QString &title,
				      QWidget *parent ) :
    QGroupBox( title, parent ),
    dclickLabel_( createColorLabel( "Double Click",
				    panesDClickColor,
				    panesOpacity ) ),
    rclickLabel_( createColorLabel( "Right Click",
				    panesRightClickColor,
				    panesOpacity ) ),
    dragLabel_( createColorLabel( "Drag",
				  panesDragColor,
				  panesOpacity ) ),
    lclickLabel_( createColorLabel( "Left Click",
				    panesLeftClickColor,
				    panesOpacity ) )
    ,
    dclickColorDialog_( new QColorDialog( NoClick::instance() ) ),
    rclickColorDialog_( new QColorDialog( NoClick::instance() ) ),
    dragColorDialog_( new QColorDialog( NoClick::instance() ) ),
    lclickColorDialog_( new QColorDialog( NoClick::instance()  ) )
  {
    dclickLabel_->setToolTip(
R"(Left click to change the colour 
of the Double Click pane)" );

    rclickLabel_->setToolTip(
R"(Left click to change the colour 
of the Right Click pane)" );

    dragLabel_->setToolTip(
R"(Left click to change the colour 
of the Drag pane)" );

    lclickLabel_->setToolTip(
R"(Left click to change the colour 
of the Left Click pane)" );

    dclickColorDialog_->setWindowTitle( "Select DoubleClick Color" );
    rclickColorDialog_->setWindowTitle( "Select RightClick Color" );
    lclickColorDialog_->setWindowTitle( "Select LeftClick Color" );
    dragColorDialog_->setWindowTitle( "Select Drag Color" );

    QGridLayout* coloursLayout = new QGridLayout;
    coloursLayout->setSpacing(0);
    setAlignment( Qt::AlignCenter );
    setLayout( coloursLayout );

    coloursLayout->addWidget( dclickLabel_, 0, 0 );
    coloursLayout->addWidget( rclickLabel_, 0, 1 );
    coloursLayout->addWidget( dragLabel_, 1, 0 );
    coloursLayout->addWidget( lclickLabel_, 1, 1 );

    connect( dclickLabel_,
	     &QAbstractButton::clicked,
	     this,
	     &ColoursBoxWidget::dclickLabelClicked );
    connect( dclickColorDialog_,
	     &QColorDialog::colorSelected,
	     this,
	     &ColoursBoxWidget::dclickColored );
     
    connect( lclickLabel_,
	     &QAbstractButton::clicked,
	     this,
	     &ColoursBoxWidget::lclickLabelClicked );
    connect( lclickColorDialog_,
	     &QColorDialog::colorSelected,
	     this,
	     &ColoursBoxWidget::lclickColored );
     
    connect( rclickLabel_,
	     &QAbstractButton::clicked,
	     this,
	     &ColoursBoxWidget::rclickLabelClicked );
    connect( rclickColorDialog_,
	     &QColorDialog::colorSelected,
	     this,
	     &ColoursBoxWidget::rclickColored );
     
    connect( dragLabel_,
	     &QAbstractButton::clicked,
	     this,
	     &ColoursBoxWidget::dragLabelClicked );
    connect( dragColorDialog_,
	     &QColorDialog::colorSelected,
	     this,
	     &ColoursBoxWidget::dragColored );

 }// ColoursBoxWidget::ColoursBoxWidget


  ColoursBoxWidget::~ColoursBoxWidget() {
    disconnect( dclickLabel_,
		&QAbstractButton::clicked,
		this,
		&ColoursBoxWidget::dclickLabelClicked );

    disconnect( rclickLabel_,
		&QAbstractButton::clicked,
		this,
		&ColoursBoxWidget::rclickLabelClicked );

     disconnect( lclickLabel_,
		&QAbstractButton::clicked,
		this,
		&ColoursBoxWidget::lclickLabelClicked );

    disconnect( dragLabel_,
		&QAbstractButton::clicked,
		this,
		&ColoursBoxWidget::dragLabelClicked );
   #if 0
    // These cause core dumps
    disconnect( dclickColorDialog_,
		&QColorDialog::colorSelected,
		this,
		&ColoursBoxWidget::dclickColored );

    disconnect( lclickColorDialog_,
		&QColorDialog::colorSelected,
		this,
		&ColoursBoxWidget::lclickColored );
     
    disconnect( rclickColorDialog_,
		&QColorDialog::colorSelected,
		this,
		&ColoursBoxWidget::rclickColored );
   
    disconnect( dragColorDialog_,
		&QColorDialog::colorSelected,
		this,
		&ColoursBoxWidget::dragColored );
 #endif    
  }// ColoursBoxWidget::~ColoursBoxWidget

  
  void ColoursBoxWidget::reset( void ) {
    ssrd_mouse::reset( dclickLabel_, panesDClickColor, panesOpacity );
    ssrd_mouse::reset( rclickLabel_, panesRightClickColor, panesOpacity );
    ssrd_mouse::reset( dragLabel_, panesDragColor, panesOpacity );
    ssrd_mouse::reset( lclickLabel_, panesLeftClickColor, panesOpacity );
  }// ColoursBoxWidget::reset

}// anonymous


  void NoClick::squareClicked(bool checked ) {
    //qInfo() << "squareClicked";
    setPanesSet( SquarePanesType );
    //    panesTypeBox_->hide();
  }// NoClick::squareClicked
  
  void NoClick::lineClicked(bool checked ) {
    //qInfo() << "lineClicked";
    setPanesSet( LinePanesType );
    //    panesTypeBox_->hide();
   }// NoClick::lineClicked

  void NoClick::starOpacityChanged( double opacity ) {
    reset( starLabel, starColor, opacity );
    resetPanesSet();
  }// NoClick::starOpacityChanged
  
  void NoClick::panesOpacityChanged( double opacity ) {
    coloursBox_->reset();
    
    resetPanesSet();
  }// NoClick::starOpacityChanged
  

  void NoClick::hoverButtonToggled( bool checked ) {
    labelAllowHover = checked;
  }// NoClick::hoverButtonToggled


  void NoClick::starLabelClicked(bool checked ) {

    if( !starColorDialog->isVisible() ) {
      starColorDialog->setCurrentColor( starColor );
      starColorDialog->show();
    }
    else {
      starColorDialog->raise();
    }

  }//NoClick::starLabelClicked
  
  void NoClick::starLabelColored(const QColor &color) {
    starColor = color;
    reset( starLabel, color, starOpacity );
    resetPanesSet();
  }// NoClick::starLabelColored
  

  QGroupBox* createPanesTypeWidget( void ) {
    QBoxLayout* layout =
      new QBoxLayout( QBoxLayout::Direction::TopToBottom );

    squareButton = new QRadioButton( "Square" );
    lineButton = new QRadioButton( "Line" );

    if( panesType == LinePanesType ) {
      lineButton->setChecked( true );
    }
    else {
      squareButton->setChecked( true );
    }    

    layout->addWidget( squareButton );
    layout->addWidget( lineButton );

    QGroupBox* box = new QGroupBox( "Panes configuration:" );
    box->setLayout( layout );

    box->resize( 200, 50 );

    return box;
  }// createPanesTypeWidget
  


  
  void NoClick::toggleSettingsHelpViewer( bool checked ) {
    //qInfo() << "in toggleSettingsHelpViewer";
    
    if( settingsHelpViewer_->isVisible() ) {
      settingsHelpViewer_->hide();
    }
    else {
      settingsHelpViewer_->show();
    }
  }// toggleSettingsHelpViewer  

  void NoClick::triggerExit( bool checked ) {
    close();
  }// triggerExit


  void NoClick::toggleAboutHelpViewer( bool checked ) {
    //qInfo() << "in toggleAboutHelpViewer";
    
    if( aboutHelpViewer_->isVisible() ) {
      aboutHelpViewer_->hide();
    }
    else {
      aboutHelpViewer_->show();
    }
  }// toggleAboutHelpViewer  


  QMainWindow* NoClick::createMainWindow( void ) {
    // Settings viewer
    {
      settingsHelpViewer_->setGeometry( 100, 100, 600, 800 );
      QSettings s;
      QString fileName = s.fileName();
      QFileInfo info( fileName );
      QString helpName = info.absolutePath() + "/NoClick.html";

      QUrl url = QUrl::fromLocalFile( helpName );

      settingsHelpViewer_->setUrl( url );      
    }

    // About viewer
    {
      QSettings s;
      QString iniName = s.fileName();

      QString version = QCoreApplication::applicationVersion();

      QString info( "Version: " );
      info += version += "\n";
      ( info += "INI File: " ) += iniName;

      aboutHelpViewer_->setText( info );
      aboutHelpViewer_->setTextInteractionFlags( Qt::TextSelectableByMouse );
      aboutHelpViewer_->setContentsMargins( 10, 10, 10, 10 );
 
      //mbox_->setGeometry( 100, 100, 600, 800 );
    }
    
    QMainWindow* mainW = this;//new QMainWindow;
    QWidget* centralW = new QWidget();

    //------- Central widget
    /*
4  panes
2  opacity
     */
    QGridLayout* gridLayout = new QGridLayout( centralW );

    starColorDialog = new QColorDialog( starColor, this );
    starColorDialog->setWindowTitle( "Select Star Color" );

    coloursBox_ = new ColoursBoxWidget( "Click Panes" );
    gridLayout->addWidget( coloursBox_, 0, 0, 4, 1 );
    

    // ShowHide
    {
      showHideW_ = new NoClickActive( "Hide: " );

      showHideW_->setToolTip(
R"(-- Left click to toggle Show/Hide
-- Right click to set Hot Key)" );

      auto box = new QFrame();
      auto layout = new QBoxLayout( QBoxLayout::LeftToRight );
      box->setLayout( layout );
      layout->addWidget( new QLabel( "Hover" ) );
      
      hoverButton = new QPushButton();
      hoverButton->setCheckable( true );
      hoverButton->setChecked( labelAllowHover );
      hoverButton->setSizePolicy
	( QSizePolicy( QSizePolicy::MinimumExpanding,
		       QSizePolicy::MinimumExpanding,
		       QSizePolicy::DefaultType ) );

      hoverButton->setToolTip(
R"(Allow hovering to turn Show to Hide )" );

      connect( hoverButton,
	       &QAbstractButton::toggled,
	       this,
	       &NoClick::hoverButtonToggled );
	       
    
      layout->addWidget( hoverButton );
    
      gridLayout->addWidget( showHideW_, 0, 1 );
      gridLayout->addWidget( box, 1, 1 );
    }


    // Panes-type box
    {
      panesTypeBox_ = createPanesTypeWidget();

      panesTypeBox_->setToolTip(
R"(Choose whether the panes are arranged in a square,
or roughly in a line)" );

      gridLayout->addWidget( panesTypeBox_, 2, 1, Qt::AlignCenter );

      connect( squareButton,
	       &QAbstractButton::clicked,
	       this,
	       &NoClick::squareClicked );

      connect( lineButton,
	       &QAbstractButton::clicked,
	       this,
	       &NoClick::lineClicked );      
    }

    // Star
    {
      auto box = new QFrame();
      auto layout = new QBoxLayout( QBoxLayout::LeftToRight );
      box->setLayout( layout );

      layout->addWidget( new QLabel( "Star" ) );

      starLabel = createColorLabel( "",
				    starColor,
				    starOpacity );

    starLabel->setToolTip(
R"(Left click to change the colour 
of the Star)" );

      layout->addWidget( starLabel, Qt::AlignLeft );

      gridLayout->addWidget( box, 3, 1 );
    }
    
    gridLayout->addWidget( radioBox_, 4, 1 );

    // Opacity
    {
       QGroupBox* box = new QGroupBox( "Opacity" );
      auto layout = new QFormLayout();
      box->setLayout( layout );
      box->setAlignment( Qt::AlignHCenter );

      panesOpacityField = ssrd_gui::CreateAnchoredSpinBox( panesOpacity );
      panesOpacityField->setButtonSymbols( QAbstractSpinBox::NoButtons );
      panesOpacityField->setRange( 0.0, 1.0 );

      starOpacityField = ssrd_gui::CreateAnchoredSpinBox( starOpacity );
      starOpacityField->setButtonSymbols( QAbstractSpinBox::NoButtons );
      starOpacityField->setRange( 0.0, 1.0 );

      layout->addRow( "Panes", panesOpacityField );
      layout->addRow( "Star", starOpacityField );
 
      box->setToolTip(
R"(Set the opacity of the panes and star)" );

      gridLayout->addWidget( box, 4, 0, Qt::AlignCenter );

      connect( starOpacityField,
	       ( void (QDoubleSpinBox::*)(double) )
	       &QDoubleSpinBox::valueChanged,
	       this,
	       &NoClick::starOpacityChanged );
      
      connect( panesOpacityField,
	       ( void (QDoubleSpinBox::*)(double) )
	       &QDoubleSpinBox::valueChanged,
	       this,
	       &NoClick::panesOpacityChanged );

      connect( starLabel,
	       &QAbstractButton::clicked,
	       this,
	       &NoClick::starLabelClicked );

      connect( starColorDialog,
	       &QColorDialog::colorSelected,
	       this,
	       &NoClick::starLabelColored );
      
    }


    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(line, 5, 0, 1, 2 );
    
    centralW->setLayout( gridLayout );
    mainW->setCentralWidget( centralW );

    

    //--------- Menu bar

    // File
    NoClickQMenu* fileMenu = new NoClickQMenu( "File", mainW->menuBar() );
    mainW->menuBar()->addAction( fileMenu->menuAction() );

    exitAction_ = fileMenu->addAction( "Exit" );

    connect( exitAction_,
	     &QAction::triggered,
	     this,
	     &NoClick::triggerExit );

    // Configure
    QMenu* systemMenu = mainW->menuBar()->addMenu( "Configure" );
    
    QAction* detailsAction = systemMenu->addAction( "Timings" ); // Toggle
    detailsWindow_ =
      new NoClickDetailsWindow ( detailsAction );

    QAction* panesAction = systemMenu->addAction( "Panes" ); // Toggle
    panesWindow_ =
      new NoClickPanesWindow ( panesAction );

    // Help
    QMenu* helpMenu = mainW->menuBar()->addMenu( "Help" );

    settingsAction_ = helpMenu->addAction( "Settings" );

    connect( settingsAction_,
	     &QAction::triggered,
	     this,
	     &NoClick::toggleSettingsHelpViewer );

    aboutAction_ = helpMenu->addAction( "About" );
   
    connect( aboutAction_,
	     &QAction::triggered,
	     this,
	     &NoClick::toggleAboutHelpViewer );
    //------------ done!
    return mainW;
  }// NoClick::createMainWindow
  

  void NoClick::shutDownMainWindow( void ) {
    disconnect( squareButton,
		&QAbstractButton::clicked,
		this,
		&NoClick::squareClicked );
    
    disconnect( lineButton,
		&QAbstractButton::clicked,
		this,
		&NoClick::lineClicked );

    disconnect( starOpacityField,
		( void (QDoubleSpinBox::*)(double) )
		&QDoubleSpinBox::valueChanged,
		this,
		&NoClick::starOpacityChanged );      
 
    disconnect( panesOpacityField,
		( void (QDoubleSpinBox::*)(double) )
		&QDoubleSpinBox::valueChanged,
		this,
		&NoClick::panesOpacityChanged );

    disconnect( starLabel,
		&QAbstractButton::clicked,
		this,
		&NoClick::starLabelClicked );

    disconnect( starColorDialog,
		&QColorDialog::colorSelected,
		this,
		&NoClick::starLabelColored );

    disconnect( hoverButton,
		&QAbstractButton::toggled,
		this,
		&NoClick::hoverButtonToggled );
    
  }// shutDownMainWindow



}
