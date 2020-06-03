/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
https://unix.stackexchange.com/questions/74903/explain-ev-in-proc-bus-input-devices-data
 */


#include "NoClickKbd.hpp"
#include <QDebug>

#include <linux/input.h> // input_event

// open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


namespace ssrd_mouse {

  extern int hotKeyKey;
  extern int hotKeyModifiers;

  namespace {
    struct input_event ev;
    int mask = 0;
  }// anonymous;
  

  void NoClickKbd::run() {
    
     qInfo( "Kbd initializing ..." );
#if 0
     const char* kbdFileName = "/dev/input/event2";

     int kbdFileDescriptor = open( kbdFileName,
			       O_RDONLY);

#else
    int kbdFileDescriptor = getKbdFileDescriptor();
#endif
    
    if( kbdFileDescriptor == -1 ) {
      qCritical( "Cannot open kbd file descriptor" );
      
      emit kbdInterrupt( NoClickKbdResult::Error );
      return;
    }

    qInfo( "Kbd interrupt ready: %d", kbdFileDescriptor );

    while(1) {
      ssize_t n = read( kbdFileDescriptor,
			&ev,
			sizeof ev);

      if (n == (ssize_t)-1) {
	if (errno == EINTR)
	  continue;
	else
	  qCritical( "read error: %s", strerror(errno));
	  emit kbdInterrupt( NoClickKbdResult::Error );
	  break;
      } else
	if (n != sizeof ev) {
	  qCritical( "Read partial event, length= %d", n );
	  emit kbdInterrupt( NoClickKbdResult::Error );
 	  break;
	}

      // ev.value: 1=press, 0=release, 2=repeat
      if( (EV_KEY == ev.type) &&
	  ( (1 == ev.value ) || (0 == ev.value) ) ) {

	switch( state() ) {
	case KeyboardState::Drag:
	  doDrag();
	  break;

	case KeyboardState::Normal:
	  doNormal();
	  break;

	case KeyboardState::HotKey:
	  doHotKey();
	  break;
	}
	
      }
    }
  }// run

  
  void NoClickKbd::doDrag( void ) {
    switch( ev.code ) {
    case KEY_LEFTCTRL:
    case KEY_LEFTSHIFT:
    case KEY_RIGHTSHIFT:
    case KEY_LEFTALT:
    case KEY_RIGHTCTRL:
    case KEY_RIGHTALT:
      setState( KeyboardState::Normal );
      emit kbdInterrupt( NoClickKbdResult::EndDrag );
      break;

    case KEY_ESC:
      setState( KeyboardState::Normal );
      emit kbdInterrupt( NoClickKbdResult::CancelDrag );
      break;
	  
    default:
      break;
    }//Switch
  }// oClickKbd::doDrag

  
  void NoClickKbd::doHotKey( void ) {
     switch( ev.code ) {
    case KEY_LEFTCTRL:
    case KEY_RIGHTCTRL:
      if( 1 == ev.value ) {
	mask |= HotKeyModifier::Ctl;
      } else {
	mask &= notCtl;
      }
      break;
      
    case KEY_LEFTSHIFT:
    case KEY_RIGHTSHIFT:
      if( 1 == ev.value ) {
	mask |= HotKeyModifier::Shift;
      } else {
	mask &= notShift;
      }
      break;
      
    case KEY_LEFTALT:
    case KEY_RIGHTALT:
      if( 1 == ev.value ) {
	mask |= HotKeyModifier::Alt;
      } else {
	mask &= notAlt;
      }
      break;

    default:
      emit hotKeyInterrupt( ev.code, mask );
      setState( KeyboardState::Normal );      
      //qInfo() << "Emitting " << ev.code << ' ' << mask;
      break;
    }
  }// NoClickKbd::doHotKey

  
  void NoClickKbd::doNormal( void ) {
    //qInfo() << "mask= " << mask;
    
    switch( ev.code ) {
    case KEY_LEFTCTRL:
    case KEY_RIGHTCTRL:
      if( 1 == ev.value ) {
	mask |= HotKeyModifier::Ctl;
      } else {
	mask &= notCtl;
      }
      break;
      
    case KEY_LEFTSHIFT:
    case KEY_RIGHTSHIFT:
      if( 1 == ev.value ) {
	mask |= HotKeyModifier::Shift;
      } else {
	mask &= notShift;
      }
      break;
      
    case KEY_LEFTALT:
    case KEY_RIGHTALT:
      if( 1 == ev.value ) {
	mask |= HotKeyModifier::Alt;
      } else {
	mask &= notAlt;
      }
      break;

    default:
      if( (mask == hotKeyModifiers) &&
	  (1 == ev.value) &&
	  (ev.code == hotKeyKey ) ) {
	    emit kbdInterrupt( NoClickKbdResult::ToggleShow );
	  }
      break;
    }// switch

    // Check for modifier-only hot key.
    // Ignores the state of the non-modifier keys
    if( (mask == hotKeyModifiers) &&
	(0 > hotKeyKey) ) {
      emit kbdInterrupt( NoClickKbdResult::ToggleShow );
    }
  }// NoClickKbd::doNormal
  
}// ssrd_mouse
