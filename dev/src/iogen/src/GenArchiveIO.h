#ifndef GenArchiveIO_H
#define GenArchiveIO_H
/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
  Methods used in the auto-generated IO source files.
 */

#include <vector>
#include <string>
#include <memory>
#include <type_traits>
//#include "GenArchiveRootIOBase.h"
#include "GenArchiveWriteDirector.h"
#include "GenArchiveReadDirector.h"

//////////////////////////////////////////////////////////////
// A convenience to be able to write enums to ostream's. 
// For overloading operator<< 
// Don't want to do this with straight enums
  template< typename E >
    struct GenEnumType
    {
      E value_;

    GenEnumType( E val ) :
      value_( val )
      {}
    };

template< typename E >
GenEnumType<E> GenEnum( E val ) {
  return GenEnumType<E>( val );
}

// Will fail at link time if the GenArchiveIO has not been generated 
// for the enum.
template< typename E >
std::ostream& operator<<( std::ostream& os,
			  const GenEnumType<E>& var );

template< typename T >
std::string toString( T val );

/////////////////////////////////////////////////////////////////

namespace GenArchive 
{
  class WriteDirector;
  class ReadDirector;
  class Manager3;

  //-------- Redo with Root implementation ----------
  template< class RWD, typename T >
  bool copy_poly( RWD& rwd, T& var ) {
    return copy( rwd, var ); }
  //-------------------------------------------------
  

  // For now, only support 1D arrays
  // For non-char types, just list, separated by space.
  template< typename T >
  typename std::enable_if< 0 < std::extent<T>::value&&
  !std::is_same< char,
		typename std::remove_all_extents
    < typename std::remove_const<T>::type >::type >::value,
			       bool >::type
  copy( ReadDirector& rd,
	T& var );

  // For now, only support 1D arrays
  template< typename T >
  typename std::enable_if< 0 < std::extent<T>::value &&
  !std::is_same< char,
		 typename std::remove_all_extents<T>::type >::value,
			       bool >::type
  copy( WriteDirector& wd,
	const T& var );

  // For now, only support 1D arrays
  // For char, use std::string for the io.
  template< typename T >
  typename std::enable_if< 0 < std::extent<T>::value &&
  std::is_same< char,
		typename std::remove_all_extents<T>::type >::value,
			       bool >::type
			   copy( WriteDirector& wd,
				const  T& var );

  // For now, only support 1D arrays
  template< typename T >
  typename std::enable_if< 0 < std::extent<T>::value &&
  std::is_same< char,
		typename std::remove_all_extents<T>::type >::value,
			       bool >::type
			   copy( ReadDirector& rd,
				T& var );
  
  //========== For the  name = value; statements ===============
  // For all types other than enums
  template< typename T >
  typename std::enable_if< !std::is_enum< T >::value, bool >::type 
  copy( WriteDirector& wd, 
	const T& var, 
	const char* memb );

  // For enums.  Write a comment with the name of the integer value
  template< typename T >
  typename std::enable_if< std::is_enum< T >::value, bool >::type 
  copy( WriteDirector& wd, 
	const T& var, 
	const char* memb );

  template<>
  bool copy( WriteDirector& rd, 
	     const bool& var, 
	     const char* memb );

  template< typename T >
  bool write( std::ostream& os, 
	      const T& var, 
	      const char* memb ) {
    WriteDirector wd( os );
    return copy( wd, var, memb );
  }

  // For all other types
  template< typename T >
  bool copy( ReadDirector& rd, 
	     T& var, 
	     const char* memb );

  // For all other types
  template< typename T >
  bool read( std::istream& is, 
	     T& var, 
	     const char* memb )
  {
    ReadDirector rd( is );
    return copy( rd, var, memb );
  }

  //=========== Atomic IO =======================================
  bool copy( WriteDirector& wd, const int& val );
  bool copy( ReadDirector& rd, int& val );

  bool copy( WriteDirector& wd, const long int& val );
  bool copy( ReadDirector& rd, long int& val );

