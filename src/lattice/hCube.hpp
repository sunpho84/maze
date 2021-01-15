#ifndef _H_CUBE_HPP
#define _H_CUBE_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file hCube.hpp

#include <lattice/lxCoordsProvider.hpp>
#include <lattice/world.hpp>

namespace maze
{
  /// Hypercube
  template <int _NDims,
	    typename _Index,
	    UseHashedCoords UHC>
  struct HCube
  {
    /// Number of dimensions
    static constexpr int nDims=
      _NDims;
    
    /// Index type
    using Index=
      _Index;
    
    /// Direction index
    using Direction=
      typename World<nDims>::Direction;
    
    /// Keep trace if hashed or not
    static constexpr UseHashedCoords isHashed=
      UHC;
    
    /// Sizes of the hypercube
    const Coords<nDims> sizes;
    
    /// Keeps note whether each direction is periodic, or not
    const Coords<nDims> periodic;
    
    /// Hypercube volume
    const Index vol;
    
    /// Half the hypervolume
    const Index volH;
    
    /// Check if it has bulk
    const bool hasBulk;
    
    /// Compute the bulk volume
    const Index bulkVol;
    
    /// Holds the coordinates or compute them
    HashedOrNotLxCoords<nDims,Index,UHC> coordsProvider;
    
    /// Construct from sizes
    HCube(const Coords<nDims>& sizes,
	   const Coords<nDims>& periodic) :
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
    
    /// Check if the hypercube has a bulk
    ///
    /// Each dimension must be periodic, or have size>=2
    bool computeHasBulk() const
    {
      /// Result initialized to true
      bool hasBulk=true;
      
      for(Direction mu=0;mu<nDims;mu++)
	hasBulk&=(periodic[mu] or sizes[mu]>=2);
      
      return hasBulk;
    }
    
    /// Compute the bulk volume
    Index computeBulkVol() const
    {
      /// Initial value
      Index bulkVol=1;
      
      for(Direction mu=0;mu<nDims;mu++)
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
    Index computeLxOfCoords(const Coords<nDims>& coords) const
    {
      Index out=0;
      
      for(Direction mu=0;mu<nDims;mu++)
	out=out*sizes[mu]+coords[mu];
      
      return out;
    }
    
    /// Computes the coordinates from the lexicographic index
    Coords<nDims> computeCoordsOfLx(Index site /* don't make it const */) const
    {
      /// Result coordinates
      Coords<nDims> coords;
      
      for(Direction mu=nDims-1;mu>=0;mu--)
	{
	  coords[mu]=site%sizes[mu];
	  site/=sizes[mu];
	}
      
      return coords;
    }
    
    /// Computes the table of the coordinates of all sites
    Vector<Coords<nDims>> computeCoordsOfAllLx() const
    {
      /// Precompute volume
      const Index vol=computeVol();
      
      /// Allocates the table
      Vector<Coords<nDims>> coordsOfAllLx(vol);
      
      for(Index site=0;site<vol;site++)
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
