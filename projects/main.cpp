#include "lattice/geometry.hpp"
#include "lattice/hCubeIndexer.hpp"
#include "tensors/componentsList.hpp"
#include "tensors/tensorDecl.hpp"
#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

#include <Maze.hpp>

using namespace maze;

///  index
///
///
struct SiteMultiIndex;

template <typename Lx,
	  typename TC>
struct SitesOrdering;

template <typename Lx,
	  typename...C>
struct SitesOrdering<Lx,TensorComps<C...>>
{
  /// Components
  using TC=
    TensorComps<C...>;
  
  Tensor<TC,Lx> lxOfSite;
  
  Vector<TC> siteOfLx;
  
  template <typename...Args>
  SitesOrdering(Args&&...args) :
    lxOfSite(std::forward<Args>(args)...),
    siteOfLx(lxOfSite.data.getSize()) // hack
  {
    // forEachInTuple(extSizes, [this](const auto&
    // e){std::get<decltype(e)>(sizes)=e;});
  }
};

DECLARE_COMPONENT(EosSite,int64_t,DYNAMIC,eosSite);
DECLARE_COMPONENT(LebSite,int64_t,DYNAMIC,lebSite);

DECLARE_COMPONENT(BlockId,int64_t,DYNAMIC,blockId);
DECLARE_COMPONENT(BlockedSite,int64_t,DYNAMIC,blockedSite);
DECLARE_COMPONENT(BlockedEosSite,int64_t,DYNAMIC,blockedEosSite);



// /// Initializer for a even/odd split index
// template <typename G>
// struct EosIndexInitializer
// {
//   /// Reference geometry
//   const G& geo;
  
//   static constexpr int nDims=
//     G::nDims;
  
//   using Direction=
//     typename G::Direction;
  
//   /// Even/Odd split dimension
//   const Direction eoSplitDimension;
  
//   using LocSite=
//     typename G::LocSite;
  
//   using Parity=
//     typename G::Parity;
  
//   using ParityHCube=
//     HCube<nDims,Parity,NOT_HASHED>;
  
//   ParityHCube parityHCube;
  
//   /// Sizes of the even/odd split sites
//   const Coords<nDims> locEoSizes;
  
//   /// Even/Odd split hypercube grouping two sites
//   const HCube<nDims,EosSite,UseHashedCoords::HASHED> eosHCube;
  
//   /// Constructor
//   EosIndexInitializer(const Geometry<nDims>& geo,
// 		      const Direction& eoSplitDimension) :
//     geo(geo),
//     eoSplitDimension(eoSplitDimension),
//     parityHCube(Coords<nDims>::versor(eoSplitDimension)+
// 		Coords<nDims>::getAll(1),allDimensions<nDims>),
//     locEoSizes(geo.locSizes/parityHCube.sizes),
//     eosHCube(locEoSizes,geo.isDirectionFullyLocal)
//   {
//     LOGGER<<"e/o split dimension: "<<eoSplitDimension<<endl;
//     LOGGER<<"loceo sizes: "<<locEoSizes<<endl;
//   }
  
//   /// Computes the two local sites of a given eosSite
//   std::array<LocSite,2> locLxPairOfEosSite(const EosSite& eosSite) const
//   {
//       /// Coordinates in the eos hypercube
//       const Coords<nDims> eosCoords=
// 	eosHCube.coordsOfLx(eosSite);
      
//       /// Result
//       std::array<LocSite,2> out;
      
//       for(int i=0;i<2;i++)
// 	{
// 	  /// Coordinates
// 	  Coords<nDims> lxCoords=
// 	    eosCoords;
	  
// 	  // Double in the eo dimension and increase in turn
// 	  lxCoords[eoSplitDimension]=
// 	    2*lxCoords[eoSplitDimension]+i;
	  
// 	  /// Local site
// 	  const LocSite locLx=
// 	    geo.computeLxOfLocCoords(lxCoords);
	  
// 	  /// Parity of the site
// 	  const Parity par=
// 	    geo.parityOfLocLx(locLx);
	  
// 	  out[par]=locLx;
// 	}
      
