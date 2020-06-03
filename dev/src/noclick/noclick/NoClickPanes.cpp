/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "NoClickPanes.hpp"
#include "LeftClickPane.hpp"
#include "RightClickPane.hpp"
#include "DClickPane.hpp"
#include "DragPane.hpp"


namespace ssrd_mouse {

  NoClickPanes::NoClickPanes( int borderWidth ) :
    leftClickPane_( new LeftClickPane( this,
				       borderWidth ) ),
		    
    rightClickPane_( new RightClickPane( this,
					 borderWidth ) ),
    
    dClickPane_( new DClickPane( this,
				 borderWidth ) ),
    
    dragPane_( new DragPane( this,
			     borderWidth ) )
  {
    setWindowFlags( Qt::Tool
    		   | Qt::FramelessWindowHint
    		   | Qt::WindowStaysOnTopHint
    		   | Qt::WindowDoesNotAcceptFocus
    		   );

    setFocusPolicy( Qt::NoFocus );
    setAttribute(Qt::WA_ForceUpdatesDisabled);
    setAttribute(Qt::WA_ForceDisabled);
    setAttribute(Qt::WA_Disabled);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    
  }// NoClickPanes::NoClickPanes

}
