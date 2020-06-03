#ifndef TriggeredSpinBox_HPP
#define TriggeredSpinBox_HPP

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <QSpinBox>
#include <TypedSpinBox.hpp>

namespace ssrd_gui {

  template< typename T >
  using TriggerType = void (*)( T );

  template <typename T>
  class TriggeredSpinBox : public TypedSpinBox< T > {
  public:
    typedef typename TypedSpinBox<T>::SpinBoxType SpinBoxType;
    typedef void (SpinBoxType::*SigType)(T);

    TriggeredSpinBox( T loc, TriggerType<T> trig );
    ~TriggeredSpinBox();

    virtual void updateValue( T val ); // slot
    
  private:
    TriggerType<T> trigger_;
  };

  template <typename T>
  TriggeredSpinBox<T>* CreateTriggeredSpinBox(  double val,
						TriggerType<T> trig ) {
    return new TriggeredSpinBox<T>( val, trig );
  }

}

#endif
