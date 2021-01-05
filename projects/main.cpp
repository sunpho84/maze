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
  const HashedLxGrid<NDim,Index>& grid;
  
  /// Index of a given lexicographic site
  Vector<Index> idOfLx;
  
  /// Lexicographic site of a given index
  Vector<Index> lxOfId;
  
  /// Constructor taking a reference grid and a function to compute id from lx
  template <typename F>
  IndexedGrid(const HashedLxGrid<NDim,Index>& grid,
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

void a()
{

}

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
    HashedLxGrid<NDim,ShortIndex>;
  
  /// Local lattice lx grid
  using LocLxGrid=
    HashedLxGrid<NDim,LongIndex>;
  
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
  
  Geometry(const Coords<NDim>& glbSizes,
	   const Coords<NDim>& ranksSizes) :
    glbLxGrid(glbSizes),
    ranksLxGrid(ranksSizes),
    locLxGrid(glbSizes/ranksSizes)
  {
    LOGGER<<glbSizes<<" "<<ranksSizes<<" "<<endl;
  }
};


void in_main(int narg,char** arg)
{
  Geometry<4> geometry({2,2,2,2},{2,1,1,1});
  
  LOGGER<<geometry.computeGlbLxCoordsOfLocLx(3)<<endl;
  
  // HashedLxGrid<4,int> grid({4,2,2,2});
    
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
