#ifndef NO_CLICK_Action_HPP
#define NO_CLICK_Action_HPP
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// iogen cannot handle this being in NoClick.hpp on Windows


namespace ssrd_mouse {

    enum Action { //None = 0,
		  LeftClick = 1,
		  MiddleClick = 2,
		  RightClick = 3,
		  DoubleClick = 4,
		  Drag = 5,   // 1-stage drage
		  Drag0 = 6,  // Waiting to be stationary
		  EDrag = 7,  // Extended: 2-stage drag
		  EDrag0 = 8,  // Waiting to be switched to Drag0
		  MovingEmpty = 100,
		  
		  StationaryStar = 101,
		  MovingPanes = 102,
		  StationaryEmpty = 103,
		  
		  StationaryStarB = 201,
		  MovingPanesB = 202,
		  StationaryEmptyB = 203
    };


}

#endif

