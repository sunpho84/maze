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
    DECLARE_COMPONENT(Parity,int8_t,2,eoDiscriminator);
    
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
      LxGrid<NDim,GlbSite,NOT_HASHED>;
    
    /// Ranks grid
    using RanksGrid=
      LxGrid<NDim,Rank,HASHED>;
    
    /// Local lattice grid
    using LocGrid=
      LxGrid<NDim,LocSite,HASHED>;
    
    /// Global site grid
    const GlbGrid glbGrid;
    
    /// Global sizes
    const Coords<NDim>& glbSizes=
	    glbGrid.sizes;
    
    /// Global volume
    const GlbSite& glbVol=
      glbGrid.vol;
    
    /// Ranks grid
    const RanksGrid ranksGrid;
    
    /// Number of ranks per dimension
    const Coords<NDim>& nRanksPerDim=
	    ranksGrid.sizes;
    
    /// Local dimensions
#warning change again name
    const Coords<NDim> isDimensionLocal;
    
    /// Local sites grid
    const LocGrid locGrid;
    
    /// Local sizes
    const Coords<NDim>& locSizes=
	    locGrid.sizes;
    
    /// Local volume
    const LocSite& locVol=
      locGrid.vol;
    
    /// Half the local volume
    const LocSite& locVolH=
      locGrid.volH;
    
    /// Parity and eos index of local sites
    const Vector<Parity> locLxParityTable;
    
    /// Parity of a site, given its global coordinates
    Parity parityOfGlbCoords(const Coords<NDim>& c) const
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
      return locLxParityTable[id];
    }
    
    /// Compute explicitly the parity of local sites
    Parity computeParityOfLocLx(const LocSite& id) const
    {
      return parityOfGlbCoords(glbCoordsOfLocLx(id));
    }
    
    /// Compute the lx site of a given set of coords
    LocSite computeLxOfLocCoords(const Coords<NDim>& coords) const
    {
      return locGrid.computeLxOfCoords(coords);
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
    
    /// Return fasted dimension of even local size
    int fastestLocalEvenDimension() const
    {
      /// Result
      int res=
	NDim;
      
      // Find last even dimension
      for(int mu=0;mu<NDim;mu++)
	if(locSizes[mu]%2==0)
	  res=mu;
      
      if(res==NDim)
	CRASHER<<"No even dimension found"<<endl;
      
      return res;
    }
    
    /// Constructor
    Geometry(const Coords<NDim>& glbSizes,
	     const Coords<NDim>& ranksSizes) :
      glbGrid(glbSizes,allDimensions<NDim>),
      ranksGrid(ranksSizes,allDimensions<NDim>),
      isDimensionLocal(ranksSizes==1),
      locGrid(glbSizes/ranksSizes,isDimensionLocal),
      locLxParityTable(locVol,[this](const size_t& lx){return this->computeParityOfLocLx(locSite(lx));})
    {
      if((glbSizes%ranksSizes).sumAll())
	CRASHER<<"Global sizes "<<glbSizes<<" incompatible with rank sizes "<<ranksSizes<<endl;
      
      if(locVolH*2!=locVol)
	CRASHER<<"Local volume must be even, is "<<locVol<<endl;
    }
  };
}

#endif
