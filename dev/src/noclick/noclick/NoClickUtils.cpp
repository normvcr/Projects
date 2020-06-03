/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickUtils.hpp"
#include "BootParse.h"
#include <QDebug>
#include <QColor>
#include <QPushButton>

#include "BootArch.h"

#if OS_IS_LINUX

#include <sstream>
#include <fstream>
#include <string.h>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#endif

namespace ssrd_mouse {

  void hideToolTip( QWidget* w ) {
    if( nullptr == w ) return;
    
    w->setToolTip(" ");
    w->setToolTipDuration(1);
  }// hideToolTip

  std::string toStdString( const QString& qs ) {
    const QChar* qcp = qs.constData();

    std::string stdStr;

    for( int ind = 0; ind < qs.size(); ++ind ) {
      char c =  qcp[ind].toLatin1();

      //qInfo() << "char= " << c;
      
      stdStr += c;
    }

    return stdStr;
  }// toStdString
  

  QColor lighten( const QColor& color, double opacity ) {
    QColor rgb( color.red() * opacity + 255*(1.0 - opacity),
		color.green() * opacity + 255*(1.0 - opacity),
		color.blue() * opacity + 255*(1.0 - opacity) );

    return rgb;
  }//lighten


  QColor complement( const QColor& color ) {
    
    QColor rgb( color.red() < 128 ? 255 : 0,
		color.green() < 128 ? 255 : 0,
		color.blue() < 128 ? 255 : 0 );

    return rgb;
  }// complement

  void reset( QPushButton* label,
	      const QColor& color,
	      double opacity ) {
    QColor rgb( lighten( color, opacity ) );
    QColor complColor( complement( rgb ) );
 
    QString result;
    QTextStream(&result) << "QPushButton { background-color : "
			 << rgb.name()
			 << "; color : "
			 << complColor.name()
			 << "; }";
    
    label->setStyleSheet( result );    
  }// reset
  
  
  QPushButton* createColorLabel( const char* labelText,
			    const QColor& labelColor,
			    double opacity ) {
    const QSizePolicy sizePol( QSizePolicy::MinimumExpanding,
			       QSizePolicy::MinimumExpanding,
			       QSizePolicy::DefaultType );
    
    QPushButton* label = new QPushButton( labelText );
 
    reset( label, labelColor, opacity );

    label->setSizePolicy( sizePol );

    return label;
  }// createColourLabel

  
 #if OS_IS_LINUX

  using namespace std;
 
  // Adapted from
  // https://github.com/kernc/logkeys/blob/master/src/logkeys.cc
  //
  // Returns -1 on failure
  int getKbdFileDescriptor( void ) {
    const char* deviceListFileName = "/proc/bus/input/devices";

    ifstream is( deviceListFileName );

    if( !is ) {
      qCritical() << "getKbdFileDescriptor: Opening "
	       << deviceListFileName;

      return -1;
    }

    /*
      Every block has these two lines, in order:

      H: Handlers=kbd event1 
      B: EV=3
     */
    ostringstream fileNameStr;
    string fileName;
    
    const int BufSize = 1000;
    char lineBuf[BufSize];
    char handlersBuf[BufSize];
    
    while( 1 ) {
      if( !find( is, "Handlers=" ) ) {
	qInfo() << "getKbdFileDescriptor: "
		<< "No more handlers";

	break;
      }
      
      if( !is.getline( handlersBuf, BufSize ) ){
	qCritical() << "getKbdFileDescriptor: "
		    << "Copying the handler buf";
	
	return -1;
      }

      //      qInfo() << "Handlers= " << handlersBuf;

      if( BufSize <= is.gcount() ) {
	qCritical() << "getKbdFileDescriptor: "
		    << BufSize
		    << " too small for handlers";
	return -1;
      }

      const char Event[] = "event";
      char* event = strstr( handlersBuf, Event );

      if( nullptr == event ) {
	continue;
      }

      int index = -1;
      if( 1 != sscanf( event+strlen(Event), "%d", &index ) ) {
	continue;
      }

      //      qInfo() << "Event index= " << index;

      //------------------------

      if( !find( is, "EV=" ) ) {
	qWarning() << "getKbdFileDescriptor: "
		   << "Missing EV line for Handlers:\n"
		   << handlersBuf;

	break;
      }
      
      if( '1' != is.get() ) continue;

      char c = is.get();

      //      qInfo() << "c= " << c;

      // Made this more restrictive so as to be unique
      // on my machine.
      if( /*('0' != c) && */ ('2' != c) ) continue;

      if( (is.get() != '0') ||
	  (is.get() != '0') ||
	  (is.get() != '1') ) continue;

      char c2 = is.get();

      //      qInfo() << "c2= " << c2;

      if( (c2 != '3') &&
	  (c2 != 'F') &&
	  (c2 != 'f') ) continue;


      if( !fileName.empty() ) {
	qWarning()  << "getKbdFileDescriptor: "
		    << "Multiple event files\n"
		    << fileName.c_str() << ", and \n"
		    << "handlers= " << handlersBuf
		    << ", 1" << c << "001" << c2;
	continue;
      }

      fileNameStr << "/dev/input/event";
      fileNameStr << index;
      fileName = fileNameStr.str();
    }// while

    if( fileName.empty() ) {
      qCritical()  << "getKbdFileDescriptor: "
		  << "No keyboard found";
      return -1;
    }

    int kbdFileDescriptor = open( fileName.c_str(),
				  O_RDONLY );

    if( kbdFileDescriptor == -1 ) {
      qCritical( "Cannot open %s: %s.\n",
		 fileName.c_str(),
		 strerror(errno) );
	 
      return -1;
    }
	
    return kbdFileDescriptor;
  }// getKbdFileDescriptor

#endif
}
