/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/* Used:
https://docs.microsoft.com/en-gb/windows/win32/api/winuser/nf-winuser-sendinput

User32.dll
 */


#include "EventOS.hpp"

#include <Windows.h>

#include <iostream>
using namespace std;

namespace ssrd_event {

  bool EventOS::init( void ) {

    return true;
  }// EventOS::init

  EventOS::~EventOS() {

  }// EventOS::init
  

  bool EventOS::sendMouseMove( int screenX, // Screen XY of pt
			       int screenY ) {
    INPUT input;

    /*
      ypedef struct tagMOUSEINPUT {
      LONG      dx;
      LONG      dy;
      DWORD     mouseData;
      DWORD     dwFlags;
      DWORD     time;
      ULONG_PTR dwExtraInfo;
      } MOUSEINPUT, *PMOUSEINPUT, *LPMOUSEINPUT;
    */

    // (0,0) is ul pixel;
    // (65535,65535) is lr pixel
    const double normalizedSize = 65535.0;
    
    input.type = INPUT_MOUSE;
    MOUSEINPUT& mousin = input.mi;

    mousin.mouseData = 0; // Not used with specified flags
    mousin.time = 0;  // System will provide time stamp
    mousin.dwExtraInfo = NULL;  // We're not using
      
    mousin.dx =
      std::round( (screenX - dtMinx_) * (normalizedSize/dtWidth_) );
      
    mousin.dy =
      std::round( (screenY - dtMiny_) * (normalizedSize/dtHeight_) );

    mousin.dwFlags =
      MOUSEEVENTF_MOVE
      | MOUSEEVENTF_ABSOLUTE
      | MOUSEEVENTF_VIRTUALDESK;

    UINT retval = SendInput(
			    1, 
			    &input,
			    sizeof( INPUT )
			    );
      
    if( 1 != retval ) {
      cerr << "EventOS_Win::sendMouseMove: Error from SendInput move"
	   << endl;
	
      return false;
    }

    return true;
  }// EventOS::sendMouseMove


  bool EventOS::sendMouseClick( unsigned int button,
				int screenX,
				int screenY,
				bool move ) {

    /*
UINT SendInput(
  UINT    cInputs,
  LPINPUT pInputs,
  int     cbSize
);
     */

    INPUT inputArray[3];// Move, Down, Up

    /*
ypedef struct tagMOUSEINPUT {
  LONG      dx;
  LONG      dy;
  DWORD     mouseData;
  DWORD     dwFlags;
  DWORD     time;
  ULONG_PTR dwExtraInfo;
} MOUSEINPUT, *PMOUSEINPUT, *LPMOUSEINPUT;
     */

     // (0,0) is ul pixel;
    // (65535,65535) is lr pixel
    const double normalizedSize = 65535.0;
    
    // Move
    {
      INPUT& input = inputArray[0];
      input.type = INPUT_MOUSE;
      MOUSEINPUT& mousin = input.mi;

      mousin.mouseData = 0; // Not used with specified flags
      mousin.time = 0;  // System will provide time stamp
      mousin.dwExtraInfo = NULL;  // We're not using
      
      mousin.dx =
	std::round( (screenX - dtMinx_) * (normalizedSize/dtWidth_) );
      
      mousin.dy =
	std::round( (screenY - dtMiny_) * (normalizedSize/dtHeight_) );

      if( move )  {
	mousin.dwFlags =
	  MOUSEEVENTF_MOVE
	  | MOUSEEVENTF_ABSOLUTE
	  | MOUSEEVENTF_VIRTUALDESK;

	UINT retval = SendInput(
				1, 
				inputArray,
				sizeof( INPUT )
				);
      
	if( 1 != retval ) {
	  cerr << "EventOS_Win::sendMouseClick: Error from SendInput move away"
	       << endl;
	
	  return false;
	}
      }// if move

    }// Move

    // Down
    {
      INPUT& input = inputArray[1];
      input = inputArray[0];
      MOUSEINPUT& mousin = input.mi;

      const DWORD flags[3] = { MOUSEEVENTF_LEFTDOWN,
			       MOUSEEVENTF_MIDDLEDOWN,
			       MOUSEEVENTF_RIGHTDOWN };
      
      mousin.dwFlags =
	flags[button-1]
	| MOUSEEVENTF_ABSOLUTE
	| MOUSEEVENTF_VIRTUALDESK;
    }// Down

    // Sleep(100);
  
    // Up
    {
      INPUT& input = inputArray[2];
      input = inputArray[0];
      MOUSEINPUT& mousin = input.mi;
     
      const DWORD flags[3] = { MOUSEEVENTF_LEFTUP,
			       MOUSEEVENTF_MIDDLEUP,
			       MOUSEEVENTF_RIGHTUP };

      mousin.dwFlags =
	flags[button-1]
	| MOUSEEVENTF_ABSOLUTE
	| MOUSEEVENTF_VIRTUALDESK;
    }// Up

    UINT retval = SendInput(
			    3, 
			    inputArray,
			    sizeof( INPUT )
			    );
      
    if( 3 != retval ) {
      cerr << "EventOS_Win::sendMouseClick: Error from SendInput dn/up"
	   << endl;
	
      return false;
    }
   
    return true;	       
  }// EventOS::sendMouseClick