  bool copy( WriteDirector& wd, const short int& val );
  bool copy( ReadDirector& rd, short int& val );

  bool copy( WriteDirector& wd, const unsigned int& val );
  bool copy( ReadDirector& rd,  unsigned int& val );

  bool copy( WriteDirector& wd, const uintptr_t& val );
  bool copy( ReadDirector& rd,  uintptr_t& val );

  bool copy( WriteDirector& wd, const float& val );
  bool copy( ReadDirector& rd, float& val );

  bool copy( WriteDirector& wd, const char& val );
  bool copy( ReadDirector& rd, char& val );

  bool copy( WriteDirector& wd, const double& val );
  bool copy( ReadDirector& rd, double& val );

  bool copy( WriteDirector& wd, const bool& val );
  bool copy( ReadDirector& rd, bool& val );

  // Redefine the template interface for char, in order
  // for overloading to work.  Same implmentations, though.
  template< typename T >
  typename std::enable_if< 0 == std::extent<T>::value &&
	  std::is_same< const char*, T >::value,
    bool >::type
  copy( WriteDirector& wd, const T& val );
  //  bool copy( WriteDirector& wd, const char* val );

  template< typename T >
  typename std::enable_if< 0 == std::extent<T>::value &&
	  std::is_same< const char*, T >::value,
    bool >::type
  copy( ReadDirector& rd, const T& val );
  // bool copy( ReadDirector& rd, const char* val );

  bool copy( WriteDirector& wd, const std::string& val );
  bool copy( ReadDirector& rd, std::string& val );

  template< typename D >
  bool copy( WriteDirector& wd, const std::vector<D>& val );

  template< typename D> 
  bool copy( ReadDirector& rd, std::vector<D>& val );

  template< typename T >
  typename std::enable_if< std::is_enum< T >::value, bool >::type 
  copy( WriteDirector& wd, const T& var );
  
  template< typename T >
  typename std::enable_if< std::is_enum< T >::value, bool >::type 
  copy( ReadDirector& rd, T& var );

  //=============== bare pointers =========================
  // char pointers are handled elsewhere, and differently.
  template< typename T >
  typename std::enable_if<
    !std::is_same< char, typename std::remove_const< T >::type >::value,
    bool >::type
  copy( WriteDirector& wd,
	     const T*& var );

  template< typename T >
  typename std::enable_if<
    !std::is_same< char, typename std::remove_const< T >::type >::value,
    bool >::type
  copy( ReadDirector& wd,
	T*&  var );

  //================ Miscellaneous ===========================
  inline
  bool writeOnly( WriteDirector& wd, const char* var )
  { 
    return copy( wd, var ); 
  }

  inline
  bool writeOnly( ReadDirector& rd, const char* var ) 
  { 
    return true; 
  }

  template< typename RD >
  bool readWrite( RD& rd, const char* var )
  {
    return copy( rd, var );
  }

  template< typename T >
  bool write( std::ostream& os, const T& var, Manager3* mgr );


  template< typename T >
  bool write( std::ostream& os, const T& var )
  {
    return write( os, var, (Manager3*) nullptr );
  }

  template< typename T >
  bool write( const char* fileName, const T& var, Manager3* mgr );

  template< typename T >
  bool write( const char* fileName, const T& var ) {
    return write( fileName, var, nullptr );
  }

  template< typename T >
  bool write( const std::string fileName, 
	      const T& var )
  {
    return write( fileName.c_str(), var );
  }


  template< typename T >
  bool read( std::istream& is, T& var, Manager3* mgr );

  template< typename T >
  bool read( std::istream& is, T& var )
  {
    return read( is, var, (Manager3*) nullptr );
  }

  template< typename T >
  bool read( const char* fileName, T& var, Manager3* mgr );

  template< typename T >
  bool read( const char* fileName, T& var ) {
    return read( fileName, var, nullptr );
  }
    
  template< typename T >
  bool read( const std::string fileName, 
	     T& var )
  {
    return read( fileName.c_str(), var );
  }

}// namespace GenArchive


#endif
