/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickQMenu.hpp"
#include "NoClick.hpp"
#include <QDebug>
#include <QHideEvent>

namespace ssrd_mouse {

  void NoClickQMenu::hideEvent( QHideEvent *event ) {
    qInfo() << "NoClickQMenu::hideEvent spontaneous="
	    << event->spontaneous()
	    << ", Type= " << event->type()
	    << ", WindowType= " << windowType()
	    << ", isWindow= " << isWindow();

    
#if 0
	if( NoClick::instance()->currentPanes()->isVisible() ) {
      qInfo() << "NoClickQMenu::hideEvent Cancelling";
      
	event->accept();
	return;
      }
    
    qInfo() << "NoClickQMenu::hideEvent Accepting";
#endif
    QMenu::hideEvent( event );
  }// NoClickQMenu::hideEvent




}