//       return out;
//     }
//  };
  
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


  // /// Structure to compute Lebesgue index
  // template <typename HC,
  // 	    typename Index>
  // struct LxOfSomethingCalculator :
  //   public IndexDeducerFromLx<LxOfSomethingCalculator<HC,Index>>
  // {
  //   /// Reference hCube
  //   const HC& hCube;
    
  //   /// Number of dimensions
  //   static constexpr int nDims=
  //     HC::nDims;
    
  //   /// Index type
  //   using LxIndex=
  //     typename HC::Index;
    
  //   /// Factors needed to compute Leb index
  //   std::vector<std::vector<int>> factors;
    
  //   /// Compute the Lx of a given Lebesgue
  //   Index operator()(const LxIndex& _ind) const
  //   {
  //     Index id;
      
  //     return id;
  //   }
    
  //   /// Constructor
  //   LxOfSomethingCalculator(const HC& hCube) : hCube(hCube)
  //   {
  //   }
  // };



void inMain(int narg,char** arg)
{
  /// Number of dimensions
  static constexpr int nDims=4;
  
  /// Global lattice size
  const Coords<nDims> glbSizes{10,4,9,12};
  
  /// Number or ranks per direction
  const Coords<nDims> nRanksPerDim{1,1,1,2};
  
  /// Lattice geometry
  Geometry<nDims> geometry(glbSizes,nRanksPerDim);
  
  auto ind=getLebesgueIndexer<LebSite>(geometry.locGrid);
 
  using LocSite=
    Geometry<nDims>::LocSite;
  
  using Parity=
    Geometry<nDims>::Parity;
  
  //(rank,)eo,block,blocked
  // using EoBlockedComps=
  //   TensorComps<Parity,BlockId,BlockedEosSite>;
  
//   TensorComps<BlockId,BlockedEosSite> eoBlockedCompInits{};
// #warning
//   HCubeIndexer<EoBlockedComps,Geometry<nDims>::LocGrid> EoBlockedComp(geometry.locGrid,
// 								      LxOfSomethingCalculator<Geometry<nDims>::LocGrid,EoBlockedComps>(geometry.locGrid),eoBlockedCompInits
// 								      );
  
  
  Tensor<TensorComps<Parity>> test;
  test[geometry.parity(0)]=0;
  
  /// Sizes of the block
  const Coords<nDims> nBlockedSitesPerDir=
    {2,2,3,3};
  
  //EosIndexInitializer<Geometry<nDims>> eosIndexInitializer(geometry,nBlockedSitesPerDir.fastestLocalEvenDimension());
  //LOGGER<<"Parity hypercube: "<<eosIndexInitializer.parityHCube.sizes<<endl;
  
  /// Compute the parity
  auto parityHCube=
    geometry.getParityGrid(nBlockedSitesPerDir.fastestLocalEvenDimension());
  
  /// Size of the parity hypercube
  const auto& paritySizes=
    parityHCube.sizes;
  
  /// Number of blocks in each direction
  const Coords<nDims> nBlocksPerDir=
    geometry.locSizes/nBlockedSitesPerDir;
  
  /// Sizes of the e/o block
  const Coords<nDims> nEosBlockedSitesPerDir=
    nBlockedSitesPerDir/paritySizes;
  
  /// Blocks hypercube
  HCube<nDims,BlockId,HASHED> blocksHCube(nBlocksPerDir,{1,1,1,1});
  
  /// Blocked e/o sites hypercube
  HCube<nDims,BlockedEosSite,HASHED> blockedEosSitesHCube(nEosBlockedSitesPerDir,{1,1,1,1});
  
  //const BlockedId eoBlockedSize=geometry.locVol/bCube.vol/2;
  //Tensor<EoBlockedComps> blocked(bCube.vol,eoBlockedSize);
  
  LOGGER<<"Local size: "<<geometry.locSizes<<" , "<<geometry.locVol<<" sites"<<endl;
  LOGGER<<"N blocks per dir: "<<nBlocksPerDir<<" , "<<blocksHCube.vol<<" blocks"<<endl;
  LOGGER<<"Blocked eos sites sizes: "<<nEosBlockedSitesPerDir<<endl;
  
  for(LocSite locSite=0;locSite<geometry.locVol;locSite++)
    {
      /// Parity
      const Parity par=
	geometry.parityOfLocLx(locSite);
      
      /// Coordinates of the site
      const Coords<nDims> locSiteCoords=
	geometry.locCoordsOfLocLx(locSite);
      
      /// Coordinateds of the block
      const Coords<nDims> blockCoords=
	locSiteCoords/nBlockedSitesPerDir;
      
      /// Coordinates inside the block
      const Coords<nDims> blockedCoords=
	locSiteCoords%nBlockedSitesPerDir;
      
      /// Coordinates inside the blocked e/o hypercube
      const Coords<nDims> blockedEosCoords=
	blockedCoords/paritySizes;
      
      /// Id inside the block
      const BlockId blockId=
	blocksHCube.computeLxOfCoords(blockCoords);
      
      /// Id inside the blocked e/o hypercube
      const BlockedEosSite blockedEosSite=
	blockedEosSitesHCube.computeLxOfCoords(blockedEosCoords);
      
      LOGGER<<" Lx "<<locSite<<" "<<locSiteCoords<<" , par: "<<par<<" , block: "<<blockId<<" "<<blockCoords<<" , id: "<<blockedEosSite<<" "<<blockedEosCoords<<endl;
      //loc_site <--- eo,eosSite
      //loc_site <--- block,blockedSite
      
      // loc_site <--- eo,block,blockedEoSite
    }
  
  /* la cosa meglio sarebbe far si' che il site shuffler prendesse
     piu' componenti, da inizializzare con una funzione piu' complessa
     che prenda le coordinate di partenza e le rimappi in quelle di
     arrivo 
     
     ad esempio le coordinate di partenza potrebbero essere anche le coordinate cartesiane?
*/
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
  
  const EosSite locVolh(geometry.locVolH);
  
  LOGGER<<"/////////////////////////////////////////////////////////////////"<<endl;

  // EosIndexInitializer eosIndexInitialzier(geometry);
  
  // using ParityEos=
  //   TensorComps<Parity,EosSite>;
  
  // SitesOrdering<LocSite,ParityEos> eosOrder(static_cast<EosSite>(geometry.locVolH));
  
  // for(EosSite eos(0);eos<locVolh;eos++)
  //   {
  //     const auto locLxPair=
  // 	eosIndexInitialzier.locLxPairOfEosSite(eos);
      
  //     for(Parity par(0);par<2;par++)
  // 	{
  // 	  const LocSite& locLx=
  // 	    locLxPair[par];
	  
  // 	  eosOrder.lxOfSite[par][eos].eval()=
  // 	    locLx;
	  
  // 	  eosOrder.siteOfLx[locLx]=
  // 	    ParityEos{par,eos};
  // 	}
  //   }
  
  // setPrintingRank(mpiRank(0));
  
  // LOGGER<<"/////////////////////////////////////////////////////////////////"<<endl;
  
  // for(LocSite locLx(0);locLx<geometry.locVol;locLx++)
  //   {
  //     const ParityEos& parityEos=
  // 	eosOrder.siteOfLx[locLx];
      
  //     const Parity& par=
  // 	std::get<Parity>(parityEos);
      
  //     const EosSite& eosSite=
  // 	std::get<EosSite>(parityEos);
      
  //     LOGGER<<locLx<<" "<<par<<" , eos: "<<eosSite<<endl;
  //   }
  
  // LOGGER<<"/////////////////////////////////////////////////////////////////"<<endl;
  
  // for(Parity par(0);par<2;par++)
  //   for(EosSite eos(0);eos<locVolh;eos++)
  //     LOGGER<<par<<" , eos: "<<eos<<" : lx "<<eosOrder.lxOfSite[par][eos].eval()<<endl;
  
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
  
    
  //   LOGGER<<grid.computeSurfVol()<<endl;
    
  //   IndexedGrid<nDims,int> boh(grid,[](const int& i){return i;},HowToIndex::DEDUCE_INDEX_FROM_LX);
  //auto ind=getLebesgueIndexer<LebSite>(hCube);
  
  
  // Coords<nDims> c{};
  // for(c[2]=0;c[2]<4;c[2]++)
  //   {
  //     for(c[3]=0;c[3]<4;c[3]++)
  // 	{
  // 	  const LocSite lx=hCube.computeLxOfCoords(c);
	  
  // 	  const LebSite leb=
  // 	    ind.idOfLx(lx);
	  
  // 	  LOGGER<<leb<<" ";
  // 	  if(leb<10)
  // 	    LOGGER<<" ";
  // 	}
  //     LOGGER<<endl;
  //   }
  
  // for(LebSite leb=0;leb<hCube.vol;leb++)
  //   {
  //     const LocSite lx=ind.lxOfId(leb);
  //     const Coords<nDims> c=hCube.coordsOfLx(lx);
      
  //     LOGGER<<" "<<c[1]<<" "<<c[2]<<" "<<c[3]<<endl;
  //   }
  
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
