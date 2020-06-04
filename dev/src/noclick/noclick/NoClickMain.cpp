/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <QApplication>
#include <QSettings>
#include <QLoggingCategory>
#include <QString>
#include <QDebug>
#include <QIcon>
#include <QFileInfo>
#include <QCommandLineParser>

#include "NoClick.hpp"
#include "NoClickSettings.hpp"
#include "NoClickUtils.hpp"

#include <iostream>
#include <string>
using namespace std;

namespace ssrd_mouse {
  // Both in NoClickSettings.cpp
  extern bool writeIniFileNameToScreen;
  extern int lowestLoggingLevel;
}

void enableLoggingLevels( int level ) {
  ssrd_mouse::lowestLoggingLevel = level;

  QLoggingCategory* defcat =
    QLoggingCategory::defaultCategory();
  
  
  if( 0 > level ) level = 0;
  
  switch (level) {
  case 0:
    defcat->setEnabled( QtDebugMsg, true );
  case 1:
    defcat->setEnabled( QtInfoMsg, true );
  case 2:
    defcat->setEnabled( QtWarningMsg, true );
  case 3:
    defcat->setEnabled( QtCriticalMsg, true );
  case 4:
    defcat->setEnabled( QtFatalMsg, true );    
  }

  if( 4 < level ) level = 5;
  
  switch (level-1) {
  case 4:
    defcat->setEnabled( QtFatalMsg, false );  
  case 3:
    defcat->setEnabled( QtCriticalMsg, false );
  case 2:
    defcat->setEnabled( QtWarningMsg, false );
  case 1:
    defcat->setEnabled( QtInfoMsg, false );
  case 0:
    defcat->setEnabled( QtDebugMsg, false );
  }
}// enableLoggingLevels

namespace {
  #if 0
  int typeToLevel( QtMsgType type ) {
    switch (type) {
    case QtDebugMsg:
      return 0;
    case QtInfoMsg:
      return 1;
    case QtWarningMsg:
      return 2;
    case QtCriticalMsg:
      return 3;
    case QtFatalMsg:
      return 4;
    }

    return -1;// Keep compiler happy
  }// typeToLevel
#endif
  FILE* fileHandle = nullptr;
  bool verbose = false;

 
  void myMessageOutput( QtMsgType type,
			const QMessageLogContext &context,
			const QString &msg )
  {
    //cout << "Type= " << type;
    //if( typeToLevel( type ) < ssrd_mouse::lowestLoggingLevel ) return;
    
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
    case QtDebugMsg:
      fprintf( fileHandle, "Debug: %s", localMsg.constData() );
      break;
    case QtInfoMsg:
      fprintf( fileHandle, "Info: %s", localMsg.constData() );
      break;
    case QtWarningMsg:
      fprintf( fileHandle, "Warning: %s", localMsg.constData() );
      break;
    case QtCriticalMsg:
      fprintf( fileHandle, "Critical: %s", localMsg.constData() );
      break;
    case QtFatalMsg:
      fprintf( fileHandle, "Fatal: %s", localMsg.constData() );
      break;
    }

    if( verbose ) {
      const char *file = context.file ? context.file : "";
      const char *function = context.function ? context.function : "";

       fprintf( fileHandle, " (%s:%u, %s)\n",
		file, context.line, function );
    }
    else {
      fprintf( fileHandle, "\n" );
    }

    fflush( fileHandle );
  }// myMessageOutput

}// anonymous


int mainBody( int argc, char* argv[] )
{
  //--------- Set up application
  QApplication qapp( argc, argv );

  // Ensure the same format for the settings file
  // across the various platforms (e.g. Windows and Linux)
  QSettings::setDefaultFormat( QSettings::IniFormat );

  // To organize where to find the settings file on disk.
  // 
  QCoreApplication::setOrganizationName("SystemSolutionsRD");
  QCoreApplication::setApplicationName("NoClick");
  // for Mac:
  QCoreApplication::setOrganizationDomain("SystemSolutionsRD.com");
  QCoreApplication::setApplicationVersion("1.0");
  

  //--------- Set up command line parser
  QCommandLineParser parser;
  parser.setApplicationDescription("NoClick Mouae Handling");
  parser.addHelpOption();
  parser.addVersionOption();
  
    QCommandLineOption screenOption
    ( "s",
      QCoreApplication::translate("main", "Log to screen") );
   
  parser.addOption( screenOption );

  QCommandLineOption iniFileNameOption
    ( "i",
      QCoreApplication::translate
      ( "main",
	"Read/write settings file.\nWrite ini file name to screen, exit" ) );
   
  parser.addOption( iniFileNameOption );

  QCommandLineOption debugOption
    ( "d",
      QCoreApplication::translate("main", "Debug/verbose messages") );
   
  parser.addOption( debugOption );

  parser.process(qapp);
  
  
  QSettings s;
  QFileInfo info( s.fileName() );
  QString infoPath( info.absolutePath() );

  //------------ Process command line arguments

  ssrd_mouse::readSettings(); // In NoClickSettings.cpp

  enableLoggingLevels( ssrd_mouse::lowestLoggingLevel );

  if( parser.isSet( iniFileNameOption ) ) {
    QString temp('"' + s.fileName() + '"');
    qInfo() << ssrd_mouse::toStdString(temp).c_str();
    
    // writeSettings() is also called from NoClick::closeEvent()
    //
    ssrd_mouse::writeSettings();
    return 0;
  }

  verbose = parser.isSet( debugOption );

  if( !parser.isSet( screenOption ) ) {
    // Get file for logging
    QString loggingFileName = infoPath + "/NoClick.log";
    fileHandle = fopen( ssrd_mouse::toStdString(loggingFileName).c_str(), "w" );

    if( nullptr == fileHandle ) {
      qInfo() << "Error opening to write \""
	      << loggingFileName << "\"";

      return -1;
    }
  }
  else {
    qInfo() << "Logging to screen";
    fileHandle = stdout;
  }
    
  qInstallMessageHandler( myMessageOutput );

  if( ssrd_mouse::writeIniFileNameToScreen ) {
    QString temp('"' + s.fileName() + '"');
    cout << ssrd_mouse::toStdString(temp).c_str() << endl;
  }


  //------------------ Run the app
  ssrd_mouse::NoClick mainWindow;

  #if 0
  // Icons are for QWindow, not QMainWindow
  QIcon icon( infoPath + "/noclickIcon.png" );
  mainWindow.setIcon( icon );
  #endif
  
  int retval = qapp.exec();

  if( nullptr != fileHandle ) {
    fclose( fileHandle );
  }

  return retval;
}// mainBody


int main( int argc, char* argv[] )
{
  return mainBody( argc, argv );
}// main
