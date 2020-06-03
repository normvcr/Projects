#ifndef GenArchiveIO_TPP
#define GenArchiveIO_TPP

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "GenArchiveIO.h"
#include "GEN_IoUtil.h"
#include "GEN_Enum.h"
#include "BootParse.h"
#include "GenLogging.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include "GenArchiveWriteDirector.h"
#include "GenArchiveReadDirector.h"
//#include "GenArchiveRootIO.tpp"
//#include "GenArchiveManager3.h"

template< typename T >
std::string toString( T val ) {
  return GEN_Enum<T>::toString( val );
}

template< typename T >
std::string toInteger( T val ) {
  typedef typename GEN_Enum<T>::IntegerType IntegerType;
    
  return IntegerType( val );
}

// Outputs  N | enumName
template< typename E >
std::ostream& operator<<( std::ostream& os,
			  const GenEnumType<E>& var )
{
  return
    os << toString( var.vaLue_ );
}

namespace GenArchive
{

  template< typename T >
  typename std::enable_if< 0 < std::extent<T>::value&&
  !std::is_same< char,
		typename std::remove_all_extents
			 < typename std::remove_const<T>::type >::type >::value,
			       bool >::type
  copy( ReadDirector& rd,
	T& var )
  {
    for( unsigned int ind = 0; ind < std::extent<T>::value; ++ind )
    {
      copy( rd, var[ind] );
    }

    return true;
  }
  
  template< typename T >
  typename std::enable_if< 0 < std::extent<T>::value&&
  !std::is_same< char,
		typename std::remove_all_extents<T>::type >::value,
			       bool >::type
  copy( WriteDirector& wd,
	const T& var )
  {
    for( unsigned int ind = 0; ind < std::extent<T>::value; ++ind )
    {
      writeOnly( wd, " " );
      copy( wd, var[ind] );
    }

    return true;
  }

  template< typename T >
  typename std::enable_if< 0 < std::extent<T>::value &&
  std::is_same< char,
		typename std::remove_all_extents<T>::type >::value,
			       bool >::type
			   copy( WriteDirector& wd,
				 const T& var )
  {
    std::string str( var );
    return copy( wd, str );
  }

      template< typename T >
  typename std::enable_if< 0 < std::extent<T>::value &&
  std::is_same< char,
		typename std::remove_all_extents<T>::type >::value,
			       bool >::type
			   copy( ReadDirector& rd,
				T& var )
  {
    std::string str;
    copy( rd, str );
    
    if( str.size() > std::extent<T>::value )
    {
      SLOG_ALERT( "copy(Read, char[N]): Reading string bigger than N" );
      return false;
    }

    strcpy( var, str.c_str() );
    return true;
  }
    
  template< typename T >
  typename std::enable_if< 0 == std::extent<T>::value &&
	  std::is_same< const char*, T >::value,
    bool >::type
  copy( WriteDirector& wd, const T& val )
  {
    wd.os_ << val;  
    return !wd.os_.fail();
  }

  template< typename T >
  typename std::enable_if< 0 == std::extent<T>::value &&
	  std::is_same< const char*, T >::value,
    bool >::type
  copy( ReadDirector& rd, const T& val )
  {
    rd.is_ >> skipComments >> val; 
    return !rd.is_.fail(); 
  }
    
  //=================== Statements ========================

  template< typename T >
  typename std::enable_if< !std::is_enum< T >::value, bool >::type 
  copy( WriteDirector& wd, 
	const T& var, 
	const char* memb )
  {
    std::ostream& os = wd.os_;

    wd.incrementIndentLevel();
    wd.indent();

    os 
      << statementBegin_
      << memb
      << ' ' << statementJoin_ << ' ';

    if( !os || !copy_poly( wd, var ) )
    {
      SLOG_ALERT( "copy(ostream,not enum,char*)" << "Error" );
      return false;
    }
    
    os << statementEnd_ << std::endl;
    wd.decrementIndentLevel();

    return !os.fail();
  }// copy( WD, not enum, char*)

  template< typename T >
  typename std::enable_if< std::is_enum< T >::value, bool >::type 
  copy( WriteDirector& wd, 
	const T& var, 
	const char* memb )
  {
    std::ostream& os = wd.os_;
    
    wd.incrementIndentLevel();
    wd.indent();

    os 
      << statementBegin_
      << memb
      << ' ' << statementJoin_ << ' ';

    if( !os || !write( os, var ) )
    {
      SLOG_ALERT( "copy(ostream,enum,char*)" << "Error" );
      return false;
    }
 
    os << statementEnd_;

    auto ival = GEN_Enum<T>::cast( var );
    
    os << " # " << ival << std::endl;
    wd.decrementIndentLevel();
    
    return !os.fail();    
  }// copy( ostream, enum, char*)


  template< typename T >
  bool copy( ReadDirector& rd, 
	     T& var, 
	     const char* memb )
  {
    std::istream& is = rd.is_;
    
    if( !is )
    {
      SLOG_ALERT( "copy(Read,T,char*): Error entering for member "
		  << memb );

      return false;
    }

    is >> skipComments >> memb
       >> skipComments >>  "=" >> skipComments;
    
    if( !is )
    {
      SLOG_ALERT( "copy(Read,T,char*): Error at \""
	   << memb << " = \"" );

      return false;
    }

    if( !copy_poly( rd, var ) )
    {
      SLOG_ALERT( "copy(Read,T,char*): Error from copy(Read,char*)" 
		  << " for member " << memb );

      return false;
    }

    is >> skipComments >> statementEnd_;

    if( !is )
    {
      SLOG_ALERT( "copy(Read,T,char*): Error reading ';' for member " 
		  << memb );

      return false;
    }

    return !is.fail();
  }// copy( Read, T, char*)

