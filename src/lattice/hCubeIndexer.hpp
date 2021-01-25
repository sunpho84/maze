#ifndef _H_CUBE_INDEXER_HPP
#define _H_CUBE_INDEXER_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file hCubeIndexer.hpp

#include <lattice/hCube.hpp>
#include <lattice/indexShuffler.hpp>
#include <metaProgramming/feature.hpp>
#include <tensors/componentsList.hpp>

namespace maze
{
  DEFINE_FEATURE(IndexDeducerFromLx);
  
  DEFINE_FEATURE(LxIndexDeducer);
  
  /// Keep a lookup table for lx to a given index
  template <typename IndexComps,
	    typename HC>
  struct HCubeIndexer
  {
    /// Number of dimensions
    static constexpr int nDims=
      HC::NDims;
    
    /// Reference to the hypercube
    const HC& hCube;
    
    /// Lexicographic index
    using LxIndex=
      typename HC::Index;
    
    /// Components of the Lx index
    using LxComps=
      TensorComps<LxIndex>;
    
    /// Components sizes of the Index which need initialization
    using IndexDynComps=
      GetDynamicCompsOfTensorComps<IndexComps>;
    
    /// Index of a given lexicographic site
    IndexShuffler<LxComps,IndexComps> idOfLx;
    
    /// Lexicographic site of a given index
    IndexShuffler<IndexComps,LxComps> lxOfId;
    
    /// Constructor taking a reference hypercube and a function to compute id from lx
    template <typename F>
    HCubeIndexer(const HC& hCube,
		 const IndexDeducerFromLx<F> &indexFromLx,
		 const IndexDynComps& idMaxes) : ///< Maximal values of each component of the id
      hCube(hCube),
      idOfLx(std::make_tuple(hCube.vol)),
      lxOfId(IndexDynComps{})
    {
      idOfLx.fill(indexFromLx.deFeat());
      lxOfId=idOfLx.transpose(idMaxes);
    }
    
    template <typename F>
    HCubeIndexer(const HC& hCube,
		 const LxIndexDeducer<F> &indexOfLx,
		 const IndexDynComps& idMaxes) : ///< Maximal values of each component of the id
      hCube(hCube),
      idOfLx(LxComps{}),
      lxOfId(idMaxes)
    {
      lxOfId.fill(indexOfLx.deFeat());
      idOfLx=lxOfId.transpose(LxComps{hCube.vol});
    }
  };
  
  /// Returns an indexer, deducing the index from Lx with the function
  ///
  /// The type F must derive from either LxIndexDeducer or IndexDeducerFromLx
  template <typename IndexComps,
	    typename HC,
	    typename F>
  HCubeIndexer<IndexComps,HC> getHCubeIndexer(const HC& hCube,
					 const F& f,
					 const GetDynamicCompsOfTensorComps<IndexComps>& indexCompsInitializers)
  {
    return HCubeIndexer<IndexComps,HC>(hCube,f,indexCompsInitializers);
  }
}

#endif
