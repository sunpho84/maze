#ifndef _LX_HPP
#define _LX_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file lxGrid.hpp
#include <lattice/coords.hpp>
#include <resources/vector.hpp>

namespace maze
{
  /// Options for lookup-table usage
  enum UseHashedCoords{HASHED,NOT_HASHED};
  
  /// Lexicographic grid
  template <int NDim,
	    typename Index,
	    UseHashedCoords HC>
  struct LxGrid;
  
  /// Uses a lookup table for the coordinates
  template <int NDim,
	    typename Index>
  struct LxCoordsLookupTable
  {
    /// Lookup table of coordinates
    const Vector<Coords<NDim>> coordsOfLxTable;
    
    /// Constructor
    LxCoordsLookupTable(const LxGrid<NDim,Index,HASHED>& grid) :
      coordsOfLxTable(grid.computeCoordsOfAllLx())
    {
    }
    
    /// Return data from the lookup table
    INLINE_FUNCTION const Coords<NDim>& coordsOfLx(const LxGrid<NDim,Index,HASHED>& grid,
						   const Index& id) const
    {
      return coordsOfLxTable[id];
    }
  };
  
  /// Do not hold the lookup table
  template <int NDim,
	    typename Index>
  struct LxCoordsNoLookupTable
  {
    /// Constructor
    LxCoordsNoLookupTable(const LxGrid<NDim,Index,NOT_HASHED>& grid)
    {
    }
    
    /// Computes
    INLINE_FUNCTION Coords<NDim> coordsOfLx(const LxGrid<NDim,Index,NOT_HASHED>& grid,
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
  
  /// Lexicographic grid
  template <int NDim,
	    typename Index,
	    UseHashedCoords UHC>
  struct LxGrid
  {
    /// Number of dimensions
    static constexpr int nDim=
      NDim;
    
    /// Keep trace if hashed or not
    static constexpr UseHashedCoords isHashed=
      UHC;
    
    /// Sizes of the grid
    const Coords<NDim> sizes;
    
    /// Keeps note whether each direction is periodic, or not
    const Coords<NDim> periodic;
    
    /// Grid volume
    const Index vol;
    
    /// Half the grid volume
    const Index volH;
    
    /// Check if it has bulk
    const bool hasBulk;
    
    /// Compute the bulk volume
    const Index bulkVol;
    
    /// Holds the coordinates or compute them
    HashedOrNotLxCoords<NDim,Index,UHC> coordsProvider;
    
    /// Construct from sizes
    LxGrid(const Coords<NDim>& sizes,
	   const Coords<NDim>& periodic) :
      sizes(sizes),
      periodic(periodic),
      vol(this->computeVol()),
      volH(vol/2),
      hasBulk(computeHasBulk()),
      bulkVol(computeBulkVol()),
      coordsProvider(*this)
    {
    }
    
    /// Compute the volume
    Index computeVol() const
    {
      return Index(sizes.prodAll());
    }
    
    /// Check if the grid has a bulk
    ///
    /// Each dimension must be periodic, or have size>=2
    bool computeHasBulk() const
    {
      /// Result initialized to true
      bool hasBulk=true;
      
      /// Dimension
      for(int mu=0;mu<NDim;mu++)
	hasBulk&=(periodic[mu] or sizes[mu]>=2);
      
      return hasBulk;
    }
    
    /// Compute the bulk volume
    Index computeBulkVol() const
    {
      /// Initial value
      Index bulkVol{1};
      
      for(int mu=0;mu<NDim;mu++)
	bulkVol*=
	  periodic[mu]?
	  sizes[mu]:
	  std::max(sizes[mu]-2,0);
	  
      return bulkVol;
    }
    
    /// Compute the surface volume
    Index computeSurfVol() const
    {
      return computeVol()-computeBulkVol();
    }
    
    /// Computes the lexicographic index
    Index computeLxOfCoords(const Coords<NDim>& coords) const
    {
      Index out(0);
      
      for(int mu=0;mu<NDim;mu++)
	out=out*sizes[mu]+coords[mu];
      
      return out;
    }
    
    /// Computes the coordinates from the lexicographic index
    Coords<NDim> computeCoordsOfLx(Index site /* don't make it const */) const
    {
      /// Result coordinates
      Coords<NDim> coords;
      
      for(int mu=NDim-1;mu>=0;mu--)
	{
	  coords[mu]=site%sizes[mu];
	  site/=sizes[mu];
	}
      
      return coords;
    }
    
    /// Computes the table of the coordinates of all sites
    Vector<Coords<NDim>> computeCoordsOfAllLx() const
    {
      /// Precompute volume
      const Index vol=computeVol();
      
      /// Allocates the table
      Vector<Coords<NDim>> coordsOfAllLx(vol);
      
      for(Index site(0);site<vol;site++)
	coordsOfAllLx[site]=this->computeCoordsOfLx(site);
      
      return coordsOfAllLx;
    }
    
    /// Returns coordinates of lx site, using lookup table if available
    decltype(auto) coordsOfLx(const Index& id) const
    {
      return coordsProvider.coordsOfLx(*this,id);
    }
  };
}

#endif
