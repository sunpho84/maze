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
	  typename Index,
	  UseHashedCoords UHC>
struct IndexedGrid
{
  /// Grid of reference
  const LxGrid<NDim,Index,UHC>& grid;
  
  /// Index of a given lexicographic site
  Vector<Index> idOfLx;
  
  /// Lexicographic site of a given index
  Vector<Index> lxOfId;
  
  /// Constructor taking a reference grid and a function to compute id from lx
  template <typename F>
  IndexedGrid(const LxGrid<NDim,Index,UHC>& grid,
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

template <typename Lx,
	  typename TC>
struct SitesOrdering;

template <typename Lx,
	  typename...C>
struct SitesOrdering<Lx,TensorComps<C...>>
{
  using Comps=
    TensorComps<C...>;
  
  // Comps sizes;
  
  Tensor<Comps,Lx> locLxOfSite;
  
  template <typename...Args>
  SitesOrdering(Args&&...args) : locLxOfSite(std::forward<Args>(args)...)
  {
    
    // forEachInTuple(extSizes, [this](const auto& e){std::get<decltype(e)>(sizes)=e;});
  }
};

DECLARE_COMPONENT(EosSite,int64_t,DYNAMIC,eosSite);


// X.X
// .X.

// XXX
// ...

// 012
// 345

// 0

// rank,eo,block,blocked


// /// Geometry for fused sites
// template <int NDim>
// struct FusedSitesGeometry
// {
//   ///  Reference geometry
//   using Geom=Geometry<NDim>;
  
//   /// Type used to refer to virtual nodes
//   using VNodesIndex=
//     typename Geom::ShortIndex;
  
//   // Type used to refer to fused sites
//   using FSitesIndex=
//     typename Geom::LongIndex;
  
//   /// Type used to refer to local index
//   using LocLxIndex=
//     typename Geom::LongIndex;
  
//   /// Virtual nodes grid
//   using VNodesLxGrid=
//     LxGrid<NDim,VNodesIndex>;
  
//   /// Fused sites grid
//   using FSitesLxGrid=
//     LxGrid<NDim,FSitesIndex>;
  
//   /// Reference geometry
//   const Geometry<NDim>& geom;
  
//   /// Grid to access virtual nodes
//   const VNodesLxGrid vNodesLxGrid;
  
//   /// Grid to move within fused sites
//   const FSitesLxGrid fSitesLxGrid;
  
//   /// Lebesgue ordering of fused sites
//   const IndexedGrid<NDim,FSitesIndex> LebOrder;
  
//   /// Computes the coords of a given fused site
//   const Coords<NDim>& computeFSiteCoordsOfLeb(const FSitesIndex& fSiteLeb) const
//   {
//     /// Computes the lx order
//     FSitesIndex& fSiteLx=
//       LebOrder.lxOfId[fSiteLeb];
    
//     return
//       fSitesLxGrid.coordsOfLx[fSiteLx];
//   }
  
//   /// Compute global lx of a given pair of fused/unfused indices
//   Coords<NDim> computeLocCoordsfFSiteVNode(const VNodesIndex& vNode,
// 					   const FSitesIndex& fSiteLeb) const
//   {
//     /// Fused site coordinates
//     const Coords<NDim> fSiteCoords=
//       computeFSiteCoords(fSiteLeb);
    
//     /// Virtual node coordinates
//     const Coords<NDim> vNodeCoords=
//       vNodesLxGrid.coordsOfLx[vNode];
    
//     /// Local coordinates
//     const Coords<NDim> locCoords=
//       vNodeCoords+vNodesLxGrid.sizes*fSiteCoords;
    
//     return locCoords;
//   }
  
//   /// Constructor
//   FusedSitesGeometry(const Geometry<NDim>& geom,
// 		     const Coords<NDim>& vNodesLxSizes) :
//     geom(geom),
//     vNodesLxGrid(vNodesLxSizes),
//     fSitesLxGrid(geom.locLxGrid.sizes/vNodesLxSizes),
//     LebOrder(fSitesLxGrid,LxOfLebesgueCalculator<NDim,FSitesIndex>(fSitesLxGrid),HowToIndex::DEDUCE_LX_FROM_INDEX)
//   {
//     if((geom.locLxGrid.sizes%vNodesLxSizes).sumAll())
//       CRASHER<<"Local sizes "<<geom.locLxGrid.sizes<<" incompatible with requested vNodes Sizes"<<vNodesLxSizes<<endl;
//   }
// };

void inMain(int narg,char** arg)
{
  static constexpr int NDim=4;
  
  Geometry<NDim> geometry({4,4,4,4},{2,2,2,1});
  
  LOGGER<<allDimensions<NDim><<endl;
  LOGGER<<allDimensionsBut<NDim,0><<endl;
  LOGGER<<"Bulk local volume: "<<geometry.locGrid.bulkVol<<" expected: "<<14*14*14*32<<endl;
  //Geometry<NDim> geometry({2,2,2,2},{2,1,1,1});
  
  //FusedSitesGeometry<NDim> fGeom(geometry,{1,2,2,2});
  
  LOGGER<<geometry.glbCoordsOfLocLx(Geometry<NDim>::locSite(3))<<endl;
  
  for(Geometry<NDim>::LocSite lx(0);lx<geometry.locVol;lx++)
    LOGGER<<geometry.parityOfLocLx(lx)<<endl;
  
  Tensor<TensorComps<Compl>> t;
  t[RE].eval()=1.0;
  LOGGER<<t[RE].eval()<<endl;
  
  /////////////////////////////////////////////////////////////////
  
  using LocSite=
    Geometry<NDim>::LocSite;
  
  using Parity=
    Geometry<NDim>::Parity;
  
  using ParityEos=
    TensorComps<Parity,EosSite>;
  
  const EosSite locVolh(geometry.locVolH);
  
  SitesOrdering<LocSite,ParityEos> eosOrder(static_cast<EosSite>(geometry.locVolH));
  
  LOGGER<<"/////////////////////////////////////////////////////////////////"<<endl;
  
  int lastEvenDimension=NDim;
  for(int mu=0;mu<NDim;mu++)
    if(geometry.locGrid.sizes[mu]%2==0)
      lastEvenDimension=mu;
  if(lastEvenDimension==NDim)
    CRASHER<<"No even dimension found"<<endl;
  
  Coords<NDim> loceoSizes=
    geometry.locGrid.sizes;
  loceoSizes[lastEvenDimension]/=2;
  
  LOGGER<<"loceo sizes: "<<loceoSizes<<endl;
  
  LxGrid<NDim,EosSite,UseHashedCoords::HASHED> eosGrid(loceoSizes,geometry.isDimensionLocal);
  for(EosSite eosSite(0);eosSite<locVolh;eosSite++)
    {
      const Coords<NDim> eosCoords=eosGrid.coordsOfLx(eosSite);
      Coords<NDim> lxCoordsA=
	eosCoords;
      lxCoordsA[lastEvenDimension]*=2;
      Coords<NDim> lxCoordsB=
	lxCoordsA;
      lxCoordsB[lastEvenDimension]++;
      
      const LocSite locLxA=
	geometry.locGrid.computeLxOfCoords(lxCoordsA);
      
      const LocSite locLxB=
	geometry.locGrid.computeLxOfCoords(lxCoordsB);
      
      const Parity parA=
	geometry.parityOfLocLx(locLxA);
      
      const Parity parB=
	geometry.parityOfLocLx(locLxB);
      
      eosOrder.locLxOfSite[parA][eosSite].eval()=locLxA;
      eosOrder.locLxOfSite[parB][eosSite].eval()=locLxB;
    }
  
  /// ora da sistemare
    
  for(Parity par(0);par<2;par++)
    for(EosSite eos(0);eos<locVolh;eos++)
      LOGGER<<"Parity "<<par<<" , eos: "<<eos<<" , lx: "<<eosOrder.locLxOfSite[par][eos].eval()<<endl;
  
  loopOnAllComponentsValues(eosOrder.locLxOfSite,
			    [&eosOrder](auto& id,
				 const ParityEos& comps)
			    {
			      const Geometry<NDim>::Parity& parity=
				std::get<Geometry<NDim>::Parity>(comps);
			      
			      const EosSite& eosSite=
				std::get<EosSite>(comps);
			      
			      eosOrder.locLxOfSite.data.data.data[id]=eosSite+parity;
			      
			      LOGGER<<"Decompose: "<<id<<", eos: "<<eosSite<<", par: "<<parity<<endl;
			    });
  
  for(Geometry<NDim>::Parity par(0);par<2;par++)
  for(EosSite eos(0);eos<geometry.locVolH;eos++)
    LOGGER<<eosOrder.locLxOfSite[par][eos].eval()<<endl;
  
  // LxGrid<NDim,int> grid({4,2,2,2});
    
  //   LOGGER<<grid.computeSurfVol()<<endl;
    
  //   IndexedGrid<NDim,int> boh(grid,[](const int& i){return i;},HowToIndex::DEDUCE_INDEX_FROM_LX);
  //   IndexedGrid<NDim,int> leb(grid,LxOfLebesgueCalculator<NDim,int>(grid),HowToIndex::DEDUCE_LX_FROM_INDEX);
    
  //   for(int i=0;i<grid.vol;i++)
  //     LOGGER<<i<<" "<<leb.idOfLx[i]<<endl;
}

int main(int narg,char** arg)
{
  initMaze(inMain,narg,arg);
  
  finalizeMaze();
  
  return 0;
}
