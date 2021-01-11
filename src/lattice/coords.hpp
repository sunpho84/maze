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
      
      for(int mu=0;mu<NDim;mu++)
	out[mu]=V;
      
      return out;
    }
    
    /// Semi-Homoegeneus initialization
    static constexpr Coords getAllBut(const int& V,
				      const int& Mu)
    {
      /// Result
      Coords out{};
      
      for(int mu=0;mu<NDim;mu++)
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
      for(int mu=0;mu<NDim;mu++)		\
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
      for(int mu=0;mu<NDim;mu++)		\
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
    
    /// Returns a versor
    static Coords<NDim> versor(int mu)
    {
      /// Result
      Coords<NDim> v{};
      
      v[mu]=1;
      
      return v;
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
  
  /// 1 in all directions
  template <int NDim>
  constexpr Coords<NDim> allDimensions=
	Coords<NDim>::getAll(1);
  
  /// 0 in all directions
  template <int NDim>
  constexpr Coords<NDim> noDimensions=
	Coords<NDim>::getAll(0);
  
  /// All directions but a given one
  template <int NDim,
	    int Mu>
  constexpr Coords<NDim> allDimensionsBut=
	Coords<NDim>::getAllBut(1,Mu);
}

#endif
