/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef NOCLICK_LABEL_HPP
#define NOCLICK_LABEL_HPP



#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <memory>

/*
  Abstract base class for labels that are activated 
  by the mouse being stationary inside the label.

  Intended to replace the click over a region, even
  if the cursor is hidden -- the click occurs under
  the mouse cursor, not the simulated cursor (which 
  may be hidden).
*/

namespace ssrd_mouse {

  class NoClick;

  class NoClickLabel : public QPushButton { //QLabel {
  public:

  private:
    //    Q_OBJECT

    std::unique_ptr< QTimer > timer_;

  public:
    NoClickLabel( const QString &text,
		  QWidget *parent = nullptr,
		  Qt::WindowFlags f = Qt::WindowFlags() );

    ~NoClickLabel();

    static
    NoClick* mainWindow( void );

    void startTimer( void );
    void restartTimerIfRunning( void );
    void haltTimer( void );

  protected:
    bool showingApparatus_ = false; // Set by NoClickActive

    void enterEvent( QEvent *event );
    void leaveEvent( QEvent *event );
    
    virtual void stationaryAction( bool toggle = true ) = 0;


  public slots:
    void checkStationary( void );
    void clicked( bool checked = false );
  };


}
#endif
