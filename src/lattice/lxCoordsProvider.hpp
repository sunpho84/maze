#ifndef _LX_COORDS_PROVIDER_HPP
#define _LX_COORDS_PROVIDER_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file lxCoordsProvider.hpp

#include <lattice/coords.hpp>
#include <resources/vector.hpp>

namespace maze
{
  /// Options for lookup-table usage
  enum UseHashedCoords{HASHED,NOT_HASHED};
  
  /// Uses a lookup table for the coordinates
  template <int _NDims,
	    typename Index>
  struct LxCoordsLookupTable
  {
    /// Number of dimensions
    static constexpr int nDims=
      _NDims;
    
    /// Lookup table of coordinates
    const Vector<Coords<nDims>> coordsOfLxTable;
    
    /// Constructor
    template <typename HC>
    LxCoordsLookupTable(HC&& grid) :
      coordsOfLxTable(grid.computeCoordsOfAllLx())
    {
    }
    
    /// Return data from the lookup table
    template <typename HC>
    INLINE_FUNCTION const Coords<nDims>& coordsOfLx(HC&& grid,
						   const Index& id) const
    {
      return coordsOfLxTable[id];
    }
  };
  
  /// Do not hold the lookup table
  template <int _NDims,
	    typename Index>
  struct LxCoordsNoLookupTable
  {
    /// Number of dimensions
    static constexpr int nDims=
      _NDims;
    
    /// Constructor
    template <typename HC>
    LxCoordsNoLookupTable(HC&& grid)
    {
    }
    
    /// Computes
    template <typename HC>
    INLINE_FUNCTION Coords<nDims> coordsOfLx(HC&& grid,
					     const Index& id) const
    {
      return grid.computeCoordsOfLx(id);
    }
  };
  
  /// Use or not the lookup table
  template <int NDim,
	    typename Index,
	    UseHashedCoords UHC>
  using HashedOrNotLxCoords=
    std::conditional_t<UHC==HASHED,LxCoordsLookupTable<NDim,Index>,LxCoordsNoLookupTable<NDim,Index>>;
}

#endif
