#ifndef _COORDS_HPP
#define _COORDS_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file coords.hpp

#include <array>
#include <ostream>

#include <lattice/world.hpp>

namespace maze
{
  /// Cartesian coordinates
  template <int nDims>
  struct Coords
  {
    /// Direction type
    using Direction=
      typename World<nDims>::Direction;
    
    /// Internal data
    std::array<int,nDims> data;
    
    /// Const subscribe operator
    constexpr const int& operator[](const int& mu) const
    {
      return data[mu];
    }
    
    /// Subscribe operator
    constexpr int& operator[](const int& mu)
    {
      return data[mu];
    }
    
    /// Homoegeneus initialization
    static constexpr Coords getAll(const int& V=1)
    {
      /// Result
      Coords out{};
      
      for(Direction mu=0;mu<nDims;mu++)
	out[mu]=V;
      
      return out;
    }
    
    /// Semi-Homoegeneus initialization
    static constexpr Coords getAllBut(const int& V,
				      const int& Mu)
    {
      /// Result
      Coords out{};
      
      for(Direction mu=0;mu<nDims;mu++)
	if(Mu==mu)
	  out[mu]=0;
	else
	  out[mu]=V;
      
      return out;
    }
    
#define PROVIDE_BINOP(OP)			\
    /*! Implements OP operator */		\
    Coords operator OP(const Coords& oth) const	\
    {						\
      /*! Result */				\
      Coords res;				\
						\
      for(Direction mu=0;mu<nDims;mu++)		\
	res[mu]=(*this)[mu] OP oth[mu];		\
						\
      return res;				\
    }
    
    PROVIDE_BINOP(+)
    PROVIDE_BINOP(-)
    PROVIDE_BINOP(*)
    PROVIDE_BINOP(/)
    PROVIDE_BINOP(%)
    
#undef PROVIDE_BINOP
    
#define PROVIDE_BINOP(OP)			\
    /*! Implements OP operator with scalar*/	\
    Coords operator OP(const int& oth) const	\
    {						\
      /*! Result */				\
      Coords res;				\
						\
      for(Direction mu=0;mu<nDims;mu++)		\
	res[mu]=(*this)[mu] OP oth;		\
						\
      return res;				\
    }
    
    PROVIDE_BINOP(+)
    PROVIDE_BINOP(-)
    PROVIDE_BINOP(*)
    PROVIDE_BINOP(/)
    PROVIDE_BINOP(==)
    PROVIDE_BINOP(%)
    
#undef PROVIDE_BINOP
    
    /// Sum of all the coordinates
    int sumAll() const
    {
      int coordsSum=0;
      
      for(Direction mu=0;mu<nDims;mu++)
	coordsSum+=(*this)[mu];
      
      return coordsSum;
    }
    
    /// Product of all the coordinates
    int prodAll() const
    {
      int coordsProd=1;
      
      for(Direction mu=0;mu<nDims;mu++)
	coordsProd*=(*this)[mu];
      
      return coordsProd;
    }
    
    /// Returns a versor
    static Coords<nDims> versor(Direction mu)
    {
      /// Result
      Coords<nDims> v{};
      
      v[mu]=1;
      
      return v;
    }
    
    /// Return fasted dimension of even local size
    ///
    /// Returns nDims if none found
    Direction fastestLocalEvenDimension() const
    {
      /// Result
      Direction res(nDims);
      
      // Find last even dimension
      for(Direction mu=0;mu<nDims;mu++)
	if((*this)[mu]%2==0)
	  res=mu;
      
      return res;
    }
    
  };
  
  /// Output a coord
  template <int nDims>
  std::ostream& operator<<(std::ostream& os,
			   const Coords<nDims>& c)
  {
    os<<"{"<<c[0];
    for(int mu=1;mu<nDims;mu++)
      os<<","<<c[mu];
    os<<"}";
    
    return os;
  }
  
  /// 1 in all directions
  template <int nDims>
  constexpr Coords<nDims> allDimensions=
	Coords<nDims>::getAll(1);
  
  /// 0 in all directions
  template <int nDims>
  constexpr Coords<nDims> noDimensions=
	Coords<nDims>::getAll(0);
  
  /// All directions but a given one
  template <int nDims,
	    int Mu>
  constexpr Coords<nDims> allDimensionsBut=
	Coords<nDims>::getAllBut(1,Mu);
}

#endif
