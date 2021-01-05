#ifndef _COORDS_HPP
#define _COORDS_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file coords.hpp

#include <array>
#include <ostream>

namespace maze
{
  /// Cartesian coordinates
  template <int NDim>
  struct Coords
  {
    /// Internal data
    std::array<int,NDim> data;
    
    // operator std::array<int,NDim>&()
    // {
    //   return data;
    // }
    
    // /// Convert
    // operator const std::array<int,NDim>&() const
    // {
    //   return data;
    // }
    
    /// Const subscribe operator
    const int& operator[](const int& mu) const
    {
      return data[mu];
    }
    
    /// Subscribe operator
    int& operator[](const int& mu)
    {
      return data[mu];
    }
    
#define PROVIDE_BINOP(OP)			\
    /*! Implements OP operator */		\
    Coords operator OP(const Coords& oth) const	\
    {						\
      /*! Result */				\
      Coords res;				\
						\
      for(int mu=0;mu<NDim;mu++)		\
	res[mu]=(*this)[mu] OP oth[mu];		\
						\
      return res;				\
    }
    
    PROVIDE_BINOP(+)
    PROVIDE_BINOP(-)
    PROVIDE_BINOP(*)
    PROVIDE_BINOP(/)
    
#undef PROVIDE_BINOP
    
#define PROVIDE_BINOP(OP)			\
    /*! Implements OP operator with scalar*/	\
    Coords operator OP(const int& oth) const	\
    {						\
      /*! Result */				\
      Coords res;				\
						\
      for(int mu=0;mu<NDim;mu++)		\
	res[mu]=(*this)[mu] OP oth;		\
						\
      return res;				\
    }
    
    PROVIDE_BINOP(+)
    PROVIDE_BINOP(-)
    PROVIDE_BINOP(*)
    PROVIDE_BINOP(/)
    
#undef PROVIDE_BINOP
    
    /// Sum of all the coordinates
    int sumAll() const
    {
      int coordsSum=0;
      
      for(int mu=0;mu<NDim;mu++)
	coordsSum+=(*this)[mu];
      
      return coordsSum;
    }
    
    /// Product of all the coordinates
    int prodAll() const
    {
      int coordsProd=1;
      
      for(int mu=0;mu<NDim;mu++)
	coordsProd*=(*this)[mu];
      
      return coordsProd;
    }
  };
  
  /// Output a coord
  template <int NDim>
  std::ostream& operator<<(std::ostream& os,
			   const Coords<NDim>& c)
  {
    os<<"{"<<c[0];
    for(int mu=1;mu<NDim;mu++)
      os<<","<<c[mu];
    os<<"}";
    
    return os;
  }
}

#endif
