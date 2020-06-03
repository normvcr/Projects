/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <glib.h>
#include <atspi/atspi.h>

#include "EventOS.hpp"

#include <string.h>  // memset
#include <unistd.h>  // usleep
#include <iostream>
using namespace std;

namespace {
  // Put this here, to avoid includes in the header file

  // button numbers are 1,2 or 3
  const char* atspiPress[] = { nullptr, "b1p", "b2p", "b3p" };
  const char* atspiRelease[] = { nullptr, "b1r", "b2r", "b3r" };
  
}

namespace ssrd_event {

  bool EventOS::init( void ) {

    return true;
  }// init
  
  EventOS::~EventOS() {
   }// EventOS::~EventOS


    
  bool EventOS::sendMouseMove( int screenX, 
			       int screenY ) {
    GError* error = nullptr;

    gboolean retval = 
      atspi_generate_mouse_event ( screenX,
				   screenY,
				   "abs", // Absolute move
				   &error);
      
    if( !retval ) {
      cout << "SendMouseMove: Erro from atspi_generate_mouse_event move"
	   << endl;
      return false;
    }

    return true;
  }// EventOS::sendMouseMove
  

  bool EventOS::sendMouseClick( unsigned int button,
				int screenX,
				int screenY,
				bool move ) {

    //------- Move mouse cursor
    if( move )
    {
      GError* error = nullptr;

      gboolean retval = 
	atspi_generate_mouse_event ( screenX,
				     screenY,
				     "abs", // Absolute move
				     &error);
      
      if( !retval ) {
	cout << "Erro from atspi_generate_mouse_event move" << endl;
	return false;
      }
   }

    //------- Press  button
    {
      GError* error = nullptr;
    
      gboolean retval = 
	atspi_generate_mouse_event ( screenX,
				     screenY,
				     atspiPress[ button ],
				     &error);
    
      if( !retval ) {
	cout << "Erro from atspi_generate_mouse_event press" << endl;
	return false;
      }
    }
    
    //------- Release button    
    {
      GError* error = nullptr;
    
      gboolean retval = 
	atspi_generate_mouse_event ( screenX,
				     screenY,
				     atspiRelease[ button ],
				     &error);
    
      if( !retval ) {
	cout << "Erro from atspi_generate_mouse_event release" << endl;
	return false;
      }
    }

    return true;	       
  }// EventOS::sendMouseClick
  

  bool EventOS::sendMouseDown( unsigned int button,
			       int screenX,
			       int screenY,
			       bool move ) {
    //------- Move mouse cursor
    if( move )
    {
      GError* error = nullptr;

      gboolean retval = 
	atspi_generate_mouse_event ( screenX,
				     screenY,
				     "abs", // Absolute move
				     &error);
      
      if( !retval ) {
	cout << "Erro from atspi_generate_mouse_event move" << endl;
	return false;
      }
   }

    //------- Press  button
    {
      GError* error = nullptr;
    
      gboolean retval = 
	atspi_generate_mouse_event ( screenX,
				     screenY,
				     atspiPress[ button ],
				     &error);
    
      if( !retval ) {
	cout << "Erro from atspi_generate_mouse_event press" << endl;
	return false;
      }
    }

    return true;	     
  }// EventOS::sendMouseDown
  

  bool EventOS::sendMouseUp( unsigned int button,
			     int screenX,
			     int screenY,
			     bool move  ) {
    //------- Move mouse cursor
    if( move )
    {
      GError* error = nullptr;

      gboolean retval = 
	atspi_generate_mouse_event ( screenX,
				     screenY,
				     "abs", // Absolute move
				     &error);
      
      if( !retval ) {
	cout << "Erro from atspi_generate_mouse_event move" << endl;
	return false;
      }
   }

    //------- Release button    
    {
      GError* error = nullptr;
    
      gboolean retval = 
	atspi_generate_mouse_event ( screenX,
				     screenY,
				     atspiRelease[ button ],
				     &error);
    
      if( !retval ) {
	cout << "Erro from atspi_generate_mouse_event release" << endl;
	return false;
      }
    }
 
    return true;	       
  }// EventOS::sendMouseUp


}
