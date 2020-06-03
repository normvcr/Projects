#ifndef GEN_ENUM_H
#define GEN_ENUM_H

/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <type_traits>
#include <string>
#include <vector>
#include "BootParse.h"

// Type T is only ever expected to be an enum.
// The private (integer,string) array, vsVec_, needs to have been properly
// coded.  This is verified at run-time during construction for consistency,
// but not for completeness.
template< typename T >
class GEN_Enum
{
 public:
  typedef typename std::underlying_type<T>::type IntegerType;
  
 private:
  // Looking at the set of pairs, an integer can occur on the left 
  // multiple times, but a string can occur on the right at most once.
  typedef 
    boost::bimap< boost::bimaps::multiset_of< IntegerType >, std::string >
    bm_type;

  static
  bm_type bm_;

  // Returns false if the element cannot be inserted i.e. if str is
  // already in the right side of the relation.
  static
  bool insert( IntegerType val, const std::string& str )
  {
    std::pair< typename bm_type::iterator,bool > 
      pr = bm_.insert( typename bm_type::value_type( val, str ) );

    return pr.second;
  }// insert

 public:
  // The fields must be initialized at construction of this struct.
  struct ValString
  {
    const IntegerType val_;
    const std::string str_;
  };

  GEN_Enum( void ) 
  {
    //    std::cout << "size(vsVec_)= " << vsVec_.size() << std::endl;

    for( int ind = vsVec_.size() - 1; ind >= 0; --ind )
    {
      const ValString& vs = vsVec_[ ind ];

      const std::string newStr = toCustomString(  vs.val_, vs.str_ );

      assert( ::isident( newStr.c_str() ) );

      // Check for programming error
      assert( insert( vs.val_, newStr ) );
    }

    //  write( std::cout );
  }

  static
  int size( void ) { return bm_.size(); }

  static
  bool write( std::ostream& os )
  {
    for( typename bm_type::const_iterator 
	   iter = bm_.begin(), iend = bm_.end();
	 iter != iend; 
	 ++iter )
      {
	std::cout << iter->right << " = " << iter->left << ", " << std::endl;
      }

    return !os.fail();
  }// write

  static
  IntegerType cast( T eval ) { return static_cast< IntegerType >( eval ); }


  // Returns true if val is in the range of the enum T, in which case
  // retrieves the corresponding enum string.
  static
  bool toString( IntegerType val, 
		 std::string& str ) 
  {
    return verify( val, &str );
  }//toString(IntegerType)

  static
  std::string toString( T eval )
  {
    return toString( cast( eval ) );
  }// toString(T)
    

  static
  std::string toString( IntegerType eval )
  {
    std::string str;
    toString( eval, str );
    return str;
  }// toString(T)
    

  // Returns true if val is in the range of the enum T.
  // If strp is nonnull, retrieves the corresponding enum string.
  static
  bool verify( IntegerType val, 
	       std::string* strp = nullptr )
  {
    typename bm_type::left_map::const_iterator 
      it = bm_.left.find( val );

    if( bm_.left.end() != it )
    {
      if( nullptr != strp )
      {
	*strp = it->second;
      }
      return true;
    }
    
    return false;
  }//verify(IntegerType)


  // If success is not NULL,
  // sets success to false if str is not an enum name.
  static
  bool toInteger( const std::string& str,
		  IntegerType& val )
  {
    return verify( str, &val );
  }// toInteger(string)

  static
  bool verify( const std::string& str,
	       IntegerType* valp = nullptr )
  {
    auto it = bm_.right.find( str );

    if( bm_.right.end() != it )
    {
      if( nullptr != valp )
      {
	*valp = it->second;
      }
      return true;
    }

    return false;
  }// verify(string)

 private:
  // Assumes str is one of the enum's field names for val.
  // This can be customized so as to use preferred names for io.
  static std::string toCustomString( IntegerType val, 
				     const std::string& str );

  // This vector must be initialized in the source code, when
  // the static vector is defined.
  static const std::vector< ValString > vsVec_;
};

template< typename T >
std::string GEN_Enum<T>::toCustomString( IntegerType val, 
					 const std::string& str ) 
{
  return str;
}

#endif
