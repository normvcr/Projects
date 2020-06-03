#ifndef NO_CLICK_HPP
#define NO_CLICK_HPP
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
This is a top-level window.

-- Editable Control Window for the entire GUI: 
 o colours, time delays, sizes, ...
 o Toggle activation of this utility
-- Hot Cursor indicating where the mouse click will occur
-- Simulation windows: Left click, Right click, Double Click, Drag
*/

#include <QMainWindow>
#include <QTimer>
#include "NoClickHotCursor.hpp"
#include "NoClickAction.hpp"
#include "EventOS.hpp"
#include "PanesSet.hpp"

#include <memory>

#include "BootArch.h"
#if OS_IS_LINUX
  class QWebView;
  typedef QWebView NoClickViewer;
#else
  class QWebEngineView;
  typedef QWebEngineView NoClickViewer;
#endif



class QRadioButton;
class QGroupBox;

namespace ssrd_mouse {

  class NoClickPanes;
  class NoClickActive;
  class DragPane;
  class NoClickKbd;
  class NoClickDetailsWindow;
  class NoClickPanesWindow;


  extern NoClickKbd* noClickKeyboard;

  class NoClick : public QMainWindow {
  private:
    // Thses two are top-level windows.
    NoClickHotCursor hotCursor_;
    NoClickViewer* settingsHelpViewer_ = nullptr;

    QAction* settingsAction_ = nullptr;
    QLabel* aboutHelpViewer_;
    QAction* aboutAction_ = nullptr;
    QAction* exitAction_ = nullptr;
    QGroupBox* radioBox_ = nullptr; 

    PanesSet* panesSet_ = nullptr;

    QLabel* currentPanes( void ) {
      return panesSet_->clickPanes();
    }

    QLabel* starPane( void ) const {
      return panesSet_->starPane();
    }


    //- - - - - - -

    // When true, panes, star and hot cursor are hidden.
    // When false, normal operation
    bool showApparatus_ = true;

    // How long the mouse has been stationary for
    int stationaryDuration_ = 0;  // ms
    
    // How long the mouse has been moving for
     int movingDuration_ = 0;  // ms

    // Shows only star pane when true.
    // Shows only click panes when false
    void showPanes( bool allowStarPane = true );

    // Ends the drag at the hotCursor location
    void endDrag( void );

    void showActionMessage( void );
  public:
    // This source file was too complicated for clang to
    // parse in Windows.  See NoClickAction.hpp
    typedef ssrd_mouse::Action Action;


    static NoClick* instance( void );

    const QLabel* currentPanes( void ) const {
      return panesSet_->clickPanes();
    }

     NoClick( void );
    ~NoClick();
 
    // Called by the toggle button, whether to show or
    // hide the click boxes (and, hence,
    // whether they are active)
    // 
    void showApparatus( bool flag );
    bool showingApparatus( void ) const {
      return showApparatus_;
    }

    // Also sets state to StationaryEmpty
    void hideAll( void );

    // 1 = left, 2 = middle, 3 = right
    bool sendMouseClick( int buttonNumber );
    bool sendMouseDown( int buttonNumber );

    bool sendMouseUp( int buttonNumber );
    bool sendMouseUp( int buttonNumber, const QPoint& pt );

    bool sendMouseDoubleClick( void );
    bool sendMouseMove( void );

    void getHotPoint( QPoint& hotloc ) const {
      hotCursor_.getHotPoint( hotloc );
    }

    bool mouseIsStationary( void );

    void kbdInterrupt( int flag ); // slot
    
    void resetPanesSet( void ) {
      setPanesSet( panesSet_->type() );
    }

    void refreshShowHide( void );
    void restartShowHideIfRunning( void );

    void startPositionTimer( void );
    void startActionTimer( void );

  private:
    NoClick::Action action_ = StationaryEmpty;

    QPoint recentPoint_;  // Most recent point querried from the mouse.
    std::unique_ptr< QTimer > timer_; //To querry the mouse for movement
    std::unique_ptr< QTimer > timer2_; // To querry the action

    NoClickDetailsWindow* detailsWindow_ = nullptr;
    NoClickPanesWindow* panesWindow_ = nullptr;
    NoClickActive* showHideW_ = nullptr;

    std::unique_ptr< ssrd_event::EventOS > eventOS_;
    QRadioButton* radio1_;
    QRadioButton* radio2_;

    void shutDownMainWindow( void );

    void setPanesSet( PanesType );
    QMainWindow* createMainWindow( void );
    
 
  protected:
    void closeEvent(QCloseEvent *event);

  public slots:

    // Is called by timer every mouseInteval
    void checkMouse( void );

    // Is called by timer2 every actionInterval
    void checkAction( void );

        // These are called by the Drag radio buttons
    void setDrag1( void );
    void setDrag2( void );

    void toggleSettingsHelpViewer( bool checked = false );
    void toggleAboutHelpViewer( bool checked = false );
    void triggerExit( bool checked = false );

    // For the configuration of panes: Square or Line
    void squareClicked(bool checked = false );
    void lineClicked(bool checked = false );

    // Updating the opacity
    void starOpacityChanged( double opacity );
    void panesOpacityChanged( double opacity );

    void hoverButtonToggled( bool checked );
     void starLabelClicked(bool checked = false);
    void starLabelColored(const QColor &color);
    //--------------------------------------------
  };

}
#endif
