#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file geometry.hpp

#include <lattice/lxGrid.hpp>
#include <tensors/component.hpp>

namespace maze
{
  /// Structure to incapsulate the lexicographic grids needed to move across a lattice
  ///
  /// In the naming scheme, Lx denotes a site
  template <int NDim=4>
  struct Geometry
  {
    DECLARE_COMPONENT(GlbSite,int64_t,DYNAMIC,glbSite);
    DECLARE_COMPONENT(LocSite,int64_t,DYNAMIC,locSite);
    DECLARE_COMPONENT(Rank,int32_t,DYNAMIC,rank);
    DECLARE_COMPONENT(Parity,char,2,eoDiscriminator);
    
    /// Global lattice grid
    using GlbGrid=
      LxGrid<NDim,GlbSite,NOT_HASHED>;
    
    /// Ranks grid
    using RanksGrid=
      LxGrid<NDim,Rank,HASHED>;
    
    /// Local lattice grid
    using LocGrid=
      LxGrid<NDim,LocSite,HASHED>;
    
    /// Global site grid
    const GlbGrid glbGrid;
    
    /// Global volume
    const GlbSite& glbVol=
      glbGrid.vol;
    
    /// Ranks grid
    const RanksGrid ranksGrid;
    
    /// Local dimensions
    const Coords<NDim> localDimensions;
    
    /// Local sites grid
    const LocGrid locGrid;
    
    /// Local volume
    const LocSite locVol=
      locGrid.vol;
    
    /// Parity of local sites
    const Vector<Parity> locLxParityTable;
    
    /// Parity of a site, given its global coordinates
    Parity parityOfGlbCoords(const Coords<NDim>& c) const
    {
      return static_cast<Parity>(c.sumAll()%2);
    }
    
    /// Compute the parity of a global site
    bool parityOfGlbLx(const GlbSite& id) const
    {
      return parityOfGlbCoords(glbGrid.computeCoordsOfLx(id));
    }
    /// Returns the parity of a local site from the lookup table
    bool parityOfLocLx(const LocSite& id) const
    {
      return locLxParityTable[id];
    }
    
    /// Compute explicitly the parity of local sites
    Parity computeParityOfLoclx(const LocSite& id) const
    {
      return parityOfGlbCoords(glbCoordsOfLocLx(id));
    }
    
    /// Compute global coordinates, given local site and rank
    Coords<NDim> glbCoordsOfLocLx(const LocSite& locLx,
				  const Rank& rankId=rank(thisRank())) const
    {
      /// Local coordinates
      const Coords<NDim> locCoords=
	locGrid.coordsOfLx(locLx);
      
      /// Coordinate of the rank
      const Coords<NDim> rankCoords=
	ranksGrid.coordsOfLx(rankId);
      
      /// Resulting coordinates
      Coords<NDim> glbCoords=
	locCoords+rankCoords*locGrid.sizes;
      
      return glbCoords;
    }
    
    /// Constructor
    Geometry(const Coords<NDim>& glbSizes,
	     const Coords<NDim>& ranksSizes) :
      glbGrid(glbSizes,allDimensions<NDim>),
      ranksGrid(ranksSizes,allDimensions<NDim>),
      localDimensions(ranksSizes==1),
      locGrid(glbSizes/ranksSizes,localDimensions),
      locLxParityTable(locVol,[this](const size_t& lx){return this->computeParityOfLoclx(locSite(lx));})
    {
      if((glbSizes%ranksSizes).sumAll())
	CRASHER<<"Global sizes "<<glbSizes<<" incompatible with rank sizes "<<ranksSizes<<endl;
    }
  };
}

#endif
