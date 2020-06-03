/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "ActiveHotKey.hpp"
#include <QMessageBox>
#include <QGroupBox>
#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>
#include "NoClickKbd.hpp"
#include "NoClick.hpp"

#include <string>
#include <sstream>
using namespace std;

namespace ssrd_mouse {

  extern NoClickKbd* noClickKeyboard;
  extern int hotKeyKey;
  extern int hotKeyModifiers;

  namespace {
    QPushButton* shiftButton = nullptr;
    QPushButton* ctlButton = nullptr;
    QPushButton* altButton = nullptr;

    QPushButton* modOnlyOkButton = nullptr;
    QPushButton* modOnlyCancelButton = nullptr;

    QPushButton* cancelKeyedHotKeyButton = nullptr;
    

    QPushButton* noHotKeyButton = nullptr;
    QPushButton* modifiersHotKeyButton =nullptr;
    QPushButton* keyedHotKeyButton = nullptr;
    QGroupBox* typeB = nullptr;

    QWidget* modifiersOnlyWidget = nullptr;
    QWidget* cancelKeyedHotKeyWidget = nullptr;

    void setWidgetTitle( void ) {
    stringstream str;
    str << "Choose type of hot key.  Currently: "
	<< hotKeyModifiers
	<< ' '
	<< hotKeyKey;
    
    typeB->setTitle( str.str().c_str() );
    }// setWidgetTitle

    void createModifiersOnlyWidget( void ) {
      QGroupBox* buttonB = new QGroupBox( "Define Modifiers-only Hot Key" );
      buttonB->setAlignment( Qt::AlignHCenter );
      QBoxLayout* buttonL = new QBoxLayout( QBoxLayout::LeftToRight );
      buttonB->setLayout( buttonL );

      shiftButton = new QPushButton( "Shift" );
      shiftButton->setCheckable( true );
      
      ctlButton = new QPushButton( "Ctl" );
      ctlButton->setCheckable( true );
    
      altButton = new QPushButton( "Alt" );
      altButton->setCheckable( true );

      if( 0 <= hotKeyModifiers ) {
	shiftButton->setChecked( hotKeyModifiers & HotKeyModifier::Shift );
	altButton->setChecked( hotKeyModifiers & HotKeyModifier::Alt );
	ctlButton->setChecked( hotKeyModifiers & HotKeyModifier::Ctl );
      }      

      buttonL->addWidget( shiftButton );
      buttonL->addWidget( ctlButton );
      buttonL->addWidget( altButton );
    
      //-----------
      QGroupBox* promptB = new QGroupBox( "Accept the above hot key, or cancel" );
      promptB->setAlignment( Qt::AlignHCenter );
      QBoxLayout*  promptL = new QBoxLayout( QBoxLayout::LeftToRight );
      promptB->setLayout( promptL );
      modOnlyOkButton = new QPushButton( "OK" );
      modOnlyCancelButton = new QPushButton( "Cancel" );
      promptL->addWidget( modOnlyOkButton );
      promptL->addWidget( modOnlyCancelButton );
    
     //==================
      modifiersOnlyWidget = new QWidget();
      QBoxLayout* layout = new QBoxLayout( QBoxLayout::TopToBottom );
      modifiersOnlyWidget->setLayout( layout );
      layout->addWidget( buttonB );
      layout->addWidget( promptB );
    }// createModifiersOnlyWidget
    

    void createCancelKeyedHotKeyWidget( void ) {
      QGroupBox* buttonB = new QGroupBox( "Type the new hot key" );
      buttonB->setAlignment( Qt::AlignHCenter );
      QBoxLayout* buttonL = new QBoxLayout( QBoxLayout::LeftToRight );
      buttonB->setLayout( buttonL );

      cancelKeyedHotKeyButton = new QPushButton( "Cancel" );
      buttonL->addWidget( cancelKeyedHotKeyButton );

      cancelKeyedHotKeyWidget = new QWidget();
      QBoxLayout* layout = new QBoxLayout( QBoxLayout::TopToBottom );
      cancelKeyedHotKeyWidget->setLayout( layout );
      layout->addWidget( buttonB );
    }// craeateCancelKeyedHotKeyWidget
  }// anonymous


  ActiveHotKey::ActiveHotKey( void ) {
    typeB = new QGroupBox();
    setWidgetTitle();
    
    typeB->setAlignment( Qt::AlignHCenter );
    QBoxLayout* typeL = new QBoxLayout( QBoxLayout::TopToBottom );
    typeB->setLayout( typeL );

    noHotKeyButton = new QPushButton( "No Hot Key" );
    modifiersHotKeyButton = new QPushButton( "Modifiers only " );
    keyedHotKeyButton =  new QPushButton( "Keyed Hot Key" );

    typeL->addWidget( noHotKeyButton );
    //typeL->addWidget( modifiersHotKeyButton );
    typeL->addWidget( keyedHotKeyButton );

    //============
    QBoxLayout* layout = new QBoxLayout( QBoxLayout::TopToBottom );
    this->setLayout( layout );
    layout->addWidget( typeB );

    createModifiersOnlyWidget();
    createCancelKeyedHotKeyWidget();
    
    connect( static_cast< QAbstractButton* >( noHotKeyButton ),
	     &QAbstractButton::pressed,
	     this,
	     &ActiveHotKey::noHotKeyPressed );

    connect( static_cast< QAbstractButton* >( modifiersHotKeyButton ),
	     &QAbstractButton::pressed,
	     this,
	     &ActiveHotKey::showModifiersOnlyWidget );

    connect( static_cast< QAbstractButton* >( keyedHotKeyButton ),
	     &QAbstractButton::pressed,
	     this,
	     &ActiveHotKey::keyedHotKeyRequested );

    connect( static_cast< QAbstractButton* >( modOnlyOkButton ),
	     &QAbstractButton::pressed,
	     this,
	     &ActiveHotKey::modOnlyOkPressed );
   
    connect( static_cast< QAbstractButton* >( modOnlyCancelButton ),
	     &QAbstractButton::pressed,
	     this,
	     &ActiveHotKey::modOnlyCancelPressed );
   
    connect( static_cast< QAbstractButton* >( cancelKeyedHotKeyButton ),
	     &QAbstractButton::pressed,
	     this,
	     &ActiveHotKey::cancelKeyedHotKeyRequested );

    connect( noClickKeyboard,
	     &NoClickKbd::hotKeyInterrupt,
	     this,
	     &ActiveHotKey::hotKeyChosen );
   
  }// ActiveHotKey::ActiveHotKey
  

