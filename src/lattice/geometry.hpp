#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file geometry.hpp

#include <lattice/hCube.hpp>
#include <lattice/world.hpp>
#include <tensors/component.hpp>

namespace maze
{
  /// Structure to incapsulate the lexicographic grids needed to move across a lattice
  ///
  /// In the naming scheme, Lx denotes a site
  template <int _NDims=4>
  struct Geometry
  {
    /// Number of dimensions
    static constexpr int nDims=
      _NDims;
    
    /// Direction index
    using Direction=
      typename World<nDims>::Direction;
    
    /// Number of oriented directions
    static constexpr int nOrientedDirs=
      World<nDims>::nOrientedDirs;
    
    DECLARE_COMPONENT(GlbSite,int64_t,DYNAMIC,glbSite);
    DECLARE_COMPONENT(LocSite,int64_t,DYNAMIC,locSite);
    DECLARE_COMPONENT(Rank,int32_t,DYNAMIC,rank);
    DECLARE_COMPONENT(Parity,int8_t,2,eoDiscriminator);
    
    /// Describing how to match even and odd sites
    using ParityHCube=
      HCube<nDims,Parity,NOT_HASHED>;
    
    /// Print 0 or 1 for parity (which is of unprintable type)
    INLINE_FUNCTION
    friend std::ostream& operator<<(std::ostream& os,
				    const Parity& par)
    {
      return
	os<<static_cast<int>(par);
    }
    
    /// Global lattice grid
    using GlbGrid=
      HCube<nDims,GlbSite,NOT_HASHED>;
    
    /// Ranks grid
    using RanksGrid=
      HCube<nDims,Rank,HASHED>;
    
    /// Local lattice grid
    using LocGrid=
      HCube<nDims,LocSite,HASHED>;
    
    /// Global site grid
    const GlbGrid glbGrid;
    
    /// Global sizes
    const Coords<nDims>& glbSizes=
	    glbGrid.sizes;
    
    /// Global volume
    const GlbSite& glbVol=
      glbGrid.vol;
    
    /// Ranks grid
    const RanksGrid ranksGrid;
    
    /// Number of ranks per dimension
    const Coords<nDims>& nRanksPerDim=
	    ranksGrid.sizes;
    
    /// Neighoboring ranks
    const std::array<Rank,nOrientedDirs> rankNeighs;
    
    /// Store whether the directions are fully local
    const Coords<nDims> isDirectionFullyLocal;
    
    /// Local sites grid
    const LocGrid locGrid;
    
    /// Local sizes
    const Coords<nDims>& locSizes=
	    locGrid.sizes;
    
    /// Local volume
    const LocSite& locVol=
      locGrid.vol;
    
    /// Half the local volume
    const LocSite& locVolH=
      locGrid.volH;
    
    /// Parity and eos index of local sites
    const Vector<Parity> _locLxParityTable;
    
    /// Parity of a site, given its global coordinates
    Parity parityOfGlbCoords(const Coords<nDims>& c) const
    {
      return static_cast<Parity>(c.sumAll()%2);
    }
    
    /// Compute the parity of a global site
    Parity parityOfGlbLx(const GlbSite& id) const
    {
      return parityOfGlbCoords(glbGrid.computeCoordsOfLx(id));
    }
    /// Returns the parity of a local site from the lookup table
    Parity parityOfLocLx(const LocSite& id) const
    {
      return _locLxParityTable[id];
    }
    
    /// Compute explicitly the parity of local sites
    Parity computeParityOfLocLx(const LocSite& id) const
    {
      return parityOfGlbCoords(glbCoordsOfLocLx(id));
    }
    
    /// Compute the lx site of a given set of coords
    LocSite computeLxOfLocCoords(const Coords<nDims>& coords) const
    {
      return locGrid.computeLxOfCoords(coords);
    }
    
    /// Returns local site coords
    const Coords<nDims>& locCoordsOfLocLx(const LocSite& locLx) const
    {
      return locGrid.coordsOfLx(locLx);
    }
    
    /// Compute global coordinates, given local site and rank
    Coords<nDims> glbCoordsOfLocLx(const LocSite& locLx,
				   const Rank& rankId=rank(thisRank())) const
    {
      /// Local coordinates
      const Coords<nDims> locCoords=
	locGrid.coordsOfLx(locLx);
      
      /// Coordinate of the rank
      const Coords<nDims> rankCoords=
	ranksGrid.coordsOfLx(rankId);
      
      /// Resulting coordinates
      Coords<nDims> glbCoords=
	locCoords+rankCoords*locGrid.sizes;
      
      return glbCoords;
    }
    
    /// Returns an hypercube of size 1x1x...x2(mu)x1...
    ParityHCube getParityGrid(const Direction& mu) const
    {
      return ParityHCube(Coords<nDims>::versor(mu)+
			 Coords<nDims>::getAll(1),allDimensions<nDims>);
    }
    
    /// Constructor
    Geometry(const Coords<nDims>& glbSizes,
	     const Coords<nDims>& ranksSizes) :
      glbGrid(glbSizes,allDimensions<nDims>),
      ranksGrid(ranksSizes,allDimensions<nDims>),
      isDirectionFullyLocal(ranksSizes==1 /* compare each direction to 1 */),
      locGrid(glbSizes/ranksSizes,isDirectionFullyLocal),
      _locLxParityTable(locVol,[this](const LocSite& lx)
      			      {
      				return this->computeParityOfLocLx(lx);
      			      })
    {
      if((glbSizes%ranksSizes).sumAll())
	CRASHER<<"Global sizes "<<glbSizes<<" incompatible with rank sizes "<<ranksSizes<<endl;
      
      if(locVolH*2!=locVol)
	CRASHER<<"Local volume must be even, is "<<locVol<<endl;
    }
  };
}

#endif
