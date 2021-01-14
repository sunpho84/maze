#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#include <Maze.hpp>

using namespace maze;


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
DECLARE_COMPONENT(LebSite,int64_t,DYNAMIC,lebSite);


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
  
//   /// Virtual nodes hypercube
//   using VNodesHCube=
//     HCube<NDim,VNodesIndex>;
  
//   /// Fused sites hypercube
//   using FSitesHCube=
//     HCube<NDim,FSitesIndex>;
  
//   /// Reference geometry
//   const Geometry<NDim>& geom;
  
//   /// Hypercube to access virtual nodes
//   const VNodesHCube vNodesHCube;
  
//   /// Hypercube to move within fused sites
//   const FSitesHCube fSitesHCube;
  
//   /// Lebesgue ordering of fused sites
//   const IndexedHypercube<NDim,FSitesIndex> LebOrder;
  
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
    
    /// Sizes of the even/odd split sites
    const Coords<nDims> locEoSizes;
    
    /// Even/Odd split hypercube grouping two sites
    const HCube<nDims,EosSite,UseHashedCoords::HASHED> eosHCube;
    
    /// Constructor
    EosIndexInitializer(const Geometry<nDims>& geo) :
      geo(geo),
      eoSplitDimension(geo.fastestLocalEvenDimension()),
      locEoSizes(geo.locSizes/(Coords<nDims>::versor(eoSplitDimension)+Coords<nDims>::getAll(1))),
      eosHCube(locEoSizes,geo.isDirectionFullyLocal)
    {
      LOGGER<<"e/o split dimension: "<<eoSplitDimension<<endl;
      LOGGER<<"loceo sizes: "<<locEoSizes<<endl;
    }
    
    /// Computes the two local sites of a given eosSite
    std::array<LocSite,2> locLxPairOfEosSite(const EosSite& eosSite) const
    {
      /// Coordinates in the eos hypercube
      const Coords<nDims> eosCoords=
	eosHCube.coordsOfLx(eosSite);
      
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
  
  HCube<nDims,Geometry<nDims>::LocSite,HASHED> hCube({4,2,2,2},{1,1,1,1});
    
  //   LOGGER<<grid.computeSurfVol()<<endl;
    
  //   IndexedGrid<nDims,int> boh(grid,[](const int& i){return i;},HowToIndex::DEDUCE_INDEX_FROM_LX);
  getHCubeIndexer<LebSite>(hCube,getLxOfLebesgueCalculator<LebSite>(hCube));
  
  //IndexedGrid<nDims,int> leb(grid,LxOfLebesgueCalculator<nDims,int>(grid),HowToIndex::DEDUCE_LX_FROM_INDEX);
    
  //   for(int i=0;i<grid.vol;i++)
  //     LOGGER<<i<<" "<<leb.idOfLx[i]<<endl;
}

int main(int narg,char** arg)
{
  initMaze(inMain,narg,arg);
  
  finalizeMaze();
  
  return 0;
}
