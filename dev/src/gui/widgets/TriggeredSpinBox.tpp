#ifndef TriggeredSpinBox_TPP
#define TriggeredSpinBox_TPP

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "TriggeredSpinBox.hpp"
#include <limits>

namespace ssrd_gui {

  template< typename T >
  TriggeredSpinBox<T>::TriggeredSpinBox( T loc, TriggerType<T> trig ) :
    trigger_( trig )
  {
    this->setRange( 0, std::numeric_limits<T>::max() );
    this->setValue( loc );

    QObject::
    connect( static_cast<SpinBoxType* >( this ),
	     (SigType) &SpinBoxType::valueChanged,
	     this,
	     &TriggeredSpinBox<T>::updateValue ); 
  }


  template< typename T >
  TriggeredSpinBox<T>::~TriggeredSpinBox() {
    QObject::
    disconnect( static_cast<SpinBoxType* >( this ),
		(SigType) &SpinBoxType::valueChanged,
		this,
		&TriggeredSpinBox<T>::updateValue ); 
  }

  template< typename T >
  void TriggeredSpinBox<T>::updateValue( T val ) {
    (*trigger_)(val);
  }


}

#endif
