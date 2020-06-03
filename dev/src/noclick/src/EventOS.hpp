/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef EVENT_OS_HPP
#define EVENT_OS_HPP

namespace ssrd_event {

  class EventOS {

  public:
    // "move" places the cursor at the click location.
    // Set to false for the first click of a double click.
    bool sendMouseClick( unsigned int button,
			 int screenX, // Screen XY of pt
			 int screenY,
			 bool move = true );

    bool sendMouseDown( unsigned int button,
			int screenX, // Screen XY of pt
			int screenY,
			 bool move = true  );
    
    bool sendMouseUp( unsigned int button,
		      int screenX, // Screen XY of pt
		      int screenY,
			 bool move = true  );

    bool sendMouseDClick( int screenX, // Screen XY of pt
			  int screenY,
			  bool move = true  );

    bool sendMouseMove( int screenX, // Screen XY of pt
			int screenY );

    EventOS( int minx, // Desktop coordinates
	     int miny,
	     int w,
	     int h ) :
      dtMinx_( minx ),
      dtMiny_( miny ),
      dtWidth_( w ),
      dtHeight_( h )
    {}
    
    ~EventOS();
    
  private:
    bool init( void );

    // Desktop dimensions
    const int dtMinx_;
    const int dtMiny_;
    const int dtWidth_;
    const int dtHeight_;
  };

}


#endif
