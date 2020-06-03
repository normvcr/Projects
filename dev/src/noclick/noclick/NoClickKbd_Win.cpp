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

#include <Windows.h>

/*
https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v%3Dvs.85)#requirements
 */

namespace ssrd_mouse {
  
  extern int hotKeyKey;
  extern int hotKeyModifiers;
  
  namespace {
    NoClickKbd* thisClass = nullptr;

    KBDLLHOOKSTRUCT* hookStruct = nullptr;
    int mask = 0;
  }// anonymous


  LRESULT CALLBACK LowLevelKeyboardProc(
  _In_ int    nCode,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
					) {
#if 0
    qInfo( "In LowLevelKeyboardProc. nCode wParam: %d %d",
	   nCode, wParam );
#endif
    if( nCode < 0 ) {
      return CallNextHookEx( HHOOK(0), nCode, wParam, lParam );
    }

    // We are just peeking.
    // Pass the info to other applications
    //if( !allowable_ ) return 0;

    hookStruct = (KBDLLHOOKSTRUCT*)lParam;

    //      qInfo() << "vkCode= " << hookStruct->vkCode;
       
    // syskey covers Alt.
    if( (WM_KEYDOWN == wParam) ||
	(WM_SYSKEYDOWN == wParam) ||
	(WM_KEYUP == wParam) ||
	(WM_SYSKEYUP == wParam) ){

      switch( thisClass->state() ) {
      case KeyboardState::Drag:
	return thisClass->doDrag( wParam );
	break;

      case KeyboardState::Normal:
	if( thisClass->doNormal( wParam ) < 0 ) {
	  return CallNextHookEx( HHOOK(0), nCode, wParam, lParam );
	}
	else {
	  return 1;
	}
	break;

      case KeyboardState::HotKey:
	return thisClass->doHotKey( wParam );
	break;
      }
    }
    // Just peeking.
    // Should never reach here.
    return 0;
  }// LowLevelKeyboardProc

  
  int NoClickKbd::doDrag( int wParam ) {

    switch( hookStruct->vkCode ) {

    case VK_ESCAPE:
      setState( KeyboardState::Normal );
      emit kbdInterrupt
	( ssrd_mouse::NoClickKbdResult::CancelDrag );
      return 1;
      break;
      
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_MENU:   // Alt
    case VK_LMENU:
    case VK_RMENU:
      setState( KeyboardState::Normal );
      emit kbdInterrupt
	( ssrd_mouse::NoClickKbdResult::EndDrag );
      return 1;
      break;
      
    default:
      return 0;
      break;
    }//Switch

    return 0; // should not reach here
  }// NoClickKbd::doDrag
  

  int NoClickKbd::doNormal(  int wParam ) {
    switch( hookStruct->vkCode ) {
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
      if( (WM_KEYDOWN == wParam) || (WM_SYSKEYDOWN == wParam) ){
	mask |= HotKeyModifier::Ctl;
      } else {
	mask &= notCtl;
      }
      break;
      
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
      if( (WM_KEYDOWN == wParam) || (WM_SYSKEYDOWN == wParam) ) {
	mask |= HotKeyModifier::Shift;
      } else {
	mask &= notShift;
      }
      break;
      
    case VK_MENU:   // Alt
    case VK_LMENU:
    case VK_RMENU:
       if( (WM_KEYDOWN == wParam) || (WM_SYSKEYDOWN == wParam) ) {
	mask |= HotKeyModifier::Alt;
      } else {
	mask &= notAlt;
      }
      break;

    default:
      if( (mask == hotKeyModifiers) &&
	  ((WM_KEYDOWN == wParam) || (WM_SYSKEYDOWN == wParam)) &&
	  ( hookStruct->vkCode == hotKeyKey ) ) {
	    emit kbdInterrupt( NoClickKbdResult::ToggleShow );
	    return 1;
      }
      break;
    }// switch

    // Check for modifier-only hot key.
    // Ignores the state of the non-modifier keys
    if( (mask == hotKeyModifiers) &&
	(0 > hotKeyKey) ) {
      emit kbdInterrupt( NoClickKbdResult::ToggleShow );
      return 1;
    }

    return -1;
  }// NoClickKbd::doNormal
  
    
  int NoClickKbd::doHotKey(  int wParam  ) {
    switch( hookStruct->vkCode ) {
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
      if( (WM_KEYDOWN == wParam) || (WM_SYSKEYDOWN == wParam) ){
	mask |= HotKeyModifier::Ctl;
      } else {
	mask &= notCtl;
      }
      break;
      
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
       if( (WM_KEYDOWN == wParam) || (WM_SYSKEYDOWN == wParam) ) {
	mask |= HotKeyModifier::Shift;
      } else {
	mask &= notShift;
      }
      break;
      
    case VK_MENU:   // Alt
    case VK_LMENU:
    case VK_RMENU:
      if( (WM_KEYDOWN == wParam) || (WM_SYSKEYDOWN == wParam) ) {
	mask |= HotKeyModifier::Alt;
      } else {
	mask &= notAlt;
      }
      break;

    default:
      setState( KeyboardState::Normal );
      emit hotKeyInterrupt( hookStruct->vkCode, mask );
      return 1;
      //qInfo() << "Emitting " << ev.code << ' ' << mask;
      break;
    }
    return 0;
  }// NoClickKbd::doHotKey
  

  void NoClickKbd::start( void )
  {
     qInfo( "Kbd initializing ..." );

     thisClass = this;

     HHOOK hookHandle =
     SetWindowsHookExA( WH_KEYBOARD_LL,
			LowLevelKeyboardProc,
			NULL,
			0 );

     if( NULL == hookHandle ) {
       qCritical( "Error setting keyboard hook" );
        emit kbdInterrupt( ssrd_mouse::NoClickKbdResult::Error );
     }
     else {
       qInfo( "Kbd interrupt ready." );
     }
  }//NoClickKbd::start

}// ssrd_mouse
