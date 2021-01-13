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
template <int _NDims,
	  typename Index,
	  UseHashedCoords UHC>
struct IndexedGrid
{
  /// Number of dimensions
 static constexpr int nDims=
   _NDims;
  
  /// Grid of reference
  const HCube<nDims,Index,UHC>& grid;
  
  /// Index of a given lexicographic site
  Vector<Index> idOfLx;
  
  /// Lexicographic site of a given index
  Vector<Index> lxOfId;
  
  /// Constructor taking a reference grid and a function to compute id from lx
  template <typename F>
  IndexedGrid(const HCube<nDims,Index,UHC>& grid,
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
  
  Vector<Comps> siteOfLocLx;
  
  template <typename...Args>
  SitesOrdering(Args&&...args) :
    locLxOfSite(std::forward<Args>(args)...),
    siteOfLocLx(locLxOfSite.data.getSize()) // hack
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
//   using VNodesHCube=
//     HCube<NDim,VNodesIndex>;
  
//   /// Fused sites grid
//   using FSitesHCube=
//     HCube<NDim,FSitesIndex>;
  
//   /// Reference geometry
//   const Geometry<NDim>& geom;
  
//   /// Grid to access virtual nodes
//   const VNodesHCube vNodesHCube;
  
//   /// Grid to move within fused sites
//   const FSitesHCube fSitesHCube;
  
//   /// Lebesgue ordering of fused sites
//   const IndexedGrid<NDim,FSitesIndex> LebOrder;
  
//   /// Computes the coords of a given fused site
//   const Coords<NDim>& computeFSiteCoordsOfLeb(const FSitesIndex& fSiteLeb) const
//   {
//     /// Computes the lx order
//     FSitesIndex& fSiteLx=
//       LebOrder.lxOfId[fSiteLeb];
    
//     return
//       fSitesHCube.coordsOfLx[fSiteLx];
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
//       vNodesHCube.coordsOfLx[vNode];
    
//     /// Local coordinates
//     const Coords<NDim> locCoords=
//       vNodeCoords+vNodesHCube.sizes*fSiteCoords;
    
//     return locCoords;
//   }
  
//   /// Constructor
//   FusedSitesGeometry(const Geometry<NDim>& geom,
// 		     const Coords<NDim>& vNodesLxSizes) :
//     geom(geom),
//     vNodesHCube(vNodesLxSizes),
//     fSitesHCube(geom.locHCube.sizes/vNodesLxSizes),
//     LebOrder(fSitesHCube,LxOfLebesgueCalculator<NDim,FSitesIndex>(fSitesHCube),HowToIndex::DEDUCE_LX_FROM_INDEX)
//   {
//     if((geom.locHCube.sizes%vNodesLxSizes).sumAll())
//       CRASHER<<"Local sizes "<<geom.locHCube.sizes<<" incompatible with requested vNodes Sizes"<<vNodesLxSizes<<endl;
//   }
// };

void inMain(int narg,char** arg)
{
  static constexpr int nDims=4;
  
  const Coords<nDims> glbSizes{1,1,2,6};
  const Coords<nDims> nRanksPerDim{1,1,1,2};
  
  Geometry<nDims> geometry(glbSizes,nRanksPerDim);
  
  // LOGGER<<allDimensions<nDims><<endl;
  // LOGGER<<allDimensionsBut<nDims,0><<endl;
  // LOGGER<<"Bulk local volume: "<<geometry.locGrid.bulkVol<<" expected: "<<14*14*14*32<<endl;
  //Geometry<nDims> geometry({2,2,2,2},{2,1,1,1});
  
  //FusedSitesGeometry<nDims> fGeom(geometry,{1,2,2,2});
  
  // LOGGER<<geometry.glbCoordsOfLocLx(Geometry<nDims>::locSite(3))<<endl;
  
  // for(Geometry<nDims>::LocSite lx(0);lx<geometry.locVol;lx++)
  //   LOGGER<<geometry.parityOfLocLx(lx)<<endl;
  
  // Tensor<TensorComps<Compl>> t;
  // t[RE].eval()=1.0;
  // LOGGER<<t[RE].eval()<<endl;
  
  /////////////////////////////////////////////////////////////////
  
    using LocSite=
    Geometry<nDims>::LocSite;
  
  using Parity=
    Geometry<nDims>::Parity;
  
  const EosSite locVolh(geometry.locVolH);
  
  LOGGER<<"/////////////////////////////////////////////////////////////////"<<endl;

  /// Initializer for a even/odd split index
  struct EosIndexInitializer
  {
    /// Reference geometry
    const Geometry<nDims>& geo;
    
    /// Even/Odd split dimension
    const int eoSplitDimension;
    
    /// Sizes of the even/odd split grid
    const Coords<nDims> locEoSizes;
    
    /// Even/Odd split grid grouping two sites
    const HCube<nDims,EosSite,UseHashedCoords::HASHED> eosGrid;
    
    /// Constructor
    EosIndexInitializer(const Geometry<nDims>& geo) :
      geo(geo),
      eoSplitDimension(geo.fastestLocalEvenDimension()),
      locEoSizes(geo.locSizes/(Coords<nDims>::versor(eoSplitDimension)+Coords<nDims>::getAll(1))),
      eosGrid(locEoSizes,geo.isDirectionFullyLocal)
    {
      LOGGER<<"e/o split dimension: "<<eoSplitDimension<<endl;
      LOGGER<<"loceo sizes: "<<locEoSizes<<endl;
    }
    
    /// Computes the two local sites of a given eosSite
    std::array<LocSite,2> locLxPairOfEosSite(const EosSite& eosSite) const
    {
      /// Coordinates in the eos grid
      const Coords<nDims> eosCoords=
	eosGrid.coordsOfLx(eosSite);
      
      /// Result
      std::array<LocSite,2> out;
      
      for(int i=0;i<2;i++)
	{
	  /// Coordinates
	  Coords<nDims> lxCoords=
	    eosCoords;
	  
	  // Double in the eo dimension and increase in turn
	  lxCoords[eoSplitDimension]=
	    2*lxCoords[eoSplitDimension]+i;
	  
	  /// Local site
	  const LocSite locLx=
	    geo.computeLxOfLocCoords(lxCoords);
	  
	  /// Parity of the site
	  const Parity par=
	    geo.parityOfLocLx(locLx);
	  
	  out[par]=locLx;
	}
      
      return out;
    }
  };
  
  EosIndexInitializer eosIndexInitialzier(geometry);
  
  using ParityEos=
    TensorComps<Parity,EosSite>;
  
  SitesOrdering<LocSite,ParityEos> eosOrder(static_cast<EosSite>(geometry.locVolH));
  
  for(EosSite eos(0);eos<locVolh;eos++)
    {
      const auto locLxPair=
	eosIndexInitialzier.locLxPairOfEosSite(eos);
      
      for(Parity par(0);par<2;par++)
	{
	  const LocSite& locLx=
	    locLxPair[par];
	  
	  eosOrder.locLxOfSite[par][eos].eval()=
	    locLx;
	  
	  eosOrder.siteOfLocLx[locLx]=
	    ParityEos{par,eos};
	}
    }
  
  setPrintingRank(mpiRank(0));
  
  LOGGER<<"/////////////////////////////////////////////////////////////////"<<endl;
  
  for(LocSite locLx(0);locLx<geometry.locVol;locLx++)
    {
      const ParityEos& parityEos=
	eosOrder.siteOfLocLx[locLx];
      
      const Parity& par=
	std::get<Parity>(parityEos);
      
      const EosSite& eosSite=
	std::get<EosSite>(parityEos);
      
      LOGGER<<locLx<<" "<<par<<" , eos: "<<eosSite<<endl;
    }
  
  LOGGER<<"/////////////////////////////////////////////////////////////////"<<endl;
  
  for(Parity par(0);par<2;par++)
    for(EosSite eos(0);eos<locVolh;eos++)
      LOGGER<<par<<" , eos: "<<eos<<" : lx "<<eosOrder.locLxOfSite[par][eos].eval()<<endl;
  
  // loopOnAllComponentsValues(eosOrder.locLxOfSite,
  // 			    [&eosOrder](auto& id,
  // 				 const ParityEos& comps)
  // 			    {
  // 			      const Geometry<nDims>::Parity& parity=
  // 				std::get<Geometry<nDims>::Parity>(comps);
			      
  // 			      const EosSite& eosSite=
  // 				std::get<EosSite>(comps);
			      
  // 			      eosOrder.locLxOfSite.data.data.data[id]=eosSite+parity;
			      
  // 			      LOGGER<<"Decompose: "<<id<<", eos: "<<eosSite<<", par: "<<parity<<endl;
  // 			    });
  
  // for(Geometry<nDims>::Parity par(0);par<2;par++)
  // for(EosSite eos(0);eos<geometry.locVolH;eos++)
  //   LOGGER<<eosOrder.locLxOfSite[par][eos].eval()<<endl;
  
  // HCube<nDims,int> grid({4,2,2,2});
    
  //   LOGGER<<grid.computeSurfVol()<<endl;
    
  //   IndexedGrid<nDims,int> boh(grid,[](const int& i){return i;},HowToIndex::DEDUCE_INDEX_FROM_LX);
  //   IndexedGrid<nDims,int> leb(grid,LxOfLebesgueCalculator<nDims,int>(grid),HowToIndex::DEDUCE_LX_FROM_INDEX);
    
  //   for(int i=0;i<grid.vol;i++)
  //     LOGGER<<i<<" "<<leb.idOfLx[i]<<endl;
}

int main(int narg,char** arg)
{
  initMaze(inMain,narg,arg);
  
  finalizeMaze();
  
  return 0;
}
