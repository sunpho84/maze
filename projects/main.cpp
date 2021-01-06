#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#include <Maze.hpp>

using namespace maze;


/// Transpose a shuffler
template <typename I>
Vector<I> transposeShuffler(const Vector<I>& shuffler)
{
  /// Number of elements
  const I n=
    shuffler.nEl;
  
  /// Result
  Vector<I> res(n,n /* as initializer */);
  
  for(I i=0;i<n;i++)
    {
      /// Index of the given element
      const I id=shuffler[i];
      
      // Check that id is in the correct range
      if(id<0 or id>n)
	CRASHER<<"while transposing shuffler, id of element "<<i<<": "<<id<<" out of range [0;"<<n<<")"<<endl;
      
      if(res[id]!=n)
	CRASHER<<"while transposing shuffler, id of element "<<i<<": "<<id<<" already used by element: "<<res[id]<<endl;
	
      res[id]=i;
    }
  
  return res;
}

/// Specify if index has to be deduced, or lx
enum HowToIndex{DEDUCE_INDEX_FROM_LX,DEDUCE_LX_FROM_INDEX};

/// Grid with a specific index
template <int NDim,
	  typename Index>
struct IndexedGrid
{
  /// Grid of reference
  const LxGrid<NDim,Index>& grid;
  
  /// Index of a given lexicographic site
  Vector<Index> idOfLx;
  
  /// Lexicographic site of a given index
  Vector<Index> lxOfId;
  
  /// Constructor taking a reference grid and a function to compute id from lx
  template <typename F>
  IndexedGrid(const LxGrid<NDim,Index>& grid,
	      F compute,
	      const HowToIndex& HT) :
    grid(grid)
  {
    switch(HT)
      {
      case DEDUCE_INDEX_FROM_LX:
	idOfLx=Vector<Index>(grid.vol,compute);
	lxOfId=transposeShuffler(idOfLx);
	break;
      case DEDUCE_LX_FROM_INDEX:
	lxOfId=Vector<Index>(grid.vol,compute);
	idOfLx=transposeShuffler(lxOfId);
	break;
      }
  }
};

template <int NDim=4>
struct Geometry
{
  /// Index to be used for grid that can deal with many sites
  using LongIndex=
    int64_t;
  
  /// Index to be used for grid that can deal with few sites
  using ShortIndex=
    int32_t;
  
  /// Global lattice lx grid
  using GlbLxGrid=
    LxGrid<NDim,LongIndex>;
  
  /// Ranks lx grid
  using RanksLxGrid=
    LxGrid<NDim,ShortIndex>;
  
  /// Local lattice lx grid
  using LocLxGrid=
    LxGrid<NDim,LongIndex>;
  
  /// Global site grid
  const GlbLxGrid glbLxGrid;
  
  /// Ranks grid
  const RanksLxGrid ranksLxGrid;
  
  /// Local sites grid
  const LocLxGrid locLxGrid;
  
  /// Compute the parity of a site, given its global coordinates
  bool parityOfGlbCoords(const Coords<NDim>& c) const
  {
    return c.sumAll()%2;
  }
  
  /// Compute the parity of a global site
  bool computeParityOfGlbLx(const LongIndex& id) const
  {
    return computeParityOfGlbLx(glbLxGrid.computeCoordsOfLx(id));
  }
  
  /// Compute global coordinates, given local site and rank
  Coords<NDim> computeGlbLxCoordsOfLocLx(const LongIndex& locLx,
					 const ShortIndex& rank=thisRank())
  {
    /// Local coordinates
    const Coords<NDim> locCoords=
      locLxGrid.coordsOfLx[locLx];
    
    /// Coordinate of the rank
    const Coords<NDim> rankCoords=
      ranksLxGrid.coordsOfLx[rank];
    
    /// Resulting coordinates
    Coords<NDim> glbCoords=
      locCoords+rankCoords*locLxGrid.sizes;
    
    return glbCoords;
  }
  
  /// Constructor
  Geometry(const Coords<NDim>& glbSizes,
	   const Coords<NDim>& ranksSizes) :
    glbLxGrid(glbSizes),
    ranksLxGrid(ranksSizes),
    locLxGrid(glbSizes/ranksSizes)
  {
    if((glbSizes%ranksSizes).sumAll())
      CRASHER<<"Global sizes "<<glbSizes<<" incompatible with rank sizes "<<ranksSizes<<endl;
  }
};

