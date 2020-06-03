#ifndef NOCLICK_PanesSet_HPP
#define NOCLICK_PanesSet_HPP
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
class QLabel;
template< class T > class QList;
class QScreen;
class QPoint;


namespace ssrd_mouse {

  enum PanesType { SquarePanesType, LinePanesType };

  //const char* toString( PanesType pt );

  template< typename T >
  T fromString( const char* str );

  template<>
  PanesType fromString<PanesType>( const char* str );


  // To be compatible with square panes algorithms,
  // these enum values must not be changed.
  enum PaneID { NoPaneID = -1,
		DClickPaneID =  0,
		RightClickPaneID = 1,
		DragPaneID = 2,
		LeftClickPaneID = 3 };

  class PanesSet {

  public:

    PanesSet( void );
    virtual ~PanesSet();

    // Finds the pane within the current panes
    virtual PaneID paneOf( const QPoint& pt ) = 0;

    // Closes all the panes
    virtual void shutDown( void ) = 0;

    virtual QLabel* starPane( void ) = 0;
    virtual QLabel* clickPanes( void ) = 0;
    virtual void adjustPanes( const QPoint& loc,
			      bool forStarPane = true ) = 0;

    virtual PanesType type( void ) const = 0;

    //========== static stuff

    static void initialize( void );

    // Requires GUI system to be running
    // Returns -1 if not found
    static int getScreenIndex( const QPoint& point );

    static QLabel* createStarPane( void );

    static QList< QScreen* > screenList;  
  };

}

#endif