  bool EventOS::sendMouseDown( unsigned int button,
			       int screenX, // Screen XY of pt
			       int screenY,
			       bool move ){
    
    INPUT inputArray[2];// Move, Down
    const double normalizedSize = 65535.0;
    
    // Move
    {
      INPUT& input = inputArray[0];
      input.type = INPUT_MOUSE;
      MOUSEINPUT& mousin = input.mi;

      mousin.mouseData = 0; // Not used with specified flags
      mousin.time = 0;  // System will provide time stamp
      mousin.dwExtraInfo = NULL;  // We're not using
      
      mousin.dx =
	std::round( (screenX - dtMinx_) * (normalizedSize/dtWidth_) );
      
      mousin.dy =
	std::round( (screenY - dtMiny_) * (normalizedSize/dtHeight_) );

 	mousin.dwFlags =
	  MOUSEEVENTF_MOVE
	  | MOUSEEVENTF_ABSOLUTE
	  | MOUSEEVENTF_VIRTUALDESK;
#if 1
	UINT retval = SendInput(
				1, 
				inputArray,
				sizeof( INPUT )
				);
      
	if( 1 != retval ) {
	  cerr << "EventOS_Win::sendMouseClick: Error from SendInput move away"
	       << endl;
	
	  return false;
	}
#endif
    }// Move

    // Down
    {
      INPUT& input = inputArray[1];
      input = inputArray[0];
      MOUSEINPUT& mousin = input.mi;

      const DWORD flags[3] = { MOUSEEVENTF_LEFTDOWN,
			       MOUSEEVENTF_MIDDLEDOWN,
			       MOUSEEVENTF_RIGHTDOWN };
      
      mousin.dwFlags =
	flags[button-1]
	| MOUSEEVENTF_ABSOLUTE
	| MOUSEEVENTF_VIRTUALDESK;
    }// Down

	
    UINT retval = SendInput(
			    2, 
			    &inputArray[0],
			    sizeof( INPUT )
			    );
      
    if( 2 != retval ) {
      cerr << "EventOS_Win::sendMouseClick: Error from SendInput mv/dn"
	   << endl;
	
      return false;
    }
   
    return true;
  }// EventOS::sendMouseDown
  
  
  bool EventOS::sendMouseUp( unsigned int button,
			     int screenX, // Screen XY of pt
			     int screenY,
			     bool move ){
    
    INPUT inputArray[2];// Move, Up
    const double normalizedSize = 65535.0;
 
    // Move
    {
      INPUT& input = inputArray[0];
      input.type = INPUT_MOUSE;
      MOUSEINPUT& mousin = input.mi;

      mousin.mouseData = 0; // Not used with specified flags
      mousin.time = 0;  // System will provide time stamp
      mousin.dwExtraInfo = NULL;  // We're not using
      
      mousin.dx =
	std::round( (screenX - dtMinx_) * (normalizedSize/dtWidth_) );
      
      mousin.dy =
	std::round( (screenY - dtMiny_) * (normalizedSize/dtHeight_) );

      if( move ) {
 	mousin.dwFlags =
	  MOUSEEVENTF_MOVE
	  | MOUSEEVENTF_ABSOLUTE
	  | MOUSEEVENTF_VIRTUALDESK;

	UINT retval = SendInput(
				1, 
				inputArray,
				sizeof( INPUT )
				);
      
	if( 1 != retval ) {
	  cerr << "EventOS_Win::sendMouseClick: Error from SendInput move away"
	       << endl;
	
	  return false;
	}
      }
    }// Move

    // Up
    {
      INPUT& input = inputArray[1];
      input = inputArray[0];
      MOUSEINPUT& mousin = input.mi;

      const DWORD flags[3] = { MOUSEEVENTF_LEFTUP,
			       MOUSEEVENTF_MIDDLEUP,
			       MOUSEEVENTF_RIGHTUP };
      
      mousin.dwFlags =
	flags[button-1]
	| MOUSEEVENTF_ABSOLUTE
	| MOUSEEVENTF_VIRTUALDESK;
    }// Up

	
    UINT retval = SendInput(
			    1, 
			    &inputArray[1],
			    sizeof( INPUT )
			    );
      
    if( 1 != retval ) {
      cerr << "EventOS_Win::sendMouseClick: Error from SendInput mv/dn"
	   << endl;
	
      return false;
    }
   
    return true;
  }// EventOS::sendMouseUp

}