/// Geometry for fused sites
template <int NDim>
struct FusedSitesGeometry
{
  ///  Reference geometry
  using Geom=Geometry<NDim>;
  
  /// Type used to refer to virtual nodes
  using VNodesIndex=
    typename Geom::ShortIndex;
  
  // Type used to refer to fused sites
  using FSitesIndex=
    typename Geom::LongIndex;
  
  /// Type used to refer to local index
  using LocLxIndex=
    typename Geom::LongIndex;
  
  /// Virtual nodes grid
  using VNodesLxGrid=
    LxGrid<NDim,VNodesIndex>;
  
  /// Fused sites grid
  using FSitesLxGrid=
    LxGrid<NDim,FSitesIndex>;
  
  /// Reference geometry
  const Geometry<NDim>& geom;
  
  /// Grid to access virtual nodes
  const VNodesLxGrid vNodesLxGrid;
  
  /// Grid to move within fused sites
  const FSitesLxGrid fSitesLxGrid;
  
  /// Lebesgue ordering of fused sites
  const IndexedGrid<NDim,FSitesIndex> LebOrder;
  
  /// Computes the coords of a given fused site
  const Coords<NDim>& computeFSiteCoordsOfLeb(const FSitesIndex& fSiteLeb) const
  {
    /// Computes the lx order
    FSitesIndex& fSiteLx=
      LebOrder.lxOfId[fSiteLeb];
    
    return
      fSitesLxGrid.coordsOfLx[fSiteLx];
  }
  
  /// Compute global lx of a given pair of fused/unfused indices
  Coords<NDim> computeLocCoordsfFSiteVNode(const VNodesIndex& vNode,
					   const FSitesIndex& fSiteLeb) const
  {
    /// Fused site coordinates
    const Coords<NDim> fSiteCoords=
      computeFSiteCoords(fSiteLeb);
    
    /// Virtual node coordinates
    const Coords<NDim> vNodeCoords=
      vNodesLxGrid.coordsOfLx[vNode];
    
    /// Local coordinates
    const Coords<NDim> locCoords=
      vNodeCoords+vNodesLxGrid.sizes*fSiteCoords;
    
    return locCoords;
  }
  
  /// Constructor
  FusedSitesGeometry(const Geometry<NDim>& geom,
		     const Coords<NDim>& vNodesLxSizes) :
    geom(geom),
    vNodesLxGrid(vNodesLxSizes),
    fSitesLxGrid(geom.locLxGrid.sizes/vNodesLxSizes),
    LebOrder(fSitesLxGrid,LxOfLebesgueCalculator<NDim,FSitesIndex>(fSitesLxGrid),HowToIndex::DEDUCE_LX_FROM_INDEX)
  {
    if((geom.locLxGrid.sizes%vNodesLxSizes).sumAll())
      CRASHER<<"Local sizes "<<geom.locLxGrid.sizes<<" incompatible with requested vNodes Sizes"<<vNodesLxSizes<<endl;
  }
};

void in_main(int narg,char** arg)
{
  Geometry<4> geometry({2,2,2,2},{2,1,1,1});
  
  FusedSitesGeometry<4> fGeom(geometry,{1,2,2,2});
  
  LOGGER<<geometry.computeGlbLxCoordsOfLocLx(3)<<endl;
  
  // LxGrid<4,int> grid({4,2,2,2});
    
  //   LOGGER<<grid.computeSurfVol()<<endl;
    
  //   IndexedGrid<4,int> boh(grid,[](const int& i){return i;},HowToIndex::DEDUCE_INDEX_FROM_LX);
  //   IndexedGrid<4,int> leb(grid,LxOfLebesgueCalculator<4,int>(grid),HowToIndex::DEDUCE_LX_FROM_INDEX);
    
  //   for(int i=0;i<grid.vol;i++)
  //     LOGGER<<i<<" "<<leb.idOfLx[i]<<endl;
}

int main(int narg,char** arg)
{
  initMaze(in_main,narg,arg);
  
  finalizeMaze();
  
  return 0;
}
