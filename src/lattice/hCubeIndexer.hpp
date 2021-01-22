#ifndef _H_CUBE_INDEXER_HPP
#define _H_CUBE_INDEXER_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file hCubeIndexer.hpp

#include <lattice/hCube.hpp>
#include <lattice/indexShuffler.hpp>
#include <metaProgramming/feature.hpp>

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
    
    /// Index of a given lexicographic site
    IndexShuffler<TensorComps<LxIndex>,IndexComps> idOfLx;
    
    /// Lexicographic site of a given index
    IndexShuffler<IndexComps,TensorComps<LxIndex>> lxOfId;
    
    /// Constructor taking a reference hypercube and a function to compute id from lx
    template <typename F>
    HCubeIndexer(const HC& hCube,
		 const IndexDeducerFromLx<F> &indexFromLx) :
      hCube(hCube),
      idOfLx(make_tuple(hCube.vol))
    {Bisogna passare la taglia massima delle componenti di out
      idOfLx.fill(indexFromLx.deFeat());
      lxOfId=idOfLx.transpose();
    }
    
    template <typename F>
    HCubeIndexer(const HC& hCube,
		 const LxIndexDeducer<F> &indexOfLx) :
      hCube(hCube),
      lxOfId(hCube.vol)
    {
      lxOfId.fill(indexOfLx.deFeat());
      idOfLx=lxOfId.transpose();
    }
  };
  
  /// Returns an indexer, deducing the index from Lx with the function
  ///
  /// The type F must derive from either LxIndexDeducer or IndexDeducerFromLx
  template <typename Index,
	    typename HC,
	    typename F>
  HCubeIndexer<Index,HC> getHCubeIndexer(const HC& hCube,
					 const F& f)
  {
    return HCubeIndexer<Index,HC>(hCube,f);
  }
}

#endif