  //=============== Atomic IO ==============================

  template< typename D >
  bool copy( WriteDirector& wd, const std::vector<D>& val )
  {
    wd.os_ << '[' << val.size() << ']';

    for( int ind = 0; ind < (int)val.size(); ++ind )
    {
      wd.os_ << ' ' << val[ ind ];
    }

    return !wd.os_.fail();
  }// write vector

  template< typename D> 
  bool copy( ReadDirector& rd, std::vector<D>& val )
  {
    int length = 0;

    rd.is_ >> skipComments >> "["
	   >> skipComments >> length
	   >> skipComments >> "]";

    val.resize( length );

    for( int ind = 0; ind < length; ++ind )
    {
      //      rd.is_ >> skipComments >> val[ ind ];
      rd.is_ >> skipComments;

      copy( rd, val[ ind ] );
    }

    return !rd.is_.fail();
  }// read vector

  

  template< typename T >
  typename std::enable_if< std::is_enum< T >::value, bool >::type 
  copy( WriteDirector& wd, const T& var )
  {
    wd.os_ << GEN_Enum<T>::toString( var );
    return !wd.os_.fail();
  }// write(enum)


  template< typename T >
  typename std::enable_if< std::is_enum< T >::value, bool >::type 
  copy( ReadDirector& rd, T& var )
  {
    std::istream& is = rd.is_;
    is >> skipComments;

    char c;

    if( !is.get( c ) )
    {
      SLOG_CRITICAL
	( "GEN_IoCopy<enum>(istream,T): Error getting first char" );

      return false;
    }

    // Will set this to an integer in the range of the enum.
    typename GEN_Enum< T >::IntegerType ivar;

    if( '_' == c ||
	isalpha( c ) )
    {
      // Must be string enum field name
      std::stringstream str;
      str << c;

      while( is.get(c) && ( isalnum(c) || '_' == c ) )
      {
	str << c;
      }
      
      // fail bit is set on EOF.  This is not a failure for us.
      if( is )
      {
	is.unget();
      }
      else if( is.eof() )
      {
	is.clear( );
      }

      if( !GEN_Enum<T>::toInteger( str.str().c_str(), ivar ) )
      {
	SLOG_CRITICAL( "GEN_IoCopy<enum>(istream,T): Error from toInteger: "
		       << '"' << str.str().c_str() << '"' );

	rd.setFail();
	return false;
      }
    }
    else
    {
      // Must be numeric value
      is.unget();

      if( !copy( rd, ivar ) )
      {
	SLOG_CRITICAL
	  ( "GEN_IoCopy<enum>(istream,T):Numeric:  Error from GEN_IoCopy" );

	return false;
      }

      if( !GEN_Enum<T>::verify( ivar ) )
      {
	SLOG_CRITICAL
	  ( "GEN_IoCopy<enum>(istream,T):Numeric:  Error from verify: "
	    << ivar << std::endl );

	rd.setFail();
	return false;
      }
    }

    // ivar has been verified to be in the proper range
    var = static_cast<T>( ivar );

    return true;
  }// read(enum)

  template< typename T >
  bool write( const char* fileName, const T& var, Manager3* mgr )
  {
    std::ofstream fil( fileName );
    setScientific( fil, scientificPrecision_ );

    if( !fil )
    {
      SLOG_CRITICAL( "GenArchive::write: Error opening file \""
		     << fileName
		     << "\"" );

      return false;
    }

    if( !write( fil, var, mgr ) )
    {
      SLOG_CRITICAL( "GenArchive::write: Error writing file \""
		     << fileName
		     << "\"" );

      return false;
    }

    return true;
  }// write(const char*)
  
  template< typename T >
  bool read( const char* fileName, T& var, Manager3* mgr )
  {
    if( nullptr == fileName )
    {
      SLOG_CRITICAL( "GenArchive::read: NULL ptr supplied for fileName" );
      
      return false;
    }
    
    std::ifstream fil( fileName );
  
    if( !fil )
    {
      SLOG_CRITICAL( "GenArchive::read: Error opening file \""
		     << fileName
		     << "\"" );

      return false;
    }

    if( !read( fil, var, mgr ) )
    {
      SLOG_CRITICAL( "GenArchive::read: Error reading file \""
		     << fileName
		     << "\"" );

      return false;
    }

    return true;
  }// read(const char*)

  template< typename T >
  bool read( std::istream& is, T& var, Manager3* mgr )
  {
    ReadDirector rd(is);
    rd.setManager( mgr );
    return copy_poly( rd, var );
  }
  
  template< typename T >
  bool write( std::ostream& os, const T& var, Manager3* mgr )
  {
    WriteDirector wd(os);
    wd.setManager( mgr );
    return copy( wd, var );
  }
  

  //=============== bare pointers =========================

  template< typename T >
  typename std::enable_if<
    !std::is_same< char, typename std::remove_const< T >::type >::value,
    bool >::type
  copy( WriteDirector& wd,
	     const T*& var )
  {
    return copy
      ( wd, 
	reinterpret_cast< uintptr_t >( static_cast< const void* > ( var ) ) );
  }

  template< typename T >
  typename std::enable_if<
    !std::is_same< char, typename std::remove_const< T >::type >::value,
    bool >::type
  copy( ReadDirector& wd,
	T*&  var )
  {
    uintptr_t value;

    if( !copy( wd, value) )
    {
      SLOG_ERROR( "copy(T*&): Error reading to uintptr_t" );
      return false;
    }

    var = static_cast< T* >( static_cast< void* > ( var ) );
    return true;
  }


}// namespace GenArchive

#endif
