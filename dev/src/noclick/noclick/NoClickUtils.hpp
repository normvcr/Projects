#ifndef NO_CLICK_UTILS_HPP
#define NO_CLICK_UTILS_HPP

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "BootArch.h"
#include <string>

class QString;
class QPushButton;
class QColor;
class QWidget;

namespace ssrd_mouse {

  void hideToolTip( QWidget* w );
  
  // Does affine blend with opacity
  QColor lighten( const QColor& color, double opacity );

  // Outputs one of 8 different colours
  // White, black, red, green, blue, magenta, cyan, yellow
  QColor complement( const QColor& color );

  // Sets the background colour, as specified.
  // Sets the foreground color (text) as maximum
  // distance from the bg colour.
  void reset( QPushButton* label,
	      const QColor& bg_color,
	      double bg_opacity );

  // Uses reset for the colours
  QPushButton* createColorLabel( const char* labelText,
			    const QColor& labelColor,
				 double opacity );
  

#if OS_IS_LINUX
  int getKbdFileDescriptor( void );
#endif

  // QString::toStdString is broken
  std::string toStdString( const QString& qs );
   
  enum HotKeyModifier
    { Shift=1, Ctl=2, Alt=4 };
  

  enum class KeyboardState
    { Drag,    // Watches for modifier to end drag
      Normal,  // Watches for hot key to toggle Hide/Show
      HotKey   // Reports hot key that is entered
    };
		      
		     
  namespace NoClickKbdResult {
    // Not an enum, so that can use
    // with QT's signal/slot    
    const int EndDrag = 0;
    const int CancelDrag = 1;
    const int Error = 2;
    const int ToggleShow = 3;
  }
}

#endif

