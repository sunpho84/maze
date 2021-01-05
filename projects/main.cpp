#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#include <Maze.hpp>

using namespace maze;

/// Hashed grid
template <int NDim,
	  typename Index>
struct HashedLxGrid : public LxGrid<NDim,Index>
{
  /// Grid volume
  const Index vol;
  
  /// Lookup table of coordinates
  const Vector<Coords<NDim>> coordsOfLx;
  
  /// Construct from sizes
  HashedLxGrid(const Coords<NDim>& sizes) :
    LxGrid<NDim,Index>(sizes),
    vol(this->computeVol()),
    coordsOfLx(this->getCoordsOfAllLx())
  {
  }
};

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

template <int NDim,
	  typename Index>
struct LxOfLebesgueCalculator
{
  /// Reference grid
  const HashedLxGrid<NDim,Index>& grid;
  
  /// Factors needed to compute Leb index
  std::vector<std::vector<int>> factors;
  
  /// Compute the Lx of a given Lebesgue
  Index operator()(const Index& _Leb) const
  {
    Index Leb=_Leb;
    
    /// Number of factors
    const size_t& nFactors=factors.size();
    Coords<NDim> c,t1,t2,t3;
    
    for(int i=0;i<NDim;i++)
      {
 	t1[i]=t2[i]=c[i]=0;
	t3[i]=(NDim*nFactors-1)%nFactors;
      }
    
    /// Convert to mixed base
    int xMixedBase[NDim][NDim*nFactors];
    memset(xMixedBase,0,sizeof(xMixedBase));
    
    for(int i=0;i<(int)(NDim*nFactors);i++)
      {
	int mu=NDim-1-(i%NDim);
	int f=factors[t1[mu]][mu];
	
	xMixedBase[mu][t2[mu]]=Leb%f;
	Leb/=f;
	
	t1[mu]=(t1[mu]+1)%nFactors;
	t2[mu]++;
      }
    
    //build coordinate in lx format
    for(int mu=0;mu<NDim;mu++)
      for(int j=NDim*nFactors-1;j>=0;j--)
	{
	  c[mu]=xMixedBase[mu][j]+factors[t3[mu]][mu]*c[mu];
	  t3[mu]=(t3[mu]+nFactors-1)%nFactors;
	}
    
    return grid.computeLxOfCoords(c);
  }
  
  /// Constructor
  LxOfLebesgueCalculator(const HashedLxGrid<NDim,Index>& grid) : grid(grid)
  {
    /// Get nmax_fact
    int nMaxFacts=0;
    for(int mu=0;mu<NDim;mu++)
      nMaxFacts=std::max(nMaxFacts,(int)factorize(grid.sizes[mu]).size());
    
    /// Set all factors to 1
    factors.resize(nMaxFacts,std::vector<int>(NDim));
    for(int i=0;i<nMaxFacts;i++)
      for(int mu=0;mu<NDim;mu++)
	factors[i][mu]=1;
    
    //put all the non-1 factors
    for(int mu=0;mu<NDim;mu++)
      {
	const std::vector<Index> listFactMu=factorize(grid.sizes[mu]);
	const int nFacts=listFactMu.size();
	const int nFacts1=nMaxFacts-nFacts;
	for(int ifact=0;ifact<nFacts;ifact++)
	  factors[nFacts1+ifact][mu]=listFactMu[ifact];
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
