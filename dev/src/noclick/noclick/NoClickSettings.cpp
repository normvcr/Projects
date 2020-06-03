/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <QSettings>
#include <QColor>
#include <QtMath>
#include <QDebug>
#include "PanesSet.hpp"
#include "GenArchiveIO.tpp"
#include "NoClickUtils.hpp"

#include <iostream>
using namespace std;

//----------- Macros to help use Settings---------------
//
// Declare and Define:
// Using default constructors, even if produces undefined values.
// Default values are in readSettings()
#define SETT_DECDEF( varName, keyName )		\
  varName; \
  QString varName ## Key = #keyName


// Must be ended with toInt(); or toBool();, etc
#define SETT_READ( varName, s, defaultValue ) \
  varName = s.value( varName ## Key, defaultValue )

#define SETT_READ_COLOR( varName, s, defaultValue ) \
  { \
   QString str = s.value( varName ## Key, defaultValue ).toString(); \
   varName = QColor( str ); \
   }
   
#define SETT_READ_DOUBLE( varName, s, defaultValue ) \
  { \
   QString str = s.value( varName ## Key, defaultValue ).toString(); \
   varName = str.toDouble();					     \
   }
   
   

#define SETT_WRITE( varName, s ) \
  s.setValue( varName ## Key, varName )

#define SETT_WRITE_COLOR( varName, s ) \
  s.setValue( varName ## Key, varName.name() )

#define SETT_WRITE_DOUBLE( varName, s ) \
  { \
    QString str = QString::number( varName );	\
    s.setValue( varName ## Key, str );		\
  }


//-------------------------------------------------------


namespace ssrd_mouse {


  bool SETT_DECDEF( writeIniFileNameToScreen, System/writeIniFileNameToScreen );
  int SETT_DECDEF( lowestLoggingLevel, System/lowestLoggingLevel );
  double SETT_DECDEF( speedScaleFactor, System/speedScaleFactor );

  int SETT_DECDEF( mouseInterval, Mouse/timeInterval_ms );
  int SETT_DECDEF( mouseStationaryThreshold,
		   Mouse/stationaryThreshold_ms );
  
  bool SETT_DECDEF( hotCursorShow, HotCursor/show );
  int SETT_DECDEF( hotCursorSemiWidth, HotCursor/semiWidth_pix );
  int SETT_DECDEF( hotCursorInset, HotCursor/inset_pix );
  QColor SETT_DECDEF( hotCursorColor, HotCursor/color );

  int SETT_DECDEF( labelInterval, Label/timeInterval_ms );
  bool SETT_DECDEF( labelAllowHover, Label/allowHover);

  PanesType SETT_DECDEF( panesType, Panes/type );
  QColor SETT_DECDEF( panesLeftClickColor, Panes/leftClickColor );
  QColor SETT_DECDEF( panesRightClickColor, Panes/rightClickColor );
  QColor SETT_DECDEF( panesDClickColor, Panes/dClickColor );
  QColor SETT_DECDEF( panesDragColor, Panes/dragColor );
  int SETT_DECDEF( panesSideLength, Panes/sideLength_pix );
  int SETT_DECDEF( panesLineOffset, Panes/lineOffset_pix );
  int SETT_DECDEF( panesBorderWidth, Panes/borderWidth_pix );
  QColor SETT_DECDEF( panesBorderColor, Panes/borderColor );
  double SETT_DECDEF( panesOpacity, Panes/opacity);
  double SETT_DECDEF( panesBorderOpacity, Panes/borderOpacity);
#if 0  
  int SETT_DECDEF( panesStationaryTimeCutoff,
		 Panes/stationaryTimeCutoff_ms );
#endif  
  int SETT_DECDEF( panesMovingTimeCutoff,
		 Panes/movingTimeCutoff_ms );
  int SETT_DECDEF( hotPointOffset, Panes/hotPointOffset_pix );

  QColor SETT_DECDEF( dragStageColor, Drag/stageColor );
  int SETT_DECDEF( dragStopDelay, Drag/stopDelay_ms );
  int SETT_DECDEF( dragStopDelay2, Drag/stopDelay2_ms );
  int SETT_DECDEF( dragType, Drag/type );

  int SETT_DECDEF( starStationaryTimeCutoff,
		 Star/stationaryTimeCutoff_ms );
  int SETT_DECDEF( starStationaryTimeAppear,
		 Star/stationaryTimeAppear_ms );
  QColor SETT_DECDEF( starColor, Star/color );
  double SETT_DECDEF( starOpacity, Star/opacity );
  int SETT_DECDEF( starOffset, Star/hotPointOffset_pix );
  int SETT_DECDEF( starSideLength, Star/sideLength_pix );
  double SETT_DECDEF( starAxisAngle, Star/axisAngle_deg );
  
  int SETT_DECDEF( actionInterval, Action/timeInterval_ms );

  int SETT_DECDEF( hotKeyKey, HotKey/key );
  int SETT_DECDEF( hotKeyModifiers, HotKey/modifiers );
  

  // This avoids the initial value being changed when
  // the sampler is initialized.
  template< typename T >
  void adjustSamplerBounds( T val, double& bottom, double& top) {
    if( val < bottom ) {
      bottom  = val;
    }

    // Not using "else if" to cover the oversight case
    // when bottom > top 
    if( val > top ) {
      top = val;
    }
  }// adjustSamplerBounds


  void readSettings( void ) {
    QSettings s;

    SETT_READ( writeIniFileNameToScreen, s, true ).toBool();
    SETT_READ( lowestLoggingLevel, s, 1 ).toInt(); // Warning and higher
    //    SETT_READ( speedScaleFactor, s, 1.0 ).toDouble(); // No change
    SETT_READ( mouseStationaryThreshold, s, 600 ).toInt();
    SETT_READ( actionInterval, s, 4 ).toInt();
    SETT_READ( mouseInterval, s, 1 ).toInt();
    SETT_READ( labelInterval, s, 3 ).toInt();
    SETT_READ( labelAllowHover, s, false ).toBool();
    SETT_READ( hotCursorSemiWidth, s, 15 ).toInt();
    SETT_READ( panesSideLength, s, 30 ).toInt();
    SETT_READ( panesLineOffset, s, 10 ).toInt();
    SETT_READ( panesBorderWidth, s, 2 ).toInt();
    SETT_READ_COLOR( panesBorderColor, s, "#000000" );
    SETT_READ( panesOpacity, s, 0.3 ).toDouble();
    SETT_READ_DOUBLE( panesBorderOpacity, s, 0.6 );
    //    SETT_READ( panesStationaryTimeCutoff, s, 1300 ).toInt();
    SETT_READ( panesMovingTimeCutoff, s, 2000 ).toInt();
    SETT_READ( hotPointOffset, s, 4 ).toInt();
    SETT_READ( dragStopDelay, s, 700 ).toInt();
    SETT_READ( dragStopDelay2, s, 0 ).toInt();
    SETT_READ( dragType, s, 2 ).toInt();
    SETT_READ( hotCursorShow, s, false ).toBool();
    SETT_READ( hotCursorInset, s, 0 ).toInt();
    SETT_READ( starStationaryTimeCutoff, s, 1700 ).toInt();
    SETT_READ( starStationaryTimeAppear, s, 150 ).toInt();
    SETT_READ( starOffset, s, 4 ).toInt();
    SETT_READ( starSideLength, s, 10 ).toInt();
    SETT_READ_DOUBLE( starOpacity, s, 0.3 );
    SETT_READ( hotKeyKey, s, 0 ).toInt();
    SETT_READ( hotKeyModifiers, s, -1 ).toInt(); // No hot key, by default
    
   // panesType
    {
      const std::string defVal = toString(SquarePanesType);

      //qInfo() << "Default panes type= " << defVal.c_str();
      
      QString str =
	s.value( panesTypeKey,
		 defVal.c_str() ).toString();

      //qInfo() << "ReadSettings: str= " << str;

      //     std::wstring stdString = str.toStdString();
      std::string stdString = toStdString( str );

      const char* cp = stdString.c_str();

      panesType = fromString<PanesType>( cp );
      //qInfo() << "ReadSettings: panesType= " << toString(panesType).c_str();
   }

    // starAxisAngle
    {
      QString str;
      QVariant variant = s.value( starAxisAngleKey, "67.5" ).toString();
      str = variant.toString();
      starAxisAngle = qDegreesToRadians( str.toDouble() );
    }
    
    SETT_READ_COLOR( dragStageColor, s, "#FFEEEE" );
    SETT_READ_COLOR( panesLeftClickColor, s, "#FF0000" );
    SETT_READ_COLOR( panesRightClickColor, s, "#00FF00" );
    SETT_READ_COLOR( panesDClickColor, s, "#0000FF" );
    SETT_READ_COLOR( panesDragColor, s, "#FFFF00" );
    SETT_READ_COLOR( starColor, s,  "#FF0000" );
    SETT_READ_COLOR( hotCursorColor, s, "#FF0000" );
  }// readSettings

  void writeSettings( void ) {
    QSettings s;

    SETT_WRITE( writeIniFileNameToScreen, s );
    SETT_WRITE( lowestLoggingLevel, s );
    //    SETT_WRITE_DOUBLE( speedScaleFactor, s );
    SETT_WRITE( mouseStationaryThreshold, s );
    SETT_WRITE( actionInterval, s );
    SETT_WRITE( mouseInterval, s );
    SETT_WRITE( labelInterval, s );
    SETT_WRITE( labelAllowHover, s );
    SETT_WRITE( hotCursorSemiWidth, s );
    SETT_WRITE( panesSideLength, s );
    SETT_WRITE( panesLineOffset, s );
    SETT_WRITE( panesBorderWidth, s );
    SETT_WRITE_COLOR( panesBorderColor, s );
    SETT_WRITE_DOUBLE( panesOpacity, s );
    SETT_WRITE_DOUBLE( panesBorderOpacity, s );
        //    SETT_WRITE( panesStationaryTimeCutoff, s );
    SETT_WRITE( panesMovingTimeCutoff, s );
    SETT_WRITE( starStationaryTimeCutoff, s );
    SETT_WRITE( starStationaryTimeAppear, s );
    SETT_WRITE_DOUBLE( starOpacity, s );
    SETT_WRITE( hotPointOffset, s );
    SETT_WRITE( dragStopDelay, s );
    SETT_WRITE( dragStopDelay2, s );
    SETT_WRITE( dragType, s );
    SETT_WRITE( hotCursorShow, s );
    SETT_WRITE( hotCursorInset, s );
    SETT_WRITE( starOffset, s );
    SETT_WRITE( starSideLength, s );
    SETT_WRITE( hotKeyKey, s );
    SETT_WRITE( hotKeyModifiers, s );

    s.setValue( panesTypeKey, toString( panesType ).c_str() );

    s.setValue( starAxisAngleKey,
		QString::number( qRadiansToDegrees( starAxisAngle ) ) );

    SETT_WRITE_COLOR( dragStageColor, s );
    SETT_WRITE_COLOR( panesLeftClickColor, s );
    SETT_WRITE_COLOR( panesRightClickColor, s );
    SETT_WRITE_COLOR( panesDClickColor, s );
    SETT_WRITE_COLOR( panesDragColor, s );
    SETT_WRITE_COLOR( starColor, s );
    SETT_WRITE_COLOR( hotCursorColor, s );

  }// writeSettings
}