  void ActiveHotKey::keyedHotKeyRequested( void ) {
    noClickKeyboard->setState( KeyboardState::HotKey );
    hide();
    cancelKeyedHotKeyWidget->move( pos() );
    cancelKeyedHotKeyWidget->show();
    cancelKeyedHotKeyWidget->raise();
  }// ActiveHotKey:keyedHotKeyRequested
  

  void ActiveHotKey::cancelKeyedHotKeyRequested( void ) {
    noClickKeyboard->setState( KeyboardState::Normal );
    cancelKeyedHotKeyWidget->hide();
    show();
    raise();
  }// ActiveHotKey::cancelKeyedHotKeyRequested
  

  void ActiveHotKey::hotKeyChosen( int key, int mask ) {
    cancelKeyedHotKeyWidget->hide();
    hotKeyKey = key;
    hotKeyModifiers = mask;
    setWidgetTitle();
    NoClick::instance()->refreshShowHide();
    show();
    raise();
  }// ActiveHotKey::hotKeyChosen
  

  void ActiveHotKey::noHotKeyPressed( void ) {
    hotKeyModifiers = -1;
    setWidgetTitle();
    NoClick::instance()->refreshShowHide();
    hide();
  }// ActiveHotKey::noHotKeyPressed
  
  
  void ActiveHotKey::showModifiersOnlyWidget( void ) {
    hide();
    
    if( 0 <= hotKeyModifiers ) {
      shiftButton->setChecked( hotKeyModifiers & HotKeyModifier::Shift );
      ctlButton->setChecked( hotKeyModifiers & HotKeyModifier::Ctl );
      altButton->setChecked( hotKeyModifiers & HotKeyModifier::Alt );
    }
    
    modifiersOnlyWidget->move( pos() );
    modifiersOnlyWidget->show();
  }// ActiveHotKey::showModifiersOnlyWidget
    

  ActiveHotKey::~ActiveHotKey() {
    disconnect( static_cast< QAbstractButton* >( noHotKeyButton ),
		&QAbstractButton::pressed,
		this,
		&ActiveHotKey::noHotKeyPressed );

    disconnect( static_cast< QAbstractButton* >( modifiersHotKeyButton ),
		&QAbstractButton::pressed,
		this,
		&ActiveHotKey::showModifiersOnlyWidget );

    disconnect( static_cast< QAbstractButton* >( keyedHotKeyButton ),
		&QAbstractButton::pressed,
		this,
		&ActiveHotKey::keyedHotKeyRequested );

    disconnect( static_cast< QAbstractButton* >( modOnlyOkButton ),
		&QAbstractButton::pressed,
		this,
		&ActiveHotKey::modOnlyOkPressed );
    
    disconnect( static_cast< QAbstractButton* >( modOnlyCancelButton ),
		&QAbstractButton::pressed,
		this,
		&ActiveHotKey::modOnlyCancelPressed );

    disconnect( static_cast< QAbstractButton* >( cancelKeyedHotKeyButton ),
		&QAbstractButton::pressed,
		this,
		&ActiveHotKey::cancelKeyedHotKeyRequested );
 
    disconnect( noClickKeyboard,
		&NoClickKbd::hotKeyInterrupt,
		this,
		&ActiveHotKey::hotKeyChosen );
   
  }// ActiveHotKey::~ActiveHotKey
  

  void ActiveHotKey::modOnlyOkPressed( void ) {
    int modifiers =
      ( shiftButton->isChecked() ? HotKeyModifier::Shift : 0 ) +
      ( ctlButton->isChecked() ? HotKeyModifier::Ctl : 0 ) + 
      ( altButton->isChecked() ? HotKeyModifier::Alt : 0 );

    if( 0 == modifiers ) {
      QMessageBox* msg =
	new QMessageBox( QMessageBox::Warning,
			 "Improper specification of modifiers",
			 "At leat one modifier is required",
			 QMessageBox::Ok,
			 modifiersOnlyWidget );
      msg->show();
      return;
    }
    
    hotKeyKey = -1;
    hotKeyModifiers = modifiers;
    modifiersOnlyWidget->hide();
    setWidgetTitle();
  }// ActiveHotKey::modOnlyOkPressed
  
  
  void ActiveHotKey::modOnlyCancelPressed( void ) {
    modifiersOnlyWidget->hide();
    show();
    raise();
  }// ActiveHotKey::modOnlyCancelPressed



  void ActiveHotKey::closeEvent(QCloseEvent *event) {
    modifiersOnlyWidget->close();
    cancelKeyedHotKeyWidget->close();
  }// ActiveHotKey::closeEvent


}
