#ifndef _LX_HPP
#define _LX_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file lx.hpp

#include <geometry/coords.hpp>
#include <resources/vector.hpp>

namespace maze
{
  /// Lexicographic grid
  template <int NDim,
	    typename Index>
  struct LxGrid
  {
    /// Sizes of the grid
    const Coords<NDim> sizes;
    
    /// Constructor
    LxGrid(const Coords<NDim>& sizes) : sizes(sizes)
    {
    }
    
    /// Compute the volume
    Index computeVol() const
    {
      return sizes.prodAll();
    }
    
    /// Check if the grid has a bulk
    bool getHasBulk() const
    {
      /// Result initialized to true
      bool hasBulk=true;
      
      int mu=0;
      while(hasBulk and mu<NDim)
	hasBulk&=(sizes[mu++]>=2);
      
      return hasBulk;
    }
    
    /// Compute the bulk volume
    Index computeBulkVol() const
    {
      Index bulkVol=1;
      
      for(int mu=0;mu<NDim;mu++)
	bulkVol*=std::max(sizes[mu]-2,0);
	  
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
      Index out=0;
      
      for(int mu=0;mu<NDim;mu++)
	out=out*sizes[mu]+coords[mu];
      
      return out;
    }
    
    /// Computes the coordinates from the lexicographic index
    Coords<NDim> computeCoordsOfLx(Index site) const
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
    Vector<Coords<NDim>> getCoordsOfAllLx() const
    {
      /// Precompute volume
      const Index vol=computeVol();
      
      /// Allocates the table
      Vector<Coords<NDim>> coordsOfLx(vol);
      
      for(Index site=0;site<vol;site++)
	coordsOfLx[site]=this->computeCoordsOfLx(site);
      
      return coordsOfLx;
    }
  };
}

#endif
