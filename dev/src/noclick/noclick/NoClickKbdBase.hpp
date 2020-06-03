#ifndef NOCLICK_KBD_BASE_HPP
#define NOCLICK_KBD_BASE_HPP
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickUtils.hpp"
#include <QObject>

namespace ssrd_mouse {

  class NoClickKbdBase : public QObject{
    Q_OBJECT
    
  private:
   KeyboardState state_ = KeyboardState::Normal;

  public:
    void setState( KeyboardState st ) {
      state_ = st;
    }

    KeyboardState state( void ) const {
      return state_;
    }

  protected:
    const int notShift = ~HotKeyModifier::Shift;
    const int notCtl = ~HotKeyModifier::Ctl;
    const int notAlt = ~HotKeyModifier::Alt;
    
  signals:
    // flag is from NoClickKbdResult
    void kbdInterrupt( int flag );
    void hotKeyInterrupt( int key, int mask );
  };

}

#endif
